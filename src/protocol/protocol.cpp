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

int GetTimeNowMs() {
  std::chrono::high_resolution_clock::time_point start =
      std::chrono::high_resolution_clock::now();
  int resultMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                     start.time_since_epoch())
                     .count();
  return resultMs;
}
