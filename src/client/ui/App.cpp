
#include "App.h"
#include "MessageHub.h"
void App::Update(CEventCollector& eventCollector) {
  m_timeLine.Render(eventCollector, m_threadsRender);
}

void App::OnRecivedData() {
  m_threadsRender.InitFromMessageHub(GetMessageHub());
}
