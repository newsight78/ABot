// CsMsgDisp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CsObject.h"
//#include "CsPuppet.h"
#include "CsMsgDisp.h"


// CsMsgDisp 대화 상자입니다.

IMPLEMENT_DYNAMIC(CsMsgDisp, CDialog)

CsMsgDisp::CsMsgDisp(CWnd* pParent /*=NULL*/)
	: CDialog(CsMsgDisp::IDD, pParent)
	, m_bFistDisplay(TRUE)
{

}

CsMsgDisp::~CsMsgDisp()
{
}

void CsMsgDisp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_cRichEdit);
}


BEGIN_MESSAGE_MAP(CsMsgDisp, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_SIZE()
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// CsMsgDisp 메시지 처리기입니다.

void CsMsgDisp::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(bShow && m_bFistDisplay && m_cRichEdit)
	{
		CRect lRect(0,0,0,0);
		if(GetParent())
		{
			GetParent()->GetWindowRect(&lRect);
		}

		MoveWindow(lRect.right, lRect.top, 500, 300);
		SetWindowText("Puppet Message Dialog");  
		m_bFistDisplay = FALSE;
	}
}

void CsMsgDisp::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if(m_cRichEdit) {m_cRichEdit.MoveWindow(0,0,cx,cy);}
}

void CsMsgDisp::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if(nID==SC_CLOSE)
	{
		ShowWindow(SW_HIDE);
		return;
	}

	CDialog::OnSysCommand(nID, lParam);
}

BOOL CsMsgDisp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if(pMsg->message==WM_KEYDOWN)
	{
		if(
			pMsg->wParam==VK_RETURN || 
			pMsg->wParam==VK_ESCAPE
			)
		{
			return TRUE;	
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CsMsgDisp::ClearMessages(void)
{
	m_cRichEdit.SetSel(0, -1);
	m_cRichEdit.Clear();
	return;
}

#include <time.h>
#include <sys/timeb.h>

BOOL CsMsgDisp::AddMessage(char * i_cMsg, ...)
{
	if(!m_cRichEdit || !m_cRichEdit.GetSafeHwnd()) {return FALSE;}

	CTime time = CTime::GetCurrentTime();
	struct _timeb tstruct;
	_ftime64_s( &tstruct );

	va_list arglist;
	char arbytebuf[1024];
	char arbyteMsg[1024];

	va_start(arglist, i_cMsg);
	_vstprintf_s(arbytebuf, i_cMsg, arglist);
	_stprintf_s(arbyteMsg, "[%s:%03u] %s\r\n", time.Format("%Y/%m/%d %H:%M:%S"), tstruct.millitm, arbytebuf);

	long nOldLines = m_cRichEdit.GetLineCount();

	if (nOldLines > 100)
	{  
		m_cRichEdit.SetSel(0, -1);
		m_cRichEdit.Clear();
		m_cRichEdit.SetWindowText("");
	}

	// Initialize charactor format structure
	CHARFORMAT cf;
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR|CFM_SIZE;
	cf.dwEffects = 0; // To disable CFE_AUTOCOLOR
	cf.crTextColor = RGB(0, 0, 0);
	cf.yHeight = 200;

	// Set insertion point tp end of text
	long nLine = m_cRichEdit.GetWindowTextLength();
	m_cRichEdit.SetSel(nLine, -1);

	// Set the character format
	m_cRichEdit.SetSelectionCharFormat(cf);

	// Replace selection, Because we have nothing
	// selected, this will simply insert
	// the string at the current caret position
	m_cRichEdit.ReplaceSel(arbyteMsg);

	// Get new line count
	long nNewLines = m_cRichEdit.GetLineCount();

	// Scroll bt the number of lines just inserted
	long nScroll = nNewLines - nOldLines;
	m_cRichEdit.LineScroll(nScroll);

	return TRUE;
}

BOOL CsMsgDisp::AddMessage(CString i_strMsg)
{
	return AddMessage((TCHAR*)(LPCTSTR)i_strMsg);
}
