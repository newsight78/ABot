
// ABotDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ABot.h"
#include "ABotDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define START_TIMER	1001
#define ROUND_TIMER	1002


#define FIDLIST	_T("9001;302;10;11;25;12;13")

// {��ȸ Ű,		���� Ű,	��, ��, Ÿ��,			�� ����, ����,	�� ����, �� ����}
const stGRID lstOPTSBFID[] =
{
	{ "�����ڵ�",		"9001",	-1, 0, DT_NONE,			FALSE,	DT_LEFT,	"", "" },
	{ "�����",			"302",	-1, 1, DT_NONE,			FALSE,	DT_LEFT,	"", "" },
	{ "���簡",			"10",	-1, 2, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"", "" },
	{ "���ϴ���ȣ",	"25",	-1, 3, DT_SIGN,			TRUE,	DT_CENTER,	"", "" },
	{ "���ϴ��",		"11",	-1, 4, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"", "" },
	{ "�����",			"12",	-1, 5, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"", "%" },
	{ "�ŷ���",			"13",	-1, 6, DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,	"", "" },
};


CString GetRoundText(eProcessState aProcessState)
{
	switch (aProcessState)
	{
	default:
		return "�˼� ����";
	case ePST_IDLE:
		return "���� ����";
	case ePST_ROUND_START:
		return "���� ����";
	case ePST_WAIT_TIME:
		return "���� �ð��� ��ٸ��� ����";
	case ePST_DR_SEARCH:
		return "������ ��û ����";
	case ePST_DR_WAIT:
		return "������ ��� ��� ����";
	case ePST_ITEM_SEARCH:
		return "���� �˻� ��û ����";
	case ePST_ITEM_WAIT:
		return "���� �˻� ��� ��� ����";
	case ePST_ITEM_TRADE:
		return "���� �ŷ� ����";
	case ePST_ROUND_END:
		return "���� 1ȸ ���� ����";
	case ePST_KILL_PROC:
		return "���μ��� �ߴ�";
	}
	return "�˼� ����";
};

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CABotDlg ��ȭ ����



CABotDlg::CABotDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CABotDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	g_pMsgDisp = NULL;
	m_nRoundCount = 0;
	m_eProcessState = ePST_IDLE;
	m_bDoFinishProcess = FALSE;
	m_nProcessRetryCount = 0;
	m_lProcessDR = 0;
	m_lDepositReceived = 0;
	m_ItemCount = 0;
	m_nConditionIndex = 0;

	m_lItemBuyTimeout = 0;
	m_lItemBuyTryCount = 0;
	m_lItemHoldTimeout = 0;

	m_dSellOverThis = 0.;
	m_dSellOverThis2 = 0.;
	m_dSellUnderThis = 0.;
	m_dSellUnderThis2 = 0.;
}

void CABotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KHOPENAPICTRL1, theApp.m_khOpenApi);
	DDX_Control(pDX, IDC_COMBO_FILTER, m_cmbCon);
	DDX_Control(pDX, IDC_GRD_REALADD, m_grdRealAdd);
	DDX_Control(pDX, IDC_COMBO_ROUND, m_cmbRoundCount);
	DDX_Control(pDX, IDC_COMBO_SHOUR, m_cmbStartHour);
	DDX_Control(pDX, IDC_COMBO_SMIN, m_cmbStartMin);
	DDX_Control(pDX, IDC_COMBO_EHOUR, m_cmbEndHour);
	DDX_Control(pDX, IDC_COMBO_EMIN, m_cmbEndMin);
	DDX_Control(pDX, IDC_COMBO_DP_USE_RATE, m_cmbDpUseRate);
	DDX_Control(pDX, IDC_COMBO_MAX_TOTAL_AMOUNT, m_cmbMaxTotalAmount);
	DDX_Control(pDX, IDC_COMBO_MAX_AMOUNT, m_cmbMaxAmount);
	DDX_Control(pDX, IDC_COMBO_BUY_METHOD, m_cmbBuyMethod);
	DDX_Control(pDX, IDC_COMBO_BUY_RETRY, m_cmbBuyRetry);
	DDX_Control(pDX, IDC_COMBO_HOLD_TIMEOUT, m_cmbHoldTimeOut);
	DDX_Control(pDX, IDC_COMBO_SELL_OVERTHIS, m_cmbSellOverThis);
	DDX_Control(pDX, IDC_COMBO_SELL_OVERTHIS2, m_cmbSellOverThis2);
	DDX_Control(pDX, IDC_COMBO_SELL_UNDERTHIS, m_cmbSellUnderThis);
	DDX_Control(pDX, IDC_COMBO_SELL_UNDERTHIS2, m_cmbSellUnderThis2);
	DDX_Control(pDX, IDC_COMBO_BUY_TIMEOUT, m_cmbBuyTimeOut);
	
}

BEGIN_MESSAGE_MAP(CABotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(ID_SHOW_MESSAGE, &CABotDlg::OnBnClickedShowMessage)
	ON_BN_CLICKED(IDC_BALANCE_QUERY, &CABotDlg::OnBnClickedBalanceQuery)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_GETFILTER, &CABotDlg::OnBnClickedButtonGetfilter)
	ON_BN_CLICKED(IDC_BUTTON_GETTARGET, &CABotDlg::OnBnClickedButtonGettarget)
	ON_BN_CLICKED(IDOK, &CABotDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SAVECONFIG, &CABotDlg::OnBnClickedButtonSaveconfig)
	ON_BN_CLICKED(IDC_BUTTON_START_ROUND, &CABotDlg::OnBnClickedButtonStartRound)
	ON_BN_CLICKED(IDC_BUTTON_FINISH_ROUND, &CABotDlg::OnBnClickedButtonFinishRound)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ROUND, &CABotDlg::OnBnClickedButtonStopRound)
	ON_BN_CLICKED(IDC_BUTTON_SELL_ALL_CUR_COST, &CABotDlg::OnBnClickedButtonSellAllCurCost)
	ON_BN_CLICKED(IDC_BUTTON_REGTARGET, &CABotDlg::OnBnClickedButtonRegtarget)
END_MESSAGE_MAP()


// CABotDlg �޽��� ó����

BOOL CABotDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL CABotDlg::IsError(long lErrCode, CString &strMsg)
{
	strMsg = _T("");
	switch (lErrCode)
	{
	default:
	case OP_ERR_NONE:
		return FALSE;
		break;

	case OP_ERR_LOGIN:
		strMsg = "�����������ȯ�� �����Ͽ����ϴ�. ����� �ٽ� �����Ͽ� �ֽʽÿ�.";
		break;
	case OP_ERR_CONNECT:
		strMsg = "���� ���� ����";
		break;
	case OP_ERR_VERSION:
		strMsg = "����ó���� �����Ͽ����ϴ�.";
		break;

		////////////////////////////
		// �Ϲ� ��ȸ ���� ó��
	case OP_ERR_SISE_OVERFLOW:		// �ü���ȸ ������
		strMsg = "�ü���ȸ ������";
		break;
	case OP_ERR_RQ_STRUCT_FAIL:		// REQUEST_INPUT_st Failed
		strMsg = "REQUEST_INPUT_st Failed";
		break;
	case OP_ERR_RQ_STRING_FAIL:		// ��û ���� �ۼ� ����
		strMsg = "��û ���� �ۼ� ����";
		break;
	case OP_ERR_NO_DATA:			// �����Ͱ� �������� �ʽ��ϴ�.
		strMsg = "�����Ͱ� �������� �ʽ��ϴ�.";
		break;

		////////////////////////////
		// ���� ���� ��ȸ ���� ó��
	case OP_ERR_OVER_MAX_DATA:			// �ѹ��� ��ȸ ������ ���񰳼��� �ִ� 100���� �Դϴ�.
		strMsg = "�ѹ��� ��ȸ ������ ���񰳼��� �ִ� 100���� �Դϴ�.";
		break;

		////////////////////////////
		// �ֹ� ���� ó��
	case OP_ERR_ORD_WRONG_INPUT:		// �Է°� ����
		strMsg = "�Է°� ����";
		break;
	case OP_ERR_ORD_WRONG_ACCNO:		// ���º�й�ȣ�� �Է��Ͻʽÿ�.
		strMsg = "���º�й�ȣ�� �Է��Ͻʽÿ�.";
		break;
	case OP_ERR_OTHER_ACC_USE:			// Ÿ�ΰ��´� ����� �� �����ϴ�.
		strMsg = "Ÿ�ΰ��´� ����� �� �����ϴ�.";
		break;
	case OP_ERR_MIS_2BILL_EXC:			// �ֹ������� 20����� �ʰ��մϴ�.
		strMsg = "�ֹ������� 20����� �ʰ��մϴ�.";
		break;
	case OP_ERR_MIS_5BILL_EXC:			// �ֹ������� 50����� �ʰ��մϴ�.
		strMsg = "�ֹ������� 50����� �ʰ��մϴ�.";
		break;
	case OP_ERR_MIS_1PER_EXC:			// �ֹ������� �ѹ����ּ��� 1%�� �ʰ��մϴ�.
		strMsg = "�ֹ������� �ѹ����ּ��� 1%�� �ʰ��մϴ�.";
		break;
	case OP_ERR_MIS_3PER_EXC:			// �ֹ������� �ѹ����ּ��� 3%�� �ʰ��� �� �����ϴ�.
		strMsg = "�ֹ������� �ѹ����ּ��� 3%�� �ʰ��� �� �����ϴ�.";
		break;
	case OP_ERR_SEND_FAIL:				// �ֹ����۽���
		strMsg = "�ֹ����ۿ� �����Ͽ����ϴ�..";
		break;
	case OP_ERR_ORD_OVERFLOW:			// �ֹ����� ������
		strMsg = "�ֹ����� ������ �Դϴ�. ��� �� �ٽ� �ֹ��Ͽ� �ּ���.";
		break;
	}

	return TRUE;
}

