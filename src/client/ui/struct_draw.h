#pragma once

#include "MessageHub.h"
#include "protocol.h"

struct INode {
  ITimedEvent* pTimedEvent;
  std::vector<INode*> children;
};

struct ThreadView {
  int name;
  int topY;
  int bottomY;
};

struct ThreadsLayout {
  std::vector<ThreadView> treadViews;
};

struct ThreadsRender {
  INode* m_pRoot;
  std::vector<ITimedEvent*> m_tmpNodes;

  void Render(RenderContext&) {}

  ThreadsLayout m_layout;

  void InsertTimedEvent(ITimedEvent* e){};
  void InitFromMessageHub(MessageHub& messageHub) {
    messageHub.GetAllNodes(m_tmpNodes);
  }
};
