#include "struct_draw.h"
#include "imgui.h"

void ThreadsRender::Render(RenderContext& ctx) {
  // TODO: michaelsh: add render options, f.e ordering
  for (auto& t : m_threads) t.second.Render(ctx);
}

void ThreadView::Render(RenderContext& ctx) { Render(m_pRoot.get(), ctx); }

void RenderNode(STimeInterval* node, RenderContext& ctx) {
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const ImVec2 p = ImGui::GetCursorScreenPos();
  static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
  const ImU32 col32 = ImColor(col);

  const float x1 = p.x + (node->begin - ctx.viewBeginTime) * ctx.scale;
  const float y1 = p.y + 30.0f;

  const float x2 = p.x + (node->end - ctx.viewBeginTime) * ctx.scale;
  const float y2 = p.y + 50.0f;

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

      for (auto& c : node->children) Render(c.get(), ctx);
    }
  }
}
