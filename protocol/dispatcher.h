#pragma once

#include "protocol.h"
#include <functional> 


class CDispatcher
{
	using THndlr = std::function<bool(void*)>;
public:
	template<class argType>
	void Bind(EMsgType type, bool(*callback)(argType))
	{
	
		argType a;
		auto lambda = [callback](void* buff)
		{
			argType strct;
			strct.Serialize(buff);

			return callback(buff);
		};
			
		m_hndlrs[type] = 	lambda;
	}

	template<class argType>
	void Send(EMsgType type, argType& arg)
	{
		void* buff;
		//arg.ser(buff);
		//send buff
	}
	
private:
	std::array<THndlr, EMsgType::MsgCnt> m_hndlrs;
};
