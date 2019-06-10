#pragma once

#include "MessageHub.h"
#include "server.h"
#include "timer.h"

void MainFrame();
void ProfileEvent(const STimeIntervalArg& event);
void ProfileEvent(const STracingInterval& event);
void ProfileEvent(const STracingMainFrame& event);

class CEventProvider : public CServer {
  enum class EState { Listening, Idle, Capturing };

 public:
  CEventProvider();
  ~CEventProvider();

  virtual void Update() override;
  virtual bool CanPostEvents();

  bool OnStartCapture(ServiceStartCapture&);
  bool OnStopCapture(ServiceStopCapture&);

  template <typename T>
  void StoreEvent(const T& event) {
    if (m_state == EState::Capturing)
      GetMessageHub().Get<T>().AddMessage(event);
  }

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

CEventProvider& GetEventProvider();

struct STracingMainFrameGuard {
  STracingMainFrameGuard();
  ~STracingMainFrameGuard();

  STracingMainFrame msg;
};

struct STracingIntervalGuard {
  STracingIntervalGuard();
  ~STracingIntervalGuard();

  STracingInterval msg;
};

#define TRACE(module, category) STracingIntervalGuard tmp
#define TRACE_MAIN_FRAME() STracingMainFrameGuard tmp
#define TRACE_THREAD(name)
