#include "struct_draw.h"
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
  const ImU32 col32 = ImColor(col);

  const float x1 = p.x + (node->begin - ctx.viewBeginTime) * ctx.scale;
  const float y =
      ctx.topY + ctx.currentDepth * ThreadsRender::Settings::SpanHeight;
  const float y1 = p.y + y;

  const float x2 = p.x + (node->end - ctx.viewBeginTime) * ctx.scale;
  const float y2 = p.y + y + ThreadsRender::Settings::SpanHeight;

  draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col32);
}

void ThreadView::Render(INode* node, RenderContext& ctx) {
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

      ctx.currentDepth++;

      for (auto& c : node->children) Render(c.get(), ctx);
    }
  }
}

void ThreadsRender::InitFromMessageHub(MessageHub& messageHub) {
  std::vector<ITimedEvent*> tmpNodes;
  messageHub.GetAllNodes(tmpNodes);

  int total = 0;
  for (auto& n : tmpNodes) {
    int tid = n->tid;
    auto it = m_threads.find(tid);
    if (it == m_threads.end())
      it = m_threads.insert(std::make_pair(tid, ThreadView())).first;

    it->second.Insert(n);

    STimeInterval* pTInterval = dynamic_cast<STimeInterval*>(n);
    if (pTInterval) {
      if (pTInterval->begin < minTime) minTime = pTInterval->begin;
      if (pTInterval->end > maxTime) maxTime = pTInterval->end;
    }

    total++;
  }

  InitLayout();

  count = total;
}

void ThreadsRender::InitLayout() {
  for (auto& t : m_threads) {
    ThreadView& tView = t.second;

    tView.InitData();
    m_layout.tidOrder.push_back(t.first);
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
