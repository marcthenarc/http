#include "https.h"
#include "sslexcept.h"

HTTPS::HTTPS()
	: HTTP()
{
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();
}

void HTTPS::InitContext()
{
	ctx = SSL_CTX_new(SSLv23_client_method());

	if (!ctx)
		throw SSL_Exception(SSL_Exception::SSL_CONTEXT_NOT_INITIALISED);
}

void HTTPS::Handshake()
{
	ssl = SSL_new(ctx);			// create SSL stack end point
	SSL_set_fd(ssl, (int)Sock);	// attach SSL stack to socket
	int err = SSL_connect(ssl);		// initiate SSL handshake

	if (err <= 0)
	{
		SSLError = "SSL_Connect not succesful\n";

		switch (SSL_get_error(ssl, err))
		{
			case SSL_ERROR_NONE:
				SSLError += "SSL_ERROR_NONE";
				break;

			case SSL_ERROR_ZERO_RETURN:
				SSLError += "SSL_ERROR_ZERO_RETURN";
				break;

			case SSL_ERROR_WANT_READ:
				SSLError += "SSL_ERROR_WANT_READ";
				break;

			case SSL_ERROR_WANT_WRITE:
				SSLError += "SSL_ERROR_WANT_WRITE";
				break;

			case SSL_ERROR_WANT_CONNECT:
				SSLError += "SSL_ERROR_WANT_CONNECT";
				break;

			case SSL_ERROR_WANT_ACCEPT:
				SSLError += "SSL_ERROR_WANT_ACCEPT";
				break;

			case SSL_ERROR_WANT_X509_LOOKUP:
				SSLError += "SSL_ERROR_WANT_X509_LOOKUP";
				break;

			case SSL_ERROR_SYSCALL:
				SSLError += "SSL_ERROR_SYSCALL";
				break;

			case SSL_ERROR_SSL:
				SSLError += "SSL_ERROR_SSL";
				break;
		}

		throw SSL_Exception(SSL_Exception::SSL_HANDSHAKE_FAILED);
	}
}

const char * HTTPS::GetCipher()
{
	return SSL_get_cipher(ssl);
}

void HTTPS::GetCertificate()
{
	server_cert = SSL_get_peer_certificate(ssl);

	if (!server_cert)
	{
		//		SSLError += "*** No valid cert";
		throw SSL_Exception(SSL_Exception::SSL_NO_VALID_CERTIFICATE);
	}

	//	SSLError += "(6) server's certificate was received:\n";

	char *str = X509_NAME_oneline(X509_get_subject_name(server_cert), 0, 0);

	if (str)
		printf("       subject: %s\n", str);

	str = X509_NAME_oneline(X509_get_issuer_name(server_cert), 0, 0);

	if (str)
		printf("       issuer: %s\n", str);

	/* certificate verification would happen here */

	X509_free(server_cert);
}

int HTTPS::SendData(const char *buf, int len)
{
	int err = SSL_write(ssl, buf, len);

	if (!err)
	{
		int ge = SSL_get_error(ssl, err);

		if (ge == SSL_ERROR_NONE)
			return NULL;

		if (ge == SSL_ERROR_ZERO_RETURN)
			return NULL;

		if (ge == SSL_ERROR_SSL)
			return NULL;
	}

	return err;
}

int HTTPS::GetData(char *buf, int len)
{
	int err = SSL_read(ssl, buf, len);

	if (!err)
	{
		int ge = SSL_get_error(ssl, err);

		if (ge == SSL_ERROR_NONE)
			return NULL;

		if (ge == SSL_ERROR_ZERO_RETURN)
			return NULL;

		if (ge == SSL_ERROR_SSL)
			return NULL;
	}

	buf[err] = '\0';

	return err;
}

void HTTPS::ConvertBase64(const char *in, size_t in_ln, char *out, size_t out_ln)
{
	char *ptr_64;

	BIO *bmem, *b64;
	BUF_MEM *bptr;

	b64 = BIO_new(BIO_f_base64());
	bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, in, (int)in_ln);
	BIO_flush(b64);
	BIO_get_mem_ptr(b64, &bptr);

	ptr_64 = (char *)malloc(bptr->length);
	memcpy(ptr_64, bptr->data, bptr->length - 1);
	ptr_64[bptr->length - 1] = 0;

	BIO_free_all(b64);

	strcpy_s(out, out_ln, ptr_64);

	free(ptr_64);
}

void HTTPS::Close()
{
	shutdown(Sock, 1);	// Send EOF to server

	SSL_shutdown(ssl);
	closesocket(Sock);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
}
