#pragma once

#include <netinet/in.h>
#include <sys/select.h>
#include <istream>
#include <list>
#include "defines.h"

#define kMaxClients 1

struct ISocketListener {
  virtual void OnMsg(std::istream& strm) = 0;

  // server specific
  virtual void OnNewListener() = 0;
  virtual void OnListenerDisonnected() = 0;

  // client specific
  virtual void OnHostDisconnect() = 0;
};

class ISocket {
 public:
  virtual bool ConnectSync(const char* ip, int port) = 0;
  virtual void Listen() = 0;
  virtual void Update() = 0;
  virtual void Send(const char* buff, size_t len) = 0;

  virtual void SetListener(ISocketListener* p) { m_listener = p; };
  virtual void RemoveListener() { m_listener = nullptr; };

 protected:
  ISocketListener* m_listener{nullptr};
};

// TODO:  move to separate file
class CLinuxSocket : public ISocket {
 public:
  CLinuxSocket(int port);
  virtual bool ConnectSync(const char* ip, int port) override;
  virtual void Listen() override;
  virtual void Update() override;
  virtual void Send(const char* buff, size_t len) override;

 private:
  void UpdateClient();
  void UpdateServer();

 private:
  int m_sock{0};
  struct sockaddr_in m_address;
  int m_client_sockets[kMaxClients];

  fd_set m_readfds;
  fd_set m_writefds;

  std::list<TBuff> m_outMsgs;
};
