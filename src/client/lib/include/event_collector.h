#pragma once

#include <limits>
#include "client.h"

struct IEventCollectorListener {
  virtual void OnRecivedData() = 0;
};

class CEventCollector : public CClient {
  enum class EState {
    Disconnected,
    Connecting,
    Connected,
    Capturing,
    Transfer,
    Processing,
    Analyze
  };

 public:
  CEventCollector();
  ~CEventCollector();

  void Connect();
  bool IsConnected();
  void StartCapture();
  void StopCapture();
  virtual void Update() override;

  // obtain data for drawing
  void* GetData() const {};

  virtual void OnListenerDisonnected() override;

  // callbacks
  bool OnTimeIntervalEvent(STimeIntervalArg& arg);
  bool OnCapturedSizeFeedback(SCatpuredSizeFeedback& arg);
  bool OnTracingIntervalEvent(STracingInterval& arg);
  bool OnTracingMainFrameEvent(STracingMainFrame& arg);
  bool OnTransferComplete(ServiceTransferComplete& arg);

  const std::vector<STimeIntervalArg>& GetIntervals() const;
  const std::vector<STracingInterval>& GetIntervals2() const;

  void DebugGenerateSamples();

  size_t GetCapturedSize() const { return m_capturedSize; }

  void ProcessIncommingData();

  IEventCollectorListener* m_delegate;

 private:
  void GoToState(EState s);

 private:
  EState m_state{EState::Disconnected};
  std::vector<STimeIntervalArg> m_intervals;
  std::vector<STracingInterval> m_intervals2;
  int m_startEpoch{std::numeric_limits<int>::max()};  // TODO:rework on
                                                      // receiving all events as
                                                      // batch
  size_t m_capturedSize{0};
};
