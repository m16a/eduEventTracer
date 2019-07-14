//
// Created by m16a on 24.02.19.
//
#pragma once

#include "ThreadsPanel.h"
#include "event_collector.h"

class App : public IEventCollectorListener {
 public:
  void Update();

  SThreadsPanel m_threadsPanel;

  void OnRecivedData(CEventCollector& eventCollector) override;
};
