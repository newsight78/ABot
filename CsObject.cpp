#include "stdafx.h"
#include "CsObject.h"

CsObject::CsObject(void)
{
	DelAllNoticeList();
}

CsObject::~CsObject(void)
{
}

void CsObject::DelAllNoticeList()
{
	m_arListenerList.RemoveAll();
}

BOOL CsObject::DelNoticeList(CsObject *pObject)
{
	if(!pObject) {return FALSE;}
	long nCount = m_arListenerList.GetCount();
	long i(0);
	for(i=0; i<nCount; i++)
	{
		if(m_arListenerList.GetAt(i) == pObject)
		{
			m_arListenerList.RemoveAt(i);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CsObject::AddNoticeList(CsObject *pObject)
{
	if(!pObject) {return FALSE;}
	long nCount = m_arListenerList.GetCount();
	long i(0);
	for(i=0; i<nCount; i++)
	{
		if(m_arListenerList.GetAt(i) == pObject)
		{
			return FALSE;
		}
	}

	m_arListenerList.Add(pObject);
	return TRUE;
}

BOOL CsObject::ShoutNotice()
{
	BOOL bRet(TRUE);
	long nCount = m_arListenerList.GetCount();
	long i(0);
	for(i=0; i<nCount; i++)
	{
		bRet &= m_arListenerList.GetAt(i)->ListenNotice();
	}

	return bRet;
}

BOOL CsObject::ListenNotice()
{
	return FALSE;
}