#pragma once

#include "protocol.h"
#include "defines.h"
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
			argType strct;
			strct.Serialize(buffer);

			return callback(strct);
		};
			
		m_hndlrs[type] = 	lambda;
	}

	void OnMsg(EMsgType type, TBuff& buffer);

private:
	std::array<THndlr, EMsgType::MsgCnt> m_hndlrs;
};
