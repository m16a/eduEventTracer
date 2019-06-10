#include "event_collector.h"

#include <unistd.h>
#include <iostream>
#include "MessageHub.h"

CEventCollector::CEventCollector() {
  InitProtocol();

  GetMessageHub().Bind(this, &CEventCollector::OnSampleEventInt);
  GetMessageHub().Bind(this, &CEventCollector::OnTimeIntervalEvent);
  GetMessageHub().Bind(this, &CEventCollector::OnCapturedSizeFeedback);
  GetMessageHub().Bind(this, &CEventCollector::OnTracingIntervalEvent);
  GetMessageHub().Bind(this, &CEventCollector::OnTracingMainFrameEvent);
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
  GoToState(EState::Connected);
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
  std::cout << "Entering state: " << static_cast<int>(s) << std::endl;
}

void CEventCollector::OnListenerDisonnected() {
  GoToState(EState::Disconnected);
}

bool CEventCollector::OnSampleEventInt(SSampleIntArg& arg) {
  std::cout << "Recived sample: " << arg.val << std::endl;
  return true;
}

bool CEventCollector::OnTimeIntervalEvent(STimeIntervalArg& arg) {
  std::cout << "TI: " << arg.endTime << " - " << arg.startTime << " = "
            << arg.endTime - arg.startTime << std::endl;

  if (arg.startTime < m_startEpoch) m_startEpoch = arg.startTime;

  arg.startTime -= m_startEpoch;
  arg.endTime -= m_startEpoch;
  m_intervals.emplace_back(arg);

  return true;
}

bool CEventCollector::OnTracingIntervalEvent(STracingInterval& arg) {
  std::cout << arg.name << " TI: " << arg.endTime << " - " << arg.startTime
            << " = " << arg.endTime - arg.startTime << std::endl;

  if (arg.startTime < m_startEpoch) m_startEpoch = arg.startTime;

  arg.startTime -= m_startEpoch;
  arg.endTime -= m_startEpoch;
  GetMessageHub().Get<STracingInterval>().AddMessage(arg, arg.tid);

  return true;
}

bool CEventCollector::OnTracingMainFrameEvent(STracingMainFrame& arg) {
  if (arg.startTime < m_startEpoch) m_startEpoch = arg.startTime;

  arg.startTime -= m_startEpoch;
  arg.endTime -= m_startEpoch;
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