BOOL CABotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_strConfigFile = _T("ABot.ini");
	CString strArguments = AfxGetApp()->m_lpCmdLine;
	CString strBuf;
	if (strArguments.GetLength() > 0)
	{
		AfxExtractSubString(m_strConfigFile, strArguments, 0, ' ');
	}

	if (m_strConfigFile.Find(":") < 0)
	{
		strBuf = m_strConfigFile;
		char charBuf[256];
		GetCurrentDirectory(_countof(charBuf)-1, charBuf);
		m_strConfigFile.Format("%s\\%s", charBuf, strBuf);
	}

	// �α� ���� ��� ���.
	ReadFromIniFile_String(m_strConfigFile, "SYSTEM", "logfoldername", ".\\", strBuf);
	strBuf += _T("\\"); strBuf.Replace(_T("\\\\"), _T("\\"));
	m_strLogFolderName = strBuf;
	g_logger.Initial(m_strLogFolderName + _T("ABotLog_"));
	int n = 0;
	ReadFromIniFile_Int(m_strConfigFile, "SYSTEM", "dowritelog", 1, n);
	g_logger.SetEnable(n > 0 ? TRUE : FALSE);

	//�޼��� ���÷��̸� ���� ���� �����,
	g_pMsgDisp = new CsMsgDisp(this);		
	if (!g_pMsgDisp) { return FALSE; }
	g_pMsgDisp->Create(IDD_MSG_DISP, this);

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	theApp.m_khOpenApi.CommConnect();

	m_mapJongCode.RemoveAll();
	m_mapFIDName.RemoveAll();
	m_mapNameList.RemoveAll();
	InitFIDName();
	InitRealAddGrid();
	InitComboBox();

	AddMessage(" ");
	AddMessage("=========================================================");
	AddMessage("�ȳ��ϼ���? ABot'%s'�Դϴ�.", GetSystemVersion());
	srand(GetTickCount() % 10000);
	m_strScrNo.Format("%04d", rand()%10000);
	AddMessage("��ũ����ȣ[%s], ȯ������[%s]", m_strScrNo, m_strConfigFile);
	AddMessage("�����մϴ�.");

	LoadSystemFile();

	m_nConditionIndex = -1;

	SetTimer(START_TIMER, 1000, NULL);
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CABotDlg::InitComboBox()
{
	//�޺� �ڽ� ���� ������.
	m_cmbRoundCount.InsertString(0, "�ݺ�");
	m_cmbRoundCount.InsertString(1, "1");
	m_cmbRoundCount.InsertString(2, "3");
	m_cmbRoundCount.InsertString(3, "5");
	m_cmbRoundCount.InsertString(4, "10");
	m_cmbRoundCount.InsertString(5, "100");
	m_cmbRoundCount.InsertString(6, "500");
	m_cmbRoundCount.InsertString(7, "1000");
	m_cmbRoundCount.InsertString(8, "10000");
	m_cmbRoundCount.SetCurSel(1);

	CString strBuf;
	int i = 0;
	for (i = 0; i < 24; i++)
	{
		strBuf.Format("%d", i);
		m_cmbStartHour.InsertString(i, strBuf);
		m_cmbEndHour.InsertString(i, strBuf);
	}
	m_cmbStartHour.SetCurSel(9);
	m_cmbEndHour.SetCurSel(15);
	for (i = 0; i < 60; i++)
	{
		strBuf.Format("%d", i);
		m_cmbStartMin.InsertString(i, strBuf);
		m_cmbEndMin.InsertString(i, strBuf);
	}
	m_cmbStartMin.SetCurSel(0);
	m_cmbEndMin.SetCurSel(0);

	m_cmbDpUseRate.InsertString(0, "90");
	m_cmbDpUseRate.InsertString(1, "80");
	m_cmbDpUseRate.InsertString(2, "50");
	m_cmbDpUseRate.SetCurSel(2);

	m_cmbMaxTotalAmount.InsertString(0, "10");
	m_cmbMaxTotalAmount.InsertString(1, "50");
	m_cmbMaxTotalAmount.InsertString(2, "100");
	m_cmbMaxTotalAmount.InsertString(3, "200");
	m_cmbMaxTotalAmount.InsertString(4, "500");
	m_cmbMaxTotalAmount.InsertString(5, "1000");
	m_cmbMaxTotalAmount.SetCurSel(2);

	m_cmbMaxAmount.InsertString(0, "10");
	m_cmbMaxAmount.InsertString(1, "50");
	m_cmbMaxAmount.InsertString(2, "100");
	m_cmbMaxAmount.InsertString(3, "200");
	m_cmbMaxAmount.InsertString(4, "500");
	m_cmbMaxAmount.InsertString(5, "1000");
	m_cmbMaxAmount.SetCurSel(2);

	m_cmbBuyMethod.InsertString(0, "���簡");
	m_cmbBuyMethod.InsertString(1, "���尡");
	m_cmbBuyMethod.SetCurSel(0);
	
	m_cmbBuyTimeOut.InsertString(0, "10");
	m_cmbBuyTimeOut.InsertString(1, "20");
	m_cmbBuyTimeOut.InsertString(2, "30");
	m_cmbBuyTimeOut.InsertString(3, "40");
	m_cmbBuyTimeOut.InsertString(4, "50");
	m_cmbBuyTimeOut.InsertString(5, "60");
	m_cmbBuyTimeOut.SetCurSel(2);

	m_cmbBuyRetry.InsertString(0, "0");
	m_cmbBuyRetry.InsertString(1, "1");
	m_cmbBuyRetry.InsertString(2, "2");
	m_cmbBuyRetry.InsertString(3, "3");
	m_cmbBuyRetry.SetCurSel(1);

	m_cmbSellOverThis.InsertString(0, "0.5");
	m_cmbSellOverThis.InsertString(1, "1.0");
	m_cmbSellOverThis.InsertString(2, "1.5");
	m_cmbSellOverThis.InsertString(3, "2.0");
	m_cmbSellOverThis.InsertString(4, "2.5");
	m_cmbSellOverThis.InsertString(5, "3.0");
	m_cmbSellOverThis.InsertString(6, "3.5");
	m_cmbSellOverThis.InsertString(7, "4.0");
	m_cmbSellOverThis.InsertString(8, "4.5");
	m_cmbSellOverThis.InsertString(9, "5.0");
	m_cmbSellOverThis.SetCurSel(1);

	m_cmbSellOverThis2.InsertString(0, "0.5");
	m_cmbSellOverThis2.InsertString(1, "1.0");
	m_cmbSellOverThis2.InsertString(2, "1.5");
	m_cmbSellOverThis2.InsertString(3, "2.0");
	m_cmbSellOverThis2.InsertString(4, "2.5");
	m_cmbSellOverThis2.InsertString(5, "3.0");
	m_cmbSellOverThis2.InsertString(6, "3.5");
	m_cmbSellOverThis2.InsertString(7, "4.0");
	m_cmbSellOverThis2.InsertString(8, "4.5");
	m_cmbSellOverThis2.InsertString(9, "5.0");
	m_cmbSellOverThis2.SetCurSel(0);


	m_cmbSellUnderThis.InsertString(0, "0.5");
	m_cmbSellUnderThis.InsertString(1, "1.0");
	m_cmbSellUnderThis.InsertString(2, "1.5");
	m_cmbSellUnderThis.InsertString(3, "2.0");
	m_cmbSellUnderThis.InsertString(4, "2.5");
	m_cmbSellUnderThis.InsertString(5, "3.0");
	m_cmbSellUnderThis.InsertString(6, "3.5");
	m_cmbSellUnderThis.InsertString(7, "4.0");
	m_cmbSellUnderThis.InsertString(8, "4.5");
	m_cmbSellUnderThis.InsertString(9, "5.0");
	m_cmbSellUnderThis.SetCurSel(1);

	m_cmbSellUnderThis2.InsertString(0, "0.5");
	m_cmbSellUnderThis2.InsertString(1, "1.0");
	m_cmbSellUnderThis2.InsertString(2, "1.5");
	m_cmbSellUnderThis2.InsertString(3, "2.0");
	m_cmbSellUnderThis2.InsertString(4, "2.5");
	m_cmbSellUnderThis2.InsertString(5, "3.0");
	m_cmbSellUnderThis2.InsertString(6, "3.5");
	m_cmbSellUnderThis2.InsertString(7, "4.0");
	m_cmbSellUnderThis2.InsertString(8, "4.5");
	m_cmbSellUnderThis2.InsertString(9, "5.0");
	m_cmbSellUnderThis2.SetCurSel(3);


	m_cmbHoldTimeOut.InsertString(0, "0");
	m_cmbHoldTimeOut.InsertString(1, "10");
	m_cmbHoldTimeOut.InsertString(2, "20");
	m_cmbHoldTimeOut.InsertString(3, "30");
	m_cmbHoldTimeOut.InsertString(4, "40");
	m_cmbHoldTimeOut.InsertString(5, "50");
	m_cmbHoldTimeOut.InsertString(6, "60");
	m_cmbHoldTimeOut.SetCurSel(0);

}

