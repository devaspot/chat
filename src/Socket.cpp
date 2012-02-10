#include "Socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

Socket::~Socket()
{
	Close();
}

void
Socket::Create(int domain, int sock_type, int protocol)
{
	dom = domain;
	type = sock_type;
	proto = protocol;
	sock = socket(domain, type, protocol);
	if (sock < 0)
		fprintf(stderr, "Socket creation failed.\n");
}

int
Socket::Open(BString host, int p)
{
	hostname = host;
	port = p;
	struct sockaddr_in sa;
	struct hostent *he = gethostbyname(host.String());
	if (he == NULL) return -1;
	sa.sin_len = sizeof(sa);
	sa.sin_family = dom;
	sa.sin_port = htons(port);
	bcopy((char *)he->h_addr, (char *)&sa.sin_addr.s_addr, he->h_length);
	int err = connect(sock, (struct sockaddr *)&sa, sizeof(sa));
	if (err == 0) {
		state = (SocketState)CONNECTED;
	} else {
		fprintf(stderr, "Socket can't connect to %s:%u.\n", host.String(), port);
	}
	return err;
}

int
Socket::SendData(BString xml)
{
	//fprintf(stderr, "SEND: %s\n", xml.String());
	return send(sock, xml.String(), xml.Length(), 0);
}

int
Socket::ReceiveData(BMessage *message)
{
	int LEN = 4096;
	char data[LEN];
	int length = recv(sock, data, LEN, 0);
	if (length > 0)
	{
		data[length] = 0;
		message->AddString("data", BString(data));
		message->AddInt32("length", length);
	}
	//fprintf(stderr, "RECV: %s\n", data);
	return length;
}

bool
Socket::IsConnected()
{
	return state == (SocketState)CONNECTED;
}

void
Socket::Close()
{
	state = (SocketState)NONE;
	shutdown(sock, 2);
	close(sock);
}
