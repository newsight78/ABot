#if !defined(AFX_AFLTHREAD_H__D1D510EA_7B7B_4598_9E41_87EA23A8C197__INCLUDED_)
#define AFX_AFLTHREAD_H__D1D510EA_7B7B_4598_9E41_87EA23A8C197__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AflThread.h : header file
//
//#include "AObject.h"
#include <process.h>

/////////////////////////////////////////////////////////////////////////////
// CAflThread

class CAflThread// : public CAObject
{
// Construction
public:
	CAflThread() {m_hThread = NULL; m_bActive = FALSE; m_lpId = 0;}
	virtual ~CAflThread(){Kill();}

//Thread Management
	BOOL Create();
	BOOL Wait(DWORD dwMilliseconds = INFINITE); //Wait for thread to end
	BOOL Suspend(); //Suspend the thread
	BOOL Resume(); //Resume a suspended thread
	BOOL Kill(); //Terminate a thread
	BOOL IsActive() {return m_bActive;} //Check for activity
	
//override these functions in the derived class
	virtual void ThreadEntry();//=0;
	virtual void ThreadExit();//=0;
	virtual void ThreadRun();//=0;
	
//a friend
	friend	unsigned  WINAPI _ThreadFunc(LPVOID  pvThread);
	
protected:
	HANDLE m_hThread; //Thread handle
	BOOL m_bActive; //activity indicator
	unsigned m_lpId; //Thread ID
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AFLTHREAD_H__D1D510EA_7B7B_4598_9E41_87EA23A8C197__INCLUDED_)
