#pragma once

#include <string.h>
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

struct SSampleIntArg {
  int val;
  void Serialize(Ser& ser) {
    if (ser.isReading) {
      int* pVal = reinterpret_cast<int*>(ser.buffer.data());
      val = *pVal;
    } else {
      ser.buffer.resize(4);
      memcpy(ser.buffer.data(), &val, 4);
    }
  }
};

struct STimeIntervalArg {
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

struct STracingInterval {
  int tid;

  int startTime;
  int endTime;

  std::string name;
  std::string category;
  int module;

  void Serialize(Ser& ser) {
    if (ser.isReading) {
      Tracer::STracingInterval interval;
      interval.ParseFromArray(ser.buffer.data(), ser.buffer.size());

      tid = interval.tid();
      startTime = interval.starttime();
      endTime = interval.endtime();

      name = interval.name();
      category = interval.category();
      module = interval.module();
    } else {
      Tracer::STracingInterval interval;
      interval.set_tid(tid);
      interval.set_starttime(startTime);
      interval.set_endtime(endTime);

      interval.set_name(name);
      interval.set_category(category);
      interval.set_module(module);

      size_t size = interval.ByteSizeLong();
      ser.buffer.resize(size);
      interval.SerializeToArray(ser.buffer.data(), size);
    }
  }
};

struct STracingMainFrame {
  int tid;

  int startTime;
  int endTime;

  void Serialize(Ser& ser) {}
};

struct STracingLegend {
  std::vector<std::pair<int, const char*>> threadNames;
};

void InitProtocol();

/////////////////////
/*

plugin


        struct add message
        draw self

        f.e.
                context switches
                I/O
                network
                tracing
                        spans. events. categories, show per thread









*/
