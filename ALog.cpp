// ALog.cpp: implementation of the CLogprocess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ALog.h"
#include <locale.h>

#ifdef _DEBUG
#undef THIS_FILE
static CHAR THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
// Log Class
/*__________________________________________________________________

FileName : Log File�� �̸� �� ���
flgEveryOneDay : ������ �Ϸ翡 �ϳ����� ���� �� ���ΰ�?
FALSE : File�� �ִ� Line������ �ۼ��ϰ� ���� �ٸ� Log ���� ����
TRUE  : �Ϸ翡 �ϳ��� Log ���� ����
__________________________________________________________________*/

//CRITICAL_SECTION g_LogCritic;			// ���� �����忡�� ���ÿ� file open / close�ϴ� ��쿡 ���		
long g_iLogInstanceCount = 0;

CALog::CALog()
{
	AfxTrace(_T("\nCALog::CALog(%d)\n"), g_iLogInstanceCount);
	//if(g_iLogInstanceCount==0)
		InitializeCriticalSection(&g_LogCritic);
	g_iLogInstanceCount++;
}

void CALog::Initial(CString FileName, BOOL flgEveryOneDay/*=FALSE*/, long nDuration/*=30*/, long nMaxLine/*=5000*/)
{
	CString strTime;

	m_bEnableLog = TRUE;

	m_nItemCount = 0;
	m_nFileCount = 0;

	m_nMaxLine = nMaxLine;
	m_flgEveryOneDay = flgEveryOneDay;
	m_nDuration = nDuration;

	m_strFoldName = FileName;
	m_strDate = GetDate();
	strTime = GetTime();

	m_strFileName = MakeFileName(m_strDate, strTime); 

	CreateDirectories(FileName);

	AutoDeleteFile(m_strFoldName, m_nDuration);
}

CALog::~CALog()
{

	g_iLogInstanceCount--;
	if(g_iLogInstanceCount<=0)
		DeleteCriticalSection(&g_LogCritic);

	AfxTrace(_T("\nCALog::~CALog(%d)\n"),g_iLogInstanceCount);
}

void CALog::WriteLog(LPCTSTR i_cpLog, ...)
{
	if(!m_bEnableLog) {return;}

	EnterCriticalSection(&g_LogCritic);

	CString strDate;
	CString strTime;
	CString strLog;
	va_list args;

	va_start( args, i_cpLog );
	strLog.FormatV((LPCTSTR)i_cpLog,args);
	va_end(args);

											
	TCHAR lo[] = _T("Korean");	// �ѱ� OS�� �ƴѵ����� ��� �ɱ�???? 2010.5.3
	TCHAR* pLocale = _tsetlocale(LC_ALL, lo);

	strDate = GetDate();


	if(m_strDate != strDate)
	{
		m_nFileCount = 0;
		m_strDate = strDate;
		strTime = GetTime();
		m_strFileName = MakeFileName(strDate,strTime);
		m_nItemCount = 0;

		AutoDeleteFile(m_strFoldName, m_nDuration);
	}
	if(m_nItemCount >= m_nMaxLine && m_flgEveryOneDay == FALSE) //5000�� �̻��϶� ���� ���� ����. 
	{
		m_strDate = strDate;
		strTime = GetTime();
		m_strFileName = MakeFileName(strDate,strTime); 
		m_nItemCount = 0;
	}

	strDate = GetDateData();
	strTime = GetTimeData();

	strLog = CString("[") + strDate +CString("]") + strTime + CString(" : ") + strLog + CString("\n");



	
	if(!AfxIsValidAddress(m_strFileName, 1))
	{
		LeaveCriticalSection(&g_LogCritic);
		return;
	}

	CStdioFile pFile;

	if(!pFile.Open(m_strFileName, CFile::modeReadWrite ))
	{
		if(!CreateDirectories(m_strFileName))
		{
			LeaveCriticalSection(&g_LogCritic);
			return;
		}
		if(!pFile.Open(m_strFileName, CFile::modeCreate|CFile::modeReadWrite ))
		{
			LeaveCriticalSection(&g_LogCritic);
			return;
		}

	}

	pFile.SeekToEnd();

  	
  	pFile.WriteString(strLog);

	pFile.Close();

	m_nItemCount++;

	LeaveCriticalSection(&g_LogCritic);

}


