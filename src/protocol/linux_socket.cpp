#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "socket.h"

#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include "assert.h"

static bool IsListeningSocket(int sock) {
  bool res = false;
  int val;
  socklen_t len = sizeof(val);
  if (getsockopt(sock, SOL_SOCKET, SO_ACCEPTCONN, &val, &len) == -1) {
    std::cout << "fd " << sock << " is not a socket\n";
  } else if (val) {
    res = true;
  }

  return res;
}

CLinuxSocket::CLinuxSocket(int port) {
  m_sock = socket(AF_INET, SOCK_STREAM, 0);

  if (m_sock <= 0) {
    std::cout << "Socket creation error" << std::endl;
    exit(1);
  } else {
    std::cout << "Socket was created: " << m_sock << std::endl;
  }

  memset(&m_address, '0', sizeof(m_address));
  m_address.sin_family = AF_INET;
  m_address.sin_port = htons(port);

  for (int i = 0; i < kMaxClients; i++) {
    m_client_sockets[i] = 0;
  }
}

bool CLinuxSocket::ConnectSync(const char* ip, int port) {
  if (!m_sock) {
    std::cout << "Socket is not created" << std::endl;
    exit(1);
  }

  if (!ip) {
    std::cout << "Provide IP address to connect" << std::endl;
    exit(1);
  }

  if (inet_pton(AF_INET, ip, &m_address.sin_addr) <= 0) {
    std::cout << "Invalid address: " << ip << std::endl;
    exit(1);
  }

  int res = connect(m_sock, (struct sockaddr*)&m_address, sizeof(m_address));
  if (res < 0) {
    std::cout << "Socket connection err: " << errno << std::endl;
  }
  return res >= 0;
}

void CLinuxSocket::Listen() {
  if (!m_sock) {
    std::cout << "Socket is not created" << std::endl;
    exit(1);
  }

  // set master socket to allow multiple connections ,
  // this is just a good habit, it will work without this
  int opt = 1;
  if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) <
      0) {
    std::cout << "Setsockopt error" << std::endl;
    exit(1);
  }

  // TODO: what is it???
  m_address.sin_addr.s_addr = INADDR_ANY;

  if (bind(m_sock, (struct sockaddr*)&m_address, sizeof(m_address)) < 0) {
    std::cout << "Bind err: " << errno << std::endl;
    exit(1);
  }

  if (listen(m_sock, kMaxClients) < 0) {
    std::cout << "Listen failed" << std::endl;
    exit(1);
  }
  std::cout << "Start listening" << std::endl;
}

void CLinuxSocket::Update() {
  if (IsListeningSocket(m_sock)) {
    UpdateServer();
  } else {
    UpdateClient();
  }
}

void CLinuxSocket::Send(const char* buff, size_t len) {
  if (len) {
    // TODO:avoid copy
    std::vector<char> tmp;
    tmp.reserve(len + 4);
    char* p = (char*)&len;
    for (int i = 0; i < 4; i++) {
      tmp.push_back(*(p + i));
    }
    tmp.insert(tmp.end(), buff, buff + len);

    m_outMsgs.push_back(tmp);
  }
}

