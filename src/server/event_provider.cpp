#include "event_provider.h"
#include <unistd.h>
#include <iostream>
#include <thread>

std::unique_ptr<CEventProvider> gpEventProvider;

void Sleep500ms() {
  struct timespec t, empty;
  t.tv_sec = 0;
  t.tv_nsec = 500 * 1000000L;
  nanosleep(&t, &empty);
}

void MainFrame() {
  if (!gpEventProvider) {
    gpEventProvider = std::make_unique<CEventProvider>();

    std::thread t([&gpEventProvider] {
      while (true) {
        gpEventProvider->Update();
        Sleep500ms();
      }
    });
    t.detach();
  }
}

void ProfileEvent(const STimeIntervalArg& event) {
  if (gpEventProvider) {
    gpEventProvider->StoreEvent(event);
  }
}

CEventProvider::CEventProvider() {
  CEndPoint::Bind(EMsgType::StartCapture, this,
                  &CEventProvider::OnStartCapture);
  CEndPoint::Bind(EMsgType::StopCapture, this, &CEventProvider::OnStopCapture);
}

CEventProvider::~CEventProvider() {}

void CEventProvider::Update() {
  switch (m_state) {
    case EState::Listening:
      Listen();
      break;
    case EState::Idle:
      break;
    case EState::Capturing:
      break;
  }

  CServer::Update();
}

void CEventProvider::GoToState(EState s) {
  m_state = s;
  std::cout << "Entering state: " << static_cast<int>(s) << std::endl;
}

void CEventProvider::OnNewListener() { GoToState(EState::Idle); }

void CEventProvider::OnHostDisconnect() { GoToState(EState::Listening); }

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
  if (m_state == EState::Capturing) m_storedEvents.push_back(timeIntervalEvent);
}