CString CALog::GetTime()
{
	CString strResult;
	CString strHour;
	CString strMinute;

	CTime t = CTime();
	t = t.GetCurrentTime();

	strHour.Format(TEXT("%02d"), t.GetHour());
	strMinute.Format(TEXT("%02d"), t.GetMinute());

	strResult = strHour + strMinute;

	return strResult;
}

CString CALog::GetDate()
{
	CString strResult;
	CString strYear;
	CString strMonth;
	CString strDay;
	CTime t;

	t = t.GetCurrentTime();

	strYear.Format(TEXT("%04d"), t.GetYear());
	strMonth.Format(TEXT("%02d"), t.GetMonth());
	strDay.Format(TEXT("%02d"), t.GetDay());

	strResult = strYear + strMonth + strDay;

	return strResult;
}

CString CALog::GetTimeData()
{

	SYSTEMTIME  systmCurTime;

	GetLocalTime(&systmCurTime);

	long	iHour = systmCurTime.wHour;
	long	iMin = systmCurTime.wMinute;
	long	iSec = systmCurTime.wSecond;
	long	iMili = systmCurTime.wMilliseconds;
	CString sLogContent; 

	sLogContent.Format(TEXT("[%02d:%02d:%02d.%03d]"), iHour, iMin, iSec, iMili);

	return sLogContent;
}

CString CALog::GetDateData()
{
	CString strResult;
	CString strYear;
	CString strMonth;
	CString strDay;
	CTime t;

	t = t.GetCurrentTime();

	strYear.Format(TEXT("%04d"), t.GetYear());
	strMonth.Format(TEXT("%02d"), t.GetMonth());
	strDay.Format(TEXT("%02d"), t.GetDay());

	strResult = strYear+ CString("/") + strMonth + CString("/") + strDay;

	return strResult;
}

CString CALog::MakeFileName(CString Date, CString Time)
{
	CString Result;
	CString strtmp;

	if(m_flgEveryOneDay)
	{
		Result = m_strFoldName + Date + CString(".log");
	}
	else
	{
		strtmp.Format(TEXT("%d"), m_nFileCount);
		Result = m_strFoldName + Date + Time + CString("[") + strtmp + CString("].log");
		m_nFileCount++;
	}

	return Result;
}

//_____________________________________________________________________________
// Delete Auto Log Files
// Description	: �� �Լ��� Target Folder���� �����߿� ���� ��¥�κ��� ���� ���� ������
//				  ������ �����ϴ� �Լ��̴�. <Target - �ش�Folder, Duration - ����(day)>
void CALog::AutoDeleteFile(CString Target, long nDaysAgo)
{
	BOOL	bWorking;
	CFileFind	finder;
	CString strTemp;

	CTime tmWhen = CTime::GetCurrentTime();
	CTimeSpan ts( nDaysAgo, 0, 0, 0); 

	tmWhen = tmWhen - ts;

	strTemp.Format(TEXT("%04d%02d%02d"), tmWhen.GetYear(), tmWhen.GetMonth(), tmWhen.GetDay());
	Target = Target + strTemp + CString("*.*");

	bWorking = finder.FindFile(Target);

	while(bWorking)
	{
		bWorking = finder.FindNextFile();

		if(finder.IsDots() != TRUE || finder.IsDirectory() != TRUE)
			DeleteFile(finder.GetFilePath());			
	}
}

BOOL CALog::CreateDirectories(CString jstr)
{

	long cnt = 0;

	while(jstr.Find(TEXT("\\"),cnt)>0)
	{
		cnt = jstr.Find(TEXT("\\"),cnt);
		CString strDit = jstr.Left(cnt);
		if(strDit.Right(1)!=":")
			CreateDirectory(strDit,NULL);
		cnt++;
		cnt++;
	}

	return TRUE;
}