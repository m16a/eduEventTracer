#pragma once

#include <string.h>
#include "buffer.h"
#include "protocol.pb.h"

enum EMsgType {
  StartCapture,
  StopCapture,
  SampleEventInt,
  TimeInterval,
  CapuredSizeFeedback,
  TracingInterval,
  MsgCnt
};

struct SEmptyArg {
  void Serialize(Ser& ser) {}
};

bool Serialize(Ser& ser);

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

  const char* name;
  const char* category;
  int module;

  void Serialize(Ser& ser) {
    if (ser.isReading) {
      /*
int* pVal = reinterpret_cast<int*>(ser.buffer.data());
startTime = *pVal;
pVal += 1;
endTime = *pVal;
      */
    } else {
      Tracer::STracingInterval interval;
      interval.set_tid(tid);
      interval.set_starttime(startTime);
      interval.set_endtime(endTime);

      interval.set_name(name);
      interval.set_category(category);
      interval.set_module(module);

			interval.SerializeToOstream

      /*
ser.buffer.resize(sizeof(int) * 2);
memcpy(ser.buffer.data(), &startTime, 4);
memcpy(ser.buffer.data() + 4, &endTime, 4);
      */
    }
  }
};

struct STracingMainFrame {
  int tid;

  int startTime;
  int endTime;
};

struct STracingLegend {
  std::vector<std::pair<int, const char*>> threadNames;
};

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
