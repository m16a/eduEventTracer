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

void ProfileEvent(const STracingInterval& event) {
  if (gpEventProvider) {
    gpEventProvider->StoreEvent(event);
  }
}

CEventProvider::CEventProvider() {
  InitProtocol();
  CEndPoint::Bind(this, &CEventProvider::OnStartCapture);
  CEndPoint::Bind(this, &CEventProvider::OnStopCapture);
}

CEventProvider::~CEventProvider() {}

void CEventProvider::Update() {
  switch (m_state) {
    case EState::Listening:
      Listen();
      break;
    case EState::Idle:
      break;
    case EState::Capturing: {
      if (m_FeedbakTimer.elapsedSeconds() > m_kCaptureSizeFeedbackPeriodSec) {
        size_t capturedBytes = GetMessageHub().Size();

        SCatpuredSizeFeedback e;
        e.size = capturedBytes;
        PostEvent(e);
      }

      m_FeedbakTimer.start();  // requeue timer
    } break;
  }

  CServer::Update();
}

void CEventProvider::GoToState(EState s) {
  OnStateLeaved(m_state);
  m_state = s;
  OnStateEntered(m_state);
  std::cout << "Entering state: " << static_cast<int>(s) << std::endl;
}

void CEventProvider::OnStateLeaved(EState s) {
  if (s == EState::Capturing) m_FeedbakTimer.stop();
}

void CEventProvider::OnStateEntered(EState s) {
  if (s == EState::Capturing) m_FeedbakTimer.start();
}

void CEventProvider::OnNewListener() { GoToState(EState::Idle); }

void CEventProvider::OnHostDisconnect() { GoToState(EState::Listening); }

// callbacks
bool CEventProvider::OnStartCapture(ServiceStartCapture&) {
  GoToState(EState::Capturing);
  return true;
}

void CEventProvider::SendCollectedData() {
  std::cout << "Send starting..." << std::endl;

  // assert(0 && "TODO: implement sending");
  // TODO:michaelsh: implement all sending
  //for (auto& e : GetMessageHub().Get<STracingInterval>().GetTmp()) PostEvent(e);
	GetMessageHub().SendOverNetwork(*this);

  std::cout << "Send completed" << std::endl;
}

bool CEventProvider::OnStopCapture(ServiceStopCapture&) {
  SendCollectedData();
  GetMessageHub().Clear();
  GoToState(EState::Idle);
  return true;
}

bool CEventProvider::CanPostEvents() { return m_state == EState::Capturing; }

void CEventProvider::StoreEvent(const STimeIntervalArg& timeIntervalEvent) {
  if (m_state == EState::Capturing)
    GetMessageHub().Get<STimeIntervalArg>().AddMessage(timeIntervalEvent);
}

void CEventProvider::StoreEvent(const STracingInterval& event) {
  if (m_state == EState::Capturing)
    GetMessageHub().Get<STracingInterval>().AddMessage(event);
}
