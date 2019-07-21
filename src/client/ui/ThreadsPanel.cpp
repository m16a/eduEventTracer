#include "ThreadsPanel.h"

#include "MessageHub.h"
#include "event_collector.h"
#include "imgui.h"

#include <iostream>

enum class EEncompasRes { Yes, No, Error };
// is a encompases b
//       [     a     ]
//					[  b  ]
inline EEncompasRes IsEncompas(const STimeInterval *a, const STimeInterval *b) {
  if (a->begin <= b->begin && a->end >= b->end)
    return EEncompasRes::Yes;
  else if (a->begin > b->end || a->end < b->begin)
    return EEncompasRes::No;
  else
    return EEncompasRes::Error;
}

inline EEncompasRes IsEncompas(const STimeInterval *a, const STimePoint *b) {
  if (a->begin <= b->time && b->time <= a->end)
    return EEncompasRes::Yes;
  else
    return EEncompasRes::No;
}

bool INode::AddChild(std::unique_ptr<INode> pNewChild) {
  bool bSuccess = true;
  STimeInterval *pTIntervalA =
      dynamic_cast<STimeInterval *>(pNewChild->pTimedEvent);

  STimePoint *pTPointA = dynamic_cast<STimePoint *>(pNewChild->pTimedEvent);

  if (children.empty()) {
    children.emplace_back(std::move(pNewChild));
  } else if (pTIntervalA) {
    int start = 0;
    int end = 0;
    int p = start;
    bool wasAdded = false;
    while (p < children.size()) {
      STimeInterval *pTIntervalB =
          dynamic_cast<STimeInterval *>(children[p]->pTimedEvent);

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
        } else {
          break;
        }

        STimePoint *pTPointB =
            dynamic_cast<STimePoint *>(children[p]->pTimedEvent);

        if (pTPointB) {
          if (pTPointB->time < pTIntervalA->begin) {
            start++;
            end++;
          } else if (pTIntervalA->end > pTPointB->time) {
            end++;
          } else {
            break;
          }
        }

        p++;
      }
    }

    if (!wasAdded) {
      if (start == end) {
        if (start == children.size()) {
          children.insert(children.begin() + start, std::move(pNewChild));
        } else {
          STimeInterval *pTIntervalB =
              dynamic_cast<STimeInterval *>(children[start]->pTimedEvent);
          STimePoint *pTPointB =
              dynamic_cast<STimePoint *>(children[start]->pTimedEvent);
          if ((pTIntervalB &&
               IsEncompas(pTIntervalA, pTIntervalB) == EEncompasRes::Yes) ||
              (pTPointB &&
               IsEncompas(pTIntervalA, pTPointB) == EEncompasRes::Yes)) {
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
  } else if (pTPointA) {
    int p = 0;
    while (p < children.size()) {
      STimeInterval *pTIntervalB =
          dynamic_cast<STimeInterval *>(children[p]->pTimedEvent);

      STimePoint *pTPointB =
          dynamic_cast<STimePoint *>(children[p]->pTimedEvent);

      if (pTPointB) {
        if (pTPointB->time > pTPointA->time) {
          break;
        }
      } else if (pTIntervalB) {
        if (pTIntervalB->begin > pTPointA->time) {
          break;
        } else if (IsEncompas(pTIntervalB, pTPointA) == EEncompasRes::Yes) {
          return children[p]->AddChild(std::move(pNewChild));
        }
      }
      p++;
    }
    children.insert(children.begin() + p, std::move(pNewChild));
  }

  return bSuccess;
}

int INode::GetDepth() { return GetDepthInternal(this); }

int INode::GetDepthInternal(const INode *node) {
  if (node->children.empty()) return 0;

  int max = 0;
  for (int i = 0; i < node->children.size(); ++i) {
    max = std::max(GetDepthInternal(node->children[i].get()), max);
  }

  return max + 1;
}

void SThreadData::Insert(ITimedEvent *e) {
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

void INode::Dump(const INode *node, int depth) {
  for (int i = 0; i < depth; ++i) std::cout << "\t";

  STimeInterval *pTInterval = dynamic_cast<STimeInterval *>(node->pTimedEvent);
  if (pTInterval) {
    std::cout << "TI: " << pTInterval->begin << " " << pTInterval->end
              << std::endl;
  }

  STimePoint *pTPoint = dynamic_cast<STimePoint *>(node->pTimedEvent);
  if (pTPoint) {
    std::cout << "TP: " << pTPoint->time << std::endl;
  }

  depth++;

  for (int i = 0; i < node->children.size(); i++)
    Dump(node->children[i].get(), depth);
}

void INode::Dump() { Dump(this, 0); }

void SThreadsData::Init(MessageHub &messageHub,
                        CEventCollector &eventCollector) {
  std::vector<ITimedEvent *> tmpNodes;
  messageHub.GetAllNodes(tmpNodes);

  {
    // find min/max
    for (auto &node : tmpNodes) {
      STimeInterval *pTInterval = dynamic_cast<STimeInterval *>(node);
      if (pTInterval) {
        if (pTInterval->begin < m_minTime) m_minTime = pTInterval->begin;
        if (pTInterval->end > m_maxTime) m_maxTime = pTInterval->end;
      }

      SEvent *pEvent = dynamic_cast<SEvent *>(node);
      if (pEvent) {
        TTime t = pEvent->time;
        if (t < m_minTime) m_minTime = t;
        if (t > m_maxTime) m_maxTime = t;
      }
    }

    for (auto &node : tmpNodes) {
      STimeInterval *pTInterval = dynamic_cast<STimeInterval *>(node);
      if (pTInterval) {
        pTInterval->begin -= m_minTime;
        pTInterval->end -= m_minTime;
      }

      SEvent *pEvent = dynamic_cast<SEvent *>(node);
      if (pEvent) {
        pEvent->time -= m_minTime;
      }
    }

    m_maxTime -= m_minTime;
    m_minTime = 0;
  }

  int total = 0;
  for (auto &n : tmpNodes) {
    int tid = n->tid;
    auto it = m_mapTidToThreadData.find(tid);
    if (it == m_mapTidToThreadData.end()) {
      it =
          m_mapTidToThreadData.insert(std::make_pair(tid, SThreadData())).first;
      it->second.m_tid = tid;
    }

    it->second.Insert(n);

    total++;
  }

  for (auto &t : m_mapTidToThreadData) {
    SThreadData &data = t.second;

    data.m_depth = data.m_pRoot->GetDepth();

    auto it = eventCollector.mapTidToName.find(t.first);
    if (it != eventCollector.mapTidToName.end()) data.m_name = it->second;

    std::cout << "********************\n";
    data.m_pRoot->Dump();
  }
}

void SThreadsPanel::Init(MessageHub &messageHub,
                         CEventCollector &eventCollector) {
  m_data.Init(messageHub, eventCollector);
  m_view.Init(m_data);
  m_data.m_initialized = true;
}

void SThreadsPanel::Update() { m_view.Render(m_data); }
