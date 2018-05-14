#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <sys/socket.h>

#define PORT 8888

CLinuxSocket::CLinuxSocket()
{
	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (m_sock < 0)
	{
		printf("\n Socket creation error \n");
		exit(1);
	}

	m_address.sin_family = AF_INET;
	m_address.sin_port = htons(PORT);

}

bool CLinuxSocket::ConnectSync(const char* ip, int port)
{
	if (!ip)
	{
		printf("\n Provide IP address to connect \n");
		exit(1);
	}


	if (inet_pton(AF_INET, ip, &m_address.sin_addr) <= 0)
	{
		printf("\n Invalid address: %s \n", ip);
		exit(1);
	}

	if (!m_sock)
	{
		printf("\n Socket is not created \n");
		exit(1);
	}


	return connect(m_sock, (struct sockaddr*)&m_address, sizeof(m_address)) >= 0;
}


void CLinuxSocket::Listen()
{
	if (!m_sock)
	{
		printf("\n Socket is not created \n");
		exit(1);
	}

	//TODO: what it is???
	m_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_sock, (struct sockaddr*)&m_address, sizeof(m_address)) < 0)
	{
		printf("\n Bind failed \n");
		exit(1);
	}

	if (listen(m_sock, 1) < 0)
	{
		printf("\n Listen failed \n");
		exit(1);
	}

	
}
