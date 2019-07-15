#pragma once

#include <string.h>
#include <sys/types.h>
#include "defines.h"
#include "protocol.pb.h"

int GenerateMessageId();

template <typename T>
int GetMessageId() {
  static int id = GenerateMessageId();
  return id;
}

template <typename T>
void RegisterServiceMessage() {
  GetMessageId<T>();
}

/*-----------------------------------------------------*/
struct ServiceStartCapture {};

std::istream &operator>>(std::istream &in, ServiceStartCapture &c);

std::ostream &operator<<(std::ostream &out, const ServiceStartCapture &c);

/*-----------------------------------------------------*/
struct ServiceStopCapture {};

std::istream &operator>>(std::istream &in, ServiceStopCapture &c);

std::ostream &operator<<(std::ostream &out, const ServiceStopCapture &c);

/*-----------------------------------------------------*/
struct ServiceTransferComplete {};

std::istream &operator>>(std::istream &in, ServiceTransferComplete &c);

std::ostream &operator<<(std::ostream &out, const ServiceTransferComplete &c);

/*-----------------------------------------------------*/
struct RenderContext;

struct ITimedEvent {
  virtual void Render(RenderContext &) = 0;
  int tid;
};

struct STimePoint : public ITimedEvent {
  TTime time;

  void Render(RenderContext &) override {}
};

/*-----------------------------------------------------*/
struct STimeInterval : public ITimedEvent {
  TTime begin;
  TTime end;

  void Render(RenderContext &) override;
};

/*-----------------------------------------------------*/
struct SCatpuredSizeFeedback {
  size_t size;
};

std::istream &operator>>(std::istream &in, SCatpuredSizeFeedback &c);

std::ostream &operator<<(std::ostream &out, const SCatpuredSizeFeedback &c);

// ----------------------- Tracing ------------------------

struct STracingInterval : public STimeInterval {
  std::string name;
  std::string category;
  int module;
};

std::istream &operator>>(std::istream &in, STracingInterval &c);

std::ostream &operator<<(std::ostream &out, const STracingInterval &c);

// ----------------------- Tracing Main Frame ------------------------
struct STracingMainFrame : public STimeInterval {};

std::istream &operator>>(std::istream &in, STracingMainFrame &c);

std::ostream &operator<<(std::ostream &out, const STracingMainFrame &c);

TTime GetTimeNowMs();

// ----------------------- Tracing Legend ------------------------
struct STracingLegend {
  std::map<int, std::string> mapTidToName;
};

std::istream &operator>>(std::istream &in, STracingLegend &c);

std::ostream &operator<<(std::ostream &out, const STracingLegend &c);

struct SEvent : public STimePoint {
  std::string name;
};

std::istream &operator>>(std::istream &in, SEvent &c);
std::ostream &operator<<(std::ostream &out, const SEvent &c);

void InitProtocol();
