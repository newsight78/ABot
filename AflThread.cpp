// AflThread.cpp : implementation file
//
#include "stdafx.h"
#include "AflThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAflThread
unsigned  WINAPI _ThreadFunc(LPVOID  pvThread)
{
	CAflThread* pThread = (CAflThread*)pvThread;  //typecast
	
	pThread->ThreadEntry(); //initialize
	pThread->ThreadRun();
	pThread->ThreadExit(); //finalize

	return 0;
}

BOOL CAflThread::Create()	//Created in Suspend state.
{
	m_hThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		&_ThreadFunc,
		(LPVOID) this,
		CREATE_SUSPENDED,
		&m_lpId); //THREAD_PRIORITY_NORMAL로 생성된다.
	
	if(m_hThread == NULL)
	{
	//	SetErrorMsg(_T("CreateThread(...) 함수 실패"));
		return FALSE;
	}

	m_bActive = FALSE;
	return TRUE;
}

BOOL CAflThread::Suspend()
{
	if(-1 == SuspendThread(m_hThread))
	{
	//	SetErrorMsg(_T("SuspendThread(...) 함수 실패"));
		return FALSE;
	}
	m_bActive = FALSE;
	return TRUE;
}

BOOL CAflThread::Kill()
{
	if(!TerminateThread(m_hThread, 1))
	{
	//	SetErrorMsg(_T("TerminateThread(...) 함수 실패"));
		return FALSE;
	}
	m_bActive = FALSE;
	return TRUE;
}

BOOL CAflThread::Resume()
{
	if(ResumeThread(m_hThread)>1)
	{
	//	SetErrorMsg(_T("ResumeThread(...) 함수 실패"));
		return FALSE;
	}
	m_bActive = TRUE;
	return TRUE;
}

BOOL CAflThread::Wait(DWORD dwMilliseconds)
{
	if(WAIT_OBJECT_0 != WaitForSingleObject(m_hThread, dwMilliseconds))
	{
	//	SetErrorMsg(_T("WaitForSingleObject(...) 함수 실패"));
		return FALSE;
	}
	return TRUE;
}

void CAflThread::ThreadEntry()
{
	AfxMessageBox(_T("CAflThread::ThreadEntry() Called"));
}

void CAflThread::ThreadExit()
{
	AfxMessageBox(_T("CAflThread::ThreadExit() Called"));
}

void CAflThread::ThreadRun()
{
	AfxMessageBox(_T("CAflThread::ThreadRun() Called"));
}
