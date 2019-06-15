//
// Created by m16a on 24.02.19.
//

#include "TimeLine.h"

#include "event_collector.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cmath>
#include <iostream>

bool IsEqual(ImVec2& a, ImVec2& b, float prec = 10e-5f) {
  return !(abs(a.x - b.x) > prec || abs(a.y - b.y) > prec);
}

void MouseHandler::Update(MouseListener& listener) {
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

void TimeLine::Render(CEventCollector& eventCollector,
                      ThreadsRender& thrdsRend) {
  ImGui::Begin("Window2", nullptr, ImGuiWindowFlags_NoMove);
  // ImGui::Begin("Window2");

  const float rootWinHeight = ImGui::GetWindowHeight();
  ImGui::DragFloat("Scale", &m_scale, 0.01f, 0.01f, 2.0f, "%.2f");

  ImGui::BeginChild("scrolling", ImVec2(0, 0), true,
                    ImGuiWindowFlags_HorizontalScrollbar);

  m_mouseHandler.Update(*this);

  if (thrdsRend.count) {
    float width = (thrdsRend.maxTime - thrdsRend.minTime);

    const float winWidth = ImGui::GetWindowWidth();
    const float widthCoef = winWidth / width;

    ImGui::BeginChild("scrolling2", ImVec2(0, 0), false);

    m_begin = thrdsRend.minTime;
    m_end = thrdsRend.maxTime;

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
    thrdsRend.Render(ctx);

    RenderMarks();
    ImGui::EndChild();
  }

  ImGui::EndChild();
  ImGui::End();
}

void TimeLine::OnMouseDown() { std::cout << "OnMouseDown" << std::endl; }

void TimeLine::OnMouseMoved(float dx, float dy) {
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

void TimeLine::OnMouseUp() { std::cout << "OnMouseUp" << std::endl; }

void TimeLine::OnMouseClicked() { std::cout << "OnMouseClicked" << std::endl; }

void TimeLine::OnMouseWheel(float value) {
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

void TimeLine::RenderMarks() {
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
        /*m_viewBegin + */ (m_viewEnd - m_viewBegin) * (i + 1) /
        (kMarksCount + 1);

    const float x = p.x + winWidth * (i + 1) / (kMarksCount + 1);
    const float y1 = p.y + winHeight - 20.0f;
    const float y2 = p.y + winHeight;
    draw_list->AddLine(ImVec2(x, y1), ImVec2(x, y2), col32);

    snprintf(buff, sizeof(buff), "%.2f ms", value);
    draw_list->AddText(ImVec2(x + 4.0f, y1 + 5.0f), col32, buff);
  }
}