void CABotDlg::LoadSystemFile()
{
	int i = 0, n = 0;
	double d = 0.;
	CString strBuf, strCombo;

	// ���� ��ȣ.
	ReadFromIniFile_String(m_strConfigFile, "ACCOUNT", "number", "-", strBuf);
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->SetWindowText(strBuf);
	AddMessage("     ���� ��ȣ[%s]", strBuf);

	// �ݺ� ȸ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "count", "1", strBuf);
	if ("�ݺ�" == strBuf)
	{
		m_cmbRoundCount.SetCurSel(0);
	}
	else
	{
		n = atoi((LPSTR)(LPCSTR)strBuf);
		strBuf.Format("%d", n);
		for (i = 1; i<m_cmbRoundCount.GetCount(); i++)
		{
			m_cmbRoundCount.GetLBText(i, strCombo);
			if (strCombo == strBuf)
			{
				break;
			}
		}
		if (i >= m_cmbRoundCount.GetCount())
		{
			m_cmbRoundCount.InsertString(i, strBuf);
		}
		m_cmbRoundCount.SetCurSel(i);
	}
	AddMessage("     �ݺ� ȸ��[%s].", strBuf);

	CString strBuf2 = "���� ";
	// ���� �ð��� ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "start_hour", "9", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	m_cmbStartHour.SetCurSel(n);
	strBuf2 += strBuf + "�� ";

	// ���� �ð��� ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "start_min", "0", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	m_cmbStartMin.SetCurSel(n);
	strBuf2 += strBuf + "��,  ���� ";

	// ���� �ð��� ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "end_hour", "15", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	m_cmbEndHour.SetCurSel(n);
	strBuf2 += strBuf + "�� ";

	// ���� �ð��� ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "end_min", "0", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	m_cmbEndMin.SetCurSel(n);
	strBuf2 += strBuf + "��";
	AddMessage("     ���� �ð�[%s].", strBuf2);

	// �ż��� �������� �ִ� ��� �ۼ�Ʈ. [%]
	ReadFromIniFile_String(m_strConfigFile, "BUY", "use_rate", "50", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	if (n > 95) {n = 95;}
	strBuf.Format("%d", n);
	for (i = 0; i<m_cmbDpUseRate.GetCount(); i++)
	{
		m_cmbDpUseRate.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbDpUseRate.GetCount())
	{
		m_cmbDpUseRate.InsertString(i, strBuf);
	}
	m_cmbDpUseRate.SetCurSel(i);
	AddMessage("     ������ ��� [%s]�ۼ�Ʈ.", strBuf);

	// �ż��� �������� �ִ� ��� �ݾ�. [����]
	ReadFromIniFile_String(m_strConfigFile, "BUY", "max_total_amount", "100", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	strBuf.Format("%d", n);
	for (i = 0; i<m_cmbMaxTotalAmount.GetCount(); i++)
	{
		m_cmbMaxTotalAmount.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbMaxTotalAmount.GetCount())
	{
		m_cmbMaxTotalAmount.InsertString(i, strBuf);
	}
	m_cmbMaxTotalAmount.SetCurSel(i);
	AddMessage("     �ִ� ������ ��� ��� �ݾ� [%s]����.", strBuf);

	// ����� �ִ� ���� ��� �ݾ�
	ReadFromIniFile_String(m_strConfigFile, "BUY", "max_amount", "100", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	strBuf.Format("%d", n);
	for (i = 0; i<m_cmbMaxAmount.GetCount(); i++)
	{
		m_cmbMaxAmount.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbMaxAmount.GetCount())
	{
		m_cmbMaxAmount.InsertString(i, strBuf);
	}
	m_cmbMaxAmount.SetCurSel(i);
	AddMessage("     ����� �ִ� ���� ��� �ݾ� [%s]����.", strBuf);

	// �ż� ���, ���簡, ���尡. ����..�ۼ�Ʈ ����.
	ReadFromIniFile_String(m_strConfigFile, "BUY", "method", "���簡", strBuf);
	for (i = 0; i<m_cmbBuyMethod.GetCount(); i++)
	{
		m_cmbBuyMethod.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbBuyMethod.GetCount())
	{
		i = 0;	//"���簡"
		m_cmbBuyMethod.GetLBText(i, strBuf);
	}
	m_cmbBuyMethod.SetCurSel(i);
	AddMessage("     �ż� ��� [%s].", strBuf);

	// �ż� ü�� ��� Ÿ�� �ƿ�.
	ReadFromIniFile_String(m_strConfigFile, "BUY", "timeout", "30", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	if (n > 60) { n = 60; }
	strBuf.Format("%d", n);
	for (i = 0; i<m_cmbBuyTimeOut.GetCount(); i++)
	{
		m_cmbBuyTimeOut.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbBuyTimeOut.GetCount())
	{
		m_cmbBuyTimeOut.InsertString(i, strBuf);
	}
	m_cmbBuyTimeOut.SetCurSel(i);
	AddMessage("     �ż� ü�� ��� Ÿ�� �ƿ� [%s]��.", strBuf);

	// �ż� ���н� ��õ� ȸ��
	ReadFromIniFile_String(m_strConfigFile, "BUY", "retry", "1", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	if (n > 5) { n = 5; }
	strBuf.Format("%d", n);
	for (i = 0; i<m_cmbBuyRetry.GetCount(); i++)
	{
		m_cmbBuyRetry.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbBuyRetry.GetCount())
	{
		m_cmbBuyRetry.InsertString(i, strBuf);
	}
	m_cmbBuyRetry.SetCurSel(i);
	AddMessage("     �ż� ���н� ��õ� ȸ�� [%s]ȸ.", strBuf);

	// �ֽ� ���� Ÿ�� �ƿ�.
	ReadFromIniFile_String(m_strConfigFile, "HOLD", "timeout", "0", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	if (n > 60) { n = 60; }
	strBuf.Format("%d", n);
	for (i = 0; i<m_cmbHoldTimeOut.GetCount(); i++)
	{
		m_cmbHoldTimeOut.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbHoldTimeOut.GetCount())
	{
		m_cmbHoldTimeOut.InsertString(i, strBuf);
	}
	m_cmbHoldTimeOut.SetCurSel(i);
	AddMessage("     ���� ��� Ÿ�� �ƿ� [%s]��.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "over_this_1", "1.0", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	if (d > 30.0) { d = 30.0; }
	strBuf.Format("%3.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellOverThis.GetCount(); i++)
	{
		m_cmbSellOverThis.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbSellOverThis.GetCount())
	{
		m_cmbSellOverThis.InsertString(i, strBuf);
	}
	m_cmbSellOverThis.SetCurSel(i);
	AddMessage("     �ŵ� ���� 1-1. ���簡�� �ż� �ݾ��� [%s] �ۼ�Ʈ �̻� ��½�.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "over_this_2", "0.5", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	if (d > 30.0) { d = 30.0; }
	strBuf.Format("%3.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellOverThis2.GetCount(); i++)
	{
		m_cmbSellOverThis2.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbSellOverThis2.GetCount())
	{
		m_cmbSellOverThis2.InsertString(i, strBuf);
	}
	m_cmbSellOverThis2.SetCurSel(i);
	AddMessage("     �ŵ� ���� 1-2. ���簡�� 1-1 Ʈ������ �ż� �ݾ��� [%s] �ۼ�Ʈ ���� ���޽� ���尡 �ŵ�.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "under_this_1", "1.0", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	if (d > 30.0) { d = 30.0; }
	strBuf.Format("%.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellUnderThis.GetCount(); i++)
	{
		m_cmbSellUnderThis.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbSellUnderThis.GetCount())
	{
		m_cmbSellUnderThis.InsertString(i, strBuf);
	}
	m_cmbSellUnderThis.SetCurSel(i);
	AddMessage("     �ŵ� ���� 2-1. ���簡�� �ż� �ݾ��� [%s] �ۼ�Ʈ �̻� �϶���.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "under_this_2", "2.0", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	if (d > 30.0) { d = 30.0; }
	strBuf.Format("%.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellUnderThis2.GetCount(); i++)
	{
		m_cmbSellUnderThis2.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			break;
		}
	}
	if (i >= m_cmbSellUnderThis2.GetCount())
	{
		m_cmbSellUnderThis2.InsertString(i, strBuf);
	}
	m_cmbSellUnderThis2.SetCurSel(i);
	AddMessage("     �ŵ� ���� 2-2. ���簡�� 2-2 Ʈ������ �ż� �ݾ��� [%s] �ۼ�Ʈ ���� ���޽� ���尡 �ŵ�.", strBuf);
}

void CABotDlg::SaveSystemFile()
{
	int i = 0, n = 0;
	double d = 0.;
	CString strBuf, strCombo;

	// ���� ��ȣ.
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ACCOUNT", "number", strBuf);

	// �ݺ� ȸ��
	m_cmbRoundCount.GetLBText(m_cmbRoundCount.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "count", strBuf);

	// ���� �ð��� ��
	m_cmbStartHour.GetLBText(m_cmbStartHour.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "start_hour", strBuf);

	// ���� �ð��� ��
	m_cmbStartMin.GetLBText(m_cmbStartMin.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "start_min", strBuf);

	// ���� �ð��� ��
	m_cmbEndHour.GetLBText(m_cmbEndHour.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "end_hour", strBuf);

	// ���� �ð��� ��
	m_cmbEndMin.GetLBText(m_cmbEndMin.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "end_min", strBuf);

	// �˻� ����
	if (m_cmbCon.GetCurSel() > 0)
	{
		m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strBuf);
		WriteToIniFile_String(m_strConfigFile, "FILTER", "name", strBuf);
	}

	// �ż��� �������� �ִ� ��� �ۼ�Ʈ. [%]
	m_cmbDpUseRate.GetLBText(m_cmbDpUseRate.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "use_rate", strBuf);

	// �ż��� �������� �ִ� ��� �ݾ�. [����]
	m_cmbMaxTotalAmount.GetLBText(m_cmbMaxTotalAmount.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "max_total_amount", strBuf);	

	// ����� �ִ� ���� ��� �ݾ�
	m_cmbMaxAmount.GetLBText(m_cmbMaxAmount.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "max_amount", strBuf);

	// �ż� ���, ���簡, ���尡. ����..�ۼ�Ʈ ����.
	m_cmbBuyMethod.GetLBText(m_cmbBuyMethod.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "method", strBuf);

	// �ż� ü�� ��� Ÿ�� �ƿ�.
	m_cmbBuyTimeOut.GetLBText(m_cmbBuyTimeOut.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "timeout", strBuf);

	// �ż� ���н� ��õ� ȸ��
	m_cmbBuyRetry.GetLBText(m_cmbBuyRetry.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "retry", strBuf);

	// �ֽ� ���� Ÿ�� �ƿ�.
	m_cmbHoldTimeOut.GetLBText(m_cmbHoldTimeOut.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "HOLD", "timeout", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	m_cmbSellOverThis.GetLBText(m_cmbSellOverThis.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "over_this_1", strBuf);

	m_cmbSellOverThis2.GetLBText(m_cmbSellOverThis2.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "over_this_2", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	m_cmbSellUnderThis.GetLBText(m_cmbSellUnderThis.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "under_this_1", strBuf);

	m_cmbSellUnderThis2.GetLBText(m_cmbSellUnderThis2.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "under_this_2", strBuf);
}


void CABotDlg::OnBnClickedButtonSaveconfig()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SaveSystemFile();
}


void CABotDlg::InitRealAddGrid()
{
	m_grdRealAdd.SetEditable(false);				//cell�� ����Ʈ ���ϰ� ��.
//	m_grdRealAdd.EnableScrollBars(SB_BOTH, FALSE);
	m_grdRealAdd.EnableScrollBars(SB_VERT, TRUE);

	COLORREF clr = RGB(215, 227, 241);

	// ���� ��/�� ����
	m_grdRealAdd.SetFixedRowCount(1);

	// ��/�� ���� ����
	m_grdRealAdd.SetRowCount(1);
	SetGridHeight(0, 24); // grid ���̴� ������ ���� �����ϹǷ� ���� �Լ��� ȣ���Ѵ�.

	m_grdRealAdd.SetColumnCount(7);

	// ���� ���� ����
	int i = 0, nWidth[] = { 50, 80, 70, 30, 70, 45, 75 };
	CString strHeader[] = { "�ڵ�", "�����", "���簡", "��ȣ", "���ϴ��", "�����", "�ŷ���" };
	int nCnt = sizeof(nWidth) / sizeof(*nWidth);		// ��üũ�� / ����ũ�� = ���Ұ���
	m_grdRealAdd.SetColumnCount(nCnt);
	for (i = 0; i < nCnt; i++)
	{
		SetGridWidth(i, nWidth[i]); // grid ���� ������ ���� �����ϹǷ� ���� �Լ��� ȣ���Ѵ�.
		m_grdRealAdd.SetItemFormat(0, i, DT_CENTER);
		m_grdRealAdd.SetItemText(0, i, strHeader[i]);
		m_grdRealAdd.SetItemBkColour(0, i, clr);	// ������ ���� ���� ����
	}

	m_grdRealAdd.SetRowCount(21);
	m_grdRealAdd.Invalidate();
}

void CABotDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

}

void CABotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (START_TIMER == nIDEvent)
	{
		if (g_pMsgDisp && g_pMsgDisp->GetSafeHwnd() )
		{
			g_pMsgDisp->ShowWindow(SW_SHOW);
			KillTimer(nIDEvent);
		}
	}

	if (ROUND_TIMER == nIDEvent)
	{
		CString strBuf;
		strBuf.Format(_T("ABot ����[%d] [%s]"), m_nRoundCount, GetRoundText(m_eProcessState));
		SetWindowText(strBuf);
		ProcessSequence();
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CABotDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CABotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CABotDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));

	if (m_strConditionName != "" && m_nConditionIndex > -1)
	{
		theApp.m_khOpenApi.SendConditionStop(m_strScrNo, m_strConditionName, m_nConditionIndex);
	}

	CDialogEx::OnOK();
}

void CABotDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// ��� ���� ó��
//	theApp.m_khOpenApi.CommTerminate();	//�������� ����.

	AddMessage("ABot ����.");
	AddMessage("=========================================================");
	AddMessage(" ");

	delete g_pMsgDisp; g_pMsgDisp = NULL;	//�޼��� ���÷��̸� ���� ���߿� �Ҹ� ���Ѿ� �Ѵ�.
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CABotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CABotDlg::AddMessage(char * i_cMsg, ...)
{
	va_list arglist;

	char arbytebuf[1024];
	char arbyteMsg[1024];

	va_start(arglist, i_cMsg);
	_vstprintf_s(arbytebuf, i_cMsg, arglist);
	_stprintf_s(arbyteMsg, "%s", arbytebuf);

	return g_pMsgDisp->AddMessage(arbyteMsg);
}

BOOL CABotDlg::AddMessage(CString i_strMsg)
{
	return g_pMsgDisp->AddMessage(i_strMsg);
}

void CABotDlg::InitFIDName()
{
	m_mapFIDName.SetAt(_T("9201"), _T("���¹�ȣ"));
	m_mapFIDName.SetAt(_T("9203"), _T("�ֹ���ȣ"));
	m_mapFIDName.SetAt(_T("9001"), _T("�����ڵ�"));
	m_mapFIDName.SetAt(_T("913"), _T("�ֹ�����"));
	m_mapFIDName.SetAt(_T("302"), _T("�����"));
	m_mapFIDName.SetAt(_T("900"), _T("�ֹ�����"));
	m_mapFIDName.SetAt(_T("901"), _T("�ֹ�����"));
	m_mapFIDName.SetAt(_T("902"), _T("��ü�����"));
	m_mapFIDName.SetAt(_T("903"), _T("ü�ᴩ��ݾ�"));
	m_mapFIDName.SetAt(_T("904"), _T("���ֹ���ȣ"));
	m_mapFIDName.SetAt(_T("905"), _T("�ֹ�����"));
	m_mapFIDName.SetAt(_T("906"), _T("�Ÿű���"));
	m_mapFIDName.SetAt(_T("907"), _T("�ŵ�������"));
	m_mapFIDName.SetAt(_T("908"), _T("�ֹ�/ü��ð�"));
	m_mapFIDName.SetAt(_T("909"), _T("ü���ȣ"));
	m_mapFIDName.SetAt(_T("910"), _T("ü�ᰡ"));
	m_mapFIDName.SetAt(_T("911"), _T("ü�ᷮ"));
	m_mapFIDName.SetAt(_T("10"), _T("���簡"));
	m_mapFIDName.SetAt(_T("27"), _T("(�ֿ켱)�ŵ�ȣ��"));
	m_mapFIDName.SetAt(_T("28"), _T("(�ֿ켱)�ż�ȣ��"));
	m_mapFIDName.SetAt(_T("914"), _T("����ü�ᰡ"));
	m_mapFIDName.SetAt(_T("915"), _T("����ü�ᷮ"));
	m_mapFIDName.SetAt(_T("919"), _T("�źλ���"));
	m_mapFIDName.SetAt(_T("920"), _T("ȭ���ȣ"));

	m_mapFIDName.SetAt(_T("917"), _T("�ſ뱸��"));
	m_mapFIDName.SetAt(_T("916"), _T("������"));
	m_mapFIDName.SetAt(_T("930"), _T("��������"));
	m_mapFIDName.SetAt(_T("931"), _T("���Դܰ�"));
	m_mapFIDName.SetAt(_T("932"), _T("�Ѹ��԰�"));
	m_mapFIDName.SetAt(_T("933"), _T("�ֹ����ɼ���"));
	m_mapFIDName.SetAt(_T("945"), _T("���ϼ��ż�����"));
	m_mapFIDName.SetAt(_T("946"), _T("�ŵ�/�ż�����"));
	m_mapFIDName.SetAt(_T("950"), _T("�����Ѹŵ�����"));
	m_mapFIDName.SetAt(_T("951"), _T("������"));
	m_mapFIDName.SetAt(_T("307"), _T("���ذ�"));
	m_mapFIDName.SetAt(_T("8019"), _T("������"));
	m_mapFIDName.SetAt(_T("957"), _T("�ſ�ݾ�"));
	m_mapFIDName.SetAt(_T("958"), _T("�ſ�����"));
	m_mapFIDName.SetAt(_T("918"), _T("������"));
	m_mapFIDName.SetAt(_T("990"), _T("���Ͻ�������(����"));
	m_mapFIDName.SetAt(_T("991"), _T("���Ͻ������ͷ�(����"));
	m_mapFIDName.SetAt(_T("992"), _T("���Ͻ�������(�ſ�"));
	m_mapFIDName.SetAt(_T("993"), _T("���Ͻ������ͷ�(�ſ�"));

	m_mapFIDName.SetAt(_T("397"), _T("�Ļ���ǰ�ŷ�����"));
	m_mapFIDName.SetAt(_T("305"), _T("���Ѱ�"));
	m_mapFIDName.SetAt(_T("306"), _T("���Ѱ�"));
}


void CABotDlg::OnBnClickedShowMessage()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (g_pMsgDisp) { g_pMsgDisp->ShowWindow(g_pMsgDisp->IsWindowVisible() ? SW_HIDE : SW_SHOW); }
}
BEGIN_EVENTSINK_MAP(CABotDlg, CDialogEx)
	ON_EVENT(CABotDlg, IDC_KHOPENAPICTRL1, 5, CABotDlg::OnEventConnect, VTS_I4)
	ON_EVENT(CABotDlg, IDC_KHOPENAPICTRL1, 3, CABotDlg::OnReceiveErrorMsg, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CABotDlg, IDC_KHOPENAPICTRL1, 4, CABotDlg::OnReceiveChejanData, VTS_BSTR VTS_I4 VTS_BSTR)
	ON_EVENT(CABotDlg, IDC_KHOPENAPICTRL1, 2, CABotDlg::OnReceiveRealData, VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CABotDlg, IDC_KHOPENAPICTRL1, 1, CABotDlg::OnReceiveTrData, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CABotDlg, IDC_KHOPENAPICTRL1, 8, CABotDlg::OnReceiveTrCondition, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

//*******************************************************************/
//! Function Name : OnEventConnect
//! Function      : ���� �뺸 ó��
//! Param         : long nErrCode
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CABotDlg::OnEventConnect(long nErrCode)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AddMessage("���� ���� %s. [%d]", (nErrCode== 0?"����":"����"), nErrCode);
	if (nErrCode == 0)
	{
		int nGetConditionRet = theApp.m_khOpenApi.GetConditionLoad();
		AddMessage("�˻� ���� ��ȸ %s. [%d]", (nGetConditionRet>=0?"����":"����"), nGetConditionRet);
	}
}
//*******************************************************************/
//! Function Name : OnReceiveErrorMsg
//! Function      : ��ȸ ���� ó��
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveErrorMsg(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AddMessage("��ȸ����,��ũ����ȣ[%s],RQName[%s],TrCode[%s],�޽���[%s]", sScrNo, sRQName, sTrCode, sMsg);
}


void CABotDlg::OnBnClickedBalanceQuery()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	REQ_DepositReceived();
}

BOOL CABotDlg::REQ_DepositReceived()
{
	m_strAccNo = "";
	CString strAccNo;
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->GetWindowText(strAccNo);
	strAccNo.Remove('-');
	if (strAccNo.GetLength() != 10)
	{
		AfxMessageBox("���¹�ȣ 10�ڸ� �Է� �� �ּ���~!");
		((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->SetFocus();
		return FALSE;
	}
	m_strAccNo = strAccNo;
	theApp.m_khOpenApi.SetInputValue("���¹�ȣ", m_strAccNo);
	//	theApp.m_khOpenApi.SetInputValue("��й�ȣ", _T("0000"));
	theApp.m_khOpenApi.SetInputValue("��й�ȣ�Է¸�ü����", _T("00"));
	theApp.m_khOpenApi.SetInputValue("��ȸ����", _T("1"));

	long lRet = theApp.m_khOpenApi.CommRqData(_T("������"), _T("OPW00001"), 0, m_strScrNo);
	AddMessage("������ ��ȸ ��û %s. [%d]", (lRet >= 0 ? "����" : "����"), lRet);
	return (lRet >= 0 ? TRUE : FALSE);
}

/*
void CABotDlg::RealData()
{
	static bool btoggle = true;
	CString strScrNo = "7777";
	CString strCode = "039490";
	CString strIndex;
	// �����//
	CString strCodeName;
	strCodeName = theApp.m_khOpenApi.GetMasterCodeName(strCode);

	if (btoggle)
	{
		long lRet = theApp.m_khOpenApi.SetRealReg(strScrNo, strCode, FIDLIST, "1");
		AddMessage("BalanceQuery Requested ret[%d]", lRet);
		if (!m_mapJongCode.Lookup(strCode, strIndex))
		{
			m_mapJongCode.SetAt(strCode, strCodeName);
		}
	}
	else
	{
		theApp.m_khOpenApi.SetRealRemove(strScrNo, strCode);
		AddMessage("BalanceQueryRemove Requested");
		if (m_mapJongCode.Lookup(strCode, strIndex))
		{
			m_mapJongCode.RemoveKey(strCode);
		}
	}
	btoggle = btoggle?false:true;
}
*/

//*******************************************************************/
//! Function Name : OnReceiveChejanData
//! Function      : ü���ܰ� �ǽð� ó��
//! Param         : LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveChejanData(LPCTSTR sGubun, long nItemCnt, LPCTSTR sFIdList)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AddMessage(_T("OnReceiveChejanData::"));
	CString strGubun(sGubun), strFidList(sFIdList), strText;
	CString	strAccNo, strOrdNo, strOrdPrice, strOrdCnt;

	int			nIndex(0);
	LONG		lFid = 0;
	CString		strFID, strOneData, strFIDName;
	while (AfxExtractSubString(strFID, strFidList, nIndex++, _T(';')))
	{
		if (m_mapFIDName.Lookup(strFID, strFIDName) == FALSE)
			strFIDName = strFID;

		if (strFIDName.IsEmpty())	strFIDName = strFID;

		if (m_eProcessState != ePST_IDLE)
		{
			if (strFID == "9203")
			{
				CString strOrderCode;
				int nItemIndex = 0;
				strOrderCode = theApp.m_khOpenApi.GetChejanData(atoi(strFID)).Trim();
				if (m_OrderCodeMap.Lookup(strOrderCode, nItemIndex))
				{
					long lQuantity = 0, lPrice = 0;
					CABotItem &aItem = m_Item[nItemIndex];

					strOneData = theApp.m_khOpenApi.GetChejanData(911).Trim();
					lQuantity = atoi(strOneData);

					strOneData = theApp.m_khOpenApi.GetChejanData(910).Trim();
					lPrice = atoi(strOneData);
					
					if (aItem.m_eitemState == eST_WAITBUY)
					{
						aItem.m_lBuyCost += lPrice*lQuantity;
						aItem.m_lBuyQuantity += lQuantity;

						AddMessage(_T("����[%d], ����[%s][%s],�ܰ�[%d],����[%d],�ܷ�[%d], �ż� �Ǿ����ϴ�."),
							m_nRoundCount, aItem.m_strCode, aItem.m_strName, lPrice, lQuantity, aItem.m_lQuantity - aItem.m_lBuyQuantity);

						if (aItem.m_lBuyQuantity >= aItem.m_lQuantity)
						{
						//	m_OrderCodeMap.RemoveKey(strOrderCode);
							aItem.m_lholdTime = GetTickCount();
							aItem.m_lholdTimeout = (m_lItemHoldTimeout>0?aItem.m_lholdTime + m_lItemHoldTimeout:0);
							aItem.m_eitemState = eST_HOLDING;
							AddMessage(_T("����[%d], ����[%s][%s],��մܰ�[%d],����[%d],��õ�[%d], �ż� �Ϸ� �Ǿ����ϴ�."),
								m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.BuyPrice(), aItem.m_lQuantity, aItem.m_ltryBuyCount);
						}
					}
					else if (aItem.m_eitemState == eST_WAITSELL)
					{
						aItem.m_lSellCost += lPrice*lQuantity;
						aItem.m_lSellQuantity += lQuantity;

						AddMessage(_T("����[%d], ����[%s][%s],�ܰ�[%d],����[%d],�ܷ�[%d], �ŵ� �Ǿ����ϴ�."),
							m_nRoundCount, aItem.m_strCode, aItem.m_strName, lPrice, lQuantity, aItem.m_lQuantity - aItem.m_lSellQuantity);

						if (aItem.m_lSellQuantity >= aItem.m_lQuantity)
						{
						//	m_OrderCodeMap.RemoveKey(strOrderCode);
							aItem.m_eitemState = eST_TRADEDONE;
							AddMessage(_T("����[%d], ����[%s][%s],��մܰ�[%d],����[%d],��õ�[%d], �ŷ� �Ϸ� �Ǿ����ϴ�."),
								m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.SellPrice(), aItem.m_lQuantity, aItem.m_ltryBuyCount);
						}
					}
				}
			}
		}
		else
		{
			strOneData = theApp.m_khOpenApi.GetChejanData(atoi(strFID)).Trim();

			strText.Format(_T("����[%s] FID[%4s:%s] = [%s]"), strGubun, strFID, strFIDName, strOneData);
			AddMessage(strText);
			strFIDName.Empty();
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveRealDataKhopenapictrl
//! Function      : �ǽð� ó��
//! Param         : LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveRealData(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	CString strIndex;

	if (!m_mapJongCode.Lookup(sJongmokCode, strIndex))
	{
		return;
	}

	int nItemIndex = -1;
	if (!m_ItemCodeMap.Lookup(sJongmokCode, nItemIndex))
	{
		nItemIndex = -1;
	}

	AddMessage(_T("OnReceiveRealData::"));
	CString strReceivedData;
	CString strData;
	CStringArray arrData;

	arrData.Add(sJongmokCode);
	int i, nFieldCnt = sizeof(lstOPTSBFID) / sizeof(*lstOPTSBFID);		// ��üũ�� / ����ũ�� = ���Ұ���
	for (i = 1; i < nFieldCnt; i++)
	{
		if (atoi(lstOPTSBFID[i].strRealKey) < 0)
		{
			arrData.Add("");
			continue;
		}
		strData = theApp.m_khOpenApi.GetCommRealData(sJongmokCode, atoi(lstOPTSBFID[i].strRealKey));
		strData.Trim();
		arrData.Add(strData);
		strReceivedData += lstOPTSBFID[i].strKey;
		strReceivedData += _T(":");
		strReceivedData += strData;
		if (i < nFieldCnt - 2) { strReceivedData += _T(" ,"); }

		if (nItemIndex>0 && nFieldCnt==2)
		{
			m_Item[nItemIndex].m_lcurPrice = atol(strData);
		}
	}
	AddMessage(strReceivedData);
	SetDataRealAddGrid(arrData, sRealType);
}


//*******************************************************************/
//! Function Name : ConvDataFormat
//! Function      : ���ڿ� ���� ����
//! Param         : int nType, LPCTSTR szData, LPCTSTR szBeforeData/* = ""*/, LPCTSTR szAfterData/* = ""*/
//! Return        : CString
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
CString ConvDataFormat(int nType, LPCTSTR szData, LPCTSTR szBeforeData/* = ""*/, LPCTSTR szAfterData/* = ""*/)
{
	CString strReturn, strData, strTemp = strData = szData;
	switch (nType)
	{
	case DT_DATE:			// ����
	{
			if (strTemp.GetLength() == 6)
			{
				strData.Format("%02s/%02s/%02s", strTemp.Left(2), strTemp.Mid(2, 2), strTemp.Right(2));
			}
			else if (strTemp.GetLength() == 8)
			{
				strData.Format("%04s/%02s/%02s", strTemp.Left(4), strTemp.Mid(4, 2), strTemp.Right(2));
			}
	}
		break;
	case DT_TIME:			// �ð�
	{
			if (strTemp.GetLength() == 6)
			{
				strData.Format("%02s:%02s:%02s", strTemp.Left(2), strTemp.Mid(2, 2), strTemp.Right(2));
			}
			else if (strTemp.GetLength() == 8)
			{
				strData.Format("%02s:%02s:%02s:%02s", strTemp.Left(2), strTemp.Mid(2, 2), strTemp.Mid(4, 2), strTemp.Right(2));
			}
	}
		break;
	case DT_NUMBER:			// ����
	case DT_ZERO_NUMBER:	// ����(0ǥ��)
	{
			strTemp.Replace("+", "");
			if (atof(strTemp) == 0.00)
			{
				strData = nType == DT_ZERO_NUMBER ? strTemp : "";
				break;
			}

			int nFind = strTemp.Find(".");
			if (nFind < 0)
			{
				strData = strTemp;
			}
			else
			{
				strData = strTemp.Left(nFind);
				strTemp = strTemp.Mid(nFind);
			}

			TCHAR szFormatData[1024] = { 0, };
			NUMBERFMT fmt = { 0, 0, 3, _T("."), _T(","), 1 };
			::GetNumberFormat(NULL, 0, strData, &fmt, szFormatData, 1024);

			if (nFind < 0)
			{
				nType == DT_ZERO_NUMBER ? strData.Format("%01s", szFormatData) : strData.Format("%s", szFormatData);
			}
			else
			{
				strData.Format("%01s%s", szFormatData, strTemp);
			}
	}
		break;
	}

	strReturn.Format("%s%s%s", szBeforeData, strData, szAfterData);

	return strReturn;
}

//*******************************************************************/
//! Function Name : SetSignData
//! Function      : �׸��� �� ���ڻ� ���� �� ��ȣ ������ ����
//! Param         : CGridCtrl *pGrid, int nRow, int nCol, LPCTSTR szData
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void SetSignData(CGridCtrl *pGrid, int nRow, int nCol, LPCTSTR szData)
{
	CString strData = szData;
	switch (atoi(strData))
	{
	case 1:
		strData = "��";
		pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// ������ ���� �ؽ�Ʈ ���� ����
		break;
	case 2:
		strData = "��";
		pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// ������ ���� �ؽ�Ʈ ���� ����
		break;
	case 3:	strData = "";		break;
	case 4:
		strData = "��";
		pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// ������ ���� �ؽ�Ʈ ���� ����
		break;
	case 5:
		strData = "��";
		pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// ������ ���� �ؽ�Ʈ ���� ����
		break;
	}
	pGrid->SetItemText(nRow, nCol, strData);
}

//*******************************************************************/
//! Function Name : SetDataFgColour
//! Function      : �׸��� �� ���ڻ� ����
//! Param         : CGridCtrl *pGrid, int nRow, int nCol, LPCTSTR szData
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void SetDataFgColour(CGridCtrl *pGrid, int nRow, int nCol, LPCTSTR szData)
{
	CString strData = szData;
	if (atof(strData) > 0)
	{
		pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// ������ ���� �ؽ�Ʈ ���� ����
	}
	else if (atof(strData) < 0)
	{
		pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// ������ ���� �ؽ�Ʈ ���� ����
	}
}


//*******************************************************************/
//! Function Name : SetDataRealAddGrid
//! Function      : �׸��� ����Ÿ ����(������������ ����)
//! Param         : CStringArray &arrData, CString strRealType/* = ""*/
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::SetDataRealAddGrid(CStringArray &arrData, CString strRealType/* = ""*/)
{
	// �����ڵ忡 �´� �� ã��
	CString strData, strTemp;
	strData = arrData.GetAt(0);
	if (!m_mapJongCode.Lookup(strData, strTemp))
	{
		return;
	}

	int i, nRow = atol(strTemp), nCnt = arrData.GetSize();

	// ����Ÿ ����
	CString strTempData;
	for (i = 0; i < nCnt; i++)
	{
		strData = arrData.GetAt(i);
		/////////////////////////////////////////////
		// ���� �� ������ ���� ó������������
		if (strRealType != "" && (i == 2 || i == 4 || i == 5))
		{
			strTempData = strData;
			strTempData.Replace("+", "");	strTempData.Replace("-", "");	strTempData.Replace(".", "");

			strTemp = m_grdRealAdd.GetItemText(nRow, lstOPTSBFID[i].nCol);
			strTemp.Replace("%", "");	strTemp.Replace("+", "");	strTemp.Replace("-", "");
			strTemp.Replace(",", "");	strTemp.Replace(".", "");
			if (strTempData != strTemp)
			{
				m_cellSelect.col = lstOPTSBFID[i].nCol;
				m_grdRealAdd.SetFocusCell(m_cellSelect);
				m_grdRealAdd.SetSelectedRange(nRow, m_cellSelect.col, nRow, m_cellSelect.col);
				m_grdRealAdd.Invalidate();
			}
		}
		// ���� �� ������ ���� ó������������
		/////////////////////////////////////////////
		if (lstOPTSBFID[i].bTextColor)
		{
			strTemp = arrData.GetAt(3);
			if (strTemp == "1" || strTemp == "2")	// ��ȣ�� ���� ���󺯰�
			{
				strTemp = "1";
			}
			else if (strTemp == "4" || strTemp == "5")	// ��ȣ�� ���� ���󺯰�
			{
				strTemp = "-1";
			}
			else
			{
				strTemp = "0";
			}
			SetDataFgColour(&m_grdRealAdd, nRow, lstOPTSBFID[i].nCol, strTemp);
		}
		if (lstOPTSBFID[i].nDataType == DT_SIGN)
		{
			if (strData.IsEmpty())
			{
				continue;
			}
			SetSignData(&m_grdRealAdd, nRow, lstOPTSBFID[i].nCol, strData);
		}
		else
		{
			if (strData.IsEmpty())
			{
				continue;
			}
			int nCol = lstOPTSBFID[i].nCol;
			m_grdRealAdd.SetItemText(nRow, lstOPTSBFID[i].nCol, ConvDataFormat(lstOPTSBFID[i].nDataType, strData, lstOPTSBFID[i].strBeforeData, lstOPTSBFID[i].strAfterData));
		}
	}
	m_grdRealAdd.Invalidate();
}

//*******************************************************************/
//! Function Name : SetGridHeight
//! Function      : �׸��� ���� �����Ѵ�.
//! Param         : -
//! Return        : void
//! Create        : 2016/09/13
//! Comment       : �����츶�� �׸��� ����� �پ��ϹǷ� �̸� ���� �Լ��� ����.
//******************************************************************/
void CABotDlg::SetGridHeight(long row, long height)
{
	if (GetWindowsVersion() > eWindows_7) {
		height *= 2;
	}
	m_grdRealAdd.SetRowHeight(row, height);
}

//*******************************************************************/
//! Function Name : SetGridWidth
//! Function      : �׸��� ���� �����Ѵ�.
//! Param         : -
//! Return        : void
//! Create        : 2016/09/13
//! Comment       : �����츶�� �׸��� ����� �پ��ϹǷ� �̸� ���� �Լ��� ����.
//******************************************************************/
void CABotDlg::SetGridWidth(long col, long width/*=0*/)
{
	if (GetWindowsVersion() > eWindows_7) {
		width = (long)((double)width * 1.7);
	}
	m_grdRealAdd.SetColumnWidth(col, width);
}

//*******************************************************************/
//! Function Name : GetWindowsVersion
//! Function      : ������ ������ ������.
//! Param         : -
//! Return        : void
//! Create        : , 2016/09/12
//! Comment       : �� �Լ��� �׸����� ����� ������ �������� �ٸ� 
//					��츦 ó���ϱ� ���ؼ� �߰���.
//******************************************************************/
eWinVersion CABotDlg::GetWindowsVersion()
{
	OSVERSIONINFO osVersionInfo;
	eWinVersion ret = eNotSupportedVersion;

	ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFO));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osVersionInfo);

	if (osVersionInfo.dwMajorVersion == 5 && osVersionInfo.dwMinorVersion == 1)
	{
		ret = eWindowsXP;
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 0)
	{
		ret = eWindows_Vista;
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 1)
	{
		ret = eWindows_7;
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 2)
	{
		ret = eWindows_8;
	}
	else if (osVersionInfo.dwMajorVersion == 6 && osVersionInfo.dwMinorVersion == 3)
	{
		ret = eWindows_8_1;
	}
	else if (osVersionInfo.dwMajorVersion == 10 && osVersionInfo.dwMinorVersion == 0)
	{
		ret = eWindows_10;
	}

	if (ret == eNotSupportedVersion) 
	{
		AddMessage("Can't not support this version.\n\nmajor version=%d\nminor version=%d",
					osVersionInfo.dwMajorVersion,
					osVersionInfo.dwMinorVersion);
	}
	return ret;
}

//*******************************************************************/
//! Function Name : OnReceiveTrData
//! Function      : ��ȸ ���� ó��
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveTrData(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AddMessage(_T("OnReceiveTrData::"));

	CString strRQName = sRQName;
	if (strRQName == _T("������"))		// �ֽı⺻���� ����
	{
		// ������
	//	CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("������"));	strData.Trim();
	//	CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("d+1����������"));	strData.Trim();
		CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("d+2����������"));	strData.Trim();
		m_lDepositReceived = atol((LPSTR)(LPCSTR)strData);
		CString strBuf;
		strBuf.Format("%d", m_lDepositReceived);
		((CEdit*)GetDlgItem(IDC_EDIT_DEPOSITRECEIVED))->SetWindowText(strBuf);
		strBuf = strBuf + " KRW";
		AddMessage(strBuf);
	}
	else if (strRQName == _T("�ֽ��ֹ�"))		// �ֽı⺻���� ����
	{
		CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("�ֹ���ȣ"));	strData.Trim();
		AddMessage(strData);
	}
	else if (strRQName.Find("ORDER_")>0)
	{
		CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("�ֹ���ȣ"));	strData.Trim();
		strRQName.Replace("ORDER_","00");
		long nItemIndex = atol((LPSTR)(LPCSTR)strRQName);
		if (0 <= nItemIndex && nItemIndex < _countof(m_Item))
		{
			CABotItem &aItem = m_Item[nItemIndex];
			// �ֹ� ��ȣ
			if (aItem.m_eitemState == eST_WAITBUY)
			{
				aItem.m_strBuyOrder = strData;
			}
			else if (aItem.m_eitemState == eST_WAITSELL)
			{
				aItem.m_strSellOrder = strData;
			}
			m_OrderCodeMap[strData] = nItemIndex;
			AddMessage(_T("����[%d], ����[%s][%s],code[%s] �ֹ� �Ϸ�."), m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_strBuyOrder);
		}
	}
	else if (strRQName == _T("��������ȸ"))
	{/// 
		CString		strTRCode(sTrCode);
		int		nCnt = theApp.m_khOpenApi.GetRepeatCnt(strTRCode, _T(""));

		if (nCnt < 1)
		{
			AddMessage(_T("������ ��ȸ �ݺ������� ����."));
			return;
		}

		CString		strSymbol, strSymbolName, strLoanDate, strText;
		for (int nIdx = 0; nIdx < nCnt; nIdx++)
		{
			strSymbol = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), nIdx, _T("�����ȣ"));
			strSymbolName = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), nIdx, _T("�����"));
			strLoanDate = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), nIdx, _T("������"));

			strSymbol.Trim();
			if (!strSymbol.IsEmpty())
			{
				strSymbol = strSymbol.Mid(1, 6);
			}
			strSymbolName.Trim();
			strLoanDate.Trim();
			strText.Format(_T("������[%s]    �����ȣ[%s]    �����[%s] \n"), strLoanDate, strSymbol, strSymbolName);
			AddMessage(strText);
		}
	}
	else
	{
		AddMessage("�˼����� �޼��� 1/3,ScrNo[%s],RQName[%s],TrCode[%s]", sScrNo, sRQName, sTrCode);
		AddMessage("�˼����� �޼��� 2/3,sRecordName[%s],sPrevNext[%s],nDataLength[%d]", sRecordName, sPrevNext, nDataLength);
		AddMessage("�˼����� �޼��� 3/3,sErrorCode[%s],sMessage[%s],sSplmMsg[%s]", sErrorCode, sMessage, sSplmMsg);
	}
}



