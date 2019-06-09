#pragma once

#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include "assert.h"
#include "endpoint.h"

struct MessageContainerBase {
  virtual void SendOverNetwork(CEndPoint& endpoint) = 0;
  virtual void Clear() = 0;
  virtual size_t Size() = 0;
};

template <typename T>
struct MessageContainer : public MessageContainerBase {
  using TMessages = std::vector<T>;
  using TMessagesPerThreadMap = std::map<std::thread::id, TMessages>;

  void SendOverNetwork(CEndPoint& endpoint) override {
    if (!messagesPerThread.empty()) {
      std::lock_guard<std::mutex> lock(mut);
      for (auto& messages : messagesPerThread) 
				for (auto& msg : messages)
					endpoint.PostEvent(msg);
    }
	}

  TMessages& GetTmp() { return messagesPerThread.begin()->second; }

  void Clear() override {
    std::lock_guard<std::mutex> lock(mut);
    messagesPerThread.clear();
  }

  size_t Size() override {
    size_t res = 0;

    if (!messagesPerThread.empty()) {
      std::lock_guard<std::mutex> lock(mut);
      for (auto& con : messagesPerThread) res += con.second.size() * sizeof(T);
    }

    return res;
  }

  void AddMessage(const T& message) {
    std::thread::id tid = std::this_thread::get_id();

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

 private:
  TMessagesPerThreadMap messagesPerThread;
  std::mutex mut;
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
