#include "protocol.h"

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
}
