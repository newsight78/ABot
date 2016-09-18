// ALog.h: interface for the CLogprocess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALOG_H__84F73D08_AC30_49F1_A5C2_A046922415703141592653__INCLUDED_)
#define AFX_ALOG_H__84F73D08_AC30_49F1_A5C2_A046922415703141592653__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////
// CALog Class
class CALog
{
public:
	CString GetTimeData();
	CString GetDateData();
	void WriteSystemClockLog(CString strLog);
	CString GetSystemClock();
	virtual void WriteLog(LPCTSTR i_cpLog,...);
	void Initial(CString FileName, BOOL flgEveryOneDay=FALSE, long nDuration=30, long nMaxLine=5000);
	void SetEnable(BOOL bEnableLog) {m_bEnableLog = bEnableLog;}
	CALog();
	virtual ~CALog();
	
private:
	CRITICAL_SECTION g_LogCritic;			// 여러 스레드에서 동시에 file open / close하는 경우에 대비		
	CString GetTime();
	CString GetDate();
	CString MakeFileName(CString Date, CString Time);
	CString m_strFoldName;
	CString m_strFileName;
	CString m_strDate;
	void AutoDeleteFile(CString Target, long nDaysAgo);
	BOOL CreateDirectories(CString strDir);

	long m_nItemCount;
	long m_nFileCount;

	long m_nMaxLine;
	long m_nDuration;
	BOOL m_flgEveryOneDay;
	BOOL m_bEnableLog;

};

#endif // !defined(AFX_ALOG_H__84F73D08_AC30_49F1_A5C2_A046922415703141592653__INCLUDED_)