void CABotDlg::OnBnClickedButtonGetfilter()
{
	int nGetConditionRet = theApp.m_khOpenApi.GetConditionLoad();
	if (nGetConditionRet <= 0)
	{
		AddMessage("�˻� ���� ��ȸ ����. ret[%d]", nGetConditionRet);
		return;
	}

	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strConditionName = _T(""), strConditionNameList = _T(""), strCondition = _T(""), strIndex = _T("");

	//���ǰ˻��� ����Ʈ�� �ҷ��´�.
	strConditionNameList = theApp.m_khOpenApi.GetConditionNameList();

	//�޾ƿ� ���ǰ˻����� �޺��� �ִ´�.
	m_cmbCon.ResetContent();

	int i(0);

	while (AfxExtractSubString(strCondition, strConditionNameList, i++, _T(';')))
	{
		if (strCondition != _T(""))
		{
			strCondition.Trim();
			int nPos = strCondition.Find("^");
			int nConditionNo = atoi(strCondition.Left(nPos));
			strConditionName = strCondition.Mid(nPos + 1, strCondition.GetLength());
			strIndex.Format(_T("%d"), nConditionNo);
			m_mapNameList.SetAt(strConditionName, strIndex);
			m_cmbCon.InsertString(i - 1, strConditionName);
		}
	}

	static bool bConfigApplied = false;

	if (m_cmbCon.GetCount() > 0)
	{
		m_cmbCon.SetCurSel(0);

		if (!bConfigApplied)
		{
			CString strBuf, strCombo;
			// �˻� ����
			ReadFromIniFile_String(m_strConfigFile, "FILTER", "name", "-", strBuf);
			for (i = 0; i < m_cmbCon.GetCount(); i++)
			{
				m_cmbCon.GetLBText(i, strCombo);
				if (strCombo == strBuf)
				{
					m_cmbCon.SetCurSel(i);
				}
			}
			bConfigApplied = true;
		}
	}
}


