#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>   //close 

static bool IsListeningSocket(int sock)
{
	bool res = false;
	int val;
	socklen_t len = sizeof(val);
	if (getsockopt(sock, SOL_SOCKET, SO_ACCEPTCONN, &val, &len) == -1)
	{
			printf("fd %d is not a socket\n", sock);
	}
	else if (val)
	{
			res = true;
	}

	return res;
}

CLinuxSocket::CLinuxSocket(int port)
{
	m_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (m_sock <= 0)
	{
		printf("Socket creation error \n");
		exit(1);
	}
	else
	{
		printf("Socket was created: %d\n", m_sock);
	}

  memset(&m_address, '0', sizeof(m_address));
	m_address.sin_family = AF_INET;
	m_address.sin_port = htons(port);

	for (int i = 0; i < kMaxClients; i++)  
	{  
			m_client_sockets[i] = 0;  
	}  
}

bool CLinuxSocket::ConnectSync(const char* ip, int port)
{
	if (!m_sock)
	{
		printf("\n Socket is not created \n");
		exit(1);
	}

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

	int res = connect(m_sock, (struct sockaddr*)&m_address, sizeof(m_address));
	if (res < 0)
	{
		printf("\n Socket connection err %d\n", errno);
	}
	return res >= 0; 
}


void CLinuxSocket::Listen()
{
	if (!m_sock)
	{
		printf("\n Socket is not created \n");
		exit(1);
	}
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	int opt = 1;
	if(setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)  
	{  
			printf("\n Setsockopt error \n");  
			exit(1);  
	}  

	//TODO: what is it???
	m_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_sock, (struct sockaddr*)&m_address, sizeof(m_address)) < 0)
	{
		printf("Bind err: %d \n", errno);
		exit(1);
	}

	if (listen(m_sock, kMaxClients) < 0)
	{
		printf("\n Listen failed \n");
		exit(1);
	}
	printf("Start listening\n");
}

void CLinuxSocket::Update()
{
	if (IsListeningSocket(m_sock))
	{
		UpdateServer();
	}
	else
	{
		UpdateClient();
	}

}


void CLinuxSocket::Send(TBuff buff)
{
	//TODO:avoid copy
	m_outMsgs.push_back(buff);
}

