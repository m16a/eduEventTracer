#pragma once

#include <imgui.h>
#include <limits>
#include <map>
#include <string>
#include <vector>
#include "defines.h"

struct SThreadData;
struct SThreadsData;
struct INode;
struct CEventCollector;

struct RenderContext {
  TTime viewBeginTime;
  TTime viewEndTime;
  float scale;
  float topY;
  int currentDepth;
};

struct SThreadView {
  int m_tid;
  int m_topY;
  int m_bottomY;

  void Render(const SThreadData& data, RenderContext& ctx);
  void Render(INode* node, RenderContext& ctx);
};

struct ThreadsLayout {
  std::vector<int> tidOrder;
};

struct IMouseListener {
  virtual void OnMouseDown() = 0;
  virtual void OnMouseUp() = 0;
  virtual void OnMouseClicked() = 0;
  virtual void OnMouseWheel(float value) = 0;

  virtual void OnMouseMoved(float dx, float dy) = 0;
};

struct MouseHandler {
  void Update(IMouseListener& listener);

  bool bIsClickInWindow{false};
  bool bIsDragged{false};
  ImVec2 prevPos;
};

class SThreadsView : public IMouseListener {
 public:
  struct Settings {
    static constexpr float SpanHeight = 20.0f;
    static constexpr float OffsetBetweenThreadsY = 40.0f;
    static constexpr float PointEventWidth = 10.0f;
  };

  void Render(SThreadsData& data);
  void Init(SThreadsData& data);

  ThreadsLayout m_layout;

  std::map<int, SThreadView> m_threads;
  int count{0};

 private:
  void RenderInternal(const SThreadsData& data, RenderContext& ctx);
  void InitLayout(CEventCollector& eventCollector);

  MouseHandler m_mouseHandler;

 private:
  void RenderMarks();

  // IMouseListener
  void OnMouseDown() override;
  void OnMouseMoved(float dx, float dy) override;
  void OnMouseUp() override;
  void OnMouseClicked() override;
  void OnMouseWheel(float value) override;
  // IMouseListener

  TTime m_begin{std::numeric_limits<TTime>::max()};
  TTime m_end{0};
  float m_viewBegin;
  float m_viewEnd;
  float m_scale{1.0f};
};
