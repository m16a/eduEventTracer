#pragma once

#include "client.h"

class CEventCollector : public CClient {
  enum class EState { Disconnected, Connecting, Connected, Capturing };

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
  bool OnSampleEventInt(SSampleIntArg& arg);
  bool OnTimeIntervalEvent(STimeIntervalArg& arg);
  bool OnCapturedSizeFeedback(SCatpuredSizeFeedbakc& arg);

  const std::vector<STimeIntervalArg>& GetIntervals() const;

  void DebugGenerateSamples();

  size_t GetCapturedSize() const { return m_capturedSize; }

 private:
  void GoToState(EState s);

 private:
  EState m_state{EState::Disconnected};
  std::vector<STimeIntervalArg> m_intervals;
  int m_startEpoch{0};  // TODO:rework on receiving all events as batch
  size_t m_capturedSize{0};
};
