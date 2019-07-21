//
// Created by m16a on 24.02.19.
//

#include "ThreadsView.h"

#include "event_collector.h"

#include "ThreadsPanel.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cmath>
#include <iostream>

bool IsEqual(ImVec2& a, ImVec2& b, float prec = 10e-5f) {
  return !(abs(a.x - b.x) > prec || abs(a.y - b.y) > prec);
}

void MouseHandler::Update(IMouseListener& listener) {
  ImGuiIO& io = ImGui::GetIO();
  if (ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows)) {
    if (!bIsClickInWindow && ImGui::IsMouseClicked(0)) {
      bIsClickInWindow = true;
      listener.OnMouseDown();
    }

    if (bIsClickInWindow) {
      if (!IsEqual(io.MousePos, io.MouseClickedPos[0])) {
        bIsDragged = true;
      }
    }

    if (bIsDragged) {
      if (!IsEqual(prevPos, io.MousePos)) {
        ImVec2 zero;
        if (!IsEqual(prevPos, zero))
          listener.OnMouseMoved(prevPos.x - io.MousePos.x,
                                prevPos.y - io.MousePos.y);
        prevPos = io.MousePos;
      }
    }

    if (io.MouseWheel) {
      listener.OnMouseWheel(io.MouseWheel);
    }
  }

  if (bIsClickInWindow && ImGui::IsMouseReleased(0)) {
    listener.OnMouseUp();
    bIsClickInWindow = false;
    bIsDragged = false;
    prevPos.x = prevPos.y = 0.0f;
  }
}

void SThreadsView::Render(SThreadsData& data) {
  ImGui::Begin("Window2", nullptr, ImGuiWindowFlags_NoMove);
  // ImGui::Begin("Window2");

  const float rootWinHeight = ImGui::GetWindowHeight();
  ImGui::DragFloat("Scale", &m_scale, 0.01f, 0.01f, 2.0f, "%.2f");

  ImGui::BeginChild("scrolling", ImVec2(0, 0), true,
                    ImGuiWindowFlags_HorizontalScrollbar);

  m_mouseHandler.Update(*this);

  if (data.m_initialized) {
    float width = (m_end - m_begin);

    const float winWidth = ImGui::GetWindowWidth();
    const float widthCoef = winWidth / width;

    ImGui::BeginChild("scrolling2", ImVec2(0, 0), false);

    static bool bOnce = false;

    // TODO:michaelsh: rework
    if (!bOnce) {
      bOnce = true;
      m_viewBegin = m_begin;
      m_viewEnd = m_end;
    }
    //    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    RenderContext ctx;
    ctx.scale = m_scale * widthCoef;
    ctx.viewBeginTime = m_viewBegin;
    ctx.viewEndTime = m_viewEnd;
    RenderInternal(data, ctx);

    RenderMarks();
    ImGui::EndChild();
  }

  ImGui::EndChild();
  ImGui::End();
}

void SThreadsView::OnMouseDown() { std::cout << "OnMouseDown" << std::endl; }

void SThreadsView::OnMouseMoved(float dx, float dy) {
  ImGuiIO& io = ImGui::GetIO();

  float width = m_end - m_begin;
  const float winWidth = ImGui::GetWindowWidth();
  const float coef = winWidth / width;

  float offset = dx / coef / m_scale;

  if (offset < 0) {
    if (m_viewBegin + offset < m_begin) offset = 0.0f;
  } else if (offset > 0) {
    if (m_viewEnd + offset > m_end) offset = 0.0f;
  }

  m_viewBegin += offset;
  m_viewEnd += offset;
  std::cout << "OnMouseMoved: " << dx << " " << offset << " " << m_viewBegin
            << " " << m_viewEnd << std::endl;
}

void SThreadsView::OnMouseUp() { std::cout << "OnMouseUp" << std::endl; }

void SThreadsView::OnMouseClicked() {
  std::cout << "OnMouseClicked" << std::endl;
}

void SThreadsView::OnMouseWheel(float value) {
  float old = m_scale;
  m_scale = std::max(1.0f, m_scale + value);

  float currWidth = m_viewEnd - m_viewBegin;

  float width = m_end - m_begin;
  float newWidth = width / m_scale;

  float sign = (m_scale / old > 1.0f) ? 1.0f : -1.0f;

  m_viewBegin = m_viewBegin + sign * fabs(currWidth - newWidth) / 2.0f;
  m_viewEnd = m_viewEnd - sign * fabs(currWidth - newWidth) / 2.0f;

  assert(m_viewBegin < m_viewEnd);

  std::cout << "wheel: " << m_viewBegin << " " << m_viewEnd << std::endl;
}

