#pragma once

#include "server.h"
#include "timer.h"

void MainFrame();
void ProfileEvent(const STimeIntervalArg& event);
void ProfileEvent(const STracingInterval& event);

class CEventProvider : public CServer {
  enum class EState { Listening, Idle, Capturing };

 public:
  CEventProvider();
  ~CEventProvider();

  virtual void Update() override;
  virtual bool CanPostEvents();

  bool OnStartCapture(ServiceStartCapture&);
  bool OnStopCapture(ServiceStopCapture&);

  void StoreEvent(const STimeIntervalArg& timeIntervalEvent);
  void StoreEvent(const STracingInterval& timeIntervalEvent);

 private:
  void GoToState(EState s);
  void OnStateEntered(EState s);
  void OnStateLeaved(EState s);

  virtual void OnHostDisconnect() override;
  virtual void OnNewListener() override;
  void SendCollectedData();

 private:
  std::vector<STimeIntervalArg> m_storedEvents;
  std::vector<STracingInterval> m_storedEvents2;

  Timer m_FeedbakTimer;
  const int m_kCaptureSizeFeedbackPeriodSec{1};

  EState m_state{EState::Listening};
};

#define TRACE(module, category)
#define TRACE_MAIN_FRAME()
#define TRACE_THREAD(name)