void CABotDlg::OnBnClickedButtonGettarget()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	REQ_ItemSearch();
}

BOOL CABotDlg::REQ_ItemSearch()
{
//	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));
	theApp.m_khOpenApi.SetRealRemove(_T("ALL"), _T("ALL"));

	CString strConditionCodeList = _T(""), strIndex;

	//������ ���Ǹ��� �˻��� ���񸮽�Ʈ�� �ҷ��´�.
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), m_strConditionName);
	m_mapNameList.Lookup(m_strConditionName, strIndex);
	m_nConditionIndex = _ttoi(strIndex);

	//	m_strPreConditionName = m_strConditionName;
	//	m_nPreConditionIndex = m_nConditionIndex;

	//��������, ��Ż �α� ����
	//	m_listCtl_Insert.ResetContent();
	//	m_listCtl_Delete.ResetContent();

	//���˰˻� �ǽð� ��ȸ
	//���񸮽�Ʈ�� OnReceiveTrCondition()�� ���Ͽ� ���´�.
	//�ǽð� ���� �˻��� �ϰ� ������ ������ �Ķ���Ϳ� 1�� �ش�.
	//	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 1);
	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 0);
	AddMessage("���ǽ�[%d][%s], ���� �˻� ��û %s.", m_nConditionIndex, m_strConditionName, (bRet ? "����" : "����"));
	return bRet;
}


