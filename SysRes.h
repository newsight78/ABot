#pragma once
#include <windows.h>

class CSysRes
{
public:
	CSysRes();
	~CSysRes();

private:
	friend class CGetAndPutRes;

	void   Get();
	void   Put();

	CRITICAL_SECTION m_Sect;

private:
	CSysRes& operator=(const CSysRes& res);
	CSysRes(const CSysRes& res);
};

class CGetAndPutRes
{
protected:
	CSysRes* m_pRes;
	int   m_GetCnt;

	inline CGetAndPutRes() { m_pRes = NULL; m_GetCnt = 0; }

public:
	inline CGetAndPutRes(CSysRes& aRes)
		: m_pRes(&aRes)
	{
		m_pRes->Get();
		m_GetCnt = 1;
	}

	inline ~CGetAndPutRes()
	{
		PutAll();
	}

	inline void Get()
	{
		if (m_pRes)
		{
			m_pRes->Get();
			m_GetCnt++;
		}
	}

	inline void Put()
	{
		if (m_pRes && m_GetCnt > 0)
		{
			m_GetCnt--;
			m_pRes->Put();
		}
	}

	inline void PutAll()
	{
		for (int i = m_GetCnt; i > 0; i--)
		{
			Put();
		}
	}

private:
	CGetAndPutRes& operator=(const CGetAndPutRes& arC);
	CGetAndPutRes(const CGetAndPutRes& arC);
};