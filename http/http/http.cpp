#include <windows.h>
#include <sstream>

#include "http.h"
#include "httpexcept.h"
#include "parsing.h"

const std::string HTTP::EOL = "\r\n";
std::string HTTP::UserAgent = "Custom HTTP class";

void HTTP::InitWinsock()
{
	WORD sockVersion;
	WSADATA wsaData;

	sockVersion = MAKEWORD(1, 1);

	// Initialiser Winsock
	if (WSAStartup(sockVersion, &wsaData))
		throw HTTP_Exception(HTTP_Exception::HTTP_WINSOCK_FAILED);
}

void HTTP::CleanupWinsock()
{
	WSACleanup();
}

HTTP::HTTP()
	: TEValue(te_none)
{

}

void HTTP::Connect(const std::string& host, int port)
{
	Host = host;

	LPHOSTENT hostEntry = gethostbyname(host.c_str());

	if (!hostEntry)
		throw HTTP_Exception(HTTP_Exception::HTTP_GETHOSTBYNAME_FAILED);

	Sock = socket(AF_INET,
		SOCK_STREAM,
		IPPROTO_TCP);

	if (Sock == INVALID_SOCKET)
		throw HTTP_Exception(HTTP_Exception::HTTP_SOCKET_FAILED);

	SOCKADDR_IN serverInfo;
	serverInfo.sin_family = AF_INET;

	serverInfo.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	serverInfo.sin_port = htons(port);

	int ret = connect(Sock,
		(LPSOCKADDR)& serverInfo,
		sizeof(struct sockaddr));

	if (ret == SOCKET_ERROR)
		throw HTTP_Exception(HTTP_Exception::HTTP_CONNECT_FAILED);
}

void HTTP::Request(const std::string& req)
{
	std::ostringstream oss;

	oss << "GET " << req << " HTTP/1.1" << EOL
		<< "Host:" << Host << EOL
		<< "User-Agent: " << UserAgent << EOL
		<< EOL;

	ProcessRequest(oss.str());
}

void HTTP::ProcessRequest(const std::string& req)
{
	SendData(req.c_str(), (int)req.size());

	int len = GetData(BufferReading, 5000);

	if (len == SOCKET_ERROR)
		throw HTTP_Exception(HTTP_Exception::HTTP_RECV_FAILED);

	size_t header_len = GetHeaders(BufferReading, len);

	if (header_len)
	{
		// Get Rest of buffer after headers
		const size_t EOLs_size = (EOL + EOL).size();

		size_t rest = len - header_len - EOLs_size;
		char *ptr = &BufferReading[header_len + EOLs_size];

		for (size_t i = 0; i < rest; i++, ptr++)
			BufferData.push_back(*ptr);
	}
}

int HTTP::GetData(char *buf, int n)
{
	int err = recv(Sock, buf, n, 0);

	if (err == SOCKET_ERROR)
		throw HTTP_Exception(HTTP_Exception::HTTP_RECV_FAILED);

	return err;
}

int HTTP::SendData(const char *buffer, int len)
{
	int err = send(Sock, buffer, len, 0);

	if (err == SOCKET_ERROR)
		throw HTTP_Exception(HTTP_Exception::HTTP_SEND_FAILED);

	return err;
}

size_t HTTP::GetHeaders(const char *buffer, int len)
{
	std::vector<std::string> list;
	Parsing::Delimited(buffer, len, EOL + EOL, list);

	if (list.size() != 0)
	{
		std::vector<std::string> entry_list;

		Parsing::Delimited(list[0], EOL, entry_list);

		if (!Parsing::FindKey("HTTP/1.1 ", entry_list, true, HTTPStatus))
			throw HTTP_Exception(HTTP_Exception::HTTP_MISSING_HTTP_ENTRY);

		for (auto& e : entry_list)
			Parsing::Split(e, ":", HeaderEntriesMap);

		if (HTTPStatus.find("200") == std::string::npos)
			throw HTTP_Exception(HTTP_Exception::HTTP_NOT_200_OK, HTTPStatus);

		return list[0].size();
	}

	return 0;
}

std::string HTTP::GetTransferEncoding()
{
	auto it = HeaderEntriesMap.find("Transfer-Encoding");

	if ( it == HeaderEntriesMap.end())
		throw HTTP_Exception(HTTP_Exception::HTTP_TRANSFER_ENCODING_MISSING);

	return it->second;
}

bool HTTP::IsChunked()
{
	if (TEValue == te_none)
	{
		if (GetTransferEncoding() == "chunked")
			TEValue = te_chunked;
	}

	return (TEValue == te_chunked);
}

bool HTTP::IsComplete()
{
	if (TEValue == te_none)
	{
//		if (GetTransferEncoding() == "stream")
			TEValue = te_complete;
	}

	return (TEValue == te_complete);
}

bool HTTP::IsContinuing()
{
	if (TEValue == te_none)
	{
		//		if (GetTransferEncoding() == "stream")
		TEValue = te_continuing;
	}

	return (TEValue == te_continuing);
}

std::vector<unsigned char>& HTTP::ReadData()
{
	if (IsChunked())
		ReadChunk();
	else if (IsComplete())
		ReadComplete();

	return BufferData;
}

void HTTP::ReadChunk()
{
	// TODO: INCOMPLETE
	if (ChunkOverflow.size())
	{
		BufferData = ChunkOverflow;
		ChunkOverflow.empty();
	}
}

void HTTP::ReadComplete()
{
	auto it = HeaderEntriesMap.find("Content-Length");

	if (it == HeaderEntriesMap.end())
		throw HTTP_Exception(HTTP_Exception::HTTP_CONTENT_LENGTH_MISSING);

	uint64_t content_len = Parsing::StringToUInt64(it->second);

	while (BufferData.size() < content_len)
	{
		int len = GetData(BufferReading, 5000);

		if (len == SOCKET_ERROR)
			throw HTTP_Exception(HTTP_Exception::HTTP_RECV_FAILED);

		char *ptr = &BufferReading[0];

		for (size_t i = 0; i < len; i++, ptr++)
			BufferData.push_back(*ptr);
	}
}

void HTTP::Close()
{
	closesocket(Sock);
}

std::string HTTP::GetCookie()
{
	auto it = HeaderEntriesMap.find("Set-Cookie");
	
	if (it == HeaderEntriesMap.end())
		return "";

	return it->second;
}
