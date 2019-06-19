#pragma once

#include <map>
#include "MessageHub.h"
#include "server.h"
#include "timer.h"

void MainFrame();
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

  std::map<int, std::string> mapTidToNames;

  void AddTidName(int tid, const char* name);

 private:
  void GoToState(EState s);
  void OnStateEntered(EState s);
  void OnStateLeaved(EState s);

  virtual void OnHostDisconnect() override;
  virtual void OnNewListener() override;
  void SendCollectedData();

 private:
  Timer m_FeedbakTimer;
  const int m_kCaptureSizeFeedbackPeriodSec{1};

  EState m_state{EState::Listening};

  std::mutex m_tidToNamesMutex;
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

struct SThreadName {
  SThreadName(const char* name);
};

#define TRACE(module, category) STracingIntervalGuard tmp_STracingIntervalGuard
#define TRACE_MAIN_FRAME() STracingMainFrameGuard tmp_STracingMainFrameGuard
#define THREAD_BEGIN(name) static SThreadName tmp_SThreadName(name)
