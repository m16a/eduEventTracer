//
// Created by m16a on 24.02.19.
//

#ifndef EDUEVENTTRACER_TIMELINE_H
#define EDUEVENTTRACER_TIMELINE_H

#include <imgui.h>

struct MouseListener {
  virtual void OnMouseDown() = 0;
  virtual void OnMouseUp() = 0;
  virtual void OnMouseClicked() = 0;
  virtual void OnMouseWheel(float value) = 0;

  virtual void OnMouseMoved(float dx, float dy) = 0;
};

class CEventCollector;

struct MouseHandler {
  void Update(MouseListener& listener);

  bool bIsClickInWindow{false};
  bool bIsDragged{false};
  ImVec2 prevPos;
};

class TimeLine : public MouseListener {
 public:
  void Render(CEventCollector& eventCollector);

  MouseHandler m_mouseHandler;

 private:
  void OnMouseDown() override;
  void OnMouseMoved(float dx, float dy) override;
  void OnMouseUp() override;
  void OnMouseClicked() override;
  void OnMouseWheel(float value) override;

  int m_begin;
  int m_end;
  float m_viewBegin;
  float m_viewEnd;
  float m_scale{1.0f};
};

#endif  // EDUEVENTTRACER_TIMELINE_H