void CABotDlg::OnBnClickedButtonRegtarget()
{
	REQ_ItemRealReg();
}

BOOL CABotDlg::REQ_ItemRealReg()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strTargetCodeList;
	int nIndex = 0;
	int nMaxIndex = min(m_ItemCount, _countof(m_Item));
	for (nIndex = 0; nIndex < nMaxIndex; nIndex++)
	{
		strTargetCodeList += m_Item[nIndex].m_strCode + ";";
	}

	long lRet = theApp.m_khOpenApi.SetRealReg(m_strScrNo, strTargetCodeList, FIDLIST, "1");
	AddMessage("�˻��� ����鿡 ���� �ǽð� ��ȸ ��û %s. ret[%d]", (lRet >= 0 ? "����" : "����"), lRet);
	return (lRet >= 0 ? TRUE : FALSE);
}

//*******************************************************************/
//! Function Name	: OnReceiveTrCondition
//! Function			: ���ǰ˻� ���񸮽�Ʈ ��ȸ����
//! Param				: LPCTSTR sScrNo					- ȭ���ȣ
//!						: PCTSTR strCodeList				- ���񸮽�Ʈ
//!						: LPCTSTR strConditionName			- ���Ǹ�
//!						: int nConditionIndex				- ���Ǹ� �ε�����ȣ
//!						: int nNext							- ������ȸ(2:������ȸ, 0:������ȸ����)
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveTrCondition(LPCTSTR sScrNo, LPCTSTR strCodeList, LPCTSTR strConditionName, long nConditionIndex, long nNext)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AddMessage(_T("OnReceiveTrCondition::"));

	CString strCndName, sConditionName;
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strCndName);	//���õ� ���Ǹ�

	if (strConditionName == strCndName)	//���� ���Ǹ�� ��ȸ�������� ���� ���Ǹ��� �������� ó��.
	{
		AddMessage("���ǽ�[%d][%s], ���� �˻� ��û ���,", nConditionIndex, strConditionName);

		if (strlen(strCodeList)>0)
		{
			int nIndex(0);
			CString strConditionCode;
			m_mapJongCode.RemoveAll();		// �����ڵ� �ʱ�ȭ
			m_grdRealAdd.SetRowCount(1);

			while (AfxExtractSubString(strConditionCode, strCodeList, nIndex++, _T(';')))
			{
			}

			m_grdRealAdd.SetRowCount(nIndex - 1);

			m_ItemCount = 0;
			m_ItemCodeMap.RemoveAll();

			nIndex = 0;
			CString strIndex;
			while (AfxExtractSubString(strConditionCode, strCodeList, nIndex++, _T(';')))
			{
				if (strConditionCode != "")
				{
					SetGridHeight(nIndex, 24); // grid ���̴� ������ ���� �����ϹǷ� ���� �Լ��� ȣ���Ѵ�.
					m_grdRealAdd.SetItemText(nIndex, 0, strConditionCode);

					// �����
					CString strCodeName;
					strCodeName = theApp.m_khOpenApi.GetMasterCodeName(strConditionCode);
					m_grdRealAdd.SetItemText(nIndex, 1, strCodeName);

					strIndex.Format("%3d", nIndex);
					m_mapJongCode.SetAt(strConditionCode, strIndex);
					AddMessage(_T("[%s][%s][%s] �˻���."), strIndex, strConditionCode, strCodeName);

					if (m_ItemCount < _countof(m_Item))
					{
						m_Item[m_ItemCount].m_eitemState = eST_ADDED;
						m_Item[m_ItemCount].m_strName = strCodeName;
						m_Item[m_ItemCount].m_strCode = strConditionCode;
						m_ItemCodeMap[m_Item[m_ItemCount].m_strCode] = m_ItemCount;
						m_ItemCount++;
					}
				}
			}
		}
		else
		{
			m_mapJongCode.RemoveAll();		// �����ڵ� �ʱ�ȭ
			m_grdRealAdd.SetRowCount(1);

			AddMessage(_T("�˻��� ������ �����ϴ�."));
		}
	}
}

