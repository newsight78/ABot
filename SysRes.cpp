#include "stdafx.h"
#include "SysRes.h"

CSysRes::CSysRes()
{
 ::InitializeCriticalSection( &m_Sect );
}

CSysRes::~CSysRes()
{
 ::DeleteCriticalSection( &m_Sect);
}

void
CSysRes::Get()
{
 ::EnterCriticalSection( &m_Sect);
}

void
CSysRes::Put()
{
 ::LeaveCriticalSection( &m_Sect);
}