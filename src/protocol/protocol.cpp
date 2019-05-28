#include "protocol.h"

bool Serialize(Ser& ser) { return true; }

void InitProtocol() {
  GetMessageHub().RegisterMessage<STracingInterval>();
  GetMessageHub().RegisterMessage<STracingMainFrame>();
}