void CABotDlg::SetControls(BOOL bEnable)
{
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->EnableWindow(bEnable);
	m_cmbCon.EnableWindow(bEnable);
	m_cmbRoundCount.EnableWindow(bEnable);
	m_cmbStartHour.EnableWindow(bEnable);
	m_cmbStartMin.EnableWindow(bEnable);
	m_cmbEndHour.EnableWindow(bEnable);
	m_cmbEndMin.EnableWindow(bEnable);
	m_cmbDpUseRate.EnableWindow(bEnable);
	m_cmbMaxTotalAmount.EnableWindow(bEnable);
	m_cmbMaxAmount.EnableWindow(bEnable);
	m_cmbBuyMethod.EnableWindow(bEnable);
	m_cmbBuyTimeOut.EnableWindow(bEnable);
	m_cmbBuyRetry.EnableWindow(bEnable);
	m_cmbHoldTimeOut.EnableWindow(bEnable);
	m_cmbSellOverThis.EnableWindow(bEnable);
	m_cmbSellOverThis2.EnableWindow(bEnable);
	m_cmbSellUnderThis.EnableWindow(bEnable);
	m_cmbSellUnderThis2.EnableWindow(bEnable);
}

void CABotDlg::OnBnClickedButtonStartRound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_eProcessState == ePST_IDLE)
	{
		SetDisableControls();
		m_nRoundCount = 1;
		m_bDoFinishProcess = FALSE;
		m_eProcessState = ePST_ROUND_START;
		SetTimer(ROUND_TIMER, 1, NULL);
		AddMessage(_T("���� ���� ��û."));
	}
}


void CABotDlg::OnBnClickedButtonFinishRound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_eProcessState <= ePST_ITEM_WAIT)
	{
		SetEnableControls();
		KillTimer(ROUND_TIMER);
		m_eProcessState = ePST_IDLE;
		AddMessage(_T("���� �ߴ�."));
	}
	else
	{
		m_bDoFinishProcess = TRUE;
		AddMessage(_T("�ŷ��� �̹� ���۵Ǿ����ϴ�. ���� ���� ��û."));
	}
}


void CABotDlg::OnBnClickedButtonStopRound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_eProcessState <= ePST_ITEM_WAIT)
	{
		SetEnableControls();
		KillTimer(ROUND_TIMER);
		m_eProcessState = ePST_IDLE;
		AddMessage(_T("���� �ߴ�."));
	}
	else
	{
		m_bDoFinishProcess = TRUE;
		AddMessage(_T("�ŷ��� �̹� ���۵Ǿ����ϴ�. ���� ���� ��û."));
	}
}


void CABotDlg::OnBnClickedButtonSellAllCurCost()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	AddMessage(_T("��ü ���� ���� ���簡�� �ŵ� ��û."));	
}

BOOL CABotDlg::IsInRoundTime()
{
	SYSTEMTIME  systmCurTime;
	GetLocalTime(&systmCurTime);

	int	iHour = systmCurTime.wHour;
	int	iMin = systmCurTime.wMinute;

	CString strBuf;
	m_cmbStartHour.GetLBText(m_cmbStartHour.GetCurSel(), strBuf);
	int iSHour = atoi((LPSTR)(LPCSTR)strBuf);
	
	m_cmbStartMin.GetLBText(m_cmbStartMin.GetCurSel(), strBuf);
	int iSMin = atoi((LPSTR)(LPCSTR)strBuf);

	m_cmbEndHour.GetLBText(m_cmbEndHour.GetCurSel(), strBuf);
	int iEHour = atoi((LPSTR)(LPCSTR)strBuf);

	m_cmbEndMin.GetLBText(m_cmbEndMin.GetCurSel(), strBuf);
	int iEMin = atoi((LPSTR)(LPCSTR)strBuf);

	m_cmbRoundCount.GetLBText(m_cmbRoundCount.GetCurSel(), strBuf);
	int iRoundCount = atoi((LPSTR)(LPCSTR)strBuf);

	if (iRoundCount <= m_nRoundCount &&
		iSHour <= iHour && iSMin <= iMin)
	{
		if ( (iHour < iEHour) || (iHour == iEHour && iMin < iEMin) )
		{
			return TRUE;
		}
	}
	return FALSE;
}
void CABotDlg::LoadProcessCondition()
{
	CString strBuf;
	m_cmbDpUseRate.GetLBText(m_cmbDpUseRate.GetCurSel(), strBuf);
	int nUseRate = atoi((LPSTR)(LPCSTR)strBuf);

	m_cmbMaxTotalAmount.GetLBText(m_cmbMaxTotalAmount.GetCurSel(), strBuf);
	long lMaxTotalAmount = atoi((LPSTR)(LPCSTR)strBuf) * 10000;

	m_lProcessDR = min(long(m_lDepositReceived*(nUseRate / 100.0)), lMaxTotalAmount);
	AddMessage(_T("����[%d], ��� �ݾ� ������ %d[��] �Դϴ�."), m_nRoundCount, m_lProcessDR);

	m_cmbBuyTimeOut.GetLBText(m_cmbBuyTimeOut.GetCurSel(), strBuf);
	m_lItemBuyTimeout = atoi((LPSTR)(LPCSTR)strBuf) * 1000;
	AddMessage(_T("����[%d], ����� �ż� timeout�� %d[clock] �Դϴ�."), m_nRoundCount, m_lItemBuyTimeout);

	m_cmbBuyRetry.GetLBText(m_cmbBuyRetry.GetCurSel(), strBuf);
	m_lItemBuyTryCount = atoi((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("����[%d], ����� �ż� ��õ� ȸ���� %d[ȸ] �Դϴ�."), m_nRoundCount, m_lItemBuyTryCount);

	m_cmbHoldTimeOut.GetLBText(m_cmbHoldTimeOut.GetCurSel(), strBuf);
	m_lItemHoldTimeout = atoi((LPSTR)(LPCSTR)strBuf) * 1000;
	AddMessage(_T("����[%d], ����� ���� timeout�� %d[clock] �Դϴ�."), m_nRoundCount, m_lItemHoldTimeout);

	m_cmbSellOverThis.GetLBText(m_cmbSellOverThis.GetCurSel(), strBuf);
	m_dSellOverThis = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("����[%d], ����� �ŵ� Sell'OVER'This�� %f[�ۼ�Ʈ] �Դϴ�."), m_nRoundCount, m_dSellOverThis);

	m_cmbSellOverThis2.GetLBText(m_cmbSellOverThis2.GetCurSel(), strBuf);
	m_dSellOverThis2 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("����[%d], ����� �ŵ� Sell'OVER'This2�� %f[�ۼ�Ʈ] �Դϴ�."), m_nRoundCount, m_dSellOverThis2);

	m_cmbSellOverThis.GetLBText(m_cmbSellOverThis.GetCurSel(), strBuf);
	m_dSellUnderThis = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("����[%d], ����� �ŵ� Sell'UNDER'This�� %f[�ۼ�Ʈ] �Դϴ�."), m_nRoundCount, m_dSellUnderThis);

	m_cmbSellOverThis2.GetLBText(m_cmbSellOverThis2.GetCurSel(), strBuf);
	m_dSellUnderThis2 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("����[%d], ����� �ŵ� Sell'UNDER'This2�� %f[�ۼ�Ʈ] �Դϴ�."), m_nRoundCount, m_dSellUnderThis2);

}

void CABotDlg::ProcessSequence()
{
	switch (m_eProcessState)
	{
	case ePST_ROUND_START:	//���� ���� ����.
		m_OrderCodeMap.RemoveAll();
		m_ItemCodeMap.RemoveAll();
		m_nProcessRetryCount = 0;
		LoadProcessCondition();
		AddMessage(_T("����[%d], ���� �մϴ�."), m_nRoundCount);
		m_eProcessState = ePST_WAIT_TIME;
		break;

	case ePST_WAIT_TIME:	//���� ���� ���� �ð��� ��ٸ��� ����.
		m_nProcessRetryCount++;
		if (IsInRoundTime())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����[%d], ������ �˻��� ���� �մϴ�."), m_nRoundCount);
			m_eProcessState = ePST_DR_SEARCH;
			break;
		}
		break;

	case  ePST_DR_SEARCH:	//������ ��û ����.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) {	m_eProcessState = ePST_ROUND_END; break; }	//���� ���� �ð� Ȯ��

		m_lDepositReceived = -1;
		if (REQ_DepositReceived())
		{
			m_nProcessRetryCount = 0;
			m_eProcessState = ePST_DR_WAIT;
			break;
		}

		if (m_nProcessRetryCount > 10)
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����[%d], ������ ��ȸ�� ���� �����Ͽ� ���μ����� �ߴ��մϴ�."), m_nRoundCount);
			m_eProcessState = ePST_KILL_PROC;
			break;
		}
		break;

	case ePST_DR_WAIT:		//������ ��� ��� ����.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//���� ���� �ð� Ȯ��

		if (m_lDepositReceived == 0)
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����[%d], ������ %d[��], ���带 �����մϴ�."), m_nRoundCount, m_lDepositReceived);
			m_eProcessState = ePST_ROUND_END;
			break;
		}
		else if (m_lDepositReceived > 0)
		{
			m_ItemCount = -1;
			int i = 0;
			for (i = 0; i<_countof(m_Item); i++)
			{
				m_Item[i].m_index = i;
				m_Item[i].Init();
			}
			m_nProcessRetryCount = 0;
			AddMessage(_T("����[%d], ���� �˻��� ���� �մϴ�."), m_nRoundCount);
			m_eProcessState = ePST_ITEM_SEARCH;
			break;
		}
		break;

	case ePST_ITEM_SEARCH:	//���� �˻� ��û ����.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//���� ���� �ð� Ȯ��

		if (m_cmbCon.GetCount()<=0)
		{
			OnBnClickedButtonGetfilter();
			break;
		}

		if (REQ_ItemSearch())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����[%d], ���� ��û �Ϸ�. ����� ��ٸ��ϴ�."), m_nRoundCount);
			m_eProcessState = ePST_ITEM_WAIT;
			break;
		}
		else
		{
			//�˻� ��û ���н�. ȭ�� ��ȣ�� �ٲ㺸��,. �׷��� �ҿ� ����.
			m_strScrNo.Format("%04d", rand() % 10000);
			Sleep(min(2000, m_nProcessRetryCount*10));
		}
		break;

	case ePST_ITEM_WAIT:	//���� �˻� ��� ��� ����.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//���� ���� �ð� Ȯ��

		if (m_ItemCount == -1)
		{
			break;
		}
		AddMessage(_T("����[%d], [%d]���� ������ �˻��Ǿ����ϴ�."), m_nRoundCount, m_ItemCount);

		if (m_ItemCount == 0)
		{
			m_ItemCount = -1;
			m_nProcessRetryCount = 0;
			AddMessage(_T("����[%d], ���� �˻��� �ٽ� ���� �մϴ�."), m_nRoundCount);
			m_eProcessState = ePST_ITEM_SEARCH;
			break;
		}
		
		if (m_ItemCount > 0)
		{
			if (REQ_ItemRealReg())
			{
				CString strBuf;
				m_cmbMaxAmount.GetLBText(m_cmbMaxAmount.GetCurSel(), strBuf);
				long lItemMaxAmount = atoi((LPSTR)(LPCSTR)strBuf) * 10000;

				long lItemProcessAmount = m_lProcessDR / m_ItemCount;
				long m_lProcessItemDR = min(lItemMaxAmount, lItemProcessAmount);
				AddMessage(_T("����[%d], ����� ��� ��� �ݾ��� %d[��] �Դϴ�."), m_nRoundCount, m_lProcessItemDR);

				m_nProcessRetryCount = 0;
				AddMessage(_T("����[%d], ���� �ŷ��� ���� �մϴ�."), m_nRoundCount);
				m_eProcessState = ePST_ITEM_TRADE;
				break;
			}
		}
		break;

	case ePST_ITEM_TRADE:	//���� �ŷ� ����.
		m_nProcessRetryCount++;
		ProcessTrade();

		if (!IsEndTrade())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����[%d], ���� �ŷ��� ��� ���� �Ǿ����ϴ�."), m_nRoundCount);
			m_eProcessState = ePST_ROUND_END;
			break;
		}
		break;

	case ePST_ROUND_END:	//���� 1ȸ ���� ����.
		m_nProcessRetryCount++;
		AddMessage(_T("����[%d], ���� �մϴ�."), m_nRoundCount);
		m_nRoundCount++;
		if ( !IsInRoundTime() )
		{
			m_nProcessRetryCount = 0;
			m_eProcessState = ePST_KILL_PROC;
			break;
		}
		m_eProcessState = ePST_ROUND_START;
		break;

	case ePST_KILL_PROC:
		m_nProcessRetryCount = 0;
		m_nRoundCount = 0;
		SetEnableControls();
		KillTimer(ROUND_TIMER);
		SetWindowText(_T("ABot"));
		m_eProcessState = ePST_IDLE;
		break;

	case ePST_IDLE:	//���� ����.
	default:
		SetWindowText(_T("ABot"));
		break;
	}
	
}

