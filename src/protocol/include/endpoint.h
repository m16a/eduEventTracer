#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <type_traits>
#include "buffer.h"
#include "protocol.h"
#include "socket.h"

class CEndPoint : public ISocketListener {
 public:
  CEndPoint(int port);
  ~CEndPoint();

  template <class TArg>
  void PostEvent(TArg&& data) {
    if (!CanPostEvents()) return;

    std::stringbuf buff;
    std::ostream out(&buff);
    int id = GetMessageId<typename std::remove_reference<TArg>::type>();
    out << id;
    out << data;

    std::cout << "posting msg:" << static_cast<int>(id) << std::endl;

    m_pSock->Send(buff.str().c_str(), buff.str().length());
  }

  bool ConnectSync(const char* ip, int port);

  // TODO:michaelsh:set port
  void Listen();
  virtual void Update();

  virtual bool CanPostEvents() { return true; };

 protected:
  virtual void OnMsg(std::istream& strm) override;
  virtual void OnNewListener() override{};
  virtual void OnHostDisconnect() override{};
  virtual void OnListenerDisonnected() override{};

 protected:
  std::unique_ptr<ISocket> m_pSock;
};
