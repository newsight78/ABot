#pragma once
#include "afxcmn.h"
#include "CsObject.h"
#include "resource.h"
// CsMsgDisp 대화 상자입니다.

class CsMsgDisp : public CDialog, public CsObject
{
protected:
	DECLARE_DYNAMIC(CsMsgDisp)

public:
	CsMsgDisp(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CsMsgDisp();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MSG_DISP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	BOOL m_bFistDisplay;

	DECLARE_MESSAGE_MAP()
public:
	BOOL AddMessage(char * i_cMsg, ...);
	BOOL AddMessage(CString i_strMsg);
	void ClearMessages(void);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CRichEditCtrl m_cRichEdit;
};
