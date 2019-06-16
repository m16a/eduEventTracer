#include "struct_draw.h"
#include <iostream>
#include "imgui.h"

void ThreadsRender::Render(RenderContext& ctx) {
  for (auto& t : m_threads) {
    ctx.currentDepth = 0;
    ctx.topY = t.second.m_topY;
    t.second.Render(ctx);
  }
}

void ThreadView::Render(RenderContext& ctx) { Render(m_pRoot.get(), ctx); }

void RenderNode(STimeInterval* node, RenderContext& ctx) {
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const ImVec2 p = ImGui::GetCursorScreenPos();
  static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
  ImU32 col32 = ImColor(col);

  const float x1 = p.x + (node->begin - ctx.viewBeginTime) * ctx.scale;
  const float y =
      ctx.topY + ctx.currentDepth * ThreadsRender::Settings::SpanHeight;
  const float y1 = p.y + y;

  const float x2 = p.x + (node->end - ctx.viewBeginTime) * ctx.scale;
  const float y2 = p.y + y + ThreadsRender::Settings::SpanHeight;

  draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col32);

  static ImVec4 col2 = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  col32 = ImColor(col2);
  draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), col32);
}

void ThreadView::Render(INode* node, RenderContext& ctx) {
  if (ctx.currentDepth != 0) {
    if (!node->pTimedEvent) return;

    STimeInterval* pTInterval = dynamic_cast<STimeInterval*>(node->pTimedEvent);

    if (pTInterval) {
      if ((pTInterval->begin > ctx.viewBeginTime &&
           pTInterval->begin < ctx.viewEndTime) ||
          (pTInterval->end > ctx.viewBeginTime &&
           pTInterval->end < ctx.viewEndTime) ||
          (pTInterval->begin < ctx.viewBeginTime &&
           pTInterval->end > ctx.viewEndTime)) {
        RenderNode(pTInterval, ctx);

        RenderContext tmp_ctx(ctx);
        tmp_ctx.currentDepth++;

        for (auto& c : node->children) Render(c.get(), tmp_ctx);
      }
    }
  } else {
    RenderContext tmp_ctx(ctx);
    tmp_ctx.currentDepth++;

    for (auto& c : node->children) Render(c.get(), tmp_ctx);
  }
}

void ThreadsRender::InitFromMessageHub(MessageHub& messageHub) {
  std::vector<ITimedEvent*> tmpNodes;
  messageHub.GetAllNodes(tmpNodes);

  {
    // find min/max
    for (auto& node : tmpNodes) {
      STimeInterval* pTInterval = dynamic_cast<STimeInterval*>(node);
      if (pTInterval) {
        if (pTInterval->begin < minTime) minTime = pTInterval->begin;
        if (pTInterval->end > maxTime) maxTime = pTInterval->end;
      }
    }

    for (auto& node : tmpNodes) {
      STimeInterval* pTInterval = dynamic_cast<STimeInterval*>(node);
      if (pTInterval) {
        pTInterval->begin -= minTime;
        pTInterval->end -= minTime;
      }
    }
    maxTime -= minTime;
    minTime = 0;
  }

  int total = 0;
  for (auto& n : tmpNodes) {
    int tid = n->tid;
    auto it = m_threads.find(tid);
    if (it == m_threads.end())
      it = m_threads.insert(std::make_pair(tid, ThreadView())).first;

    it->second.Insert(n);

    total++;
  }

  InitLayout();

  // TODO:michealsh: do better sync
  count = total;
}

void ThreadsRender::InitLayout() {
  for (auto& t : m_threads) {
    ThreadView& tView = t.second;

    tView.InitData();
    m_layout.tidOrder.push_back(t.first);
    std::cout << "********************\n";
    tView.m_pRoot->Dump();
  }

  float currentY = 10.0f;
  float height = 20.0f;
  float offsetY = 30.0f;
  for (auto& t : m_layout.tidOrder) {
    m_threads[t].m_topY = currentY;
    currentY += m_threads[t].m_depth * Settings::SpanHeight +
                Settings::OffsetBetweenThreadsY;
  }
}

void ThreadView::InitData() { m_depth = m_pRoot->GetDepth(); }

void INode::Dump(const INode* node, int depth) {
  for (int i = 0; i < depth; ++i) std::cout << "\t";

  STimeInterval* pTInterval = dynamic_cast<STimeInterval*>(node->pTimedEvent);
  if (pTInterval) {
    std::cout << pTInterval->begin << " " << pTInterval->end << std::endl;
  }
  depth++;

  for (int i = 0; i < node->children.size(); i++)
    Dump(node->children[i].get(), depth);
}

void INode::Dump() { Dump(this, 0); }
