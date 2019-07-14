
#include "App.h"
#include "MessageHub.h"
void App::Update() { m_threadsPanel.Update(); }

void App::OnRecivedData(CEventCollector& eventCollector) {
  m_threadsPanel.Init(GetMessageHub(), eventCollector);
}
