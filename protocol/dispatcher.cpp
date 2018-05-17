#include "dispatcher.h"

void CDispatcher::OnMsg(EMsgType type, TBuff& buff)
{
	m_hndlrs[type](buff);
}