// poll socket
void CLinuxSocket::UpdateClient() {
  if (!m_sock) return;

  int max_sd = 0;
  // clear the socket set
  FD_ZERO(&m_readfds);
  FD_ZERO(&m_writefds);

  // add master socket to set
  FD_SET(m_sock, &m_readfds);
  FD_SET(m_sock, &m_writefds);
  max_sd = m_sock;

  int activity = select(max_sd + 1, &m_readfds, &m_writefds, NULL, NULL);

  if ((activity < 0) && (errno != EINTR)) {
    std::cout << "Select error: " << errno << std::endl;
  }

  struct sockaddr_in address;
  int addrlen = sizeof(address);

  static std::string buffer(100 * 1024, 0);  // 100KB
  if (FD_ISSET(m_sock, &m_readfds)) {
    ssize_t valread = 0;
    if ((valread = read(m_sock, &buffer[0], buffer.length())) == 0) {
      // Somebody disconnected , get his details and print
      getpeername(m_sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
      std::cout << "Host disconnected , ip: " << inet_ntoa(address.sin_addr)
                << ", port: " << ntohs(address.sin_port) << std::endl;

      // Close the socket and mark as 0 in list for reuse
      close(m_sock);

      if (m_listener) m_listener->OnHostDisconnect();
    } else if (valread > 0) {
      buffer.resize(valread);
      m_inputBuffer.append(buffer);

      std::cout << "somthing was recived, size: " << valread << std::endl;
      Dump(buffer.c_str(), valread);

      ParseMessages(m_inputBuffer);

    } else {
      std::cout << "read error: " << errno << ", valread: " << valread
                << std::endl;
    }
  }

  // else its some IO operation on some other socket
  if (FD_ISSET(m_sock, &m_writefds)) {
    while (!m_outMsgs.empty()) {
      TBuff& msg = m_outMsgs.front();

      std::cout << "[" << m_sentMsgCnt++
                << "] msg sending, size: " << msg.size() - 4 << std::endl;
      Dump(msg.data(), msg.size());

      ssize_t size = send(m_sock, msg.data(), msg.size(), 0);

      if (size <= 0) {
        getpeername(m_sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        std::cout << "Can't send msg to ip: " << inet_ntoa(address.sin_addr)
                  << ", port: " << ntohs(address.sin_port) << std::endl;
      } else {
        m_outMsgs.pop_front();
      }
    }
  }
}

void CLinuxSocket::ParseMessages(std::string& buffer) {
  ssize_t curr = 0;
  while (curr < buffer.size()) {
    int msgSize = 0;

    if (curr + 4 > buffer.size()) break;

    char* p = (char*)&msgSize;
    const char* pBegin = buffer.c_str() + curr;
    for (int i = 0; i < 4; i++) {
      *(p + i) = buffer[curr + i];
    }
    curr += 4;

    if (curr + msgSize > buffer.size()) {
      curr -= 4;
      break;
    }

    std::string sB(buffer, curr, msgSize);
    std::stringbuf strBuff(sB);
    std::istream in(&strBuff);

    std::cout << "[" << m_recivedMsgCnt++ << "] read msg, size: " << msgSize
              << std::endl;
    if (m_listener) m_listener->OnMsg(in);

    Dump(pBegin, msgSize + 4);
    curr += msgSize;
  }

  buffer.erase(buffer.begin(), buffer.begin() + curr);
}
// check new connections + poll socket + send if can
void CLinuxSocket::UpdateServer() {
  int max_sd = 0;
  // clear the socket set
  FD_ZERO(&m_readfds);
  FD_ZERO(&m_writefds);

  // add master socket to set
  FD_SET(m_sock, &m_readfds);
  max_sd = m_sock;

  // add child sockets to set
  int sd = 0;
  for (int i = 0; i < kMaxClients; i++) {
    // socket descriptor
    sd = m_client_sockets[i];

    // if valid socket descriptor then add to read list
    if (sd > 0) {
      FD_SET(sd, &m_readfds);
      FD_SET(sd, &m_writefds);
    }

    // highest file descriptor number, need it for the select function
    if (sd > max_sd) max_sd = sd;
  }

  // wait for an activity on one of the sockets , timeout is NULL ,
  // so wait indefinitely
  int activity = select(max_sd + 1, &m_readfds, &m_writefds, NULL, NULL);

  if ((activity < 0) && (errno != EINTR)) {
    std::cout << "Select error" << std::endl;
  }

  // If something happened on the master socket ,
  // then its an incoming connection
  int new_socket = 0;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  if (FD_ISSET(m_sock, &m_readfds)) {
    if ((new_socket = accept(m_sock, (struct sockaddr*)&address,
                             (socklen_t*)&addrlen)) < 0) {
      std::cout << "Accept error: " << errno << std::endl;
      // exit(1);
    }

    // inform user of socket number - used in send and receive commands
    std::cout << "New connection , socket fd is " << new_socket
              << ", ip is: " << inet_ntoa(address.sin_addr)
              << ", port:" << ntohs(address.sin_port) << std::endl;

    if (m_listener) m_listener->OnNewListener();

    // add new socket to array of sockets
    for (int i = 0; i < kMaxClients; i++) {
      // if position is empty
      if (m_client_sockets[i] == 0) {
        m_client_sockets[i] = new_socket;
        std::cout << "Adding to list of sockets as " << i << std::endl;

        break;
      }
    }
  }

  // else its some IO operation on some other socket
  for (int i = 0; i < kMaxClients; i++) {
    sd = m_client_sockets[i];

    if (FD_ISSET(sd, &m_readfds)) {
      // Check if it was for closing , and also read the
      // incoming message
      std::cout << "somthing to read" << std::endl;

      static std::string buffer(100 * 1024, 0);  // 100KB
      size_t valread = 0;
      if ((valread = read(sd, &buffer[0], buffer.size())) == 0) {
        // Somebody disconnected , get his details and print
        getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        std::cout << "Host disconnected , ip: " << inet_ntoa(address.sin_addr)
                  << ", port: " << ntohs(address.sin_port) << std::endl;

        // Close the socket and mark as 0 in list for reuse
        close(sd);
        m_client_sockets[i] = 0;
        m_listener->OnListenerDisonnected();
        continue;

      } else {
        buffer.resize(valread);
        m_inputBuffer.append(buffer);

        std::cout << "somthing was recived, size: " << valread << std::endl;
        Dump(buffer.c_str(), valread);

        ParseMessages(m_inputBuffer);
      }
    }

    if (FD_ISSET(sd, &m_writefds)) {
      while (!m_outMsgs.empty()) {
        TBuff& msg = m_outMsgs.front();

        std::cout << "[" << m_sentMsgCnt++
                  << "] msg sending, size: " << msg.size() - 4 << std::endl;
        Dump(msg.data(), msg.size());

        ssize_t size = send(sd, msg.data(), msg.size(), 0);

        if (size <= 0) {
          getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
          std::cout << "Can't send msg to ip: " << inet_ntoa(address.sin_addr)
                    << ", port: " << ntohs(address.sin_port) << std::endl;
        } else {
          m_outMsgs.pop_front();
        }
      }
    }
  }
}
