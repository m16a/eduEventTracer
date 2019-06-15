#pragma once

#include <string.h>
#include <sys/types.h>
#include "buffer.h"
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

bool Serialize(Ser& ser);

struct ServiceStartCapture {
  void Serialize(Ser& ser) {}
};

struct ServiceStopCapture {
  void Serialize(Ser& ser) {}
};

struct ServiceTransferComplete {
  void Serialize(Ser& ser) {}
};

struct RenderContext;

struct ITimedEvent {
  virtual void Render(RenderContext&) = 0;
  int tid;
};

struct STimePoint : public ITimedEvent {
  TTime time;

  void Render(RenderContext&) override {}
};

struct STimeInterval : public ITimedEvent {
  TTime begin;
  TTime end;

  void Render(RenderContext&) override;
};

struct STimeIntervalArg : public STimeInterval {
  int startTime;
  int endTime;

  STimeIntervalArg() {}
  STimeIntervalArg(int _startTime, int _endTime)
      : startTime(_startTime), endTime(_endTime) {}

  void Serialize(Ser& ser) {
    if (ser.isReading) {
      int* pVal = reinterpret_cast<int*>(ser.buffer.data());
      startTime = *pVal;
      pVal += 1;
      endTime = *pVal;
    } else {
      ser.buffer.resize(sizeof(int) * 2);
      memcpy(ser.buffer.data(), &startTime, 4);
      memcpy(ser.buffer.data() + 4, &endTime, 4);
    }
  }
};

struct SCatpuredSizeFeedback {
  size_t size;

  void Serialize(Ser& ser) {
    if (ser.isReading) {
      int* pVal = reinterpret_cast<int*>(ser.buffer.data());
      size = *pVal;
    } else {
      ser.buffer.resize(4);
      memcpy(ser.buffer.data(), &size, 4);
    }
  }
};

// ----------------------- Tracing ------------------------

struct STracingInterval : public STimeInterval {
  std::string name;
  std::string category;
  int module;

  void Serialize(Ser& ser) {
    if (ser.isReading) {
      Tracer::STracingInterval interval;
      interval.ParseFromArray(ser.buffer.data(), ser.buffer.size());

      tid = interval.tid();
      begin = interval.begin();
      end = interval.end();

      name = interval.name();
      category = interval.category();
      module = interval.module();
    } else {
      Tracer::STracingInterval interval;
      interval.set_tid(tid);
      interval.set_begin(begin);
      interval.set_end(end);

      interval.set_name(name);
      interval.set_category(category);
      interval.set_module(module);

      size_t size = interval.ByteSizeLong();
      ser.buffer.resize(size);
      interval.SerializeToArray(ser.buffer.data(), size);
    }
  }
};
struct STracingMainFrame : public STimeInterval {
  void Serialize(Ser& ser) {
    if (ser.isReading) {
      TTime* pVal = reinterpret_cast<TTime*>(ser.buffer.data());
      begin = *pVal;
      pVal += 1;
      end = *pVal;
      pVal += 1;
      tid = *pVal;
    } else {
      size_t tS = sizeof(TTime);
      ser.buffer.resize(tS * 3);
      memcpy(ser.buffer.data(), &begin, tS);
      // TODO: michaelsh: pay attention to x86 and x64
      memcpy(ser.buffer.data() + 1, &end, tS);
      memcpy(ser.buffer.data() + 2, &tid, tS);
    }
  }
};

TTime GetTimeNowMs();

struct STracingLegend {
  std::vector<std::pair<int, const char*>> threadNames;
};

void InitProtocol();
