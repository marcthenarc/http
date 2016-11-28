#pragma once

#include <string>
#include <vector>
#include <map>

#include <winsock.h>

class HTTP
{
protected:

	std::string Host;
	SOCKET Sock;

	static const std::string EOL;
	static std::string UserAgent;

	int64_t ExpectedLength;
	char BufferReading[5001];
	std::vector<unsigned char> BufferData;
	std::vector<unsigned char> ChunkOverflow;

	void ProcessRequest(const std::string& req);
	size_t GetHeaders(const char *buffer, int len);

	std::string HTTPStatus;
	std::map<std::string, std::string> HeaderEntriesMap;

	std::string GetTransferEncoding();
	void ReadChunk();
	void ReadComplete();

	enum TEType { te_none, te_chunked, te_complete, te_continuing };

	TEType TEValue;

	virtual int GetData(char *buffer, int len);
	virtual int SendData(const char *buffer, int len);

public:

	HTTP();
	void Connect(const std::string& host, int port);
	void Request(const std::string& req);

	static void InitWinsock();
	static void CleanupWinsock();

	inline static void SetUserAgent(const std::string& name)
	{
		UserAgent = name;
	}

	std::string GetCookie();

	bool IsChunked();
	bool IsContinuing();
	bool IsComplete();
	std::vector<unsigned char>& ReadData();

	virtual void Close();
};