#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include "assert.h"
#include "endpoint.h"

#include <functional>
#include "defines.h"
#include "protocol.h"
#include "utills.h"

#include <iostream>

struct ITimedEvent;

struct MessageContainerBase {
  virtual void SendOverNetwork(CEndPoint& endpoint) = 0;
  virtual void Clear() = 0;
  virtual size_t SizeInBytes() = 0;
  virtual void GetAllNodes(std::vector<ITimedEvent*>& outNodes) = 0;
};

template <typename T>
struct MessageContainer : public MessageContainerBase {
  using TMessages = std::vector<T>;
  using TMessagesPerThreadMap = std::map<int, TMessages>;

  void SendOverNetwork(CEndPoint& endpoint) override {
    if (!messagesPerThread.empty()) {
      std::lock_guard<std::mutex> lock(mut);
      for (auto& messages : messagesPerThread)
        for (auto& msg : messages.second) endpoint.PostEvent(msg);
    }
  }

  TMessages& GetTmp() { return messagesPerThread.begin()->second; }

  void Clear() override {
    std::lock_guard<std::mutex> lock(mut);
    messagesPerThread.clear();
  }

  size_t SizeInBytes() override {
    size_t res = 0;

    if (!messagesPerThread.empty()) {
      std::lock_guard<std::mutex> lock(mut);
      for (auto& con : messagesPerThread) res += con.second.size() * sizeof(T);
    }

    return res;
  }

  void AddMessage(const T& message, int tid = -1) {
    if (tid == -1) tid = GetTid();

    typename TMessagesPerThreadMap::iterator it;
    {
      std::lock_guard<std::mutex> lock(mut);
      it = messagesPerThread.find(tid);

      if (it == messagesPerThread.end())
        it = messagesPerThread.insert(std::make_pair(tid, std::vector<T>()))
                 .first;
    }

    it->second.emplace_back(message);
  }

  void GetAllNodes(std::vector<ITimedEvent*>& outNodes) override {
    for (auto& con : messagesPerThread)
      for (auto& msg : con.second) outNodes.push_back(&msg);
  }

 private:
  TMessagesPerThreadMap messagesPerThread;
  std::mutex mut;
};

const constexpr int MaxMessageTypeCount = 100;
class CDispatcher {
  using THndlr = std::function<bool(std::istream&)>;

 public:
  template <class TCaller, class argType>
  void Bind(TCaller* ownr, bool (TCaller::*callback)(argType&)) {
    auto lambda = [ownr, callback](std::istream& strm) {
      argType strct;
      strm >> strct;

      return (ownr->*callback)(strct);
    };

    int index = GetMessageId<argType>();
    std::cout << "binded: " << index << std::endl;
    m_hndlrs[index] = lambda;
  }

  void OnMsg(int index, std::istream& strm) { m_hndlrs[index](strm); }

 private:
  std::array<THndlr, MaxMessageTypeCount> m_hndlrs;
};

class MessageHub {
 public:
  // TODO:ensure destructor call
  ~MessageHub() {}

  template <typename T>
  void RegisterMessage();

  void SendOverNetwork(CEndPoint& endpoint) {
    for (auto& c : m_messageContainers) c->SendOverNetwork(endpoint);
  }

  template <typename T>
  MessageContainer<T>& Get();

  void Clear();
  size_t Size();

  template <class TCaller, class argType>
  void Bind(TCaller* ownr, bool (TCaller::*callback)(argType&)) {
    m_dispatcher.Bind(ownr, callback);
  }

  CDispatcher m_dispatcher;

  void GetAllNodes(std::vector<ITimedEvent*>& outNodes) {
    for (auto& c : m_messageContainers) {
      std::vector<ITimedEvent*> nodes;
      c->GetAllNodes(nodes);
      outNodes.insert(outNodes.end(), nodes.begin(), nodes.end());
    }
  }

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
MessageContainer<T>& MessageHub::Get() {
  int id = GetTypeId<T>();
  assert(id < m_messageContainers.size());
  MessageContainerBase* p = m_messageContainers[id].get();

  return *(static_cast<MessageContainer<T>*>(p));
}

MessageHub& GetMessageHub();
