
// ABot.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once
#include "CDKHOpenAPI.h"

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CABotApp:
// �� Ŭ������ ������ ���ؼ��� ABot.cpp�� �����Ͻʽÿ�.
//

class CABotApp : public CWinApp
{
public:
	CABotApp();

public:
	CDKHOpenAPI m_khOpenApi;
	
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CABotApp theApp;