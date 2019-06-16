#include "protocol.h"
#include "MessageHub.h"

#include <chrono>

bool Serialize(Ser& ser) { return true; }

int GenerateMessageId() {
  static int id = 0;
  return id++;
}

void InitServiceMessages() {
  RegisterServiceMessage<ServiceStartCapture>();
  RegisterServiceMessage<ServiceStopCapture>();
  RegisterServiceMessage<SCatpuredSizeFeedback>();
  RegisterServiceMessage<ServiceTransferComplete>();
  RegisterServiceMessage<STracingLegend>();
}

template <typename T>
void RegisterMessage() {
  GetMessageId<T>();
  GetMessageHub().RegisterMessage<T>();
}

void InitProtocol() {
  InitServiceMessages();

  RegisterMessage<STracingInterval>();
  RegisterMessage<STracingMainFrame>();
  RegisterMessage<STimeIntervalArg>();
}

TTime GetTimeNowMs() {
  std::chrono::high_resolution_clock::time_point start =
      std::chrono::high_resolution_clock::now();
  TTime resultMs = std::chrono::duration_cast<std::chrono::microseconds>(
                       start.time_since_epoch())
                       .count();
  return resultMs;
}

void STimeInterval::Render(RenderContext&) {}
