
// ABotDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ABot.h"
#include "ABotDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define START_TIMER		(1001)
#define CONDITION_TIMER	(1002)
#define ROUND_TIMER		(1003)


//#define FIDLIST	_T("9001;302;10;11;25;12;13")
#define FIDLIST	_T("9001;10;11;25;12;13")

//�����.
#define DEF_CUR_PRICE	_T("���簡")

//����
//27 (�ֿ켱)�ŵ� ȣ��
//28 (�ֿ켱)�ż� ȣ��
//14 �����ŷ����
//16 �ð�
//17 ��
//18 ����
//21 ȣ��
//26 ���ϰŷ������(���,��)
//29 �ŷ��������
//30 ���ϰŷ������(����)
//31 �ŷ�ȸ����
//32 �ŷ����
//311 �ð��Ѿ�(��)
//567 ���Ѱ��߻��ð�
//568 ���Ѱ��߻��ð�

// {��ȸ Ű,		���� Ű,	��, ��, Ÿ��,			�� ����, ����,		�� ����, �� ����}
const stGRID lstOPTSBFID[] =
{
	{ "�����ڵ�",		"9001",	-1, 0, DT_NONE,			FALSE,	DT_LEFT,		"", "" },
//	{ "�����",			"302",	-1, 1, DT_NONE,			FALSE,	DT_LEFT,		"", "" },
	{ DEF_CUR_PRICE,	"10",	-1, 2, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,		"", "" },
	{ "���ϴ���ȣ",	"25",	-1, 3, DT_SIGN,			TRUE,	DT_CENTER,		"", "" },
	{ "���ϴ��",		"11",	-1, 4, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,		"", "" },
	{ "�����",			"12",	-1, 5, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,		"", "%" },
	{ "�����ŷ���",		"13",	-1, 6, DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,		"", "" },
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
		return "���� ������ ����";
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
	m_eProcessState = ePST_IDLE;
	m_bDoFinishProcess = FALSE;
	m_nProcessRetryCount = 0;
	m_lProcessDR = 0;
	m_lDepositReceived = 0;
	m_ItemCount = 0;
	m_nConditionIndex = 0;
	m_nProcessItemCount = 0;

	m_lItemBuyTimeout = 0;
	m_lItemBuyTryCount = 0;
	m_lItemHoldTimeout = 0;

	m_dSellOverThis0 = 0.;
	m_dSellUnderThis0 = 0.;
	m_dSellOverThis1 = 0.;
	m_dSellUnderThis1 = 0.;
	m_dSellOverThis2 = 0.;
	m_dSellUnderThis2 = 0.;

	m_lItemSellTimeout0 = 0;
	m_lItemSellTimeout1 = 0;

	m_dBuyTradeFee = 0.;
	m_dSellTradeFee = 0.;
	m_bDoSellItemMarketValueAtRoundEnd = FALSE;
}

void CABotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_KHOPENAPICTRL1, theApp.m_khOpenApi);
	DDX_Control(pDX, IDC_GRD_REALADD, m_grdRealAdd);
	DDX_Control(pDX, IDC_GRD_BUY_ITEM, m_grdBuyItem);
	DDX_Control(pDX, IDC_COMBO_CONDITION, m_cmbCondtion);
	DDX_Control(pDX, IDC_COMBO_ITEM_COUNT, m_cmbItemCount);
	DDX_Control(pDX, IDC_COMBO_SHOUR, m_cmbStartHour);
	DDX_Control(pDX, IDC_COMBO_SMIN, m_cmbStartMin);
	DDX_Control(pDX, IDC_COMBO_EHOUR, m_cmbEndHour);
	DDX_Control(pDX, IDC_COMBO_EMIN, m_cmbEndMin);
	DDX_Control(pDX, IDC_COMBO_DP_USE_RATE, m_cmbDpUseRate);
	DDX_Control(pDX, IDC_COMBO_MAX_TOTAL_AMOUNT, m_cmbMaxTotalAmount);
	DDX_Control(pDX, IDC_COMBO_MAX_AMOUNT, m_cmbMaxAmount);
	DDX_Control(pDX, IDC_COMBO_BUY_METHOD, m_cmbBuyMethod);
	DDX_Control(pDX, IDC_COMBO_BUY_RETRY, m_cmbBuyRetry);
	DDX_Control(pDX, IDC_COMBO_BUY_TIMEOUT, m_cmbBuyTimeOut);
	DDX_Control(pDX, IDC_COMBO_HOLD_TIMEOUT, m_cmbHoldTimeOut);
	DDX_Control(pDX, IDC_COMBO_SELL_TIMEOUT0, m_cmbSellTimeOut0);
	DDX_Control(pDX, IDC_COMBO_SELL_TIMEOUT1, m_cmbSellTimeOut1);
	DDX_Control(pDX, IDC_COMBO_SELL_OVERTHIS0, m_cmbSellOverThis0);
	DDX_Control(pDX, IDC_COMBO_SELL_UNDERTHIS0, m_cmbSellUnderThis0);
	DDX_Control(pDX, IDC_COMBO_SELL_OVERTHIS1, m_cmbSellOverThis1);
	DDX_Control(pDX, IDC_COMBO_SELL_UNDERTHIS1, m_cmbSellUnderThis1);
	DDX_Control(pDX, IDC_COMBO_SELL_OVERTHIS2, m_cmbSellOverThis2);
	DDX_Control(pDX, IDC_COMBO_SELL_UNDERTHIS2, m_cmbSellUnderThis2);
	DDX_Control(pDX, IDC_CHECK_SELL_ITEM_MARKETVALUE_AT_ROUNDEND, m_checkDoSellItemMarketValueAtRoundEnd);
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
	ON_BN_CLICKED(IDOK, &CABotDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_GETCONDITION, &CABotDlg::OnBnClickedButtonGetCondition)
	ON_BN_CLICKED(IDC_BUTTON_GETITEM, &CABotDlg::OnBnClickedButtonGetCondItems)
	ON_BN_CLICKED(IDC_BUTTON_REGITEM, &CABotDlg::OnBnClickedButtonRegItems)
	ON_BN_CLICKED(IDC_BUTTON_SAVECONFIG, &CABotDlg::OnBnClickedButtonSaveconfig)
	ON_BN_CLICKED(IDC_BUTTON_START_ROUND, &CABotDlg::OnBnClickedButtonStartRound)
	ON_BN_CLICKED(IDC_BUTTON_FINISH_ROUND, &CABotDlg::OnBnClickedButtonFinishRound)
	ON_BN_CLICKED(IDC_BUTTON_STOP_ROUND, &CABotDlg::OnBnClickedButtonStopRound)
	ON_BN_CLICKED(IDC_BUTTON_SELL_ALL_CUR_COST, &CABotDlg::OnBnClickedButtonSellAllCurCost)
	ON_BN_CLICKED(IDC_BUTTON_GET_BALANCE, &CABotDlg::OnBnClickedButtonGetBalance)
	ON_BN_CLICKED(IDC_BUTTON_DEBUG_TEST, &CABotDlg::OnBnClickedButtonDebugTest)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_INROUND, &CABotDlg::OnBnClickedButtonEditInround)
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
		char charBuf[256] = { 0 };
		GetCurrentDirectory(_countof(charBuf)-1, charBuf);
		m_strConfigFile.Format("%s\\%s", charBuf, strBuf);
	}

	if (InitLogData() == FALSE) {
		return FALSE;
	}

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	theApp.m_khOpenApi.CommConnect();

	m_mapJongCode.RemoveAll();
	m_mapFIDName.RemoveAll();
	m_mapNameList.RemoveAll();
	InitFIDName();
	InitRealAddGrid();
	InitBuyItemGrid();
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

BOOL CABotDlg::InitLogData()
{
	CRect rect(0, 0, 0, 0);
	CString strBuf;
	long n = 0;

	// �α� ���� ��� ���.
	ReadFromIniFile_String(m_strConfigFile, "SYSTEM", "logfoldername", ".\\", strBuf);
	strBuf += _T("\\"); strBuf.Replace(_T("\\\\"), _T("\\"));
	m_strLogFolderName = strBuf;
	g_logger.Initial(m_strLogFolderName + _T("ABotLog_"));
	ReadFromIniFile_Int(m_strConfigFile, "SYSTEM", "dowritelog", 1, n);
	g_logger.SetEnable(n > 0 ? TRUE : FALSE);

	//�޼��� ���÷��̸� ���� ���� �����,
	g_pMsgDisp = new CsMsgDisp(this);
	if (!g_pMsgDisp) { 
		return FALSE; 
	}
	if (g_pMsgDisp->Create(IDD_MSG_DISP, this) == FALSE) {
		return FALSE;
	}
	
	GetDlgItem(IDC_STATIC_LOG_VIEW)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	g_pMsgDisp->MoveWindow(rect);

	return TRUE;
}

