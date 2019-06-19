#include "endpoint.h"
#include <istream>
#include "MessageHub.h"

CEndPoint::CEndPoint(int port) {
  m_pSock = std::make_unique<CLinuxSocket>(port);
  m_pSock->SetListener(this);
}

CEndPoint::~CEndPoint() { m_pSock->RemoveListener(); }

bool CEndPoint::ConnectSync(const char* ip, int port) {
  m_pSock->ConnectSync(ip, port);
}

void CEndPoint::Listen() { m_pSock->Listen(); }

void CEndPoint::Update() { m_pSock->Update(); }

void CEndPoint::OnMsg(std::istream& strm) {
  // TODO:michaelsh: check stream integrity
  /*
if (0 && buff.empty()) {
std::cout << "received empty msg" << std::endl;
return;
}
  */

  int msgId = -1;
  strm >> msgId;
  std::cout << "recived msg: " << msgId << std::endl;

  GetMessageHub().m_dispatcher.OnMsg(msgId, strm);
}
