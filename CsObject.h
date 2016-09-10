#pragma once

class CsObject
{
public:
	CsObject(void);
	virtual ~CsObject(void);
protected:
	CArray<CsObject*> m_arListenerList;

public:
	///////////////////////////////////////////
	//Notice Function /////////////////////////
	void DelAllNoticeList();
	BOOL AddNoticeList(CsObject *pObject);
	BOOL DelNoticeList(CsObject *pObject);
	BOOL ShoutNotice();
	virtual BOOL ListenNotice();
	//Notice Function /////////////////////////
	///////////////////////////////////////////
};
