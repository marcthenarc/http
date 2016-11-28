#pragma once

#include "http.h"

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <string>

class HTTPS : public HTTP
{
protected:

	SSL *ssl;
	SSL_CTX *ctx;
	X509 *server_cert;
	char inbuf[4096];

	std::string SSLError;

public:

	HTTPS();

	void InitContext();
	void Handshake();
	const char * GetCipher();
	void GetCertificate();
	void ConvertBase64(const char *in, size_t in_ln, char *out, size_t out_ln);

	virtual int GetData(char* buf, int len) override;
	virtual int SendData(const char *buf, int len) override;

	// HTTPS has its own special cleanup code.
	virtual void Close() override;

	std::string GetSSLError()
	{
		return SSLError;
	}
};
