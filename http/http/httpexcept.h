#pragma once

#include <string>

class HTTP_Exception
{
	int Message;
	std::string Extra;

public:

	enum messages {

		HTTP_WINSOCK_FAILED,
		HTTP_GETHOSTBYNAME_FAILED,
		HTTP_SOCKET_FAILED,
		HTTP_CONNECT_FAILED,
		HTTP_SEND_FAILED,
		HTTP_RECV_FAILED,
		HTTP_MISSING_HTTP_ENTRY,
		HTTP_NOT_200_OK,
		HTTP_TRANSFER_ENCODING_MISSING,
		HTTP_CONTENT_LENGTH_MISSING,
	};

	HTTP_Exception(int m) : Message(m)
	{

	}

	HTTP_Exception(int m, const std::string& e) : Message(m), Extra(e)
	{

	}

	int GetErrorMessage() const
	{
		return Message;
	}

	const std::string GetExtra() const
	{
		return Extra;
	}
};