void CABotDlg::ProcessTrade()
{
	int i = 0;
	for (i = 0; i < m_ItemCount; i++)
	{
		CABotItem &aItem = m_Item[i];
		switch (aItem.m_eitemState)
		{
		default:
		case eST_NONE:		//�ƹ��͵� �ƴ� ����.
			break;
		case eST_ADDED:		//���� ���� �߰� �Ǿ�����.
			if (aItem.m_lcurPrice>0)
			{
				aItem.m_eitemState = eST_TRYBUY;
				break;
			}
			break;

		case eST_TRYBUY:	//�ż� �õ� ����.
			if (REQ_ItemBuyOrder(aItem))
			{
				aItem.m_ltryBuyTime = GetTickCount();
				aItem.m_ltryBuyTimeout = GetTickCount() + m_lItemBuyTimeout;
				
				AddMessage(_T("����[%d], ����[%s][%s],�ܰ�[%d],����[%d], �ż��� �õ� �Ǿ����ϴ�."), 
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lbuyPrice, aItem.m_lQuantity);
				aItem.m_eitemState = eST_WAITBUY;
				break;
			}
			break;

		case eST_WAITBUY:	//�ż� ü�� ��� ����.
			if (aItem.m_ltryBuyTime + long(GetTickCount()) > aItem.m_ltryBuyTimeout)
			{
				aItem.m_ltryBuyCount++;
				if (aItem.m_ltryBuyCount > m_lItemBuyTryCount)
				{
					AddMessage(_T("����[%d], ����[%s][%s],�ܰ�[%d],����[%d],��õ�[%d], �ŷ��� ���� �մϴ�."),
						m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_ltryBuyCount);
					if (aItem.m_lBuyQuantity > 0)
					{
						aItem.m_lQuantity = aItem.m_lBuyQuantity;
						aItem.m_eitemState = eST_HOLDING;
						break;
					}
					else
					{
						aItem.m_eitemState = eST_TRADEDONE;
						break;
					}
					break;
				}

				AddMessage(_T("����[%d], ����[%s][%s],�ܰ�[%d],����[%d], �ż��� ��õ�[%d] �Ǿ����ϴ�."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_ltryBuyCount);
				aItem.m_eitemState = eST_TRYBUY;
				break;
			}
			break;

		case eST_HOLDING:	//���� ����.
			if (aItem.m_lcurPrice > long(aItem.BuyPrice() + aItem.BuyPrice()*m_dSellOverThis / 100.))
			{
				AddMessage(_T("����[%d], ����[%s][%s],���簡��[%d],����[%d], �ŵ��� �����մϴ�. ���."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity);
				aItem.m_eitemState = eST_TRYSELL;
				break;
			}
			if (aItem.m_lcurPrice < long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis / 100.))
			{
				AddMessage(_T("����[%d], ����[%s][%s],���簡��[%d],����[%d], �ŵ��� �����մϴ�. �϶�."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity);
				aItem.m_eitemState = eST_TRYSELL;
				break;
			}
			if (aItem.m_lholdTimeout>0 && aItem.m_lholdTime + long(GetTickCount()) > aItem.m_lholdTimeout)
			{
				AddMessage(_T("����[%d], ����[%s][%s],���簡[%d],����[%d], �ŵ��� �����մϴ�. Ÿ�Ӿƿ�."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity);
				aItem.m_eitemState = eST_TRYSELL;
				break;
			}
			break;

		case eST_TRYSELL:	//�ŵ� �õ� ����.
			if (REQ_ItemSellOrder(aItem))
			{
				aItem.m_ltrySellTime = GetTickCount();
				aItem.m_ltrySellTimeout = GetTickCount() + 10000;	//�ȷ��� �ϴµ�, 10���̻� ���ȸ��� �̻��Ѱ���. �׷��� ���尡�� ��.

				AddMessage(_T("����[%d], ����[%s][%s],�ܰ�[%d],����[%d], �ŵ��� �õ� �Ǿ����ϴ�."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity - aItem.m_lSellQuantity);
				aItem.m_eitemState = eST_WAITSELL;
				break;
			}
			break;

		case eST_WAITSELL:	//�ŵ� ü�� ��� ����.
			if (aItem.m_ltrySellTime + long(GetTickCount()) > aItem.m_ltrySellTimeout)
			{
				AddMessage(_T("����[%d], ����[%s][%s],�ܰ�[%d],����[%d], �ŵ��� ���尡�� ��õ� �Ǿ����ϴ�."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity - aItem.m_lSellQuantity);
				aItem.m_eitemState = eST_TRYSELL;
				break;
			}
			break;

		case eST_TRADEDONE:	//�ŷ� �Ϸ� ����.
			break;
		}
	}
}

BOOL CABotDlg::REQ_ItemBuyOrder(CABotItem &aItem)
{
	CString strRQName;
	strRQName.Format("ORDER_%04d", aItem.m_index);

	// �Ÿű��� ���(1:�űԸż�, 2:�űԸŵ� 3:�ż����, 4:�ŵ����, 5:�ż�����, 6:�ŵ�����)
	long lOrderType = 1;
	if (aItem.m_ltryBuyCount > 0)
	{
		lOrderType = 5;	//5:�ż�����
	}

	// �ŷ����� ���
	// 00:������, 03:���尡, 05:���Ǻ�������, 06:������������, 07:�ֿ켱������, 
	// 10:������IOC, 13:���尡IOC, 16:������IOC, 20:������FOK, 23:���尡FOK, 
	// 26:������FOK, 61:�尳�����ð���, 62:�ð��ܴ��ϰ��Ÿ�, 81:���Ľð�������
	CString strHogaGb = "00";
	ASSERT(aItem.m_lcurPrice != 0);
	if (aItem.m_lcurPrice == 0){ return FALSE; }

	aItem.m_lbuyPrice = aItem.m_lcurPrice;
	aItem.m_lQuantity = (m_lProcessItemDR - aItem.m_lBuyCost) / aItem.m_lbuyPrice;

	if (aItem.m_lQuantity == 0)	//���� ���� ������ 0�� ��� ���� ó��.
	{
		if (aItem.m_ltryBuyCount > 0)
		{
			lOrderType = 3;	//3:�ż����
			long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
				lOrderType, aItem.m_strCode,
				aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_lbuyPrice, strHogaGb, aItem.m_strBuyOrder);

			if (lRet >= 0)
			{
				aItem.m_eitemState = eST_TRADEDONE;
				return FALSE;
			}
		}
		else
		{
			AddMessage(_T("����[%d], ����[%s][%s],�ܰ�[%d],����[%d], �ŷ��� ���� �մϴ�."),
				m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity);
			aItem.m_eitemState = eST_TRADEDONE;
			return FALSE;
		}
	}

	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lOrderType, aItem.m_strCode, 
		aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_lbuyPrice, strHogaGb, aItem.m_strBuyOrder);

	return (lRet >= 0 ? TRUE : FALSE);
}

BOOL CABotDlg::REQ_ItemSellOrder(CABotItem &aItem)
{
	CString strRQName;
	strRQName.Format("ORDER_%04d", aItem.m_index);

	// �Ÿű��� ���(1:�űԸż�, 2:�űԸŵ� 3:�ż����, 4:�ŵ����, 5:�ż�����, 6:�ŵ�����)
	long lOrderType = 2;
	if (aItem.m_strSellOrder.GetLength() > 0)
	{
		lOrderType = 6;	//6:�ŵ�����
	}

	// �ŷ����� ���
	// 00:������, 03:���尡, 05:���Ǻ�������, 06:������������, 07:�ֿ켱������, 
	// 10:������IOC, 13:���尡IOC, 16:������IOC, 20:������FOK, 23:���尡FOK, 
	// 26:������FOK, 61:�尳�����ð���, 62:�ð��ܴ��ϰ��Ÿ�, 81:���Ľð�������
	CString strHogaGb = "00";
	ASSERT(aItem.m_lcurPrice != 0);
	if (aItem.m_lcurPrice == 0){ return FALSE; }

	if (aItem.m_ltrySellTime + long(GetTickCount()) > aItem.m_ltrySellTimeout)
	{
		strHogaGb = "03"; //03:���尡,
	}

	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lOrderType, aItem.m_strCode,
		aItem.m_lQuantity - aItem.m_lSellQuantity, aItem.m_lcurPrice, strHogaGb, aItem.m_strBuyOrder);

	return (lRet >= 0 ? TRUE : FALSE);
}

BOOL CABotDlg::IsEndTrade()
{
	int i = 0;
	for (i = 0; i < m_ItemCount; i++)
	{
		if (eST_ADDED <= m_Item[i].m_eitemState &&
			m_Item[i].m_eitemState < eST_TRADEDONE)
		{
			return FALSE;
		}
	}
	return TRUE;
}