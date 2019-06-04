#pragma once

#include <functional>
#include "buffer.h"
#include "defines.h"
#include "protocol.h"

// TODO:delete
#include <iostream>

const constexpr int MaxMessageTypeCount = 100;
class CDispatcher {
  using THndlr = std::function<bool(TBuff&)>;

 public:
  template <class TCaller, class argType>
  void Bind(TCaller* ownr, bool (TCaller::*callback)(argType&)) {
    auto lambda = [ownr, callback](TBuff& buffer) {
      Ser ser;
      ser.buffer = buffer;  // TODO:michaelsh:extra copy
      ser.isReading = true;
      argType strct;
      strct.Serialize(ser);

      return (ownr->*callback)(strct);
    };

    int index = GetMessageId<argType>();
    std::cout << "binded: " << index << std::endl;
    m_hndlrs[index] = lambda;
  }

  void OnMsg(int index, TBuff& buffer);

 private:
  std::array<THndlr, MaxMessageTypeCount> m_hndlrs;
};