//poll socket
void CLinuxSocket::UpdateClient()
{

	if (!m_sock)
		return;

	int max_sd = 0;
	//clear the socket set 
	FD_ZERO(&m_readfds);  
	FD_ZERO(&m_writefds);  

	//add master socket to set 
	FD_SET(m_sock, &m_readfds);  
	FD_SET(m_sock, &m_writefds);  
	max_sd = m_sock;  
			
	int activity = select(max_sd+1, &m_readfds, &m_writefds, NULL , NULL);  

	if ((activity < 0) && (errno!=EINTR))  
	{  
			printf("Select error:%d\n", errno);  
	}  
			
  struct sockaddr_in address;  
	int addrlen = sizeof(address);

	TBuff buffer;
	buffer.resize(1024);
	if (FD_ISSET(m_sock, &m_readfds))  
	{  
		ssize_t valread = 0;
		if ((valread = read(m_sock, buffer.data(), buffer.size())) == 0)  
		{  
			//Somebody disconnected , get his details and print 
			getpeername(m_sock, (struct sockaddr*)&address , (socklen_t*)&addrlen);  
			printf("Host disconnected , ip: %s , port: %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
					
			//Close the socket and mark as 0 in list for reuse 
			close(m_sock);  

			if (m_listener)
				m_listener->OnHostDisconnect();
		}  
		else if (valread > 0)
		{  
			buffer.resize(valread);

			if (m_listener)
				m_listener->OnMsg(buffer);
		}  
		else
		{
			printf("read error: %d valread: %d\n", errno, valread);
		}
	}  
			
	//else its some IO operation on some other socket
	if (FD_ISSET(m_sock, &m_writefds))
	{
		while (!m_outMsgs.empty())
		{
			TBuff& msg = m_outMsgs.front();

			ssize_t size = send(m_sock, msg.data(), msg.size(), 0);

			if (size <= 0)
			{
				getpeername(m_sock, (struct sockaddr*)&address , (socklen_t*)&addrlen);  
				printf("Can't send msg to  ip: %s , port: %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
			}
			else
			{
				m_outMsgs.pop_front();
			}

		}
	}
}

//check new connections + poll socket + send if can
void CLinuxSocket::UpdateServer()
{
	int max_sd = 0;
	//clear the socket set 
	FD_ZERO(&m_readfds);  
	FD_ZERO(&m_writefds);  

	//add master socket to set 
	FD_SET(m_sock, &m_readfds);  
	max_sd = m_sock;  
			
	//add child sockets to set 
	int sd = 0;
	for (int i = 0 ; i < kMaxClients; i++)  
	{  
			//socket descriptor 
			sd = m_client_sockets[i];  
					
			//if valid socket descriptor then add to read list 
			if(sd > 0)  
			{
					FD_SET(sd, &m_readfds);  
					FD_SET(sd, &m_writefds);  
			}
					
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd)  
					max_sd = sd;  
	}  

	//wait for an activity on one of the sockets , timeout is NULL , 
	//so wait indefinitely 
	int activity = select(max_sd+1, &m_readfds, &m_writefds, NULL , NULL);  

	if ((activity < 0) && (errno!=EINTR))  
	{  
			printf("\n Select error \n");  
	}  
			
	//If something happened on the master socket , 
	//then its an incoming connection 
	int new_socket = 0;
  struct sockaddr_in address;  
	int addrlen = sizeof(address);
	if (FD_ISSET(m_sock, &m_readfds))  
	{  
			if ((new_socket = accept(m_sock, 
							(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
			{  
					printf("Accept error:%d\n", errno);  
					//exit(1);  
			}  
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
					new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
			
			if (m_listener)
				m_listener->OnNewListener();
					
			//add new socket to array of sockets 
			for (int i = 0; i < kMaxClients; i++)  
			{  
					//if position is empty 
					if( m_client_sockets[i] == 0 )  
					{  
							m_client_sockets[i] = new_socket;  
							printf("Adding to list of sockets as %d\n" , i);  
									
							break;  
					}  
			}  
	}  
			
	//else its some IO operation on some other socket
	for (int i = 0; i < kMaxClients; i++)  
	{  
			sd = m_client_sockets[i];  
					
			if (FD_ISSET(sd ,&m_readfds))  
			{  
					//Check if it was for closing , and also read the 
					//incoming message 
					printf("somthing to read\n");
					TBuff buffer;
					buffer.resize(1024); //1K
					size_t valread = 0;
					if ((valread = read(sd , buffer.data(), 1024)) == 0)  
					{  
							//Somebody disconnected , get his details and print 
							getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
							printf("Host disconnected , ip: %s , port: %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
									
							//Close the socket and mark as 0 in list for reuse 
							close( sd );  
							m_client_sockets[i] = 0;  
							m_listener->OnListenerDisonnected();
							continue;

					}  
					//Echo back the message that came in 
					else
					{  
							//set the string terminating NULL byte on the end 
							//of the data read 
							buffer.resize(valread);

							printf("somthing recived %\n", valread);

							if (m_listener)
								m_listener->OnMsg(buffer);

							//send(sd , buffer , strlen(buffer) , 0 );  
					}  
			}  

			if (FD_ISSET(sd, &m_writefds))
			{
				while (!m_outMsgs.empty())
				{
					TBuff& msg = m_outMsgs.front();

					ssize_t size = send(sd, msg.data(), msg.size(), 0);

					if (size <= 0)
					{
						getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
						printf("Can't send msg to  ip: %s , port: %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
					}
					else
					{
						m_outMsgs.pop_front();
					}

				}
			}
	}  
}