void CABotDlg::InitComboBox()
{
	//�޺� �ڽ� ���� ������.
	m_cmbItemCount.InsertString(0, "1");
	m_cmbItemCount.InsertString(1, "3");
	m_cmbItemCount.InsertString(2, "5");
	m_cmbItemCount.InsertString(3, "10");
	m_cmbItemCount.InsertString(4, "20");
	m_cmbItemCount.InsertString(5, "50");
	m_cmbItemCount.InsertString(6, "100");
	m_cmbItemCount.InsertString(7, "500");
	m_cmbItemCount.InsertString(8, "1000");
	m_cmbItemCount.InsertString(9, "10000");
	m_cmbItemCount.InsertString(10,"99999999");
	m_cmbItemCount.SetCurSel(3);

	CString strBuf;
	long i = 0;
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

	m_cmbDpUseRate.InsertString(0, "100");
	m_cmbDpUseRate.InsertString(1, "90");
	m_cmbDpUseRate.InsertString(2, "80");
	m_cmbDpUseRate.InsertString(3, "50");
	m_cmbDpUseRate.SetCurSel(3);

	m_cmbMaxTotalAmount.InsertString(0, "50");
	m_cmbMaxTotalAmount.InsertString(1, "100");
	m_cmbMaxTotalAmount.InsertString(2, "200");
	m_cmbMaxTotalAmount.InsertString(3, "300");
	m_cmbMaxTotalAmount.InsertString(4, "400");
	m_cmbMaxTotalAmount.InsertString(5, "500");
	m_cmbMaxTotalAmount.InsertString(6, "600");
	m_cmbMaxTotalAmount.InsertString(7, "700");
	m_cmbMaxTotalAmount.InsertString(8, "800");
	m_cmbMaxTotalAmount.InsertString(9, "900");
	m_cmbMaxTotalAmount.InsertString(10, "1000");
	m_cmbMaxTotalAmount.SetCurSel(0);

	m_cmbMaxAmount.InsertString(0, "10");
	m_cmbMaxAmount.InsertString(1, "20");
	m_cmbMaxAmount.InsertString(2, "30");
	m_cmbMaxAmount.InsertString(3, "40");
	m_cmbMaxAmount.InsertString(4, "50");
	m_cmbMaxAmount.InsertString(5, "60");
	m_cmbMaxAmount.InsertString(6, "70");
	m_cmbMaxAmount.InsertString(7, "80");
	m_cmbMaxAmount.InsertString(8, "90");
	m_cmbMaxAmount.InsertString(9, "100");
	m_cmbMaxAmount.SetCurSel(0);

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
	m_cmbBuyRetry.SetCurSel(1);

	m_cmbHoldTimeOut.InsertString(0, "0");
	m_cmbHoldTimeOut.InsertString(1, "10");
	m_cmbHoldTimeOut.InsertString(2, "20");
	m_cmbHoldTimeOut.InsertString(3, "30");
	m_cmbHoldTimeOut.InsertString(4, "40");
	m_cmbHoldTimeOut.InsertString(5, "50");
	m_cmbHoldTimeOut.InsertString(6, "60");
	m_cmbHoldTimeOut.InsertString(7, "120");
	m_cmbHoldTimeOut.InsertString(8, "180");
	m_cmbHoldTimeOut.SetCurSel(0);


	m_cmbSellOverThis0.InsertString(0, "0.5");
	m_cmbSellOverThis0.InsertString(1, "1.0");
	m_cmbSellOverThis0.InsertString(2, "1.5");
	m_cmbSellOverThis0.InsertString(3, "2.0");
	m_cmbSellOverThis0.InsertString(4, "2.5");
	m_cmbSellOverThis0.InsertString(5, "3.0");
	m_cmbSellOverThis0.InsertString(6, "3.5");
	m_cmbSellOverThis0.InsertString(7, "4.0");
	m_cmbSellOverThis0.InsertString(8, "4.5");
	m_cmbSellOverThis0.InsertString(9, "5.0");
	m_cmbSellOverThis0.InsertString(10, "5.5");
	m_cmbSellOverThis0.InsertString(11, "6.0");
	m_cmbSellOverThis0.InsertString(12, "6.5");
	m_cmbSellOverThis0.InsertString(13, "7.0");
	m_cmbSellOverThis0.InsertString(14, "7.5");
	m_cmbSellOverThis0.InsertString(15, "8.0");
	m_cmbSellOverThis0.InsertString(16, "8.5");
	m_cmbSellOverThis0.InsertString(17, "9.0");
	m_cmbSellOverThis0.InsertString(18, "9.5");
	m_cmbSellOverThis0.InsertString(19, "10.0");
	m_cmbSellOverThis0.InsertString(20, "10.5");
	m_cmbSellOverThis0.SetCurSel(1);

	m_cmbSellUnderThis0.InsertString(0, "0.5");
	m_cmbSellUnderThis0.InsertString(1, "1.0");
	m_cmbSellUnderThis0.InsertString(2, "1.5");
	m_cmbSellUnderThis0.InsertString(3, "2.0");
	m_cmbSellUnderThis0.InsertString(4, "2.5");
	m_cmbSellUnderThis0.InsertString(5, "3.0");
	m_cmbSellUnderThis0.InsertString(6, "3.5");
	m_cmbSellUnderThis0.InsertString(7, "4.0");
	m_cmbSellUnderThis0.InsertString(8, "4.5");
	m_cmbSellUnderThis0.InsertString(9, "5.0");
	m_cmbSellUnderThis0.InsertString(10, "5.5");
	m_cmbSellUnderThis0.InsertString(11, "6.0");
	m_cmbSellUnderThis0.InsertString(12, "6.5");
	m_cmbSellUnderThis0.InsertString(13, "7.0");
	m_cmbSellUnderThis0.InsertString(14, "7.5");
	m_cmbSellUnderThis0.InsertString(15, "8.0");
	m_cmbSellUnderThis0.InsertString(16, "8.5");
	m_cmbSellUnderThis0.InsertString(17, "9.0");
	m_cmbSellUnderThis0.InsertString(18, "9.5");
	m_cmbSellUnderThis0.InsertString(19, "10.0");
	m_cmbSellUnderThis0.InsertString(20, "10.5");
	m_cmbSellUnderThis0.SetCurSel(1);

	m_cmbSellOverThis1.InsertString(0, "0.5");
	m_cmbSellOverThis1.InsertString(1, "1.0");
	m_cmbSellOverThis1.InsertString(2, "1.5");
	m_cmbSellOverThis1.InsertString(3, "2.0");
	m_cmbSellOverThis1.InsertString(4, "2.5");
	m_cmbSellOverThis1.InsertString(5, "3.0");
	m_cmbSellOverThis1.InsertString(6, "3.5");
	m_cmbSellOverThis1.InsertString(7, "4.0");
	m_cmbSellOverThis1.InsertString(8, "4.5");
	m_cmbSellOverThis1.InsertString(9, "5.0");
	m_cmbSellOverThis1.InsertString(10, "5.5");
	m_cmbSellOverThis1.InsertString(11, "6.0");
	m_cmbSellOverThis1.InsertString(12, "6.5");
	m_cmbSellOverThis1.InsertString(13, "7.0");
	m_cmbSellOverThis1.InsertString(14, "7.5");
	m_cmbSellOverThis1.InsertString(15, "8.0");
	m_cmbSellOverThis1.InsertString(16, "8.5");
	m_cmbSellOverThis1.InsertString(17, "9.0");
	m_cmbSellOverThis1.InsertString(18, "9.5");
	m_cmbSellOverThis1.InsertString(19, "10.0");
	m_cmbSellOverThis1.InsertString(20, "10.5");
	m_cmbSellOverThis1.SetCurSel(1);

	m_cmbSellUnderThis1.InsertString(0, "0.5");
	m_cmbSellUnderThis1.InsertString(1, "1.0");
	m_cmbSellUnderThis1.InsertString(2, "1.5");
	m_cmbSellUnderThis1.InsertString(3, "2.0");
	m_cmbSellUnderThis1.InsertString(4, "2.5");
	m_cmbSellUnderThis1.InsertString(5, "3.0");
	m_cmbSellUnderThis1.InsertString(6, "3.5");
	m_cmbSellUnderThis1.InsertString(7, "4.0");
	m_cmbSellUnderThis1.InsertString(8, "4.5");
	m_cmbSellUnderThis1.InsertString(9, "5.0");
	m_cmbSellUnderThis1.InsertString(10, "5.5");
	m_cmbSellUnderThis1.InsertString(11, "6.0");
	m_cmbSellUnderThis1.InsertString(12, "6.5");
	m_cmbSellUnderThis1.InsertString(13, "7.0");
	m_cmbSellUnderThis1.InsertString(14, "7.5");
	m_cmbSellUnderThis1.InsertString(15, "8.0");
	m_cmbSellUnderThis1.InsertString(16, "8.5");
	m_cmbSellUnderThis1.InsertString(17, "9.0");
	m_cmbSellUnderThis1.InsertString(18, "9.5");
	m_cmbSellUnderThis1.InsertString(19, "10.0");
	m_cmbSellUnderThis1.InsertString(20, "10.5");
	m_cmbSellUnderThis1.SetCurSel(1);

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
	m_cmbSellOverThis2.InsertString(10, "5.5");
	m_cmbSellOverThis2.InsertString(11, "6.0");
	m_cmbSellOverThis2.InsertString(12, "6.5");
	m_cmbSellOverThis2.InsertString(13, "7.0");
	m_cmbSellOverThis2.InsertString(14, "7.5");
	m_cmbSellOverThis2.InsertString(15, "8.0");
	m_cmbSellOverThis2.InsertString(16, "8.5");
	m_cmbSellOverThis2.InsertString(17, "9.0");
	m_cmbSellOverThis2.InsertString(18, "9.5");
	m_cmbSellOverThis2.InsertString(19, "10.0");
	m_cmbSellOverThis2.InsertString(20, "10.5");
	m_cmbSellOverThis2.SetCurSel(0);

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
	m_cmbSellUnderThis2.InsertString(10, "5.5");
	m_cmbSellUnderThis2.InsertString(11, "6.0");
	m_cmbSellUnderThis2.InsertString(12, "6.5");
	m_cmbSellUnderThis2.InsertString(13, "7.0");
	m_cmbSellUnderThis2.InsertString(14, "7.5");
	m_cmbSellUnderThis2.InsertString(15, "8.0");
	m_cmbSellUnderThis2.InsertString(16, "8.5");
	m_cmbSellUnderThis2.InsertString(17, "9.0");
	m_cmbSellUnderThis2.InsertString(18, "9.5");
	m_cmbSellUnderThis2.InsertString(19, "10.0");
	m_cmbSellUnderThis2.InsertString(20, "10.5");
	m_cmbSellUnderThis2.SetCurSel(3);

	m_cmbSellTimeOut0.InsertString(0, "0");
	m_cmbSellTimeOut0.InsertString(1, "1");
	m_cmbSellTimeOut0.InsertString(2, "2");
	m_cmbSellTimeOut0.InsertString(3, "3");
	m_cmbSellTimeOut0.InsertString(4, "4");
	m_cmbSellTimeOut0.InsertString(5, "5");
	m_cmbSellTimeOut0.InsertString(6, "6");
	m_cmbSellTimeOut0.InsertString(7, "7");
	m_cmbSellTimeOut0.InsertString(8, "8");
	m_cmbSellTimeOut0.InsertString(9, "9");
	m_cmbSellTimeOut0.InsertString(10, "10");
	m_cmbSellTimeOut0.SetCurSel(0);
	
	m_cmbSellTimeOut1.InsertString(0, "0");
	m_cmbSellTimeOut1.InsertString(1, "5");
	m_cmbSellTimeOut1.InsertString(2, "10");
	m_cmbSellTimeOut1.InsertString(3, "15");
	m_cmbSellTimeOut1.InsertString(4, "20");
	m_cmbSellTimeOut1.InsertString(5, "25");
	m_cmbSellTimeOut1.InsertString(6, "30");
	m_cmbSellTimeOut1.InsertString(7, "35");
	m_cmbSellTimeOut1.InsertString(8, "40");
	m_cmbSellTimeOut1.InsertString(9, "45");
	m_cmbSellTimeOut1.InsertString(10, "50");
	m_cmbSellTimeOut1.InsertString(11, "55");
	m_cmbSellTimeOut1.InsertString(12, "60");
	m_cmbSellTimeOut1.SetCurSel(0);
}

void CABotDlg::LoadSystemFile()
{
	long i = 0, n = 0;
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
		m_cmbItemCount.SetCurSel(0);
	}
	else
	{
		n = atol((LPSTR)(LPCSTR)strBuf);
		strBuf.Format("%d", n);
		for (i = 1; i<m_cmbItemCount.GetCount(); i++)
		{
			m_cmbItemCount.GetLBText(i, strCombo);
			if (strCombo == strBuf)
			{
				break;
			}
		}
		if (i >= m_cmbItemCount.GetCount())
		{
			m_cmbItemCount.InsertString(i, strBuf);
		}
		m_cmbItemCount.SetCurSel(i);
	}
	AddMessage("     �ݺ� ȸ��[%s].", strBuf);

	CString strBuf2 = "���� ";
	// ���� �ð��� ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "start_hour", "9", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	m_cmbStartHour.SetCurSel(n);
	strBuf2 += strBuf + "�� ";

	// ���� �ð��� ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "start_min", "0", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	m_cmbStartMin.SetCurSel(n);
	strBuf2 += strBuf + "��,  ���� ";

	// ���� �ð��� ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "end_hour", "15", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	m_cmbEndHour.SetCurSel(n);
	strBuf2 += strBuf + "�� ";

	// ���� �ð��� ��
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "end_min", "0", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	m_cmbEndMin.SetCurSel(n);
	strBuf2 += strBuf + "��";
	AddMessage("     ���� �ð�[%s].", strBuf2);

	// �ż��� �������� �ִ� ��� �ۼ�Ʈ. [%]
	ReadFromIniFile_String(m_strConfigFile, "BUY", "use_rate", "50", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	if (n > 100) {n = 100;}
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
	n = atol((LPSTR)(LPCSTR)strBuf);
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
	ReadFromIniFile_String(m_strConfigFile, "BUY", "max_amount", "10", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	strBuf.Format("%d", n);
	for (i = 0; i<m_cmbMaxAmount.GetCount(); i++)
	{
		m_cmbMaxAmount.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			m_cmbMaxAmount.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbMaxAmount.GetCount())
	{
		m_cmbMaxAmount.SetWindowText(strBuf);
	}
	AddMessage("     ����� ��� �ݾ� [%s]����.", strBuf);

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
	n = atol((LPSTR)(LPCSTR)strBuf);
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
	n = atol((LPSTR)(LPCSTR)strBuf);
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
	n = atol((LPSTR)(LPCSTR)strBuf);
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
	ReadFromIniFile_String(m_strConfigFile, "SELL", "over_this_0", "3.0", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	if (d > 30.0) { d = 30.0; }
	strBuf.Format("%3.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellOverThis0.GetCount(); i++)
	{
		m_cmbSellOverThis0.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			m_cmbSellOverThis0.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbSellOverThis0.GetCount())
	{
		m_cmbSellOverThis0.SetWindowText(strBuf);
	}
	AddMessage("     �ŵ� ���� #0. �ż� �ݾ��� [%s] �ۼ�Ʈ �̻� ��½�.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "under_this_0", "3.0", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	if (d > 30.0) { d = 30.0; }
	strBuf.Format("%.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellUnderThis0.GetCount(); i++)
	{
		m_cmbSellUnderThis0.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			m_cmbSellUnderThis0.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbSellUnderThis0.GetCount())
	{
		m_cmbSellUnderThis0.SetWindowText(strBuf);
	}
	AddMessage("     �ŵ� ���� #0. �ż� �ݾ��� [%s] �ۼ�Ʈ �̻� �϶��� ���尡 �ŵ�.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "over_this_1", "2.0", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	if (d > 30.0) { d = 30.0; }
	strBuf.Format("%3.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellOverThis1.GetCount(); i++)
	{
		m_cmbSellOverThis1.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			m_cmbSellOverThis1.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbSellOverThis1.GetCount())
	{
		m_cmbSellOverThis1.SetWindowText(strBuf);
	}
	AddMessage("     �ŵ� ���� #1. �ŵ� Ÿ�Ӿƿ�0 �� �ż� �ݾ��� [%s] �ۼ�Ʈ �̻� ��½�  �ŵ�.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "under_this_1", "2.0", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	if (d > 30.0) { d = 30.0; }
	strBuf.Format("%.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellUnderThis1.GetCount(); i++)
	{
		m_cmbSellUnderThis1.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			m_cmbSellUnderThis1.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbSellUnderThis1.GetCount())
	{
		m_cmbSellUnderThis1.SetWindowText(strBuf);
	}
	AddMessage("     �ŵ� ���� #1. �ŵ� Ÿ�Ӿƿ�0 �� �ż� �ݾ��� [%s] �ۼ�Ʈ �̻� �϶��� ���尡 �ŵ�.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "over_this_2", "1.0", strBuf);
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
			m_cmbSellOverThis2.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbSellOverThis2.GetCount())
	{
		m_cmbSellOverThis2.SetWindowText(strBuf);
	}
	AddMessage("     �ŵ� ���� #2. �ŵ� Ÿ�Ӿƿ�0+1 �� �ż� �ݾ��� [%s] �ۼ�Ʈ �̻� ��½� �ŵ�.", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "under_this_2", "1.0", strBuf);
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
			m_cmbSellUnderThis2.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbSellUnderThis2.GetCount())
	{
		m_cmbSellUnderThis2.SetWindowText(strBuf);
	}
	AddMessage("     �ŵ� ���� #2. �ŵ� Ÿ�Ӿƿ�0+1 �� �ż� �ݾ��� [%s] �ۼ�Ʈ ���� ���޽� ���尡 �ŵ�.", strBuf);

	// �ŵ� Ÿ�� �ƿ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "timeout_0", "5", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	strBuf.Format("%.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellTimeOut0.GetCount(); i++)
	{
		m_cmbSellTimeOut0.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			m_cmbSellTimeOut0.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbSellTimeOut0.GetCount())
	{
		m_cmbSellTimeOut0.SetWindowText(strBuf);
	}
	AddMessage("     �ŵ� ��� Ÿ�� �ƿ� #0 [%s]��.", strBuf);

	// �ŵ� Ÿ�� �ƿ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "timeout_1", "10", strBuf);
	d = atof((LPSTR)(LPCSTR)strBuf);
	strBuf.Format("%.1f", d);
	strBuf.TrimLeft();
	strBuf.TrimRight();
	for (i = 0; i<m_cmbSellTimeOut1.GetCount(); i++)
	{
		m_cmbSellTimeOut1.GetLBText(i, strCombo);
		if (strCombo == strBuf)
		{
			m_cmbSellTimeOut1.SetCurSel(i);
			break;
		}
	}
	if (i >= m_cmbSellTimeOut1.GetCount())
	{
		m_cmbSellTimeOut1.SetWindowText(strBuf);
	}
	AddMessage("     �ŵ� ��� Ÿ�� �ƿ� #1 [%s]��.", strBuf);

	// �ż� ������
	ReadFromIniFile_String(m_strConfigFile, "BUY", "tradefee", "0.015", strBuf);
	m_dBuyTradeFee = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage("     �ż� �ŷ� ������(���� ����)�ż� �ݾ��� [%s] �ۼ�Ʈ.", strBuf);

	// �ŵ� ������
	ReadFromIniFile_String(m_strConfigFile, "SELL", "tradefee", "0.315", strBuf);
	m_dSellTradeFee = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage("     �ŵ� �ŷ� ������(���� ����)�ŵ� �ݾ��� [%s] �ۼ�Ʈ.", strBuf);

	// ���� ������ ��ä�� ���� ���尡�� �ȱ�.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "sell_item_market_value_at_round_end", "0", strBuf);
	m_bDoSellItemMarketValueAtRoundEnd = (atoi((LPSTR)(LPCSTR)strBuf) != 0 ? TRUE : FALSE);
	m_checkDoSellItemMarketValueAtRoundEnd.SetCheck(m_bDoSellItemMarketValueAtRoundEnd ? BST_CHECKED : BST_UNCHECKED);
	AddMessage("     ���� ���� ��, ��ä�� ���� ���尡�� �ȱ� [%s].", m_bDoSellItemMarketValueAtRoundEnd?"��":"����");	
}

void CABotDlg::SaveSystemFile()
{
	long i = 0, n = 0;
	double d = 0.;
	CString strBuf, strCombo;

	// ���� ��ȣ.
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ACCOUNT", "number", strBuf);

	// �ݺ� ȸ��
	m_cmbItemCount.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "count", strBuf);

	// ���� �ð��� ��
	m_cmbStartHour.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "start_hour", strBuf);

	// ���� �ð��� ��
	m_cmbStartMin.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "start_min", strBuf);

	// ���� �ð��� ��
	m_cmbEndHour.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "end_hour", strBuf);

	// ���� �ð��� ��
	m_cmbEndMin.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "end_min", strBuf);

	// �˻� ����
	m_cmbCondtion.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "CONDITION", "name", strBuf);

	// �ż��� �������� �ִ� ��� �ۼ�Ʈ. [%]
	m_cmbDpUseRate.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "use_rate", strBuf);

	// �ż��� �������� �ִ� ��� �ݾ�. [����]
	m_cmbMaxTotalAmount.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "max_total_amount", strBuf);	

	// ����� �ִ� ���� ��� �ݾ�
	m_cmbMaxAmount.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "max_amount", strBuf);

	// �ż� ���, ���簡, ���尡. ����..�ۼ�Ʈ ����.
	m_cmbBuyMethod.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "method", strBuf);

	// �ż� ü�� ��� Ÿ�� �ƿ�.
	m_cmbBuyTimeOut.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "timeout", strBuf);

	// �ż� ���н� ��õ� ȸ��
	m_cmbBuyRetry.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "retry", strBuf);

	// �ֽ� ���� Ÿ�� �ƿ�.
	m_cmbHoldTimeOut.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "HOLD", "timeout", strBuf);

	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� ����/������ �Ǵ�.
	m_cmbSellOverThis0.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "over_this_0", strBuf);

	m_cmbSellUnderThis0.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "under_this_0", strBuf);

	m_cmbSellOverThis1.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "over_this_1", strBuf);

	m_cmbSellUnderThis1.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "under_this_1", strBuf);

	m_cmbSellOverThis2.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "over_this_2", strBuf);

	m_cmbSellUnderThis2.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "under_this_2", strBuf);
	
	// �ŵ� Ÿ�� �ƿ�.
	m_cmbSellTimeOut0.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "timeout_0", strBuf);

	m_cmbSellTimeOut1.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "timeout_1", strBuf);

	m_bDoSellItemMarketValueAtRoundEnd = (m_checkDoSellItemMarketValueAtRoundEnd.GetCheck() == BST_CHECKED);
	strBuf = (m_bDoSellItemMarketValueAtRoundEnd ? "1" : "0");
	WriteToIniFile_String(m_strConfigFile, "SELL", "sell_item_market_value_at_round_end", strBuf);
}


