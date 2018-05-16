#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>   //close 

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

	for (int i = 0; i < kMaxClients; i++)  
	{  
			m_client_sockets[i] = 0;  
	}  
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
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	int opt = 1;
	if( setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
				sizeof(opt)) < 0 )  
	{  
			printf("\n Setsockopt error \n");  
			exit(1);  
	}  

	//TODO: what it is???
	m_address.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_sock, (struct sockaddr*)&m_address, sizeof(m_address)) < 0)
	{
		printf("\n Bind failed \n");
		exit(1);
	}

	if (listen(m_sock, kMaxClients) < 0)
	{
		printf("\n Listen failed \n");
		exit(1);
	}
}

void CLinuxSocket::Update()
{
	int max_sd = 0;
	//clear the socket set 
	FD_ZERO(&m_readfds);  

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
					FD_SET( sd , &m_readfds);  
					
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd)  
					max_sd = sd;  
	}  

	//wait for an activity on one of the sockets , timeout is NULL , 
	//so wait indefinitely 
	int activity = select( max_sd + 1 , &m_readfds , NULL , NULL , NULL);  

	if ((activity < 0) && (errno!=EINTR))  
	{  
			printf("\n Select error \n");  
	}  
			
	//If something happened on the master socket , 
	//then its an incoming connection 
	int new_socket = 0;
  struct sockaddr_in address;  
	int addrlen = -1;
	if (FD_ISSET(m_sock, &m_readfds))  
	{  
			if ((new_socket = accept(m_sock, 
							(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
			{  
					printf("\n Accept error\n");  
					exit(1);  
			}  
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n",
					new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
			
			if (m_listener)
				m_listener->OnNewListener();

			//send new connection greeting message 
			/*
			if( send(new_socket, message, strlen(message), 0) != strlen(message) )  
			{  
					printf("Greating m");  
			}  
					
			puts("Welcome message sent successfully");  
			*/
					
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
					
			if (FD_ISSET( sd , &m_readfds))  
			{  
					//Check if it was for closing , and also read the 
					//incoming message 
					TBuff buffer;
					buffer.resize(1024); //1K
					size_t valread = 0;
					if ((valread = read( sd , buffer.data(), 1024)) == 0)  
					{  
							//Somebody disconnected , get his details and print 
							getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
							printf("Host disconnected , ip %s , port %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
									
							//Close the socket and mark as 0 in list for reuse 
							close( sd );  
							m_client_sockets[i] = 0;  
					}  
					//Echo back the message that came in 
					else
					{  
							//set the string terminating NULL byte on the end 
							//of the data read 
							buffer.resize(valread);

							if (m_listener)
								m_listener->OnMsg(buffer);
							//send(sd , buffer , strlen(buffer) , 0 );  
					}  
			}  
	}  

}
