#include "event_provider.h"
#include <unistd.h>
#include <iostream>

CEventProvider::CEventProvider() {
  CEndPoint::Bind(EMsgType::StartCapture, this,
                  &CEventProvider::OnStartCapture);
  CEndPoint::Bind(EMsgType::StopCapture, this, &CEventProvider::OnStopCapture);
}

CEventProvider::~CEventProvider() {}

void CEventProvider::Update() {
  switch (m_state) {
    case EState::Disconnected:
      if (ConnectSync("127.0.0.1", 60000))
        GoToState(EState::Idle);
      else
        std::cout << "Connection failed" << std::endl;
      break;
    case EState::Idle:
      break;
    case EState::Capturing:
      break;
  }

  if (m_state != EState::Disconnected) CServer::Update();
}

void CEventProvider::GoToState(EState s) {
  m_state = s;
  std::cout << "Entering state: " << static_cast<int>(s) << std::endl;
}

void CEventProvider::OnHostDisconnect() { GoToState(EState::Disconnected); }

// callbacks
bool CEventProvider::OnStartCapture(SEmptyArg&) {
  GoToState(EState::Capturing);
  return true;
}

void CEventProvider::SendCollectedData() {
  std::cout << "Send starting..." << std::endl;

  for (auto& e : m_storedEvents) PostEvent(EMsgType::TimeInterval, e);

  std::cout << "Send completed" << std::endl;
}

bool CEventProvider::OnStopCapture(SEmptyArg&) {
  SendCollectedData();
  m_storedEvents.clear();
  GoToState(EState::Idle);
  return true;
}

bool CEventProvider::CanPostEvents() { return m_state == EState::Capturing; }

void CEventProvider::StoreEvent(const STimeIntervalArg& timeIntervalEvent) {
  m_storedEvents.push_back(timeIntervalEvent);
}
