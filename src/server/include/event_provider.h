#pragma once

#include "server.h"

struct STimeIntervalArg;

void MainFrame();
void ProfileEvent(const STimeIntervalArg& event);

class CEventProvider : public CServer {
  enum class EState { Listening, Idle, Capturing };

 public:
  CEventProvider();
  ~CEventProvider();

  virtual void Update() override;
  virtual bool CanPostEvents();

  bool OnStartCapture(SEmptyArg&);
  bool OnStopCapture(SEmptyArg&);

  void StoreEvent(const STimeIntervalArg& timeIntervalEvent);

 private:
  void GoToState(EState s);
  virtual void OnHostDisconnect() override;
  virtual void OnNewListener() override;
  void SendCollectedData();

 private:
  std::vector<STimeIntervalArg> m_storedEvents;
  EState m_state{EState::Listening};
};
