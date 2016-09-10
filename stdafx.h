
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.


#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.


#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����
#include <afxdisp.h>



#define OP_ERR_SISE_OVERFLOW	-200			//"�ü���ȸ ������"
#define OP_ERR_RQ_STRUCT_FAIL	-201			//"REQUEST_INPUT_st Failed"
#define OP_ERR_RQ_STRING_FAIL	-202			//"��û ���� �ۼ� ����"
#define OP_ERR_NO_DATA			-203			//"�����Ͱ� �������� �ʽ��ϴ�."
#define OP_ERR_OVER_MAX_DATA	-204			//"�ѹ��� ��ȸ ������ ���񰳼��� �ִ� 100���� �Դϴ�."


#define OP_ERR_ORD_WRONG_INPUT	-300			//"�Է°� ����"
#define OP_ERR_ORD_WRONG_ACCNO	-301			//"���º�й�ȣ�� �Է��Ͻʽÿ�."
#define OP_ERR_OTHER_ACC_USE	-302			//"Ÿ�ΰ��´� ����� �� �����ϴ�."
#define OP_ERR_MIS_2BILL_EXC	-303			//"�ֹ������� 20����� �ʰ��մϴ�."
#define OP_ERR_MIS_5BILL_EXC	-304			//"�ֹ������� 50����� �ʰ��մϴ�."
#define OP_ERR_MIS_1PER_EXC		-305			//"�ֹ������� �ѹ����ּ��� 1%�� �ʰ��մϴ�."
#define OP_ERR_MIS_3PER_EXC		-306			//"�ֹ������� �ѹ����ּ��� 3%�� �ʰ��� �� �����ϴ�."
#define OP_ERR_SEND_FAIL		-307			//"�ֹ����۽���"
#define OP_ERR_ORD_OVERFLOW		-308			//"�ֹ����� ������"

#define UM_SCRENN_CLOSE		1000

#define TR_OPT10001		"OPT10001"
#define TR_OPT10003		"OPT10003"
#define TR_OPT10004		"OPT10004"
#define TR_OPT10080		"OPT10080"
#define TR_OPT10081		"OPT10081"

#define DT_NONE			0		// �⺻���� ����
#define DT_DATE			1		// ���� ����
#define DT_TIME			2		// �ð� ����
#define DT_NUMBER		3		// ���� ���� ����
#define DT_ZERO_NUMBER	4		// ���� ����(0ǥ��) ����
#define DT_SIGN			5		// ����ȣ ����



#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include "Util.h"
#include "CDKHOpenAPI.h"
#include "CsMsgDisp.h"

extern CsMsgDisp * g_pMsgDisp;


