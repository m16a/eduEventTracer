#pragma once

#include "MessageHub.h"
#include "protocol.h"

#include <limits>
#include <map>

class CEventCollector;

struct RenderContext {
  TTime viewBeginTime;
  TTime viewEndTime;
  float scale;
  float topY;
  int currentDepth;
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
          if (start == end) {
            if (start == children.size()) {
              children.insert(children.begin() + start, std::move(pNewChild));
            } else {
              STimeInterval* pTIntervalB =
                  dynamic_cast<STimeInterval*>(children[start]->pTimedEvent);
              if (IsEncompas(pTIntervalA, pTIntervalB) == EEncompasRes::Yes) {
                pNewChild->children.push_back(std::move(children[start]));
                children.erase(children.begin() + start);
                children.insert(children.begin() + start, std::move(pNewChild));
              } else {  // insert case
                children.insert(children.begin() + start, std::move(pNewChild));
              }
            }
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

  int GetDepth() { return GetDepthInternal(this); }

  static int GetDepthInternal(const INode* node) {
    if (node->children.empty()) return 0;

    int max = 0;
    for (int i = 0; i < node->children.size(); ++i) {
      max = std::max(GetDepthInternal(node->children[i].get()), max);
    }

    return max + 1;
  }
  static void Dump(const INode* node, int depth);
  void Dump();
};

struct ThreadView {
  std::string m_name;
  int m_tid;
  int m_topY;
  int m_bottomY;
  int m_depth;

  std::unique_ptr<INode> m_pRoot{nullptr};

  void Insert(ITimedEvent* e) {
    std::unique_ptr<INode> pNode = std::make_unique<INode>();

    if (!m_pRoot) {
      m_pRoot = std::move(pNode);
      std::unique_ptr<INode> pNode2 = std::make_unique<INode>();
      pNode2->pTimedEvent = e;

      m_pRoot->children.push_back(std::move(pNode2));
    } else {
      pNode->pTimedEvent = e;
      m_pRoot->AddChild(std::move(pNode));
    }
  };
  void Render(RenderContext& ctx);
  void Render(INode* node, RenderContext& ctx);

  void InitData();
};

struct ThreadsLayout {
  std::vector<int> tidOrder;
};

struct ThreadsRender {
  struct Settings {
    static constexpr float SpanHeight = 20.0f;
    static constexpr float OffsetBetweenThreadsY = 40.0f;
  };
  void Render(RenderContext& ctx);

  ThreadsLayout m_layout;

  std::map<int, ThreadView> m_threads;
  void InsertTimedEvent(ITimedEvent* e){};
  void InitFromMessageHub(MessageHub& messageHub,
                          CEventCollector& eventCollector);

  TTime minTime{std::numeric_limits<TTime>::max()};
  TTime maxTime{0};
  int count{0};

 private:
  void InitLayout(CEventCollector& eventCollector);
};
