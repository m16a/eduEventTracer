#pragma once

#include "protocol.h"
#include "defines.h"
#include "ser.h"
#include <functional> 


class CDispatcher
{
	using THndlr = std::function<bool(TBuff&)>;
public:
	template<class TCaller, class argType>
	void Bind(EMsgType type, TCaller* ownr, bool (TCaller::*callback)(argType&))
	{
		auto lambda = [ownr, callback](TBuff& buffer)
		{
			Ser ser;
			ser.buffer = buffer;//TODO:michaelsh:extra copy
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
