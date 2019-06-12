#pragma once

#include "MessageHub.h"
#include "protocol.h"

#include <map>

struct RenderContext {
  int viewBeginTime;
  int viewEndTime;
  float scale;
};

enum class EEncompasRes { Yes, No, Error };
// is a encompases b
//       [     a     ]
//					[  b  ]
inline EEncompasRes IsEncompas(const STimeInterval* a, const STimeInterval* b) {
  if (a->begin <= b->begin && a->end >= b->end)
    return EEncompasRes::Yes;
  else if (a->begin > b->end || a->end < b->begin)
    return EEncompasRes::No;
  else
    return EEncompasRes::Error;
}

struct INode {
  ITimedEvent* pTimedEvent;
  std::vector<std::unique_ptr<INode>> children;

  bool AddChild(std::unique_ptr<INode> pNewChild) {
    bool bSuccess = true;
    STimeInterval* pTIntervalA =
        dynamic_cast<STimeInterval*>(pNewChild->pTimedEvent);
    if (pTIntervalA) {
      if (children.empty())
        children.emplace_back(std::move(pNewChild));
      else {
        int start = 0;
        int end = 0;
        int p = start;
        bool wasAdded = false;
        while (p < children.size()) {
          STimeInterval* pTIntervalB =
              dynamic_cast<STimeInterval*>(children[p]->pTimedEvent);

          if (pTIntervalB) {
            EEncompasRes encompasRes = IsEncompas(pTIntervalB, pTIntervalA);

            if (encompasRes == EEncompasRes::Error) {
              bSuccess = false;
              break;
            } else if (encompasRes == EEncompasRes::Yes) {
              bSuccess = children[start]->AddChild(std::move(pNewChild));
              if (bSuccess) {
                wasAdded = true;
                break;
              }
            }

            if (pTIntervalA->begin > pTIntervalB->end) {
              start++;
              end++;
            } else if (pTIntervalA->end > pTIntervalB->end) {
              end++;
            }

            p++;
          }
        }

        if (!wasAdded) {
          if (start == end) {  // insert case
            children.insert(children.begin() + start, std::move(pNewChild));
          } else {  // encompas
            for (int i = start; i < end; ++i)
              pNewChild->children.push_back(std::move(children[i]));
            children.erase(children.begin() + start, children.begin() + end);
            children.insert(children.begin() + start, std::move(pNewChild));
          }
        }
      }
    }
    return bSuccess;
  }
};

struct ThreadView {
  int name;
  int topY;
  int bottomY;

  std::unique_ptr<INode> m_pRoot{nullptr};

  void Insert(ITimedEvent* e) {
    std::unique_ptr<INode> pNode = std::make_unique<INode>();
    pNode->pTimedEvent = e;

    if (!m_pRoot)
      m_pRoot = std::move(pNode);
    else
      m_pRoot->AddChild(std::move(pNode));
  };
  void Render(RenderContext& ctx);
  void Render(INode* node, RenderContext& ctx);
};

struct ThreadsLayout {
  std::vector<ThreadView> treadViews;
};

struct ThreadsRender {
  void Render(RenderContext&) {}

  ThreadsLayout m_layout;

  std::map<int, ThreadView> m_threads;
  void InsertTimedEvent(ITimedEvent* e){};
  void InitFromMessageHub(MessageHub& messageHub) {
    std::vector<ITimedEvent*> tmpNodes;
    messageHub.GetAllNodes(tmpNodes);

    for (auto& n : tmpNodes) {
      int tid = n->tid;
      auto it = m_threads.find(tid);
      if (it == m_threads.end())
        it = m_threads.insert(std::make_pair(tid, ThreadView())).first;

      it->second.Insert(n);
    }
  }
};
