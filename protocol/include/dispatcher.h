#pragma once

#include "protocol.h"
#include "defines.h"
#include "ser.h"
#include <functional> 


class CDispatcher
{
	using THndlr = std::function<bool(TBuff&)>;
public:
	template<class argType>
	void Bind(EMsgType type, bool(*callback)(argType))
	{
		argType a;
		auto lambda = [callback](TBuff& buffer)
		{
			Ser ser;
			ser.buffer = buffer;//TODO:michaelsh:extra copy
			ser.isReading = true;
			argType strct;
			strct.Serialize(ser);

			return callback(strct);
		};
			
		m_hndlrs[type] = lambda;
	}

	void OnMsg(EMsgType type, TBuff& buffer);

private:
	std::array<THndlr, EMsgType::MsgCnt> m_hndlrs;
};
