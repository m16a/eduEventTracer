#pragma once

#include <functional>
#include "buffer.h"
#include "defines.h"
#include "protocol.h"

class CDispatcher {
  using THndlr = std::function<bool(TBuff&)>;

 public:
  template <class TCaller, class argType>
  void Bind(EMsgType type, TCaller* ownr, bool (TCaller::*callback)(argType&)) {
    auto lambda = [ownr, callback](TBuff& buffer) {
      Ser ser;
      ser.buffer = buffer;  // TODO:michaelsh:extra copy
      ser.isReading = true;
      argType strct;
      strct.Serialize(ser);

      return (ownr->*callback)(strct);
    };

    m_hndlrs[type] = lambda;
  }

  void OnMsg(EMsgType type, TBuff& buffer);

 private:
  std::array<THndlr, EMsgType::MsgCnt> m_hndlrs;
};
