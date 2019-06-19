#pragma once

#include <limits>
#include "client.h"

#include <map>
#include <mutex>
#include <string>

class CEventCollector;

struct IEventCollectorListener {
  virtual void OnRecivedData(CEventCollector& eventCollector) = 0;
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
  bool OnCapturedSizeFeedback(SCatpuredSizeFeedback& arg);
  bool OnTracingIntervalEvent(STracingInterval& arg);
  bool OnTracingMainFrameEvent(STracingMainFrame& arg);
  bool OnTransferComplete(ServiceTransferComplete& arg);
  bool OnTracingLegend(STracingLegend& arg);

  size_t GetCapturedSize() const { return m_capturedSize; }

  void ProcessIncommingData();

  IEventCollectorListener* m_delegate;

  std::map<int, std::string> mapTidToName;

 private:
  void GoToState(EState s);

 private:
  EState m_state{EState::Disconnected};

  size_t m_capturedSize{0};
};
