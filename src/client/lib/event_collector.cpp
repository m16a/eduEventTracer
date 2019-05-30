#include "event_collector.h"
#include <iostream>

#include <unistd.h>

CEventCollector::CEventCollector() {
  InitProtocol();
  CEndPoint::Bind(EMsgType::SampleEventInt, this,
                  &CEventCollector::OnSampleEventInt);
  CEndPoint::Bind(EMsgType::TimeInterval, this,
                  &CEventCollector::OnTimeIntervalEvent);
  CEndPoint::Bind(EMsgType::CapuredSizeFeedback, this,
                  &CEventCollector::OnCapturedSizeFeedback);
  CEndPoint::Bind(EMsgType::TracingInterval, this,
                  &CEventCollector::OnTracingIntervalEvent);
}

CEventCollector::~CEventCollector() {
  // TODO:unsubscribe from msgs
}

void CEventCollector::Connect() { GoToState(EState::Connecting); }
bool CEventCollector::IsConnected() { return m_state != EState::Disconnected; }

void CEventCollector::StartCapture() {
  PostEvent(EMsgType::StartCapture, SEmptyArg());
  GoToState(EState::Capturing);
}

void CEventCollector::StopCapture() {
  PostEvent(EMsgType::StopCapture, SEmptyArg());
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

  if (m_intervals.empty()) m_startEpoch = arg.startTime;

  arg.startTime -= m_startEpoch;
  arg.endTime -= m_startEpoch;
  m_intervals.emplace_back(arg);

  return true;
}

bool CEventCollector::OnTracingIntervalEvent(STracingInterval& arg) {
  std::cout << arg.name << " TI: " << arg.endTime << " - " << arg.startTime
            << " = " << arg.endTime - arg.startTime << std::endl;

  if (GetMessageHub().Get<STracingInterval>().messages.empty())
    m_startEpoch = arg.startTime;

  arg.startTime -= m_startEpoch;
  arg.endTime -= m_startEpoch;
  GetMessageHub().Get<STracingInterval>().messages.emplace_back(arg);

  return true;
}

const std::vector<STimeIntervalArg>& CEventCollector::GetIntervals() const {
  return m_intervals;
}

const std::vector<STracingInterval>& CEventCollector::GetIntervals2() const {
  return GetMessageHub().Get<STracingInterval>().messages;
}

bool CEventCollector::OnCapturedSizeFeedback(SCatpuredSizeFeedback& arg) {
  m_capturedSize += arg.size;
}

void CEventCollector::DebugGenerateSamples() {
  for (int i = 0; i < 1000; ++i) {
    m_intervals.emplace_back(STimeIntervalArg(i * 10, i * 10 + 5));
  }
}
