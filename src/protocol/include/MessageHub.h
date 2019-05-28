#pragma once

#include <memory>
#include <vector>
#include "assert.h"

struct MessageContainerBase {
  virtual void SendOverNetwork() = 0;
};

// TODO: add perthread storing
template <typename T>
struct MessageContainer : public MessageContainerBase {
  void SendOverNetwork() override {}
  std::vector<T> messages;
};

class MessageHub {
 public:
  // TODO:ensure destructor call
  ~MessageHub() {}

  template <typename T>
  void RegisterMessage();

  void SendOverNetwork() {
    for (auto& c : m_messageContainers) c->SendOverNetwork();
  }

  template <typename T>
  T& Get();

 private:
  std::vector<std::unique_ptr<MessageContainerBase>> m_messageContainers;
};

int GenerateId();

template <typename T>
int GetTypeId() {
  static int id = GenerateId();
  return id;
}

template <typename T>
void MessageHub::RegisterMessage() {
  int id = GetTypeId<T>();
  assert(id == m_messageContainers.size());
  m_messageContainers.push_back(std::make_unique<MessageContainer<T>>());
}

template <typename T>
T& MessageHub::Get() {
  int id = GetTypeId<T>();
  assert(id < m_messageContainers.size());
  MessageContainerBase* p = m_messageContainers[id].get();

  return *(static_cast<T*>(p));
}

MessageHub& GetMessageHub();
