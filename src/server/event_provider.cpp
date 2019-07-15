#include "event_provider.h"
#include "utills.h"

#include <unistd.h>
#include <iostream>
#include <thread>

// TODO:michaelsh: change to shared ptr
std::mutex gGetEventProvideMutex;
std::unique_ptr<CEventProvider> gpEventProvider;

CEventProvider& GetEventProvider() {
  std::lock_guard<std::mutex> lock(gGetEventProvideMutex);
  if (!gpEventProvider) gpEventProvider = std::make_unique<CEventProvider>();

  return *gpEventProvider.get();
}

void Sleep500ms() {
  struct timespec t, empty;
  t.tv_sec = 0;
  t.tv_nsec = 500 * 1000000L;
  nanosleep(&t, &empty);
}

void MainFrame() {
  static bool sOnce = false;

  if (!sOnce) {
    sOnce = true;
    CEventProvider& tmp = GetEventProvider();
    std::thread t([&tmp] {
      while (true) {
        tmp.Update();
        Sleep500ms();
      }
    });
    t.detach();
  }
}

CEventProvider::CEventProvider() {
  InitProtocol();
  GetMessageHub().Bind(this, &CEventProvider::OnStartCapture);
  GetMessageHub().Bind(this, &CEventProvider::OnStopCapture);
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

  STracingLegend legend;
  legend.mapTidToName = mapTidToNames;

  PostEvent(legend);

  GetMessageHub().SendOverNetwork(*this);

  PostEvent(ServiceTransferComplete());
  std::cout << "Send completed" << std::endl;
}

bool CEventProvider::OnStopCapture(ServiceStopCapture&) {
  SendCollectedData();
  GetMessageHub().Clear();
  GoToState(EState::Idle);
  return true;
}

bool CEventProvider::CanPostEvents() { return m_state == EState::Capturing; }

STracingMainFrameGuard::STracingMainFrameGuard() {
  msg.begin = GetTimeNowMs();
  msg.tid = GetTid();
}

STracingMainFrameGuard::~STracingMainFrameGuard() {
  msg.end = GetTimeNowMs();

  ProfileEvent(msg);
}

void ProfileEvent(const STracingInterval& event) {
  GetEventProvider().StoreEvent(event);
}

void ProfileEvent(const STracingMainFrame& event) {
  GetEventProvider().StoreEvent(event);
}

void ProfileEvent(const SEvent& event) { GetEventProvider().StoreEvent(event); }

STracingIntervalGuard::STracingIntervalGuard() {
  msg.begin = GetTimeNowMs();
  msg.tid = GetTid();
}

STracingIntervalGuard::~STracingIntervalGuard() {
  msg.end = GetTimeNowMs();
  ProfileEvent(msg);
}

SThreadName::SThreadName(const char* name) {
  GetEventProvider().AddTidName(GetTid(), name);
}

void CEventProvider::AddTidName(int tid, const char* name) {
  std::lock_guard<std::mutex> lock(m_tidToNamesMutex);
  mapTidToNames.insert(std::make_pair(GetTid(), std::string(name)));
}
