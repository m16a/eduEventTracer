#include "dispatcher.h"

void CDispatcher::OnMsg(int index, TBuff& buff) { m_hndlrs[index](buff); }
