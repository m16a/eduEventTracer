#include "protocol.h"
#include "MessageHub.h"

#include <chrono>

// returns count of read Bytes on read

int GenerateMessageId() {
  static int id = 0;
  return id++;
}

void InitServiceMessages() {
  RegisterServiceMessage<ServiceStartCapture>();
  RegisterServiceMessage<ServiceStopCapture>();
  RegisterServiceMessage<SCatpuredSizeFeedback>();
  RegisterServiceMessage<ServiceTransferComplete>();
  RegisterServiceMessage<STracingLegend>();
}

template <typename T>
void RegisterMessage() {
  GetMessageId<T>();
  GetMessageHub().RegisterMessage<T>();
}

void InitProtocol() {
  InitServiceMessages();

  RegisterMessage<STracingInterval>();
  RegisterMessage<STracingMainFrame>();
}

TTime GetTimeNowMs() {
  std::chrono::high_resolution_clock::time_point start =
      std::chrono::high_resolution_clock::now();
  TTime resultMs = std::chrono::duration_cast<std::chrono::microseconds>(
                       start.time_since_epoch())
                       .count();
  return resultMs;
}

void STimeInterval::Render(RenderContext &) {}

std::istream &operator>>(std::istream &in, ServiceStartCapture &c) {
  return in;
}

std::ostream &operator<<(std::ostream &out, const ServiceStartCapture &c) {
  return out;
}

std::istream &operator>>(std::istream &in, ServiceStopCapture &c) { return in; }

std::ostream &operator<<(std::ostream &out, const ServiceStopCapture &c) {
  return out;
}

std::istream &operator>>(std::istream &in, ServiceTransferComplete &c) {
  return in;
}

std::ostream &operator<<(std::ostream &out, const ServiceTransferComplete &c) {
  return out;
}

std::istream &operator>>(std::istream &in, SCatpuredSizeFeedback &c) {
  in >> c.size;
  return in;
}

std::ostream &operator<<(std::ostream &out, const SCatpuredSizeFeedback &c) {
  out << c.size;
  return out;
}

std::istream &operator>>(std::istream &in, STracingInterval &c) {
  Tracer::STracingInterval interval;
  interval.ParseFromIstream(&in);

  c.tid = interval.tid();
  c.begin = interval.begin();
  c.end = interval.end();

  c.name = interval.name();
  c.category = interval.category();
  c.module = interval.module();
  return in;
}

std::ostream &operator<<(std::ostream &out, const STracingInterval &c) {
  Tracer::STracingInterval interval;
  interval.set_tid(c.tid);
  interval.set_begin(c.begin);
  interval.set_end(c.end);

  interval.set_name(c.name);
  interval.set_category(c.category);
  interval.set_module(c.module);

  interval.SerializeToOstream(&out);
  return out;
}

std::istream &operator>>(std::istream &in, STracingMainFrame &c) {
  in >> c.begin >> c.end >> c.tid;
  return in;
}

std::ostream &operator<<(std::ostream &out, const STracingMainFrame &c) {
  out << c.begin << c.end << c.tid;
  return out;
}

std::istream &operator>>(std::istream &in, STracingLegend &c) {
  Tracer::STracingLegend msg;
  msg.ParseFromIstream(&in);

  auto it = msg.maptidtoname().begin();
  while (it != msg.maptidtoname().end()) {
    c.mapTidToName.insert(std::pair<int, std::string>(it->first, it->second));
    ++it;
  }
  return in;
}

std::ostream &operator<<(std::ostream &out, const STracingLegend &c) {
  Tracer::STracingLegend msg;

  // auto test = Tracer::STracingLegend::default_instance();
  auto map = msg.mutable_maptidtoname();

  for (auto &kv : c.mapTidToName) (*map)[kv.first] = kv.second;

  msg.SerializeToOstream(&out);
  return out;
}
