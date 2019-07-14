#pragma once

#include "ThreadsView.h"
#include "defines.h"

#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct ITimedEvent;

struct MessageHub;
class CEventCollector;

struct INode {
  ITimedEvent* pTimedEvent;
  std::vector<std::unique_ptr<INode>> children;

  bool AddChild(std::unique_ptr<INode> pNewChild);

  int GetDepth();
  static int GetDepthInternal(const INode* node);
  static void Dump(const INode* node, int depth);
  void Dump();
};

struct SThreadData {
  std::unique_ptr<INode> m_pRoot{nullptr};
  int m_tid;
  int m_depth;
  std::string m_name;
  void Insert(ITimedEvent* e);
};

struct SThreadsData {
  void Init(MessageHub& messageHub, CEventCollector& eventCollector);

  std::map<int, SThreadData> m_mapTidToThreadData;
  TTime m_minTime{std::numeric_limits<TTime>::max()};
  TTime m_maxTime;

  bool m_initialized{false};
};

struct SThreadsPanel {
  void Init(MessageHub& messageHub, CEventCollector& eventCollector);
  void Update();

  SThreadsView m_view;
  SThreadsData m_data;
};
