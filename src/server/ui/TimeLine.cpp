//
// Created by m16a on 24.02.19.
//

#include "TimeLine.h"

#include "event_collector.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

void MouseIO() {
  if (ImGui::IsItemActive() && ImGui::IsWindowHovered()) {
    ImGuiIO& io = ImGui::GetIO();
    std::cout << "click: " << io.MouseClickedPos[0].x << " "
              << io.MouseClickedPos[0].y << "curr: " << io.MousePos.x << " "
              << io.MousePos.y << std::endl;
    /*
    // Draw a line between the button and the mouse cursor
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRectFullScreen();
    draw_list->AddLine(io.MouseClickedPos[0], io.MousePos,
    ImGui::GetColorU32(ImGuiCol_Button), 4.0f);
    draw_list->PopClipRect();

    // Drag operations gets "unlocked" when the mouse has moved past a certain
    threshold (the default threshold is stored in io.MouseDragThreshold)
    // You can request a lower or higher threshold using the second parameter of
    IsMouseDragging() and GetMouseDragDelta()
    ImVec2 value_raw = ImGui::GetMouseDragDelta(0, 0.0f);
    ImVec2 value_with_lock_threshold = ImGui::GetMouseDragDelta(0);
    ImVec2 mouse_delta = io.MouseDelta;
    ImGui::SameLine(); ImGui::Text("Raw (%.1f, %.1f), WithLockThresold (%.1f,
    %.1f), MouseDelta (%.1f, %.1f)", value_raw.x, value_raw.y,
    value_with_lock_threshold.x, value_with_lock_threshold.y, mouse_delta.x,
    mouse_delta.y);
     */
  }
}
/*
virtual void OnMouseDown() = 0;
virtual void OnMouseUp() = 0;
virtual void OnMouseMoved(float dx, float dy)  = 0;

virtual void OnMouseClicked() = 0;
 */

void MouseHandler::Update(MouseListener& listener) {
  if (ImGui::IsWindowHovered()) {
    if (!bIsClickInWindow && ImGui::IsMouseClicked(0)) {
      bIsClickInWindow = true;
      listener.OnMouseDown();
    }

    if (bIsClickInWindow) {
    }
  }

  if (bIsClickInWindow && ImGui::IsMouseReleased(0)) {
    listener.OnMouseUp();
    bIsClickInWindow = false;
  }
}

void TimeLine::Render(CEventCollector& eventCollector) {
  const std::vector<STimeIntervalArg>& intervals =
      eventCollector.GetIntervals();

  ImGui::Begin("Window2", nullptr, ImGuiWindowFlags_NoMove);
  // ImGui::Begin("Window2");

  static float scale = 1.0f;
  ImGui::DragFloat("Scale", &scale, 0.01f, 0.01f, 2.0f, "%.2f");

  ImGui::BeginChild("scrolling",
                    ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 20 + 30),
                    true, ImGuiWindowFlags_HorizontalScrollbar);

  m_mouseHandler.Update(*this);

  if (!intervals.empty()) {
    float width = 0.0f;
    if (intervals.size() > 1)
      width = (intervals.back().endTime - intervals.front().startTime) * scale;

    ImGui::BeginChild("scrolling2",
                      ImVec2(width, ImGui::GetFrameHeightWithSpacing() * 6),
                      false);

    const int epoch = intervals[0].startTime;
    for (const auto& interval : intervals) {
      ImDrawList* draw_list = ImGui::GetWindowDrawList();

      const ImVec2 p = ImGui::GetCursorScreenPos();
      static ImVec4 col = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
      const ImU32 col32 = ImColor(col);

      const float x1 = p.x + (interval.startTime - epoch) * scale;
      const float y1 = p.y + 30.0f;

      const float x2 = p.x + (interval.endTime - epoch) * scale;
      const float y2 = p.y + 50.0f;

      draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col32);
    }
    ImGui::EndChild();
  }

  ImGui::EndChild();
  ImGui::End();
}

void TimeLine::OnMouseDown() { std::cout << "OnMouseDown" << std::endl; }

void TimeLine::OnMouseMoved(float dx, float dy) {
  std::cout << "OnMouseMoved: " << dx << " " << dy << std::endl;
}

void TimeLine::OnMouseUp() { std::cout << "OnMouseUp" << std::endl; }

void TimeLine::OnMouseClicked() { std::cout << "OnMouseClicked" << std::endl; }
