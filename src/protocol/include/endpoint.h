#pragma once

#include <iostream>
#include <memory>
#include <type_traits>
#include "buffer.h"
#include "dispatcher.h"
#include "protocol.h"
#include "socket.h"

class CEndPoint : public ISocketListener {
 public:
  CEndPoint(int port);
  ~CEndPoint();

  template <class TArg>
  void PostEvent(TArg&& data) {
    if (!CanPostEvents()) return;

    Ser ser;
    ser.isReading = false;
    data.Serialize(ser);
    int id = GetMessageId<typename std::remove_reference<TArg>::type>();
    ser.buffer.insert(ser.buffer.begin(),
                      static_cast<int>(id));  // TODO:avoid mem shifting

    std::cout << "posting msg:" << static_cast<int>(id) << std::endl;

    m_pSock->Send(ser.buffer);
  }

  template <class TCaller, class argType>
  void Bind(TCaller* ownr, bool (TCaller::*callback)(argType&)) {
    m_dispatcher.Bind(ownr, callback);
  }

  bool ConnectSync(const char* ip, int port);

  // TODO:set port
  void Listen();
  virtual void Update();

  virtual bool CanPostEvents() { return true; };

 protected:
  virtual void OnMsg(TBuff& buff) override;
  virtual void OnNewListener() override{};
  virtual void OnHostDisconnect() override{};
  virtual void OnListenerDisonnected() override{};

 protected:
  std::unique_ptr<ISocket> m_pSock;
  CDispatcher m_dispatcher;
};
