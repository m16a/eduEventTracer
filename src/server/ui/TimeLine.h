//
// Created by m16a on 24.02.19.
//

#ifndef EDUEVENTTRACER_TIMELINE_H
#define EDUEVENTTRACER_TIMELINE_H

struct MouseListener {
  virtual void OnMouseDown() = 0;
  virtual void OnMouseUp() = 0;
  virtual void OnMouseClicked() = 0;

  virtual void OnMouseMoved(float dx, float dy) = 0;
};

class CEventCollector;

struct MouseHandler {
  void Update(MouseListener& listener);

  bool bIsClickInWindow;
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

  int begin;
  int end;
  int viewBegin;
  int viewEnd;
};

#endif  // EDUEVENTTRACER_TIMELINE_H
