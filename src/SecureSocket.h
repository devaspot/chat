
#ifndef SECURE_SOCKET_H
#define SECURE_SOCKET_H

#include "Socket.h"

#include <String.h>
#include <Message.h>
#include <openssl/ssl.h>

class SSLAdapter : public Socket
{
	public:
						SSLAdapter();
						~SSLAdapter();

		virtual	int 		ReceiveData(BMessage *message);
		virtual	int			SendData(BString xml);
		virtual void		Close();
			void		Create();
			int		StartTLS();
			int		Pending();
			bool		tls;
			
	protected:

			bool		InitializeSSL();
			bool		InitializeSSLThread();
			bool		CleanupSSL();
			
			SSL*		ssl;
			SSL_CTX*	ctx;
			

};

#endif

//--
