#pragma once

class SSL_Exception
{
	int Message;

public:

	enum messages {
		SSL_CONTEXT_NOT_INITIALISED,
		SSL_HANDSHAKE_FAILED,
		SSL_NO_VALID_CERTIFICATE
	};

	SSL_Exception(int m) : Message(m)
	{
	}

	int GetErrorMessage() const
	{
		return Message;
	}
};