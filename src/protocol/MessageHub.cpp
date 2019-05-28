#include "MessageHub.h"

int GenerateId();
static int id = 0;
return id++;
}

MessageHub& GetMessageHub() {
  static MessageHub sHub;
  return sHub;
}