void CABotDlg::OnBnClickedButtonSaveconfig()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SaveSystemFile();
}


void CABotDlg::InitProcessCondition()
{
	m_ItemCount = -1;
	long i = 0;
	for (i = 0; i<_countof(m_Item); i++)
	{
		m_Item[i].m_index = i;
		m_Item[i].Init();
	}

	SetDlgItemText(IDC_BUTTON_EDIT_INROUND, "���� ����");

	CString strBuf;

	CString strIndex;
	m_cmbCondtion.GetLBText(m_cmbCondtion.GetCurSel(), m_strConditionName);
	m_mapNameList.Lookup(m_strConditionName, strIndex);
	m_nConditionIndex = _ttoi(strIndex);
	AddMessage(_T("_____::�˻���[%d][%s]�� ����մϴ�."), m_nConditionIndex, m_strConditionName);

	m_cmbItemCount.GetWindowText(strBuf);
	m_nProcessItemCount = atol((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::��� ������� %d[��] �Դϴ�."), m_nProcessItemCount);

	m_cmbMaxAmount.GetWindowText(strBuf);
	m_lProcessItemDR = atol((LPSTR)(LPCSTR)strBuf) * 10000;
	if (m_lProcessItemDR > 100 * 10000)
	{
		m_lProcessItemDR = 100 * 10000;
	}
	AddMessage(_T("_____::����� ��� �ݾ��� %s[��] �Դϴ�."), GetCurrencyString(m_lProcessItemDR));

	m_cmbBuyTimeOut.GetWindowText(strBuf);
	m_lItemBuyTimeout = atol((LPSTR)(LPCSTR)strBuf) * 1000;
	AddMessage(_T("_____::����� �ż� timeout�� %d[��] �Դϴ�."), m_lItemBuyTimeout / 1000);

	m_cmbBuyRetry.GetWindowText(strBuf);
	m_lItemBuyTryCount = atol((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::����� �ż� ��õ� ȸ���� %d[ȸ] �Դϴ�."), m_lItemBuyTryCount);

	m_cmbHoldTimeOut.GetWindowText(strBuf);
	m_lItemHoldTimeout = atol((LPSTR)(LPCSTR)strBuf) * 1000 * 60;
	AddMessage(_T("_____::����� ���� timeout�� %d[��] �Դϴ�."), m_lItemHoldTimeout / (1000 * 60));

	m_cmbSellOverThis0.GetWindowText(strBuf);
	m_dSellOverThis0 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::����� �ŵ� Sell'OVER'This0�� %f[�ۼ�Ʈ] �Դϴ�."), m_dSellOverThis0);

	m_cmbSellUnderThis0.GetWindowText(strBuf);
	m_dSellUnderThis0 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::����� �ŵ� Sell'UNDER'This0�� %f[�ۼ�Ʈ] �Դϴ�."), m_dSellUnderThis0);

	m_cmbSellOverThis1.GetWindowText(strBuf);
	m_dSellOverThis1 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::����� Ÿ�Ӿƿ�#0�� �ŵ� Sell'OVER'This1�� %f[�ۼ�Ʈ] �Դϴ�."), m_dSellOverThis1);

	m_cmbSellUnderThis1.GetWindowText(strBuf);
	m_dSellUnderThis1 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::����� Ÿ�Ӿƿ�#0�� �ŵ� Sell'UNDER'This1�� %f[�ۼ�Ʈ] �Դϴ�."), m_dSellUnderThis1);

	m_cmbSellOverThis2.GetWindowText(strBuf);
	m_dSellOverThis2 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::����� Ÿ�Ӿƿ�#0+ #1�� �ŵ� Sell'OVER'This2�� %f[�ۼ�Ʈ] �Դϴ�."), m_dSellOverThis2);

	m_cmbSellUnderThis2.GetWindowText(strBuf);
	m_dSellUnderThis2 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::����� Ÿ�Ӿƿ�#0+ #1�� �ŵ� Sell'UNDER'This2�� %f[�ۼ�Ʈ] �Դϴ�."), m_dSellUnderThis2);

	m_cmbSellTimeOut0.GetWindowText(strBuf);
	m_lItemSellTimeout0 = long(atof((LPSTR)(LPCSTR)strBuf) * 1000.0 * 60.0);
	AddMessage(_T("_____::����� �ŵ� Ÿ�Ӿƿ�#0�� %s[��] �Դϴ�."), strBuf);

	m_cmbSellTimeOut1.GetWindowText(strBuf);
	m_lItemSellTimeout1 = long(atof((LPSTR)(LPCSTR)strBuf) * 1000.0 * 60.0);
	AddMessage(_T("_____::����� �ŵ� Ÿ�Ӿƿ�#1�� %s[��] �Դϴ�."), strBuf);

	m_bDoSellItemMarketValueAtRoundEnd = (m_checkDoSellItemMarketValueAtRoundEnd.GetCheck() == BST_CHECKED);
	AddMessage(_T("_____::���� ���� ��, ��ä�� ���� ���尡�� �ȱ� [%s]."), m_bDoSellItemMarketValueAtRoundEnd ? "��" : "����");

	theApp.m_khOpenApi.SetRealRemove(_T("ALL"), _T("ALL"));
	AddMessage(_T("_____::���� ��ϵ� �ǽð� ��ȸ ��û ������ ��� �����մϴ�."));
}


void CABotDlg::InitRealAddGrid()
{
	COLORREF clr = RGB(215, 227, 241);
	long i = 0, nWidth[] = { 50, 101, 56, 30, 56, 56, 80 };
	long nCnt = sizeof(nWidth) / sizeof(*nWidth);		// ��üũ�� / ����ũ�� = ���Ұ���
	CString strHeader[] = { "�ڵ�", "�����", "���簡", "��ȣ", "���ϴ��", "�����", "�ŷ���" };

	m_grdRealAdd.SetEditable(FALSE);				//cell�� ����Ʈ ���ϰ� ��.
//	m_grdRealAdd.EnableScrollBars(SB_BOTH, FALSE);
	m_grdRealAdd.EnableScrollBars(SB_VERT, TRUE);

	m_grdRealAdd.SetFixedRowCount(1); // ���� ��/�� ����
	m_grdRealAdd.SetRowCount(1); // ��/�� ���� ����
	SetGridHeight(m_grdRealAdd, 0, 24); // grid ���̴� ������ ���� �����ϹǷ� ���� �Լ��� ȣ���Ѵ�.
	m_grdRealAdd.SetColumnCount(nCnt); // ���� ���� ����

	for (i = 0; i < nCnt; i++)
	{
		SetGridWidth(m_grdRealAdd, i, nWidth[i]); // grid ���� ������ ���� �����ϹǷ� ���� �Լ��� ȣ���Ѵ�.
		m_grdRealAdd.SetItemFormat(0, i, DT_CENTER);
		m_grdRealAdd.SetItemText(0, i, strHeader[i]);
		m_grdRealAdd.SetItemBkColour(0, i, clr);	// ������ ���� ���� ����
	}

	m_grdRealAdd.SetRowCount(21);
	m_grdRealAdd.Invalidate();
}

void CABotDlg::InitBuyItemGrid()
{
	COLORREF clr = RGB(215, 227, 241);
	long i = 0, nWidth[] = { 101, 56, 80, 80, 56, 56, 56 };
	long nCnt = sizeof(nWidth) / sizeof(*nWidth);		// ��üũ�� / ����ũ�� = ���Ұ���
	CString strHeader[] = { "�����", "���԰�", "�򰡼���", "���Աݾ�", "������", "���ɼ���", "��������" };

	m_grdBuyItem.SetEditable(FALSE);				//cell�� ����Ʈ ���ϰ� ��.
	m_grdBuyItem.EnableScrollBars(SB_VERT, FALSE);
	m_grdBuyItem.SetFixedRowCount(1); // ���� ��/�� ����
	m_grdBuyItem.SetRowCount(1); // ��/�� ���� ����
	m_grdBuyItem.SetColumnCount(nCnt);
	
	for (i = 0; i < nCnt; i++)
	{
		SetGridWidth(m_grdBuyItem, i, nWidth[i]); // grid ���� ������ ���� �����ϹǷ� ���� �Լ��� ȣ���Ѵ�.
		m_grdBuyItem.SetItemFormat(0, i, DT_CENTER);
		m_grdBuyItem.SetItemText(0, i, strHeader[i]);
		m_grdBuyItem.SetItemBkColour(0, i, clr);	// ������ ���� ���� ����
	}
	m_grdBuyItem.SetRowCount(21);
	m_grdBuyItem.Invalidate();
}

void CABotDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

}

void CABotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (ROUND_TIMER == nIDEvent)
	{
		static eProcessState aOldProcessState = ePST_IDLE;
		if (aOldProcessState != m_eProcessState)
		{
			aOldProcessState = m_eProcessState;
			CString strBuf;
			strBuf.Format(_T("ABot [%s]"), GetRoundText(m_eProcessState));
			SetWindowText(strBuf);
		}
		ProcessSequence();
	}
	else if (START_TIMER == nIDEvent)
	{
		if (IsSafeWindow(g_pMsgDisp) == TRUE)
		{
			g_pMsgDisp->ShowWindow(SW_SHOW);
			KillTimer(nIDEvent);
		}
	}
	else if (CONDITION_TIMER == nIDEvent)
	{
		OnBnClickedButtonGetCondition();
		KillTimer(nIDEvent);
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
		long cxIcon = GetSystemMetrics(SM_CXICON);
		long cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		long x = (rect.Width() - cxIcon + 1) / 2;
		long y = (rect.Height() - cyIcon + 1) / 2;

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

	SafeDelete(g_pMsgDisp);	//�޼��� ���÷��̸� ���� ���߿� �Ҹ� ���Ѿ� �Ѵ�.
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

	char arbytebuf[1024] = { 0 };
	char arbyteMsg[1024] = { 0 };

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
	ON_EVENT(CABotDlg, IDC_KHOPENAPICTRL1, 7, CABotDlg::OnReceiveRealCondition, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
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
		long nGetConditionRet = theApp.m_khOpenApi.GetConditionLoad();
		AddMessage("�˻� ���� ��ȸ %s. [%d]", (nGetConditionRet>=0?"����":"����"), nGetConditionRet);
		SetTimer(CONDITION_TIMER, 1000, NULL);
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

	//�Ʒ� �޽��� �������� ����´�... ���� ó���ؾ� �ұ�.
	//[2016/09/27][09:07:10:466] : ��ȸ����,��ũ����ȣ[6484],RQName[�ֽ��ֹ�],TrCode[�ֽ��ֹ�],�޽���[[571552] �ֹ��ܰ��� ���Ѱ����� �����ϴ�.]

}


void CABotDlg::OnBnClickedBalanceQuery()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	REQ_DepositReceived();
}

BOOL CABotDlg::REQ_DepositReceived()
{
	if (getAccountData() == FALSE) {
		return FALSE;
	}
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
	static BOOL btoggle = TRUE;
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
	btoggle = btoggle?FALSE:TRUE;
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
	AddMessage(_T("ChejanData::"));
	CString strGubun(sGubun), strFidList(sFIdList), strText;
	CString	strAccNo, strOrdNo, strOrdPrice, strOrdCnt;

	long		nItemIndex = -1;
	BOOL		bTRADEDone = FALSE, bREQDone = FALSE;
	BOOL		bBUYDone = FALSE, bSELLDone = FALSE;
	CString		strItemCode, strOrderCode;
	long		lTradeQuantity = 0, lTradePrice = 0, lCurPrice = 0;
	long		lRemainQuantity = 0;
	long		lReqQuantity = 0, lReqPrice = 0;

	long		nIndex(0);
	long		lFid = 0;
	CString		strFID, strOneData, strFIDName;
	while (AfxExtractSubString(strFID, strFidList, nIndex++, _T(';')))
	{
		if (m_mapFIDName.Lookup(strFID, strFIDName) == FALSE)
			strFIDName = strFID;

		if (strFIDName.IsEmpty())	strFIDName = strFID;

		strOneData = theApp.m_khOpenApi.GetChejanData(atol(strFID)).Trim();

		strText.Format(_T("__________::����[%s] FID[%4s:%s] = [%s]"), strGubun, strFID, strFIDName, strOneData);
	//	if (!IsInRound()) 
			AddMessage(strText);

		strFIDName.Empty();

		//����[0] FID[9201:���¹�ȣ] = [5088156710]
		//����[0] FID[9203:�ֹ���ȣ] = [0067777]
		//����[0] FID[9205:9205] = []
		//����[0] FID[9001:�����ڵ�] = [A009180]
		//����[0] FID[912:912] = [JJ]
		//����[0] FID[913:�ֹ�����] = [ü��]
		//����[0] FID[302:�����] = [�Ѽַ�����ƽ��]
		//����[0] FID[900:�ֹ�����] = [90]
		//����[0] FID[901:�ֹ�����] = [3300]
		//����[0] FID[902:��ü�����] = [0]
		//����[0] FID[903:ü�ᴩ��ݾ�] = [296670]
		//����[0] FID[904:���ֹ���ȣ] = [0000000]
		//����[0] FID[905:�ֹ�����] = [+�ż�]
		//����[0] FID[906:�Ÿű���] = [����]
		//����[0] FID[907:�ŵ�������] = [2]
		//����[0] FID[908:�ֹ� / ü��ð�] = [124500]
		//����[0] FID[909:ü���ȣ] = [14728]
		//����[0] FID[910:ü�ᰡ] = [3296]
		//����[0] FID[911:ü�ᷮ] = [90]
		//����[0] FID[10:���簡] = [+3300]
		//����[0] FID[27:(�ֿ켱)�ŵ�ȣ��] = [+3300]
		//����[0] FID[28:(�ֿ켱)�ż�ȣ��] = [+3290]
		//����[0] FID[914:����ü�ᰡ] = [3300]
		//����[0] FID[915:����ü�ᷮ] = [24]
		//����[0] FID[938:938] = [40]
		//����[0] FID[939:939] = [0]
		//����[0] FID[919:�źλ���] = [0]
		//����[0] FID[920:ȭ���ȣ] = [7198]
		//����[0] FID[921:921] = [Y3011168]
		//����[0] FID[922:922] = [00]
		//����[0] FID[923:923] = [00000000]

	//	if (IsInRound())// && strGubun=="0")
		{
			if (strFID == "9203")	//�ֹ���ȣ
			{
				strOrderCode = strOneData;
			}
			if (strFID == "9001")	//�����ڵ�
			{
				strItemCode = strOneData.Right(6);
			}
			else if (strFID == "913")	//�ֹ�����
			{
				if (strOneData.Find("����") >= 0)
				{
					bREQDone = TRUE;
				}
				else if (strOneData.Find("ü��") >= 0)
				{
					bTRADEDone = TRUE;
				}
			}
			else if (strFID == "905" && strGubun == "0")	//�ֹ�����
			{
				if (strOneData.Find("�ż�") >= 0)
				{
					bBUYDone = TRUE;
				}
				else if (strOneData.Find("�ŵ�") >= 0)
				{
					bSELLDone = TRUE;
				}
				else
				{
					long k = 0;//�̰� ����ΰ�?
				}
			}
			else if (strFID == "911")	//ü�ᷮ
			{
				lTradeQuantity = atol(strOneData);
			}
			else if (strFID == "902")	//�� ü�ᷮ
			{
				lRemainQuantity = atol(strOneData);
			}
			else if (strFID == "910")	//ü�ᰡ
			{
				lTradePrice = atol(strOneData);
			}
			else if (strFID == "10")	//���簡
			{
				lCurPrice = atol(strOneData);
			}
			else if (strFID == "900")	//��û����
			{
				lReqQuantity = atol(strOneData);
			}
			else if (strFID == "901")	//��û��
			{
				lReqPrice = atol(strOneData);
			}
		}
	}

	long aIndex = -1;
	if (m_mapItemCode.Lookup(strItemCode, aIndex))
	{
		nItemIndex = aIndex;
	}

	if (nItemIndex >= 0 && strGubun == "0")
	{
		CSingleLock	lock(&m_criticalItemProcess);
		lock.Lock();
		if (lock.IsLocked())
		{
			ASSERT(MAX_ITEM_COUNT > nItemIndex);

			CABotItem &aItem = m_Item[nItemIndex];
			if (lCurPrice > 0 && aItem.m_lcurPrice != lCurPrice)
			{
				aItem.m_lcurPrice = lCurPrice;
				AddMessage(_T("__________::[%s][%s][%s] curPrice[%s]"),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), GetCurrencyString(aItem.m_lcurPrice));
			}

			if (bREQDone)
			{
				// �ֹ� ��ȣ
				if (aItem.m_eitemState == eST_WAITBUY)
				{
					aItem.m_ltryBuyTimeout = GetTickCount() + m_lItemBuyTimeout;

					if (lRemainQuantity != 0)
					{
						aItem.m_strBuyOrder = strOrderCode;
						AddMessage(_T("__________::[%s][%s][%s],�ܰ�[%d],����[%d],�ܷ�[%d],�ڵ�[%s] �ż� �ֹ��� �Ϸ� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lReqPrice, lReqQuantity, aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_strBuyOrder);
					}
				}
				else if (aItem.m_eitemState == eST_WAITSELL)
				{
				//	aItem.m_ltrySellTimeout = (m_lItemHoldTimeout > 0 ? GetTickCount() + m_lItemHoldTimeout : 0);
					if (lRemainQuantity != 0)
					{ 
						aItem.m_strSellOrder = strOrderCode; 
						AddMessage(_T("__________::[%s][%s][%s],�ܰ�[%d],����[%d],�ܷ�[%d],�ڵ�[%s] �ŵ� �ֹ��� �Ϸ� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lReqPrice, lReqQuantity, aItem.m_lQuantity - aItem.m_lSellQuantity, aItem.m_strSellOrder);
					}
				}
				else if (aItem.m_eitemState == eST_WAITBUYCANCEL ||
					aItem.m_eitemState == eST_BUYCANCEL)
				{
					if (aItem.m_lBuyQuantity > 0)
					{
						AddMessage(_T("__________::[%s][%s][%s],�ܰ�[%d],����[%d],�ܷ�[%d], �ż� ��� �ֹ��� �Ϸ� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lReqPrice, lReqQuantity, aItem.m_lQuantity - aItem.m_lBuyQuantity);
						aItem.m_lholdTime = GetTickCount();
						aItem.m_lholdTimeout = (m_lItemHoldTimeout > 0 ? aItem.m_lholdTime + m_lItemHoldTimeout : 0);
						aItem.m_ltrySellTimeout0 = (m_lItemSellTimeout0 > 0 ? aItem.m_lholdTime + m_lItemSellTimeout0 : 0);
						aItem.m_ltrySellTimeout1 = ((m_lItemSellTimeout0 > 0 && m_lItemSellTimeout1 > 0) ? aItem.m_lholdTime + m_lItemSellTimeout0 + m_lItemSellTimeout1 : 0);
						aItem.m_eitemState = eST_HOLDING;
					//	return;
					}
					else
					{
						AddMessage(_T("__________::[%s][%s][%s] �ŷ� �Ϸ� ó�� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString());
						aItem.m_eitemState = eST_TRADECLOSING;
					//	return;
					}
				}
				else if (aItem.m_eitemState == eST_WAITSELLCANCEL ||
					aItem.m_eitemState == eST_SELLCANCEL)
				{
					if (aItem.m_lSellQuantity > 0)
					{
						AddMessage(_T("__________::[%s][%s][%s],�ܰ�[%d],����[%d],�ܷ�[%d], �ŵ� ��� �ֹ��� �Ϸ� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lReqPrice, lReqQuantity, aItem.m_lQuantity - aItem.m_lSellQuantity);
						aItem.m_eitemState = eST_HOLDING;
					//	return;
					}
					else
					{
						AddMessage(_T("__________::[%s][%s][%s] �ŵ� ��Ұ� �Ϸ� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString());
						aItem.m_eitemState = eST_HOLDING;
					//	return;
					}
				}
			}

			if (bTRADEDone)
			{
				if (aItem.m_eitemState == eST_TRYBUY ||
					aItem.m_eitemState == eST_WAITBUY ||
					aItem.m_eitemState == eST_BUYCANCEL ||
					aItem.m_eitemState == eST_WAITBUYCANCEL)
				{
					ASSERT(bBUYDone);

					aItem.m_lBuyCost = lTradePrice*lTradeQuantity;
					aItem.m_lBuyQuantity = lTradeQuantity;
					if (lRemainQuantity == 0 && aItem.m_lQuantity != aItem.m_lBuyQuantity)
					{
						aItem.m_lQuantity = aItem.m_lBuyQuantity;
						AddMessage(_T("__________::[%s][%s][%s],�ż� �ܿ� ������ [0]���� ���� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString());
					}
					AddMessage(_T("__________::[%s][%s][%s],�ܰ�[%d],����[%d],�ܷ�[%d], �ż� �Ǿ����ϴ�."),
						aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lTradePrice, lTradeQuantity, aItem.m_lQuantity - aItem.m_lBuyQuantity);

					if (aItem.m_lBuyQuantity >= aItem.m_lQuantity)
					{
					//	m_mapOrderCode.RemoveKey(strOrderCode);
						aItem.m_lholdTime = GetTickCount();
						aItem.m_lholdTimeout	= (m_lItemHoldTimeout > 0 ? aItem.m_lholdTime + m_lItemHoldTimeout : 0);
						aItem.m_ltrySellTimeout0 = (m_lItemSellTimeout0 > 0 ? aItem.m_lholdTime + m_lItemSellTimeout0 : 0);
						aItem.m_ltrySellTimeout1 = ((m_lItemSellTimeout0 > 0 && m_lItemSellTimeout1 > 0) ? aItem.m_lholdTime + m_lItemSellTimeout0 + m_lItemSellTimeout1 : 0);
						aItem.m_eitemState = eST_HOLDING;
						AddMessage(_T("__________::[%s][%s][%s],��մܰ�[%d],����[%d],��õ�ȸ��[%d], �ż� �Ϸ� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.BuyPrice(), aItem.m_lQuantity, aItem.m_ltryBuyCount);
					}
				//	return;
				}
				else if (aItem.m_eitemState == eST_TRYSELL ||
					aItem.m_eitemState == eST_WAITSELL ||
					aItem.m_eitemState == eST_WAITSELLMARKETVALUE ||
					aItem.m_eitemState == eST_SELLCANCEL ||
					aItem.m_eitemState == eST_WAITSELLCANCEL)
				{
					ASSERT(bSELLDone);

					aItem.m_lSellCost = lTradePrice*lTradeQuantity;
					aItem.m_lSellQuantity = lTradeQuantity;
					if (lRemainQuantity == 0 && aItem.m_lQuantity != aItem.m_lSellQuantity)
					{
						aItem.m_lQuantity = aItem.m_lSellQuantity;
						AddMessage(_T("__________::[%s][%s][%s],�ŵ� �ܿ� ������ [0]���� ���� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString());
					}
					AddMessage(_T("__________::[%s][%s][%s],�ܰ�[%d],����[%d],�ܷ�[%d], �ŵ� �Ǿ����ϴ�."),
						aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lTradePrice, lTradeQuantity, aItem.m_lQuantity - aItem.m_lSellQuantity);

					if (aItem.m_lSellQuantity >= aItem.m_lQuantity)
					{
						//	m_mapOrderCode.RemoveKey(strOrderCode);

						theApp.m_khOpenApi.SetRealRemove(m_strScrNo, aItem.m_strCode);
						AddMessage(_T("__________::[%s][%s][%s],��õ�ȸ��_�ż�[%d]�ŵ�[%d], �ǽð� ���� ���� ����."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_ltryBuyCount, aItem.m_ltrySellCount);

						aItem.m_eitemState = eST_TRADECLOSING;
						AddMessage(_T("__________::[%s][%s][%s],����[%d],�ż���[%d],�ŵ���[%d], �ŷ� �Ϸ� �Ǿ����ϴ�."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.SellPrice() - aItem.BuyPrice(), aItem.BuyPrice(), aItem.SellPrice());
					}
					//	return;
				}
			}

			ProcessTradeItem(nItemIndex);
			lock.Unlock();
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
	// �����/
	CString strCodeName;
	strCodeName = theApp.m_khOpenApi.GetMasterCodeName(sJongmokCode);

	if (!m_mapJongCode.Lookup(sJongmokCode, strIndex))
	{
		AddMessage(_T("RealData::[%s][%s], grid index[%s], not in m_mapJongCode"), sJongmokCode, strCodeName, strIndex);
		return;
	}

	long nItemIndex = -1;
	if (!m_mapItemCode.Lookup(sJongmokCode, nItemIndex))
	{
		nItemIndex = -1;
		AddMessage(_T("RealData::[%s][%s], grid index[%s], not in m_mapItemCode"), sJongmokCode, strCodeName, strIndex);
	}

//	AddMessage(_T("OnReceiveRealData::"));
	CString strReceivedData;
	CString strData;
	CStringArray arrData;
	long curPrice = 0;
	arrData.Add(sJongmokCode);
	long i=0, nFieldCnt = sizeof(lstOPTSBFID) / sizeof(*lstOPTSBFID);		// ��üũ�� / ����ũ�� = ���Ұ���
	for (i = 1; i < nFieldCnt; i++)
	{
		if (atol(lstOPTSBFID[i].strRealKey) < 0)
		{
			arrData.Add("");
			continue;
		}
		strData = theApp.m_khOpenApi.GetCommRealData(sJongmokCode, atol(lstOPTSBFID[i].strRealKey));
		strData.Trim();
		arrData.Add(strData);
		strReceivedData += lstOPTSBFID[i].strKey;
		strReceivedData += _T(":");
		strReceivedData += strData;
		strReceivedData += _T(",");

		if ( DEF_CUR_PRICE == lstOPTSBFID[i].strKey)
		{
			curPrice = atol(strData);
		}
	}
	ASSERT(MAX_ITEM_COUNT > nItemIndex);
	AddMessage(_T("RealData::[%s][%s][%s],%s"), sJongmokCode, strCodeName, (nItemIndex >= 0 ? m_Item[nItemIndex].GetStateString() : "NotInItem"), strReceivedData);

	if(true)
	{
		if (IsInRound() && 0 <= nItemIndex && nItemIndex < _countof(m_Item) &&
			curPrice > 0 && m_Item[nItemIndex].m_lcurPrice != curPrice &&
			eST_NONE < m_Item[nItemIndex].m_eitemState && m_Item[nItemIndex].m_eitemState < eST_TRADEDONE)
		{
			CSingleLock	lock(&m_criticalItemProcess);
			lock.Lock();
			if (lock.IsLocked())
			{
				m_Item[nItemIndex].m_lcurPrice = curPrice;
				//	AddMessage(_T("OnReceiveRealData::����[%s][%s][%s] ���簡[%s], grid index[%s], item index[%d]"),
				//		sJongmokCode, strCodeName, m_Item[nItemIndex].GetStateString(), GetCurrencyString(m_Item[nItemIndex].m_lcurPrice), strIndex, nItemIndex);

				ProcessTradeItem(nItemIndex);
				lock.Unlock();
			}
		}
	}

//	if (m_eProcessState == ePST_IDLE) AddMessage(strReceivedData);
	SetDataRealAddGrid(arrData, sRealType);

}


//*******************************************************************/
//! Function Name : ConvDataFormat
//! Function      : ���ڿ� ���� ����
//! Param         : long nType, LPCTSTR szData, LPCTSTR szBeforeData/* = ""*/, LPCTSTR szAfterData/* = ""*/
//! Return        : CString
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
CString ConvDataFormat(long nType, LPCTSTR szData, LPCTSTR szBeforeData/* = ""*/, LPCTSTR szAfterData/* = ""*/)
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

			long nFind = strTemp.Find(".");
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
//! Param         : CGridCtrl *pGrid, long nRow, long nCol, LPCTSTR szData
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void SetSignData(CGridCtrl *pGrid, long nRow, long nCol, LPCTSTR szData)
{
	CString strData = szData;
	switch (atol(strData))
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
//! Param         : CGridCtrl *pGrid, long nRow, long nCol, LPCTSTR szData
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void SetDataFgColour(CGridCtrl *pGrid, long nRow, long nCol, LPCTSTR szData)
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

	long i=0, nRow = atol(strTemp), nCnt = arrData.GetSize();

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
			long nCol = lstOPTSBFID[i].nCol;
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
void CABotDlg::SetGridHeight(CGridCtrl& gridCtrl, long row, long height)
{
	if (GetWindowsVersion() > eWindows_7) {
		height *= 2;
	}
	gridCtrl.SetRowHeight(row, height);
}

//*******************************************************************/
//! Function Name : SetGridWidth
//! Function      : �׸��� ���� �����Ѵ�.
//! Param         : -
//! Return        : void
//! Create        : 2016/09/13
//! Comment       : �����츶�� �׸��� ����� �پ��ϹǷ� �̸� ���� �Լ��� ����.
//******************************************************************/
void CABotDlg::SetGridWidth(CGridCtrl& gridCtrl, long col, long width/*=0*/)
{
	if (GetWindowsVersion() > eWindows_7) {
		width = (long)((double)width * 1.7);
	}
	gridCtrl.SetColumnWidth(col, width);
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
	AddMessage("OnReceiveTrData::ScrNo[%s],RQName[%s],TrCode[%s]", sScrNo, sRQName, sTrCode);

	CString strRQName = sRQName;
	if (strRQName == _T("������"))		// �ֽı⺻���� ����
	{
		CString strBuff;
		// ������
	//	strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("������"));	strBuff.Trim();
	//	strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("d+1����������"));	strBuff.Trim();
		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("d+2����������"));	strBuff.Trim();
		
		m_lDepositReceived = atol((LPSTR)(LPCSTR)strBuff);
		
		strBuff = GetCurrencyString(m_lDepositReceived);
		
		((CEdit*)GetDlgItem(IDC_EDIT_DEPOSITRECEIVED))->SetWindowText(strBuff);

		AddMessage(strBuff + " [��]");
	}
	else if (strRQName == _T("�ܰ�"))
	{
		double temp = 0.0;
		CString strBuff;
		
		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("�Ѹ��Աݾ�"));	
		strBuff = GetCurrencyString(strBuff);
		((CEdit*)GetDlgItem(IDC_EDIT_GROSS_PURCHASE))->SetWindowText(strBuff);
		AddMessage("�Ѹ���" + strBuff + " KRW");

		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("���򰡱ݾ�"));
		strBuff = GetCurrencyString(strBuff);
		((CEdit*)GetDlgItem(IDC_EDIT_ASSESSMENT_TOTAL))->SetWindowText(strBuff);
		AddMessage("����" + strBuff + " KRW");

		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("���򰡼��ͱݾ�"));
		strBuff = GetCurrencyString(strBuff);
		((CEdit*)GetDlgItem(IDC_EDIT_PROFIT_LOSS_TOTAL))->SetWindowText(strBuff);
		AddMessage("�Ѽ���" + strBuff + " KRW");

		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("�Ѽ��ͷ�(%)"));
		temp = atof(strBuff);
		temp = temp / (double)100;
		strBuff.Format("%.2f", temp);
		((CEdit*)GetDlgItem(IDC_EDIT_EARNING_RATIO_TOTAL))->SetWindowText(strBuff);
		AddMessage(CString("�Ѽ��ͷ�" + strBuff + " �ۼ�Ʈ")); // %�� %%�� ��� �������� �ʰ� ���߿� ���� �߻�.

		CString	strTRCode(sTrCode);
		long rowCount = theApp.m_khOpenApi.GetRepeatCnt(strTRCode, _T(""));

		m_grdBuyItem.SetRowCount(rowCount + 1);

		if (rowCount >= 1)
		{
			CString strData;

			for (long row = 0; row < rowCount; row++)
			{
				strBuff.Empty();
				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("�����")); strData.Trim();
				strBuff += CString("�����" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 0, DT_CENTER);
				m_grdBuyItem.SetItemText(row + 1, 0, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("���԰�")); strData.Trim();
				strData = GetCurrencyString(strData);
				strBuff += CString("���԰�" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 1, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 1, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("�򰡼���")); strData.Trim();
				strData = GetCurrencyString(strData);
				m_grdBuyItem.SetItemFgColour(row + 1, 2, GetFGColor(strData));
				m_grdBuyItem.SetItemFormat(row + 1, 2, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 2, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("���Աݾ�")); strData.Trim();
				strData = GetCurrencyString(strData);
				strBuff += CString("���Աݾ�" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 3, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 3, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("���ͷ�(%)")); strData.Trim();
				temp = atof(strData);
				temp = temp / (double)100;
				strData.Format("%.2f", temp);
				strBuff += CString("���ͷ�" + strData + ", ");
				m_grdBuyItem.SetItemFgColour(row + 1, 4, GetFGColor(strData));
				m_grdBuyItem.SetItemFormat(row + 1, 4, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 4, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("�ŸŰ��ɼ���")); strData.Trim();
				strData.Format("%d", atol(strData));
				strBuff += CString("���ɼ���" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 5, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 5, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("��������")); strData.Trim();
				strData.Format("%d", atol(strData));
				strBuff += CString("��������" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 6, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 6, strData);
				
				AddMessage(strBuff);
			}
			m_grdBuyItem.Invalidate();
		}
	}
	else if (strRQName == _T("���ں��������Ϳ�û"))
	{
		CString strBuff;

		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("��������"));
		strBuff = GetCurrencyString(strBuff);
		((CEdit*)GetDlgItem(IDC_EDIT_REALIZATION_PROFIT_LOSS))->SetWindowText(strBuff);
		AddMessage("��������" + strBuff + " [��]");
	}
	else if (strRQName == _T("�ֽ��ֹ�"))		// �ֽı⺻���� ����
	{
		CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("�ֹ���ȣ"));	strData.Trim();
		AddMessage("�ֽ��ֹ� �ֹ���ȣ [" + strData + "]");
	}
	else if (strRQName == _T("��������ȸ"))
	{/// 
		CString		strTRCode(sTrCode);
		long nCnt = theApp.m_khOpenApi.GetRepeatCnt(strTRCode, _T(""));

		if (nCnt < 1)
		{
			AddMessage(_T("������ ��ȸ �ݺ������� ����."));
			return;
		}

		CString		strSymbol, strSymbolName, strLoanDate, strText;
		for (long nIdx = 0; nIdx < nCnt; nIdx++)
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



void CABotDlg::OnBnClickedButtonGetCondition()
{
	long nGetConditionRet = theApp.m_khOpenApi.GetConditionLoad();
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
	m_cmbCondtion.ResetContent();

	long i(0);

	while (AfxExtractSubString(strCondition, strConditionNameList, i++, _T(';')))
	{
		if (strCondition != _T(""))
		{
			strCondition.Trim();
			long nPos = strCondition.Find("^");
			long nConditionNo = atol(strCondition.Left(nPos));
			strConditionName = strCondition.Mid(nPos + 1, strCondition.GetLength());
			strIndex.Format(_T("%d"), nConditionNo);
			m_mapNameList.SetAt(strConditionName, strIndex);
			m_cmbCondtion.InsertString(i - 1, strConditionName);
		}
	}

	static BOOL bConfigApplied = FALSE;

	if (m_cmbCondtion.GetCount() > 0)
	{
		m_cmbCondtion.SetCurSel(0);

		if (!bConfigApplied)
		{
			CString strBuf, strCombo;
			// �˻� ����
			ReadFromIniFile_String(m_strConfigFile, "CONDITION", "name", "-", strBuf);
			for (i = 0; i < m_cmbCondtion.GetCount(); i++)
			{
				m_cmbCondtion.GetLBText(i, strCombo);
				if (strCombo == strBuf)
				{
					m_cmbCondtion.SetCurSel(i);
				}
			}
			bConfigApplied = TRUE;
		}
	}
}


void CABotDlg::OnBnClickedButtonGetCondItems()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	REQ_ItemSearch();
}

BOOL CABotDlg::REQ_ItemSearch()
{
//	DWORD pId = GetCurrentProcessId();
//	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));
	theApp.m_khOpenApi.SetRealRemove(_T("ALL"), _T("ALL"));

	CString strConditionCodeList = _T(""), strIndex;

	//������ ���Ǹ��� �˻��� ���񸮽�Ʈ�� �ҷ��´�.
	m_cmbCondtion.GetLBText(m_cmbCondtion.GetCurSel(), m_strConditionName);
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
	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 1);
//	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 0);
	AddMessage("�˻���[%d][%s], �ǽð� ���� �˻� ��û %s.", m_nConditionIndex, m_strConditionName, (bRet ? "����" : "����"));
	return bRet;
}


void CABotDlg::OnBnClickedButtonRegItems()
{
	REQ_ItemRealReg();
}

void CABotDlg::OnBnClickedButtonGetBalance()
{
	REQ_BalanceInfo();
}

void CABotDlg::OnBnClickedButtonDebugTest()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	long ret = 0;
	ret = CalcBuyAndSellPrice(5300, 1.0, FALSE);
}

BOOL CABotDlg::REQ_ItemRealReg()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (m_ItemCount <= 0) 
	{
		return FALSE;
	}
	CString strTargetCodeList;
	long nIndex = 0;
	long nMaxIndex = min(m_ItemCount, _countof(m_Item));
	ASSERT(MAX_ITEM_COUNT >= nMaxIndex);
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
//!						: long nConditionIndex				- ���Ǹ� �ε�����ȣ
//!						: long nNext							- ������ȸ(2:������ȸ, 0:������ȸ����)
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveTrCondition(LPCTSTR sScrNo, LPCTSTR strCodeList, LPCTSTR strConditionName, long nConditionIndex, long nNext)
{
//	DWORD pId = GetCurrentProcessId();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	AddMessage(_T("OnReceiveTrCondition::"));

	CString strCndName, sConditionName;
	m_cmbCondtion.GetLBText(m_cmbCondtion.GetCurSel(), strCndName);	//���õ� ���Ǹ�

	if (strConditionName == strCndName)	//���� ���Ǹ�� ��ȸ�������� ���� ���Ǹ��� �������� ó��.
	{
		AddMessage("�˻���[%d][%s], ���� �˻� ��û ���,", nConditionIndex, strConditionName);

		if (strlen(strCodeList)>0)
		{
			long nIndex(0);
			CString strConditionCode;
			m_mapJongCode.RemoveAll();		// �����ڵ� �ʱ�ȭ
			m_grdRealAdd.SetRowCount(1);

			while (AfxExtractSubString(strConditionCode, strCodeList, nIndex++, _T(';')))
			{
			}

			m_grdRealAdd.SetRowCount(nIndex - 1);

			m_ItemCount = 0;
			m_mapItemCode.RemoveAll();
			m_mapUsedItemCode.RemoveAll();

			nIndex = 0;
			CString strIndex;
			while (AfxExtractSubString(strConditionCode, strCodeList, nIndex++, _T(';')))
			{
				if (strConditionCode != "")
				{
					SetGridHeight(m_grdRealAdd, nIndex, 24); // grid ���̴� ������ ���� �����ϹǷ� ���� �Լ��� ȣ���Ѵ�.
					m_grdRealAdd.SetItemText(nIndex, 0, strConditionCode);

					// �����
					CString strCodeName;
					strCodeName = theApp.m_khOpenApi.GetMasterCodeName(strConditionCode);
					m_grdRealAdd.SetItemText(nIndex, 1, strCodeName);

					strIndex.Format("%3d", nIndex);
					m_mapJongCode.SetAt(strConditionCode, strIndex);
					AddMessage(_T("[%s][%s][%s] �˻���."), strIndex, strConditionCode, strCodeName);

					if ((IsInRound() && IsInRoundTime() && 0 <= m_ItemCount && m_ItemCount < min(long(_countof(m_Item)), m_nProcessItemCount)) ||
						(!IsInRound() && 0 <= m_ItemCount && m_ItemCount < long(_countof(m_Item))))
					{
						AddMessage("[%3d][%s][%s] ��� ����鿡 �߰�", m_ItemCount, strConditionCode, strCodeName);
						m_Item[m_ItemCount].m_eitemState = eST_ADDED;
						m_Item[m_ItemCount].m_strName = strCodeName;
						m_Item[m_ItemCount].m_strCode = strConditionCode;
						m_mapItemCode[m_Item[m_ItemCount].m_strCode] = m_ItemCount;
						m_mapUsedItemCode[m_Item[m_ItemCount].m_strCode] = m_ItemCount;
						m_ItemCount++;
					}
				}
			}
		//	m_ItemCount = 1;	//����׿�.
		}
		else
		{
			m_mapJongCode.RemoveAll();		// �����ڵ� �ʱ�ȭ
			m_grdRealAdd.SetRowCount(1);

			m_ItemCount = 0;
			m_mapItemCode.RemoveAll();
			m_mapUsedItemCode.RemoveAll();

			AddMessage(_T("�˻��� ������ �����ϴ�."));
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveRealCondition
//! Function      : ���ǰ˻� �ǽð� ���� ����/����
//! Param         : LPCTSTR strCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex
//! Return        : void
//! Create        : , 2015/04/20
//! Comment       : 
//******************************************************************/

void CABotDlg::OnReceiveRealCondition(LPCTSTR sTrCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex)
{
//	DWORD pId = GetCurrentProcessId();

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
//	AddMessage(_T("OnReceiveRealCondition::"));

	CString strName, strMsg, strIndex, sType, sCode;
	sType = strType;	//���� ����, ��Ż ����
	sCode = sTrCode;	//�����ڵ�
	m_cmbCondtion.GetLBText(m_cmbCondtion.GetCurSel(), strName);	//���� ���õ� ���Ǹ�
	if (IsInRound()) { strName = m_strConditionName; }

	// �����/
	CString strCodeName;
	strCodeName = theApp.m_khOpenApi.GetMasterCodeName(sCode);

	AddMessage(_T("OnReceiveRealCondition::�˻���[%s],[%s][%s]������ %s�Ǿ����ϴ�."), strConditionName, sCode, strCodeName, (sType=="I"?"����":"��Ż"));

	if (strName == strConditionName)	//���� ���õ� ���Ǹ�� �ǽð����� ���� ���Ǹ��� ������ ��.
	{
		if (sType == "I")	//���� ����
		{
			long aIndex = 0;
			if (m_mapItemCode.Lookup(sCode, aIndex)) // �̹� �ִ� ������ ��� �ٽ� �ż� ���� ����Ʈ�� �ű��� �ʴ´�.
			{
				return;
			}

			m_grdRealAdd.InsertRow(sCode, -1);

			SetGridHeight(m_grdRealAdd, m_ItemCount + 1, 24); // grid ���̴� ������ ���� �����ϹǷ� ���� �Լ��� ȣ���Ѵ�.
			m_grdRealAdd.SetItemText(m_ItemCount + 1, 0, sCode);

			m_grdRealAdd.SetItemText(m_ItemCount + 1, 1, strCodeName);

			strMsg.Format(_T("______________________::[%s][%s]������ ���ԵǾ����ϴ�."), sCode, strCodeName);
			AddMessage(strMsg);

			//������ȸ�� ������ �ü��� ������ �ִ� ��Ȳ���� �߰��� ���Ե� ������ �ü��� �����ٶ� 
			// ������ �Ķ���Ϳ� "1"�� �־��־�� ���� ����� �Բ� ���Ե� ������ �ü��� ���´�.
			// ���� "1" �̿��� "0"�� �־��ָ� ������ ����� ����鿡 ���� �ü��� ��������.
			//�ǽð���� �Լ� ȣ��
			long lRet = theApp.m_khOpenApi.SetRealReg(m_strScrNo, sCode, FIDLIST, "1");
			AddMessage("______________________::[%s][%s]���� �ǽð� ��ȸ ��û %s. ret[%d]", sCode, strCodeName, (lRet >= 0 ? "����" : "����"), lRet);

			if (lRet >= 0)
			{
				aIndex = 0;
				if (!m_mapUsedItemCode.Lookup(sCode, aIndex))
				{
					if ((IsInRound() && IsInRoundTime() && 0 <= m_ItemCount && m_ItemCount < min(long(_countof(m_Item)), m_nProcessItemCount)) ||
						(!IsInRound() && 0 <= m_ItemCount && m_ItemCount < long(_countof(m_Item))))
					{
						AddMessage("______________________::[%s][%s]���� �������[%3d]�� �߰�", sCode, strCodeName, m_ItemCount);
						m_Item[m_ItemCount].m_eitemState = eST_ADDED;
						m_Item[m_ItemCount].m_strName = strCodeName;
						m_Item[m_ItemCount].m_strCode = sCode;
						m_mapItemCode[m_Item[m_ItemCount].m_strCode] = m_ItemCount;
						m_mapUsedItemCode[m_Item[m_ItemCount].m_strCode] = m_ItemCount;
						m_ItemCount++;
					}
				}
			}
		}
		else if (sType == "D")	//���� ��Ż
		{
			//������ �ִ� �������� Ȯ��.
			if (!m_mapJongCode.Lookup(sCode, strIndex))
			{
				return;
			}

		//	long aIndex = 0;
		//	if (m_mapItemCode.Lookup(sCode, aIndex))
		//	{
		//		nItemIndex = aIndex;
		//
		//		if (aIndex < _countof(m_Item))
		//		{
		//			m_Item[aIndex].m_eitemState = eST_ADDED;
		//			m_Item[aIndex].m_strName = strCodeName;
		//			m_Item[aIndex].m_strCode = sCode;
		//			m_mapItemCode.RemoveKey(sCode);
		//			m_ItemCount--;
		//			//�ε��� ������ �ؾ� �ϴ±���.
		//		}
		//	}

			//��Ż ���� ����.
		//	m_grdRealAdd.DeleteRow(_ttoi(strIndex));

			//��Ż�� ������ �ü��� ���� �������� ����Ѵ�.
		//	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, sCode);

			strMsg.Format(_T("______________________::[%s][%s]������ ��Ż�Ǿ����ϴ�."), sCode, strCodeName);
			AddMessage(strMsg);
		}

		m_grdRealAdd.Invalidate();

		m_mapJongCode.RemoveAll();

		int nCount = m_grdRealAdd.GetRowCount();
		for (int i = 1; i < nCount; i++)
		{
			sCode = m_grdRealAdd.GetItemText(i, 0);
			strIndex.Format("%d", i);
			m_mapJongCode.SetAt(sCode, strIndex);
		}
	}
}

void CABotDlg::SetControls(BOOL bEnable)
{
	GetDlgItem(IDC_EDIT_ACCNO)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_GETCONDITION)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_GETITEM)->EnableWindow(bEnable);
	GetDlgItem(IDC_BUTTON_REGITEM)->EnableWindow(bEnable);
	m_cmbCondtion.EnableWindow(bEnable);
	m_cmbItemCount.EnableWindow(bEnable);
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
	m_cmbSellTimeOut0.EnableWindow(bEnable);
	m_cmbSellTimeOut1.EnableWindow(bEnable);
	m_cmbSellOverThis0.EnableWindow(bEnable);
	m_cmbSellUnderThis0.EnableWindow(bEnable);
	m_cmbSellOverThis1.EnableWindow(bEnable);
	m_cmbSellUnderThis1.EnableWindow(bEnable);
	m_cmbSellOverThis2.EnableWindow(bEnable);
	m_cmbSellUnderThis2.EnableWindow(bEnable);
	m_checkDoSellItemMarketValueAtRoundEnd.EnableWindow(bEnable);
}

void CABotDlg::OnBnClickedButtonStartRound()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (!IsInRound())
	{
		if (REQ_DepositReceived())
		{
			SetDisableControls();
			m_bDoFinishProcess = FALSE;
			m_eProcessState = ePST_ROUND_START;
			SetTimer(ROUND_TIMER, 10, NULL);
			AddMessage(_T("���� ���� ��û."));
		}
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
		SetWindowText("ABot");
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
	if (TRUE)//m_eProcessState <= ePST_ITEM_WAIT)
	{
		SetEnableControls();
		KillTimer(ROUND_TIMER);
		m_eProcessState = ePST_IDLE;
		AddMessage(_T("���� �ߴ�."));
		SetWindowText("ABot");
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


void CABotDlg::OnBnClickedButtonEditInround()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	/*
	if (IsInRound())
	{
		CString strBuf;
		GetDlgItemText(IDC_BUTTON_EDIT_INROUND, strBuf);
		if (strBuf == "���� ����")
		{
			m_cmbSellOverThis.EnableWindow(TRUE);
			m_cmbSellUnderThis.EnableWindow(TRUE);
			SetDlgItemText(IDC_BUTTON_EDIT_INROUND, "���� ����");
		}
		else if (strBuf == "���� ����")
		{
			m_cmbSellOverThis.EnableWindow(FALSE);
			m_cmbSellUnderThis.EnableWindow(FALSE);
			SetDlgItemText(IDC_BUTTON_EDIT_INROUND, "���� ����");

			m_cmbSellOverThis.GetWindowText(strBuf);
			double dSellOverThis = atof((LPSTR)(LPCSTR)strBuf);

			m_cmbSellUnderThis.GetWindowText(strBuf);
			double dSellUnderThis = atof((LPSTR)(LPCSTR)strBuf);

			if (m_dSellOverThis != dSellOverThis || m_dSellUnderThis != dSellUnderThis)
			{
				m_dSellOverThis = dSellOverThis;
				m_dSellUnderThis = dSellUnderThis;
				AddMessage(_T("_____::[����]����� �ŵ� Sell'OVER'This�� %f[�ۼ�Ʈ] �Դϴ�."), m_dSellOverThis);
				AddMessage(_T("_____::[����]����� �ŵ� Sell'UNDER'This�� %f[�ۼ�Ʈ] �Դϴ�."), m_dSellUnderThis);

				ProcessTradeAll();
			}
		}
	}
	*/
}


BOOL CABotDlg::IsInRoundTime()
{
	SYSTEMTIME  systmCurTime;
	GetLocalTime(&systmCurTime);

	long	iHour = systmCurTime.wHour;
	long	iMin = systmCurTime.wMinute;

	CString strBuf;
	m_cmbStartHour.GetLBText(m_cmbStartHour.GetCurSel(), strBuf);
	long iSHour = atol((LPSTR)(LPCSTR)strBuf);
	
	m_cmbStartMin.GetLBText(m_cmbStartMin.GetCurSel(), strBuf);
	long iSMin = atol((LPSTR)(LPCSTR)strBuf);

	m_cmbEndHour.GetLBText(m_cmbEndHour.GetCurSel(), strBuf);
	long iEHour = atol((LPSTR)(LPCSTR)strBuf);

	m_cmbEndMin.GetLBText(m_cmbEndMin.GetCurSel(), strBuf);
	long iEMin = atol((LPSTR)(LPCSTR)strBuf);

	if ( iSHour <= iHour && iSMin <= iMin)
	{
		if ( (iHour < iEHour) || (iHour == iEHour && iMin < iEMin) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CABotDlg::getAccountData()
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

	return TRUE;
}

void CABotDlg::ProcessSequence()
{
	BOOL bGoTrade = FALSE;

	switch (m_eProcessState)
	{
	case ePST_ROUND_START:	//���� ���� ����.
		m_mapOrderCode.RemoveAll();
		m_mapItemCode.RemoveAll();
		m_mapUsedItemCode.RemoveAll();
		m_grdRealAdd.SetRowCount(1);
		m_nProcessRetryCount = 0;
		InitProcessCondition();
		AddMessage(_T("����::���� �մϴ�."));
		m_eProcessState = ePST_WAIT_TIME;
		break;

	case ePST_WAIT_TIME:	//���� ���� ���� �ð��� ��ٸ��� ����.
		m_nProcessRetryCount++;
		if (IsInRoundTime())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����::������ �˻��� ���� �մϴ�."));
			m_eProcessState = ePST_DR_SEARCH;
			break;
		}
		break;

	case ePST_DR_SEARCH:	//������ ��û ����.
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
			AddMessage(_T("����::������ ��ȸ�� ���� �����Ͽ� ���μ����� �ߴ��մϴ�."));
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
			AddMessage(_T("����::������ %d[��], ���带 �����մϴ�."), m_lDepositReceived);
			m_eProcessState = ePST_ROUND_END;
			break;
		}
		else if (m_lDepositReceived > 0)
		{
			CString strBuf;
			m_cmbMaxTotalAmount.GetLBText(m_cmbMaxTotalAmount.GetCurSel(), strBuf);
			long lMaxTotalAmount = atol((LPSTR)(LPCSTR)strBuf) * 10000;	// ������ �����̹Ƿ� 10000�� ���Ѵ�.

			m_cmbDpUseRate.GetLBText(m_cmbDpUseRate.GetCurSel(), strBuf);
			long nUseRate = atol((LPSTR)(LPCSTR)strBuf);

			m_lProcessDR = min(long(m_lDepositReceived*(nUseRate / 100.0)), lMaxTotalAmount);
			AddMessage(_T("����::��� �ݾ� ������ %s[��] �Դϴ�."), GetCurrencyString(m_lProcessDR));

			if (m_lProcessDR < m_lProcessItemDR)
			{
				m_nProcessRetryCount = 0;
				AddMessage(_T("����::��ü ��� �ݾ� %s[��]�� ���� ��� �ݾ�(������ ����) %s[��]���� �۽��ϴ�. ���带 �����մϴ�."), GetCurrencyString(m_lProcessDR), GetCurrencyString(m_lProcessItemDR));
				m_eProcessState = ePST_ROUND_END;
				break;
			}
	
			m_nProcessRetryCount = 0;
			AddMessage(_T("����::���� �˻��� ���� �մϴ�."));
			m_eProcessState = ePST_ITEM_SEARCH;
			break;
		}
		break;

	case ePST_ITEM_SEARCH:	//���� �˻� ��û ����.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//���� ���� �ð� Ȯ��

		if (m_cmbCondtion.GetCount()<=0)
		{
			OnBnClickedButtonGetCondition();
			break;
		}

		if (REQ_ItemSearch())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����::���� ��û �Ϸ�. ����� ��ٸ��ϴ�."));
			m_eProcessState = ePST_ITEM_WAIT;
			break;
		}
	//	else
	//	{
	//		//�˻� ��û ���н�. ȭ�� ��ȣ�� �ٲ㺸��,. �׷��� �ҿ� ����.
	//		m_strScrNo.Format("%04d", rand() % 10000);
	//		Sleep(min(2000, m_nProcessRetryCount*10));
	//	}
		break;

	case ePST_ITEM_WAIT:	//���� �˻� ��� ��� ����.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//���� ���� �ð� Ȯ��

		if (m_ItemCount == -1)
		{
			break;
		}
		AddMessage(_T("����::[%d]���� ������ �˻��Ǿ����ϴ�."), m_ItemCount);
		bGoTrade = FALSE;
		if (m_ItemCount == 0)
		{
			AddMessage(_T("����::������ �˻��ɶ� �ŷ��� �����մϴ�."));
			bGoTrade = TRUE;
		}
		
		if (m_ItemCount > 0)
		{
			if (REQ_ItemRealReg())
			{
				bGoTrade = TRUE;
			}
		}
		
		if (bGoTrade)
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����::���� �ŷ��� ���� �մϴ�."));
			m_eProcessState = ePST_ITEM_TRADE;
			break;
		}
		break;

	case ePST_ITEM_TRADE:	//���� �ŷ� ����.
		m_nProcessRetryCount++;
		ProcessTradeAll();
		if (IsEndTrade() && 
			(m_nProcessItemCount <= m_ItemCount || !IsInRoundTime()) )
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("����::���� �ŷ��� ��� ���� �Ǿ����ϴ�."));
			m_eProcessState = ePST_ROUND_END;
			break;
		}
		m_nProcessRetryCount--;
		break;

	case ePST_ROUND_END:	//���� ������ ����.
		m_nProcessRetryCount++;
		AddMessage(_T("����::���� �մϴ�."));

		ReportAllTrade();

		m_nProcessRetryCount = 0;
		m_eProcessState = ePST_KILL_PROC;
		break;

	case ePST_KILL_PROC:
		m_nProcessRetryCount = 0;
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

void CABotDlg::ProcessTradeAll()
{
	CSingleLock	lock(&m_criticalItemProcess);
	lock.Lock();
	if (lock.IsLocked())
	{
		long i = 0;
		for (i = 0; i < m_ItemCount; i++)
		{
			ProcessTradeItem(i, TRUE);
		}
		lock.Unlock();
	}
}

void CABotDlg::ProcessTradeItem(int nItemId, BOOL bFromAllTrade/*=FALSE*/)
{
	if (nItemId < 0 || _countof(m_Item) <= nItemId)
	{
		return;
	}

	CABotItem &aItem = m_Item[nItemId];

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
	//	if (aItem.m_lQuantity != 0 && aItem.m_lQuantity - aItem.m_lBuyQuantity <= 0)
	//	{
	//		aItem.m_eitemState = eST_HOLDING;
	//		break;
	//	}
		if (aItem.m_ltryBuyCount == 0 && m_lProcessDR < m_lProcessItemDR)
		{
			//�ǽð� ������ �����ִ� �׸��忡�� �����
			CString strIndex;
			if (strIndex.GetLength()>0)
			{
				m_grdRealAdd.DeleteRow(_ttoi(strIndex));
			}
			//�ǽð� ������ �����ִ� �׸����� �������� �����
			m_mapJongCode.RemoveKey(aItem.m_strCode);

			//m_Item[MAX_ITEM_COUNT]������ item Index�� �㿡�� �����.
			m_mapItemCode.RemoveKey(aItem.m_strCode);

			//������ �ü��� ���� �ʴ´�.
			theApp.m_khOpenApi.SetRealRemove(m_strScrNo, aItem.m_strCode);

			//�ٽ� �������� �˻��ɶ� ���� �ȳ�~
			AddMessage(_T("����::[%s][%s][%s], �ܿ� ��� �ݾ� %s[��]�� ���� ��� �ݾ�(������ ����) %s[��]���� �۽��ϴ�. �ٽ� ���� ���� �����Ҷ����� �ŷ��� ���� �ʽ��ϴ�."), 
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), GetCurrencyString(m_lProcessDR), GetCurrencyString(m_lProcessItemDR));
			aItem.m_eitemState = eST_NONE;
			break;
		}

		if (REQ_ItemBuyOrder(aItem, bFromAllTrade))
		{
			AddMessage(_T("����::[%s][%s][%s],�ܰ�[%d],����[%d], �ż��� �õ� �Ǿ����ϴ�."), 
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_WAITBUY;

			if (aItem.m_ltryBuyCount == 0)
			{
				m_lProcessDR -= m_lProcessItemDR;
				AddMessage(_T("����::��� �ݾ� ������ %s[��]�� �Ǿ����ϴ�."), GetCurrencyString(m_lProcessDR));
			}
			break;
		}
		break;

	case eST_WAITBUY:	//�ż� ü�� ��� ����.
	//	if (aItem.m_lQuantity != 0 && aItem.m_lQuantity - aItem.m_lBuyQuantity <= 0)
	//	{
	//		aItem.m_eitemState = eST_HOLDING;
	//		break;
	//	}
		if (aItem.m_ltryBuyTimeout > 0 && long(GetTickCount()) > aItem.m_ltryBuyTimeout)
		{
			aItem.m_ltryBuyCount++;
			if (aItem.m_ltryBuyCount > m_lItemBuyTryCount)
			{
				AddMessage(_T("����::[%s][%s][%s],�ܰ�[%d],����[%d],��õ�[%d], �ŷ��� ���� �մϴ�."),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_ltryBuyCount);
				aItem.m_eitemState = eST_BUYCANCEL;
				break;
			}

			AddMessage(_T("����::[%s][%s][%s],�ܰ�[%d],����[%d], �ż��� ��õ�[%d] �Ǿ����ϴ�."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_ltryBuyCount);
			aItem.m_eitemState = eST_TRYBUY;
			break;
		}
		break;

	case eST_BUYCANCEL:
		if (REQ_ItemBuyCancel(aItem, bFromAllTrade))
		{
			aItem.m_eitemState = eST_WAITBUYCANCEL;
			break;
		}
		break;

	case eST_WAITBUYCANCEL:
		break;

	case eST_HOLDING:	//���� ����.
		if (true)//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			aItem.m_lsellPrice = CalcBuyAndSellPrice(aItem.BuyPrice(), m_dSellOverThis0, TRUE);
			aItem.m_lunderPrice = long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis0 / 100.);
			AddMessage(_T("����::[%s][%s][%s],��ǥ��[%d],��ǥ�ϰ�[%d],����[%d], �ŵ��� �����մϴ�."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lunderPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_TRYSELL;
		//	break;
		}
		/*
		else if (aItem.m_lcurPrice > long(aItem.BuyPrice() + aItem.BuyPrice()*m_dSellOverThis / 100.))
		{
			aItem.m_lsellPrice = aItem.m_lcurPrice;
			AddMessage(_T("����::[%s][%s][%s],���簡��[%d],����[%d], �ŵ��� �õ��մϴ�. ���."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_TRYSELL;
		//	break;
		}
		else if (aItem.m_lcurPrice < long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis / 100.))
		{
			aItem.m_lsellPrice = aItem.m_lcurPrice;
			AddMessage(_T("����::[%s][%s][%s],���簡��[%d],����[%d], �ŵ��� �õ��մϴ�. �϶�."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_TRYSELL;
		//	break;
		}
		else if (aItem.m_lholdTimeout>0 && aItem.m_lholdTime + long(GetTickCount()) > aItem.m_lholdTimeout)
		{
			aItem.m_lsellPrice = aItem.m_lcurPrice;
			AddMessage(_T("����::[%s][%s][%s],���簡~[%d],����[%d], �ŵ��� �õ��մϴ�. Ÿ�Ӿƿ�."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_TRYSELL;
		//	break;
		}
		*/
		if (aItem.m_eitemState == eST_TRYSELL && aItem.m_lBuyQuantity != aItem.m_lQuantity)
		{
			AddMessage(_T("����::[%s][%s][%s],�ŵ� ���� ����, ���ʼ���[%d]<==�ż�����[%d]."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lQuantity, aItem.m_lBuyQuantity);
			aItem.m_lQuantity = aItem.m_lBuyQuantity;
		}

		if (aItem.m_eitemState != eST_TRYSELL)
		{
			break;
		}

	case eST_TRYSELL:	//�ŵ� �õ� ����.
		if (aItem.m_lQuantity != 0 && aItem.m_lQuantity - aItem.m_lSellQuantity <= 0)
		{
			aItem.m_eitemState = eST_TRADECLOSING;
			break;
		}
		if (REQ_ItemSellOrder(aItem, FALSE, bFromAllTrade))
		{
		//	aItem.m_ltrySellTimeout = 0;

			AddMessage(_T("����::[%s][%s][%s],�ܰ�[%d],����[%d], �ŵ��� �õ� �Ǿ����ϴ�."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity - aItem.m_lSellQuantity);
			aItem.m_eitemState = eST_WAITSELL;
			break;
		}
		break;

	case eST_WAITSELL:	//�ŵ� ü�� ��� ����.
		if (aItem.m_lQuantity != 0 && aItem.m_lQuantity - aItem.m_lSellQuantity <= 0)
		{
			aItem.m_eitemState = eST_TRADECLOSING;
			break;
		}

		if (m_bDoSellItemMarketValueAtRoundEnd && !IsInRoundTime())
		{
			AddMessage(_T("����::[%s][%s][%s],���簡[%d],����[%d], ���� �ð� �� ���尡 �ŵ� �ɼǿ� ����, �ŵ��� ���尡�� �õ��մϴ�."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_SELLMARKETVALUE;
			break;
		}
		else if (aItem.m_lcurPrice <= aItem.m_lunderPrice)
		{
			AddMessage(_T("����::[%s][%s][%s],���簡��[%d],����[%d], �ŵ��� ���尡�� �õ��մϴ�. �϶�."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_SELLMARKETVALUE;
			break;
		}
		else if (aItem.m_ltrySellTimeout0 > 0 && long(GetTickCount()) > aItem.m_ltrySellTimeout0)
		{
			aItem.m_ltrySellTimeout0 = 0;
			long aunderPrice = long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis1 / 100.);
			long asellPrice = CalcBuyAndSellPrice(aItem.BuyPrice(), m_dSellOverThis1, TRUE);
			
			//���� ������ ���ذ� �ƴϸ� ���簡�� �ŵ� �õ��Ѵ�.
			long lestimatedSellprice = long(aItem.m_lcurPrice*(1 - m_dSellTradeFee / 100.) - long(aItem.BuyPrice()*(m_dBuyTradeFee / 100.)));
			if (aItem.BuyPrice() >= lestimatedSellprice)
			{
				aItem.m_lsellPrice = aItem.m_lcurPrice;
				AddMessage(_T("����::[%s][%s][%s],���簡[%d],����[%d], �ŵ� �ð� Ÿ�� �ƿ�#0, ���簡�� �ŵ� �õ��մϴ�."),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			}

			if (aItem.m_lsellPrice != asellPrice || aItem.m_lunderPrice != aunderPrice)
			{
				aItem.m_lunderPrice = aunderPrice;
				AddMessage(_T("����::[%s][%s][%s],��ǥ��[%d],��ǥ�ϰ�[%d],����[%d], �ŵ� �ð� Ÿ�� �ƿ�#0, ��/�ϰ� ���� �Ǿ����ϴ�."),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lunderPrice, aItem.m_lQuantity);

				if (aItem.m_lsellPrice != asellPrice)
				{
					aItem.m_lsellPrice = asellPrice;
					aItem.m_eitemState = eST_TRYSELL;
				}
			}
			break;
		}
		else if (aItem.m_ltrySellTimeout1 > 0 && long(GetTickCount()) > aItem.m_ltrySellTimeout1)
		{
			aItem.m_ltrySellTimeout1 = 0;
			long aunderPrice = long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis2 / 100.);
			long asellPrice = CalcBuyAndSellPrice(aItem.BuyPrice(), m_dSellOverThis2, TRUE);

			//���� ������ ���ذ� �ƴϸ� ���簡�� �ŵ� �õ��Ѵ�.
			long lestimatedSellprice = long(aItem.m_lcurPrice*(1 - m_dSellTradeFee / 100.) - long(aItem.BuyPrice()*(m_dBuyTradeFee / 100.)));
			if (aItem.BuyPrice() >= lestimatedSellprice)
			{
				aItem.m_lsellPrice = aItem.m_lcurPrice;
				AddMessage(_T("����::[%s][%s][%s],���簡[%d],����[%d], �ŵ� �ð� Ÿ�� �ƿ�#1, ���簡�� �ŵ� �õ��մϴ�."),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			}

			if (aItem.m_lsellPrice != asellPrice || aItem.m_lunderPrice != aunderPrice)
			{
				aItem.m_lunderPrice = aunderPrice;
				AddMessage(_T("����::[%s][%s][%s],��ǥ��[%d],��ǥ�ϰ�[%d],����[%d], �ŵ� �ð� Ÿ�� �ƿ�#1, ��/�ϰ� ���� �Ǿ����ϴ�."),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lunderPrice, aItem.m_lQuantity);

				if (aItem.m_lsellPrice != asellPrice)
				{
					aItem.m_lsellPrice = asellPrice;
					aItem.m_eitemState = eST_TRYSELL;
				}
			}
			break;
		}
		break;

	case eST_SELLCANCEL:
		if (REQ_ItemSellCancel(aItem, bFromAllTrade))
		{
			aItem.m_eitemState = eST_WAITSELLCANCEL;
			break;
		}
		break;

	case eST_WAITSELLCANCEL:
		break;

	case eST_SELLMARKETVALUE:
		if (REQ_ItemSellOrder(aItem, TRUE, bFromAllTrade))
		{
			aItem.m_eitemState = eST_WAITSELLMARKETVALUE;
			break;
		}
		break;

	case eST_WAITSELLMARKETVALUE:
		break;

	case eST_TRADECLOSING:
		{
			theApp.m_khOpenApi.SetRealRemove(m_strScrNo, aItem.m_strCode);

			long lactBuyCost = aItem.m_lBuyCost;
			long lactSellCost = long(aItem.m_lSellCost*(1 - m_dSellTradeFee / 100.) - long(aItem.m_lBuyCost*(m_dBuyTradeFee / 100.)));
			if (lactBuyCost == 0) {	lactBuyCost = 1; }
			AddMessage(_T("��� =================================================================================================================="));
			AddMessage(_T("      [%s][%s],�� ���Ա�[%s], �� �ŵ���[%s], ���� ����[%s], �� ������[%4.2f]�ۼ�Ʈ �Դϴ�."),
				aItem.m_strCode, aItem.m_strName, GetCurrencyString(lactBuyCost), GetCurrencyString(lactSellCost), GetCurrencyString(lactSellCost - lactBuyCost), double(lactSellCost - lactBuyCost) / double(lactBuyCost)*100.0);
			AddMessage(_T("��� =================================================================================================================="));

			m_lProcessDR += m_lProcessItemDR;

			m_lProcessDR -= lactBuyCost;
			m_lProcessDR += lactSellCost;
			AddMessage(_T("��� �ݾ� ������ %s[��]�� �Ǿ����ϴ�."), GetCurrencyString(m_lProcessDR));
		}
		aItem.m_eitemState = eST_TRADEDONE;
		break;

	case eST_TRADEDONE:	//�ŷ� �Ϸ� ����.
		break;
	}
}

BOOL CABotDlg::REQ_ItemBuyOrder(CABotItem &aItem, BOOL bFromAllTrade)
{
	CString strRQName = "�ֽ��ֹ�";

	// �Ÿű��� ���(1:�űԸż�, 2:�űԸŵ� 3:�ż����, 4:�ŵ����, 5:�ż�����, 6:�ŵ�����)
	long lOrderType = 1;
//	if (aItem.m_ltryBuyCount > 0)
//	{
//		lOrderType = 5;	//5:�ż�����
//	}

	// �ŷ����� ���
	// 00:������, 03:���尡, 05:���Ǻ�������, 06:������������, 07:�ֿ켱������, 
	// 10:������IOC, 13:���尡IOC, 16:������IOC, 20:������FOK, 23:���尡FOK, 
	// 26:������FOK, 61:�尳�����ð���, 62:�ð��ܴ��ϰ��Ÿ�, 81:���Ľð�������
	CString strHogaGb = "00";
	ASSERT(aItem.m_lcurPrice != 0);
	if (aItem.m_lcurPrice == 0){ return FALSE; }

	if (aItem.m_lbuyPrice == aItem.m_lcurPrice)
	{
		aItem.m_ltryBuyTimeout = GetTickCount() + m_lItemBuyTimeout;

		return TRUE;
	}
	
	aItem.m_ltryBuyTimeout = 0;

	long lcurPrice = aItem.m_lcurPrice;
	long nextQuantity = long((m_lProcessItemDR - aItem.m_lBuyCost) / lcurPrice);

	if (nextQuantity <= 0)	//���� ���� ������ 0�� ��� ���� ó��.
	{
		AddMessage(_T("BUY_ORDER::[%s],[%s][%s][%s],�ܰ�[%d],����[%d], �ŷ��� ���� �մϴ�."),
			GetOrderTypeString(lOrderType), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity);
		if (aItem.m_lBuyQuantity > 0)
		{
			//�ż� ���.
			if (!REQ_ItemBuyCancel(aItem, bFromAllTrade))
			{
				return FALSE;
			}

			aItem.m_strBuyOrder = "";
			aItem.m_lholdTime = GetTickCount();
			aItem.m_lholdTimeout = (m_lItemHoldTimeout > 0 ? aItem.m_lholdTime + m_lItemHoldTimeout : 0);
			aItem.m_ltrySellTimeout0 = (m_lItemSellTimeout0 > 0 ? aItem.m_lholdTime + m_lItemSellTimeout0 : 0);
			aItem.m_ltrySellTimeout1 = ((m_lItemSellTimeout0 > 0 && m_lItemSellTimeout1 > 0) ? aItem.m_lholdTime + m_lItemSellTimeout0 + m_lItemSellTimeout1 : 0);
			aItem.m_eitemState = eST_HOLDING;
		}
		else
		{
			aItem.m_eitemState = eST_TRADECLOSING;
		}
		return FALSE;
	}

	if (aItem.m_ltryBuyCount > 0)//aItem.m_lQuantity != nextQuantity + aItem.m_lBuyQuantity)
	{
		//�ż� ���.
		if (!REQ_ItemBuyCancel(aItem, bFromAllTrade))
		{
			return FALSE;
		}
		aItem.m_strBuyOrder = "";
	}
	
	aItem.m_lbuyPrice = lcurPrice;
	aItem.m_lQuantity = nextQuantity + aItem.m_lBuyQuantity;

	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lOrderType, aItem.m_strCode, 
		aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_lbuyPrice, strHogaGb, aItem.m_strBuyOrder);

	AddMessage(_T("BUY_ORDER::[%s],[%s][%s][%s],�ܰ�[%d],����[%d],�ŷ�����[%s],���ֹ���ȣ[%s], �ż� ��û[%s] %s!"),
		GetOrderTypeString(lOrderType), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity, strHogaGb, aItem.m_strBuyOrder, (bFromAllTrade ? "Ÿ�̸�" : "�̺�Ʈ"), (lRet >= 0 ? "����" : "����"));

	return (lRet >= 0 ? TRUE : FALSE);
}

BOOL CABotDlg::REQ_ItemBuyCancel(CABotItem &aItem, BOOL bFromAllTrade)
{
	CString strRQName = "�ֽ��ֹ�";

	// �Ÿű��� ���(1:�űԸż�, 2:�űԸŵ� 3:�ż����, 4:�ŵ����, 5:�ż�����, 6:�ŵ�����)
	long lCANCELOrder = 3; //3:�ż����

	// �ŷ����� ���
	// 00:������, 03:���尡, 05:���Ǻ�������, 06:������������, 07:�ֿ켱������, 
	// 10:������IOC, 13:���尡IOC, 16:������IOC, 20:������FOK, 23:���尡FOK, 
	// 26:������FOK, 61:�尳�����ð���, 62:�ð��ܴ��ϰ��Ÿ�, 81:���Ľð�������
	CString strHogaGb = "00";

	long lCANCELQuantity = aItem.m_lQuantity - aItem.m_lBuyQuantity;
	long lCANCELPrice = 0;
	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lCANCELOrder, aItem.m_strCode,
		lCANCELQuantity, lCANCELPrice, strHogaGb, aItem.m_strBuyOrder);

	AddMessage(_T("BUY_CANCEL::[%s],[%s][%s][%s],�ܰ�[%d],����[%d],�ŷ�����[%s],���ֹ���ȣ[%s], �ż� ��� ��û[%s] %s!"),
		GetOrderTypeString(lCANCELOrder), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lCANCELPrice, lCANCELQuantity, strHogaGb, aItem.m_strBuyOrder, (bFromAllTrade ? "Ÿ�̸�" : "�̺�Ʈ"), (lRet >= 0 ? "����" : "����"));

	return (lRet >= 0 ? TRUE : FALSE);
}

BOOL CABotDlg::REQ_ItemSellOrder(CABotItem &aItem, BOOL bMarketVale, BOOL bFromAllTrade)
{
	CString strRQName = "�ֽ��ֹ�";

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

	if (bMarketVale)
	{
		if (aItem.m_strSellOrder.GetLength() > 0)
		{
			if (!REQ_ItemSellCancel(aItem, bFromAllTrade))
			{
				return FALSE;
			}
		}

		lOrderType = 2;					//��� ��, �ű� �ŵ��� �ؾ� �Ѵ�.
		strHogaGb = "03";				//03:���尡,

		aItem.m_strSellOrder = "";		//���ֹ� ��ȣ ����.
		aItem.m_lsellPrice = 0;			//���尡�� ������ 0�̴�.
		aItem.m_ltrySellTimeout0 = 0;	//Ÿ�� �ƿ� ���ش�.
		aItem.m_ltrySellTimeout1 = 0;	//Ÿ�� �ƿ� ���ش�.
	}
	else
	{
		ASSERT(aItem.m_lsellPrice != 0);
		if (aItem.m_lsellPrice == 0){ return FALSE; }
	}

	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lOrderType, aItem.m_strCode,
		aItem.m_lQuantity - aItem.m_lSellQuantity, aItem.m_lsellPrice, strHogaGb, aItem.m_strSellOrder);

	AddMessage(_T("SELL_ORDER::[%s],[%s][%s][%s],�ܰ�[%d],����[%d],�ŷ�����[%s],���ֹ���ȣ[%s], �ŵ� ��û[%s] %s!"),
		GetOrderTypeString(lOrderType), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity - aItem.m_lSellQuantity, strHogaGb, aItem.m_strSellOrder, (bFromAllTrade ? "Ÿ�̸�" : "�̺�Ʈ"), (lRet >= 0 ? "����" : "����"));

	if (lRet >= 0)
	{
		aItem.m_strSellOrder = "";
	}
	return (lRet >= 0 ? TRUE : FALSE);
}

BOOL CABotDlg::REQ_ItemSellCancel(CABotItem &aItem, BOOL bFromAllTrade)
{
	CString strRQName = "�ֽ��ֹ�";

	// �Ÿű��� ���(1:�űԸż�, 2:�űԸŵ� 3:�ż����, 4:�ŵ����, 5:�ż�����, 6:�ŵ�����)
	long lCANCELOrder = 4;

	// �ŷ����� ���
	// 00:������, 03:���尡, 05:���Ǻ�������, 06:������������, 07:�ֿ켱������, 
	// 10:������IOC, 13:���尡IOC, 16:������IOC, 20:������FOK, 23:���尡FOK, 
	// 26:������FOK, 61:�尳�����ð���, 62:�ð��ܴ��ϰ��Ÿ�, 81:���Ľð�������
	CString strHogaGb = "00";

	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lCANCELOrder, aItem.m_strCode,
		aItem.m_lQuantity - aItem.m_lSellQuantity, aItem.m_lsellPrice, strHogaGb, aItem.m_strSellOrder);

	AddMessage(_T("SELL_CANCEL::[%s],[%s][%s][%s],�ܰ�[%d],����[%d],�ŷ�����[%s],���ֹ���ȣ[%s], �ŵ� ��� ��û[%s] %s!"),
		GetOrderTypeString(lCANCELOrder), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity - aItem.m_lSellQuantity, strHogaGb, aItem.m_strSellOrder, (bFromAllTrade ? "Ÿ�̸�" : "�̺�Ʈ"), (lRet >= 0 ? "����" : "����"));

	return (lRet >= 0 ? TRUE : FALSE);
}

CString CABotDlg::GetOrderTypeString(long lOrderType)
{
	// �Ÿű��� ���(1:�űԸż�, 2:�űԸŵ� 3:�ż����, 4:�ŵ����, 5:�ż�����, 6:�ŵ�����)

	if (lOrderType == 1)
		return "�űԸż�";
	if (lOrderType == 2)
		return "�űԸŵ�";
	if (lOrderType == 3)
		return "�ż����";
	if (lOrderType == 4)
		return "�ŵ����";
	if (lOrderType == 5)
		return "�ż�����";
	if (lOrderType == 6)
		return "�ŵ�����";

	CString strBuf;
	strBuf.Format("��(%d)", lOrderType);
	return strBuf;
}

BOOL CABotDlg::REQ_BalanceInfo()
{
	long lRet = 0;
	CString strBuff;
	CTime startDay = CTime::GetCurrentTime(); //CTime(2016, 9, 1, 0, 0, 0); 
	CTime endDay = CTime::GetCurrentTime(); //CTime(2016, 9, 17, 23, 59, 59);

	if (getAccountData() == FALSE) {
		return FALSE;
	}

	theApp.m_khOpenApi.SetInputValue("���¹�ȣ", m_strAccNo);
	theApp.m_khOpenApi.SetInputValue("��й�ȣ�Է¸�ü����", _T("00"));
	theApp.m_khOpenApi.SetInputValue("��ȸ����", _T("1")); // 1:�ջ�, 2:����
	
	lRet = theApp.m_khOpenApi.CommRqData(_T("�ܰ�"), _T("OPW00018"), 0, m_strScrNo);
	AddMessage("�ܰ� ��ȸ ��û %s. [%d]", (lRet >= 0 ? "����" : "����"), lRet);
	if (lRet < 0) {
		return FALSE;
	}

	theApp.m_khOpenApi.SetInputValue("���¹�ȣ", m_strAccNo);
	strBuff = startDay.Format("%Y%m%d");	theApp.m_khOpenApi.SetInputValue("��������", strBuff);
	strBuff = endDay.Format("%Y%m%d");		theApp.m_khOpenApi.SetInputValue("��������", strBuff);

	lRet = theApp.m_khOpenApi.CommRqData(_T("���ں��������Ϳ�û"), _T("opt10074"), 0, m_strScrNo);
	AddMessage("���ں��������Ϳ�û %s. [%d]", (lRet >= 0 ? "����" : "����"), lRet);
	if (lRet < 0) {
		return FALSE;
	}
	return TRUE;
}

BOOL CABotDlg::IsEndTrade()
{
	if (m_ItemCount <= 0)
	{
		return FALSE;
	}

	long i = 0;
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

void CABotDlg::ReportAllTrade()
{
	long i = 0;
	long laccBuyCost = 0;
	long laccSellCost = 0;
	AddMessage(_T("�Ѱ�� ==============================================================================================================="));
	ASSERT(MAX_ITEM_COUNT >= m_ItemCount);
	for (i = 0; i < m_ItemCount; i++)
	{
		if (m_Item[i].m_eitemState == eST_TRADEDONE && m_Item[i].m_lBuyCost!=0)
		{
			CString strMark("----");
			long lactBuyCost = m_Item[i].m_lBuyCost;
			long lactSellCost = long(m_Item[i].m_lSellCost*(1 - m_dSellTradeFee / 100.)) - long(m_Item[i].m_lBuyCost*(m_dBuyTradeFee / 100.));
			if (lactSellCost>lactBuyCost)
			{
				strMark = "��__";
			}
			else if (lactSellCost < lactBuyCost)
			{
				strMark = "__��";
			}

			if (lactBuyCost == 0) {	lactBuyCost = 1; }
			AddMessage(_T("       ,[%s][%3d],[%s][%s],�Ǹ��Ա�[%d],�Ǹŵ���[%d],��������[%4.2f]�ۼ�Ʈ �Դϴ�."),
				strMark, i + 1, m_Item[i].m_strCode, m_Item[i].m_strName, lactBuyCost, lactSellCost, double(lactSellCost - lactBuyCost) / double(lactBuyCost)*100.0);
			laccBuyCost += lactBuyCost;
			laccSellCost += lactSellCost;
		}
	}
	AddMessage(_T("�Ѱ�� ==============================================================================================================="));

	AddMessage(_T("�Ѱ��   �Ѹ��Ա�[%s],�Ѹŵ���[%s],�����ͱ�[%s],��������[%4.2f]�ۼ�Ʈ"),
		GetCurrencyString(laccBuyCost), GetCurrencyString(laccSellCost), GetCurrencyString(laccSellCost - laccBuyCost), double(laccSellCost - laccBuyCost) / double(laccBuyCost)*100.0);

	AddMessage(_T("�Ѱ�� ==============================================================================================================="));
}

