#include "struct_draw.h"
#include "imgui.h"

void ThreadsRender::Render(RenderContext& ctx) {
  // TODO: michaelsh: add render options, f.e ordering
  for (auto& t : m_threads) t.second.Render(ctx);
}

void ThreadView::Render(RenderContext& ctx) { Render(m_pRoot.get()); }

void Render(STimeInterval* node, RenderContext& ctx) {
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const ImVec2 p = ImGui::GetCursorScreenPos();
  static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
  const ImU32 col32 = ImColor(col);

  const float x1 =
      p.x + (node->start - ctx.viewBeginTime) * /*coef **/ ctx.scale;
  const float y1 = p.y + 30.0f;

  const float x2 = p.x + (node->end - ctx.viewBeginTime) * /*coef **/ ctx.scale;
  const float y2 = p.y + 50.0f;

  draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col32);
}

void ThreadView::Render(INode* node, RenderContext& ctx) {
  STimeInterval* pTInterval = dynamic_cast<STimeInterval*>(pTimedEvent);

  if (pTInterval) {
    if ((pTInterval->start > ctx.viewStartTime &&
         pTInterval->start < ctx.viewEndTime) ||
        (pTInterval->end > ctx.viewStartTime &&
         pTInterval->end < ctx.viewEndTime) ||
        (pTInterval->start < ctx.viewStartTime &&
         pTInterval->end > ctx.viewEndTime)) {
      // pTimedEvent->Render(ctx);

      Render(pTimedEvent);

      for (auto& c : children) Render(c.get(), ctx);
    }
  }
}

struct INode {
  ITimedEvent* pTimedEvent;
  std::vector<std::unique_ptr<INode>> children;
}
