
// Copyright 2010 Maxim Sokhatsky <maxim.sokhatsky@gmail.com>
// All rights reserved. Distributed under the terms of the MIT Licence

// SSL Adapter knows how to start SSL session over UNIX socket,
// Send and Receive over SSL.

#include "Socket.h"
#include "SecureSocket.h"

#include <sys/socket.h>
#include <openssl/ssl.h>
#include <string.h>
#include <Locker.h>

// Here some routines for making SSLAdapter and his SSL context
// thread-safe. We tell to SSL that it is free to use 
// our synchronization routines (BLocker). Thus, whenever be called
// SSL context always will know about thread context.

#define DEBUG

static BLocker** lock_cs = NULL;

extern "C"
void
CryptoLockingCallback(int mode, int type, const char *file, int line)
{
	if (mode & CRYPTO_LOCK) lock_cs[type]->Lock();
	else lock_cs[type]->Unlock();
}

extern "C"
unsigned long
CryptoCurrentThreadCallback(void)
{
	return (unsigned long)find_thread(NULL);
}

bool
SSLAdapter::InitializeSSL()
{
	if (!InitializeSSLThread() || !SSL_library_init())
	{
		fprintf(stderr, "SSL initialization failed.\n");
		return false;
	}
	
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();
	
	fprintf(stderr, "Initialize SSL.\n");

	return true;
}

bool
SSLAdapter::InitializeSSLThread()
{
	int i;

	lock_cs = (BLocker**)OPENSSL_malloc(CRYPTO_num_locks() * sizeof(BLocker*));
	if (!lock_cs)
		return 0;
	for (i=0; i<CRYPTO_num_locks(); i++)
		lock_cs[i] = new BLocker(CRYPTO_get_lock_name(i));

	CRYPTO_set_id_callback((unsigned long (*)())CryptoCurrentThreadCallback);
	CRYPTO_set_locking_callback(CryptoLockingCallback);
	return 1;
}

bool
SSLAdapter::CleanupSSL()
{
	int i;

	CRYPTO_set_id_callback(NULL);
	CRYPTO_set_locking_callback(NULL);
	for (i=0; i<CRYPTO_num_locks(); i++)
		delete lock_cs[i];
	OPENSSL_free(lock_cs);
	
	fprintf(stderr, "Cleanup SSL.\n");
	
	return 1;
}

int
SSLAdapter::StartTLS()
{
	state = (SocketState)NONE;
	
	if (ctx) {
		SSL_CTX_free(ctx);
		ctx = NULL;
	}
	
	ctx = SSL_CTX_new(SSLv3_client_method());
	
	if (ssl) {
		SSL_free(ssl);
		ssl = NULL;
	}
	
	ssl = SSL_new(ctx);
	
	SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
	SSL_set_fd(ssl, sock);
	if (SSL_connect(ssl) == 1)
	{
		state = (SocketState)CONNECTED;
		tls = true;
		return 1;
	}
		
	return 0;
}

SSLAdapter::SSLAdapter():tls(false),ssl(NULL),ctx(NULL)
{
	state = (SocketState)NONE;
	Create();
	InitializeSSL();
}

void
SSLAdapter::Create()
{
	Socket::Create(AF_INET, SOCK_STREAM, 0);
}

SSLAdapter::~SSLAdapter()
{
	Close();
	CleanupSSL();
}

void
SSLAdapter::Close()
{
	Socket::Close();
	tls = false;
	state = (SocketState)NONE;
}

int
SSLAdapter::ReceiveData(BMessage *mdata)
{
	if (!tls) return Socket::ReceiveData(mdata);
	
	int LEN = 8024;
	char data[LEN];
	
	int length = (int)SSL_read(ssl, data, LEN);
		
	if (length > 0) 
	{
		data[length] = 0;
		mdata->AddString("data", data);
		
	}
	
	mdata->AddInt32("length", length);
	return length;
}

int
SSLAdapter::Pending()
{
	return SSL_pending(ssl);
}

int
SSLAdapter::SendData(BString xml)
{
	if (!tls) return Socket::SendData(xml);
	
#ifdef DEBUG
	fprintf(stderr, "SEND SSL: %s\n", xml.String());
#endif	
	
	return SSL_write(ssl, xml.String(), xml.Length());
}

