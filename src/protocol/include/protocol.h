#pragma once

#include <string.h>
#include "buffer.h"

enum EMsgType {
  StartCapture,
  StopCapture,
  SampleEventInt,
  TimeInterval,
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
