#pragma once

#include "client.h"

struct STimeIntervalArg;

class CEventProvider : public CClient {
  enum class EState { Disconnected, Idle, Capturing };

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
  void SendCollectedData();

 private:
  std::vector<STimeIntervalArg> m_storedEvents;
  EState m_state{EState::Disconnected};
};
