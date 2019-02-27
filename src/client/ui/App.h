//
// Created by m16a on 24.02.19.
//

#ifndef EDUEVENTTRACER_APP_H
#define EDUEVENTTRACER_APP_H

#include "TimeLine.h"

class App {
 public:
  void Update(CEventCollector& eventCollector);

  TimeLine m_timeLine;
};

#endif  // EDUEVENTTRACER_APP_H
