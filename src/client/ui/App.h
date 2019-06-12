//
// Created by m16a on 24.02.19.
//

#ifndef EDUEVENTTRACER_APP_H
#define EDUEVENTTRACER_APP_H

#include "TimeLine.h"
#include "event_collector.h"
#include "struct_draw.h"

class App : public IEventCollectorListener {
 public:
  void Update(CEventCollector& eventCollector);

  TimeLine m_timeLine;
  ThreadsRender m_threadsRender;

  void OnRecivedData() override;
};

#endif  // EDUEVENTTRACER_APP_H
