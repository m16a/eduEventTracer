#include "MessageHub.h"

int GenerateId() {
  static int id = 0;
  return id++;
}

MessageHub& GetMessageHub() {
  static MessageHub sHub;
  return sHub;
}

void MessageHub::Clear() {
  for (auto& c : m_messageContainers) c->Clear();
}

size_t MessageHub::Size() {
  size_t total = 0;
  for (auto& c : m_messageContainers) total += c->Size();
}
