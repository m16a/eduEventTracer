#include "event_collector.h"

#include <unistd.h>
#include <iostream>
#include "MessageHub.h"

const char* stateNames[] = {"Disconnected", "Connecting", "Connected",
                            "Capturing",    "Transfer",   "Processing",
                            "Analyze"};
CEventCollector::CEventCollector() {
  InitProtocol();

  GetMessageHub().Bind(this, &CEventCollector::OnTimeIntervalEvent);
  GetMessageHub().Bind(this, &CEventCollector::OnCapturedSizeFeedback);
  GetMessageHub().Bind(this, &CEventCollector::OnTracingIntervalEvent);
  GetMessageHub().Bind(this, &CEventCollector::OnTracingMainFrameEvent);

  GetMessageHub().Bind(this, &CEventCollector::OnTransferComplete);
}

CEventCollector::~CEventCollector() {
  // TODO:unsubscribe from msgs
}

void CEventCollector::Connect() { GoToState(EState::Connecting); }
bool CEventCollector::IsConnected() { return m_state != EState::Disconnected; }

void CEventCollector::StartCapture() {
  PostEvent(ServiceStartCapture());
  GoToState(EState::Capturing);
}

void CEventCollector::StopCapture() {
  PostEvent(ServiceStopCapture());
  GoToState(EState::Transfer);
}

void CEventCollector::Update() {
  if (m_state != EState::Disconnected && m_state != EState::Connecting)
    CClient::Update();

  switch (m_state) {
    case EState::Disconnected:
      break;
    case EState::Connecting:
      if (ConnectSync("127.0.0.1", 60000)) GoToState(EState::Connected);
      break;
    case EState::Connected:
      break;
    case EState::Capturing:
      break;
  }
}

void CEventCollector::GoToState(EState s) {
  m_state = s;
  std::cout << "Entering state: " << stateNames[static_cast<int>(s)]
            << std::endl;
}

void CEventCollector::OnListenerDisonnected() {
  GoToState(EState::Disconnected);
}

bool CEventCollector::OnTransferComplete(ServiceTransferComplete& arg) {
  GoToState(EState::Processing);
  std::cout << "Transfer complete\n";
  ProcessIncommingData();
  GoToState(EState::Analyze);
}

bool CEventCollector::OnTimeIntervalEvent(STimeIntervalArg& arg) {
  std::cout << "TI: " << arg.endTime << " - " << arg.startTime << " = "
            << arg.endTime - arg.startTime << std::endl;

  m_intervals.emplace_back(arg);

  return true;
}

bool CEventCollector::OnTracingIntervalEvent(STracingInterval& arg) {
  std::cout << arg.name << " TI: " << arg.end << " - " << arg.begin << " = "
            << arg.end - arg.begin << std::endl;
  GetMessageHub().Get<STracingInterval>().AddMessage(arg, arg.tid);

  return true;
}

void CEventCollector::ProcessIncommingData() {
  if (m_delegate) m_delegate->OnRecivedData();
}

bool CEventCollector::OnTracingMainFrameEvent(STracingMainFrame& arg) {
  GetMessageHub().Get<STracingMainFrame>().AddMessage(arg, arg.tid);

  return true;
}

const std::vector<STimeIntervalArg>& CEventCollector::GetIntervals() const {
  return m_intervals;
}

const std::vector<STracingInterval>& CEventCollector::GetIntervals2() const {
  return GetMessageHub().Get<STracingInterval>().GetTmp();
}

bool CEventCollector::OnCapturedSizeFeedback(SCatpuredSizeFeedback& arg) {
  m_capturedSize += arg.size;
}

void CEventCollector::DebugGenerateSamples() {
  for (int i = 0; i < 1000; ++i) {
    m_intervals.emplace_back(STimeIntervalArg(i * 10, i * 10 + 5));
  }
}