void SThreadsView::RenderMarks() {
  const int kMarksCount = 6;

  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const float winWidth = ImGui::GetWindowWidth();
  const float winHeight = ImGui::GetWindowHeight();
  const ImVec2 p = ImGui::GetCursorScreenPos();

  static ImVec4 col = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
  const ImU32 col32 = ImColor(col);

  char buff[50];
  for (int i = 0; i < kMarksCount; ++i) {
    float value =
        m_viewBegin + (m_viewEnd - m_viewBegin) * (i + 1) / (kMarksCount + 1);

    const float x = p.x + winWidth * (i + 1) / (kMarksCount + 1);
    const float y1 = p.y + winHeight - 20.0f;
    const float y2 = p.y + winHeight;
    draw_list->AddLine(ImVec2(x, y1), ImVec2(x, y2), col32);

    snprintf(buff, sizeof(buff), "%.2f ms", value / 1000.f);
    draw_list->AddText(ImVec2(x + 4.0f, y1 + 5.0f), col32, buff);
  }
}

void SThreadsView::RenderInternal(const SThreadsData& data,
                                  RenderContext& ctx) {
  for (auto& tid : m_layout.tidOrder) {
    ctx.currentDepth = 0;
    auto& threadView = m_threads[tid];
    ctx.topY = threadView.m_topY;
    threadView.Render(data.m_mapTidToThreadData.at(tid), ctx);
  }
}

void SThreadsView::Init(SThreadsData& data) {
  // init order
  for (auto& t : data.m_mapTidToThreadData) {
    m_layout.tidOrder.push_back(t.first);
  }

  // init layout
  float currentY = 10.0f;
  for (auto& t : m_layout.tidOrder) {
    m_threads[t].m_topY = currentY;
    currentY += data.m_mapTidToThreadData[t].m_depth * Settings::SpanHeight +
                Settings::OffsetBetweenThreadsY;
  }

  m_begin = data.m_minTime;
  m_end = data.m_maxTime;
}

void SThreadView::Render(const SThreadData& data, RenderContext& ctx) {
  Render(data.m_pRoot.get(), ctx);

  // draw tid lable
  {
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const ImVec2 p = ImGui::GetCursorScreenPos();
    static ImVec4 col = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
    ImU32 col32 = ImColor(col);

    char buff[50];
    const float x = p.x + 30.0f;
    const float y = p.y + ctx.topY;
    // draw_list->AddLine(ImVec2(x, y1), ImVec2(x, y2), col32);
    const std::string& name = data.m_name;
    snprintf(buff, sizeof(buff), "%s (%d)", (name.c_str() ? name.c_str() : ""),
             data.m_tid);
    draw_list->AddText(ImVec2(x, y), col32, buff);
  }
}

void RenderNode(STimeInterval* node, RenderContext& ctx) {
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const ImVec2 p = ImGui::GetCursorScreenPos();
  static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
  ImU32 col32 = ImColor(col);

  const float x1 = p.x + (node->begin - ctx.viewBeginTime) * ctx.scale;
  const float y =
      ctx.topY + ctx.currentDepth * SThreadsView::Settings::SpanHeight;
  const float y1 = p.y + y;

  const float x2 = p.x + (node->end - ctx.viewBeginTime) * ctx.scale;
  const float y2 = p.y + y + SThreadsView::Settings::SpanHeight;

  draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col32);

  static ImVec4 col2 = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  col32 = ImColor(col2);
  draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), col32);
}

void RenderNode(STimePoint* node, RenderContext& ctx) {
  ImDrawList* draw_list = ImGui::GetWindowDrawList();

  const ImVec2 p = ImGui::GetCursorScreenPos();
  static ImVec4 col = ImVec4(1.0f, 0.0f, 0.4f, 1.0f);
  ImU32 col32 = ImColor(col);

  const float x1 = p.x + (node->time - ctx.viewBeginTime) * ctx.scale -
                   SThreadsView::Settings::PointEventWidth / 2.0f;
  const float y =
      ctx.topY + ctx.currentDepth * SThreadsView::Settings::SpanHeight;
  const float y1 = p.y + y;

  const float x2 = p.x + (node->time - ctx.viewBeginTime) * ctx.scale +
                   SThreadsView::Settings::PointEventWidth / 2.0f;
  const float y2 = p.y + y + SThreadsView::Settings::SpanHeight;

  draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col32);

  static ImVec4 col2 = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
  col32 = ImColor(col2);
  draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), col32);
}

void SThreadView::Render(INode* node, RenderContext& ctx) {
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
      return;
    }

    STimePoint* pTPoint = dynamic_cast<STimePoint*>(node->pTimedEvent);

    if (pTPoint) {
      if ((pTPoint->time > ctx.viewBeginTime &&
           pTPoint->time < ctx.viewEndTime)) {
        RenderNode(pTPoint, ctx);
      }
    }
  } else {
    RenderContext tmp_ctx(ctx);
    tmp_ctx.currentDepth++;

    for (auto& c : node->children) Render(c.get(), tmp_ctx);
  }
}
