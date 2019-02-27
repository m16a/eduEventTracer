//
// Created by m16a on 24.02.19.
//

#include "App.h"

void App::Update(CEventCollector& eventCollector) {
  m_timeLine.Render(eventCollector);
}
