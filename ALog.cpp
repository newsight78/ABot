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

FileName : Log File의 이름 및 경로
flgEveryOneDay : 파일을 하루에 하나씩만 생성 할 것인가?
FALSE : File의 최대 Line까지만 작성하고 이후 다른 Log 파일 생성
TRUE  : 하루에 하나씩 Log 파일 생성
__________________________________________________________________*/

//CRITICAL_SECTION g_LogCritic;			// 여러 스레드에서 동시에 file open / close하는 경우에 대비		
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

											
	TCHAR lo[] = _T("Korean");	// 한글 OS가 아닌데서는 어떻게 될까???? 2010.5.3
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
	if(m_nItemCount >= m_nMaxLine && m_flgEveryOneDay == FALSE) //5000줄 이상일때 파일 따로 생성. 
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
// Description	: 이 함수는 Target Folder안의 파일중에 현재 날짜로부터 일정 일자 이전의
//				  파일을 삭제하는 함수이다. <Target - 해당Folder, Duration - 일자(day)>
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