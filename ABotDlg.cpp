
// ABotDlg.cpp : 구현 파일
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

// {조회 키,		리얼 키,	행, 열, 타입,			색 변경, 정렬,	앞 문자, 뒷 문자}
const stGRID lstOPTSBFID[] =
{
	{ "종목코드",		"9001",	-1, 0, DT_NONE,			FALSE,	DT_LEFT,	"", "" },
	{ "종목명",			"302",	-1, 1, DT_NONE,			FALSE,	DT_LEFT,	"", "" },
	{ "현재가",			"10",	-1, 2, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"", "" },
	{ "전일대비기호",	"25",	-1, 3, DT_SIGN,			TRUE,	DT_CENTER,	"", "" },
	{ "전일대비",		"11",	-1, 4, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"", "" },
	{ "등락율",			"12",	-1, 5, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,	"", "%" },
	{ "거래량",			"13",	-1, 6, DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,	"", "" },
};


CString GetRoundText(eProcessState aProcessState)
{
	switch (aProcessState)
	{
	default:
		return "알수 없음";
	case ePST_IDLE:
		return "정지 상태";
	case ePST_ROUND_START:
		return "시작 상태";
	case ePST_WAIT_TIME:
		return "시작 시간을 기다리는 상태";
	case ePST_DR_SEARCH:
		return "예수금 요청 상태";
	case ePST_DR_WAIT:
		return "예수금 결과 대기 상태";
	case ePST_ITEM_SEARCH:
		return "종목 검색 요청 상태";
	case ePST_ITEM_WAIT:
		return "종목 검색 결과 대기 상태";
	case ePST_ITEM_TRADE:
		return "종목 거래 상태";
	case ePST_ROUND_END:
		return "라운드 1회 종료 상태";
	case ePST_KILL_PROC:
		return "프로세스 중단";
	}
	return "알수 없음";
};

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CABotDlg 대화 상자



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


// CABotDlg 메시지 처리기

BOOL CABotDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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
		strMsg = "사용자정보교환에 실패하였습니다. 잠시후 다시 시작하여 주십시요.";
		break;
	case OP_ERR_CONNECT:
		strMsg = "서버 접속 실패";
		break;
	case OP_ERR_VERSION:
		strMsg = "버전처리가 실패하였습니다.";
		break;

		////////////////////////////
		// 일반 조회 오류 처리
	case OP_ERR_SISE_OVERFLOW:		// 시세조회 과부하
		strMsg = "시세조회 과부하";
		break;
	case OP_ERR_RQ_STRUCT_FAIL:		// REQUEST_INPUT_st Failed
		strMsg = "REQUEST_INPUT_st Failed";
		break;
	case OP_ERR_RQ_STRING_FAIL:		// 요청 전문 작성 실패
		strMsg = "요청 전문 작성 실패";
		break;
	case OP_ERR_NO_DATA:			// 데이터가 존재하지 않습니다.
		strMsg = "데이터가 존재하지 않습니다.";
		break;

		////////////////////////////
		// 복수 종목 조회 오류 처리
	case OP_ERR_OVER_MAX_DATA:			// 한번에 조회 가능한 종목개수는 최대 100종목 입니다.
		strMsg = "한번에 조회 가능한 종목개수는 최대 100종목 입니다.";
		break;

		////////////////////////////
		// 주문 오류 처리
	case OP_ERR_ORD_WRONG_INPUT:		// 입력값 오류
		strMsg = "입력값 오류";
		break;
	case OP_ERR_ORD_WRONG_ACCNO:		// 계좌비밀번호를 입력하십시오.
		strMsg = "계좌비밀번호를 입력하십시오.";
		break;
	case OP_ERR_OTHER_ACC_USE:			// 타인계좌는 사용할 수 없습니다.
		strMsg = "타인계좌는 사용할 수 없습니다.";
		break;
	case OP_ERR_MIS_2BILL_EXC:			// 주문가격이 20억원을 초과합니다.
		strMsg = "주문가격이 20억원을 초과합니다.";
		break;
	case OP_ERR_MIS_5BILL_EXC:			// 주문가격이 50억원을 초과합니다.
		strMsg = "주문가격이 50억원을 초과합니다.";
		break;
	case OP_ERR_MIS_1PER_EXC:			// 주문수량이 총발행주수의 1%를 초과합니다.
		strMsg = "주문수량이 총발행주수의 1%를 초과합니다.";
		break;
	case OP_ERR_MIS_3PER_EXC:			// 주문수량은 총발행주수의 3%를 초과할 수 없습니다.
		strMsg = "주문수량은 총발행주수의 3%를 초과할 수 없습니다.";
		break;
	case OP_ERR_SEND_FAIL:				// 주문전송실패
		strMsg = "주문전송에 실패하였습니다..";
		break;
	case OP_ERR_ORD_OVERFLOW:			// 주문전송 과부하
		strMsg = "주문전송 과부하 입니다. 잠시 후 다시 주문하여 주세요.";
		break;
	}

	return TRUE;
}

BOOL CABotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
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

	// 로그 파일 경로 얻기.
	ReadFromIniFile_String(m_strConfigFile, "SYSTEM", "logfoldername", ".\\", strBuf);
	strBuf += _T("\\"); strBuf.Replace(_T("\\\\"), _T("\\"));
	m_strLogFolderName = strBuf;
	g_logger.Initial(m_strLogFolderName + _T("ABotLog_"));
	int n = 0;
	ReadFromIniFile_Int(m_strConfigFile, "SYSTEM", "dowritelog", 1, n);
	g_logger.SetEnable(n > 0 ? TRUE : FALSE);

	//메세지 디스플레이를 가장 먼저 만들고,
	g_pMsgDisp = new CsMsgDisp(this);		
	if (!g_pMsgDisp) { return FALSE; }
	g_pMsgDisp->Create(IDD_MSG_DISP, this);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	theApp.m_khOpenApi.CommConnect();

	m_mapJongCode.RemoveAll();
	m_mapFIDName.RemoveAll();
	m_mapNameList.RemoveAll();
	InitFIDName();
	InitRealAddGrid();
	InitComboBox();

	AddMessage(" ");
	AddMessage("=========================================================");
	AddMessage("안녕하세요? ABot'%s'입니다.", GetSystemVersion());
	srand(GetTickCount() % 10000);
	m_strScrNo.Format("%04d", rand()%10000);
	AddMessage("스크린번호[%s], 환경파일[%s]", m_strScrNo, m_strConfigFile);
	AddMessage("시작합니다.");

	LoadSystemFile();

	m_nConditionIndex = -1;

	SetTimer(START_TIMER, 1000, NULL);
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CABotDlg::InitComboBox()
{
	//콤보 박스 기초 데이터.
	m_cmbRoundCount.InsertString(0, "반복");
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

	m_cmbBuyMethod.InsertString(0, "현재가");
	m_cmbBuyMethod.InsertString(1, "시장가");
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

	// 계좌 번호.
	ReadFromIniFile_String(m_strConfigFile, "ACCOUNT", "number", "-", strBuf);
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->SetWindowText(strBuf);
	AddMessage("     계좌 번호[%s]", strBuf);

	// 반복 회수
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "count", "1", strBuf);
	if ("반복" == strBuf)
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
	AddMessage("     반복 회수[%s].", strBuf);

	CString strBuf2 = "시작 ";
	// 시작 시각의 시
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "start_hour", "9", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	m_cmbStartHour.SetCurSel(n);
	strBuf2 += strBuf + "시 ";

	// 시작 시각의 분
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "start_min", "0", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	m_cmbStartMin.SetCurSel(n);
	strBuf2 += strBuf + "분,  종료 ";

	// 종료 시각의 시
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "end_hour", "15", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	m_cmbEndHour.SetCurSel(n);
	strBuf2 += strBuf + "시 ";

	// 종료 시각의 분
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "end_min", "0", strBuf);
	n = atoi((LPSTR)(LPCSTR)strBuf);
	m_cmbEndMin.SetCurSel(n);
	strBuf2 += strBuf + "분";
	AddMessage("     동작 시간[%s].", strBuf2);

	// 매수시 예수금의 최대 사용 퍼센트. [%]
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
	AddMessage("     예수금 사용 [%s]퍼센트.", strBuf);

	// 매수시 예수금의 최대 사용 금액. [만원]
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
	AddMessage("     최대 예수금 사용 허용 금액 [%s]만원.", strBuf);

	// 종목당 최대 투자 허용 금액
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
	AddMessage("     종목당 최대 투자 허용 금액 [%s]만원.", strBuf);

	// 매수 방법, 현재가, 시장가. 향후..퍼센트 지정.
	ReadFromIniFile_String(m_strConfigFile, "BUY", "method", "현재가", strBuf);
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
		i = 0;	//"현재가"
		m_cmbBuyMethod.GetLBText(i, strBuf);
	}
	m_cmbBuyMethod.SetCurSel(i);
	AddMessage("     매수 방법 [%s].", strBuf);

	// 매수 체결 대기 타임 아웃.
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
	AddMessage("     매수 체결 대기 타임 아웃 [%s]초.", strBuf);

	// 매수 실패시 재시도 회수
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
	AddMessage("     매수 실패시 재시도 회수 [%s]회.", strBuf);

	// 주식 보유 타임 아웃.
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
	AddMessage("     보유 대기 타임 아웃 [%s]초.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 높아지면 판다.
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
	AddMessage("     매도 시점 1-1. 현재가가 매수 금액의 [%s] 퍼센트 이상 상승시.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 높아지면 판다.
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
	AddMessage("     매도 시점 1-2. 현재가가 1-1 트리거후 매수 금액의 [%s] 퍼센트 이하 도달시 시장가 매도.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 낮아지면 판다.
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
	AddMessage("     매도 시점 2-1. 현재가가 매수 금액의 [%s] 퍼센트 이상 하락시.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 낮아지면 판다.
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
	AddMessage("     매도 시점 2-2. 현재가가 2-2 트리거후 매수 금액의 [%s] 퍼센트 이하 도달시 시장가 매도.", strBuf);
}

void CABotDlg::SaveSystemFile()
{
	int i = 0, n = 0;
	double d = 0.;
	CString strBuf, strCombo;

	// 계좌 번호.
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ACCOUNT", "number", strBuf);

	// 반복 회수
	m_cmbRoundCount.GetLBText(m_cmbRoundCount.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "count", strBuf);

	// 시작 시각의 시
	m_cmbStartHour.GetLBText(m_cmbStartHour.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "start_hour", strBuf);

	// 시작 시각의 분
	m_cmbStartMin.GetLBText(m_cmbStartMin.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "start_min", strBuf);

	// 종료 시각의 시
	m_cmbEndHour.GetLBText(m_cmbEndHour.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "end_hour", strBuf);

	// 종료 시각의 분
	m_cmbEndMin.GetLBText(m_cmbEndMin.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "end_min", strBuf);

	// 검색 조건
	if (m_cmbCon.GetCurSel() > 0)
	{
		m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strBuf);
		WriteToIniFile_String(m_strConfigFile, "FILTER", "name", strBuf);
	}

	// 매수시 예수금의 최대 사용 퍼센트. [%]
	m_cmbDpUseRate.GetLBText(m_cmbDpUseRate.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "use_rate", strBuf);

	// 매수시 예수금의 최대 사용 금액. [만원]
	m_cmbMaxTotalAmount.GetLBText(m_cmbMaxTotalAmount.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "max_total_amount", strBuf);	

	// 종목당 최대 투자 허용 금액
	m_cmbMaxAmount.GetLBText(m_cmbMaxAmount.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "max_amount", strBuf);

	// 매수 방법, 현재가, 시장가. 향후..퍼센트 지정.
	m_cmbBuyMethod.GetLBText(m_cmbBuyMethod.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "method", strBuf);

	// 매수 체결 대기 타임 아웃.
	m_cmbBuyTimeOut.GetLBText(m_cmbBuyTimeOut.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "timeout", strBuf);

	// 매수 실패시 재시도 회수
	m_cmbBuyRetry.GetLBText(m_cmbBuyRetry.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "retry", strBuf);

	// 주식 보유 타임 아웃.
	m_cmbHoldTimeOut.GetLBText(m_cmbHoldTimeOut.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "HOLD", "timeout", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 높아지면 판다.
	m_cmbSellOverThis.GetLBText(m_cmbSellOverThis.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "over_this_1", strBuf);

	m_cmbSellOverThis2.GetLBText(m_cmbSellOverThis2.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "over_this_2", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 낮아지면 판다.
	m_cmbSellUnderThis.GetLBText(m_cmbSellUnderThis.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "under_this_1", strBuf);

	m_cmbSellUnderThis2.GetLBText(m_cmbSellUnderThis2.GetCurSel(), strBuf);
	WriteToIniFile_String(m_strConfigFile, "SELL", "under_this_2", strBuf);
}


void CABotDlg::OnBnClickedButtonSaveconfig()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SaveSystemFile();
}


void CABotDlg::InitRealAddGrid()
{
	m_grdRealAdd.SetEditable(false);				//cell을 에디트 못하게 함.
//	m_grdRealAdd.EnableScrollBars(SB_BOTH, FALSE);
	m_grdRealAdd.EnableScrollBars(SB_VERT, TRUE);

	COLORREF clr = RGB(215, 227, 241);

	// 고정 행/열 설정
	m_grdRealAdd.SetFixedRowCount(1);

	// 행/열 갯수 설정
	m_grdRealAdd.SetRowCount(1);
	SetGridHeight(0, 24); // grid 높이는 윈도우 별로 상이하므로 별도 함수로 호출한다.

	m_grdRealAdd.SetColumnCount(7);

	// 열의 넓이 설정
	int i = 0, nWidth[] = { 50, 80, 70, 30, 70, 45, 75 };
	CString strHeader[] = { "코드", "종목명", "현재가", "기호", "전일대비", "등락율", "거래량" };
	int nCnt = sizeof(nWidth) / sizeof(*nWidth);		// 전체크기 / 원소크기 = 원소개수
	m_grdRealAdd.SetColumnCount(nCnt);
	for (i = 0; i < nCnt; i++)
	{
		SetGridWidth(i, nWidth[i]); // grid 폭은 윈도우 별로 상이하므로 별도 함수로 호출한다.
		m_grdRealAdd.SetItemFormat(0, i, DT_CENTER);
		m_grdRealAdd.SetItemText(0, i, strHeader[i]);
		m_grdRealAdd.SetItemBkColour(0, i, clr);	// 지정된 셀의 배경색 설정
	}

	m_grdRealAdd.SetRowCount(21);
	m_grdRealAdd.Invalidate();
}

void CABotDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

}

void CABotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
		strBuf.Format(_T("ABot 라운드[%d] [%s]"), m_nRoundCount, GetRoundText(m_eProcessState));
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CABotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CABotDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	// 통신 종료 처리
//	theApp.m_khOpenApi.CommTerminate();	//지원하지 않음.

	AddMessage("ABot 종료.");
	AddMessage("=========================================================");
	AddMessage(" ");

	delete g_pMsgDisp; g_pMsgDisp = NULL;	//메세지 디스플레이를 가장 나중에 소멸 시켜야 한다.
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
	m_mapFIDName.SetAt(_T("9201"), _T("계좌번호"));
	m_mapFIDName.SetAt(_T("9203"), _T("주문번호"));
	m_mapFIDName.SetAt(_T("9001"), _T("종목코드"));
	m_mapFIDName.SetAt(_T("913"), _T("주문상태"));
	m_mapFIDName.SetAt(_T("302"), _T("종목명"));
	m_mapFIDName.SetAt(_T("900"), _T("주문수량"));
	m_mapFIDName.SetAt(_T("901"), _T("주문가격"));
	m_mapFIDName.SetAt(_T("902"), _T("미체결수량"));
	m_mapFIDName.SetAt(_T("903"), _T("체결누계금액"));
	m_mapFIDName.SetAt(_T("904"), _T("원주문번호"));
	m_mapFIDName.SetAt(_T("905"), _T("주문구분"));
	m_mapFIDName.SetAt(_T("906"), _T("매매구분"));
	m_mapFIDName.SetAt(_T("907"), _T("매도수구분"));
	m_mapFIDName.SetAt(_T("908"), _T("주문/체결시간"));
	m_mapFIDName.SetAt(_T("909"), _T("체결번호"));
	m_mapFIDName.SetAt(_T("910"), _T("체결가"));
	m_mapFIDName.SetAt(_T("911"), _T("체결량"));
	m_mapFIDName.SetAt(_T("10"), _T("현재가"));
	m_mapFIDName.SetAt(_T("27"), _T("(최우선)매도호가"));
	m_mapFIDName.SetAt(_T("28"), _T("(최우선)매수호가"));
	m_mapFIDName.SetAt(_T("914"), _T("단위체결가"));
	m_mapFIDName.SetAt(_T("915"), _T("단위체결량"));
	m_mapFIDName.SetAt(_T("919"), _T("거부사유"));
	m_mapFIDName.SetAt(_T("920"), _T("화면번호"));

	m_mapFIDName.SetAt(_T("917"), _T("신용구분"));
	m_mapFIDName.SetAt(_T("916"), _T("대출일"));
	m_mapFIDName.SetAt(_T("930"), _T("보유수량"));
	m_mapFIDName.SetAt(_T("931"), _T("매입단가"));
	m_mapFIDName.SetAt(_T("932"), _T("총매입가"));
	m_mapFIDName.SetAt(_T("933"), _T("주문가능수량"));
	m_mapFIDName.SetAt(_T("945"), _T("당일순매수수량"));
	m_mapFIDName.SetAt(_T("946"), _T("매도/매수구분"));
	m_mapFIDName.SetAt(_T("950"), _T("당일총매도손일"));
	m_mapFIDName.SetAt(_T("951"), _T("예수금"));
	m_mapFIDName.SetAt(_T("307"), _T("기준가"));
	m_mapFIDName.SetAt(_T("8019"), _T("손익율"));
	m_mapFIDName.SetAt(_T("957"), _T("신용금액"));
	m_mapFIDName.SetAt(_T("958"), _T("신용이자"));
	m_mapFIDName.SetAt(_T("918"), _T("만기일"));
	m_mapFIDName.SetAt(_T("990"), _T("당일실현손익(유가"));
	m_mapFIDName.SetAt(_T("991"), _T("당일실현손익률(유가"));
	m_mapFIDName.SetAt(_T("992"), _T("당일실현손익(신용"));
	m_mapFIDName.SetAt(_T("993"), _T("당일실현손익률(신용"));

	m_mapFIDName.SetAt(_T("397"), _T("파생상품거래단위"));
	m_mapFIDName.SetAt(_T("305"), _T("상한가"));
	m_mapFIDName.SetAt(_T("306"), _T("하한가"));
}


void CABotDlg::OnBnClickedShowMessage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
//! Function      : 접속 통보 처리
//! Param         : long nErrCode
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CABotDlg::OnEventConnect(long nErrCode)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AddMessage("서버 접속 %s. [%d]", (nErrCode== 0?"성공":"실패"), nErrCode);
	if (nErrCode == 0)
	{
		int nGetConditionRet = theApp.m_khOpenApi.GetConditionLoad();
		AddMessage("검색 조건 조회 %s. [%d]", (nGetConditionRet>=0?"성공":"실패"), nGetConditionRet);
	}
}
//*******************************************************************/
//! Function Name : OnReceiveErrorMsg
//! Function      : 조회 에러 처리
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveErrorMsg(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AddMessage("조회에러,스크린번호[%s],RQName[%s],TrCode[%s],메시지[%s]", sScrNo, sRQName, sTrCode, sMsg);
}


void CABotDlg::OnBnClickedBalanceQuery()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
		AfxMessageBox("계좌번호 10자를 입력 해 주세요~!");
		((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->SetFocus();
		return FALSE;
	}
	m_strAccNo = strAccNo;
	theApp.m_khOpenApi.SetInputValue("계좌번호", m_strAccNo);
	//	theApp.m_khOpenApi.SetInputValue("비밀번호", _T("0000"));
	theApp.m_khOpenApi.SetInputValue("비밀번호입력매체구분", _T("00"));
	theApp.m_khOpenApi.SetInputValue("조회구분", _T("1"));

	long lRet = theApp.m_khOpenApi.CommRqData(_T("예수곰"), _T("OPW00001"), 0, m_strScrNo);
	AddMessage("예수금 조회 요청 %s. [%d]", (lRet >= 0 ? "성공" : "실패"), lRet);
	return (lRet >= 0 ? TRUE : FALSE);
}

/*
void CABotDlg::RealData()
{
	static bool btoggle = true;
	CString strScrNo = "7777";
	CString strCode = "039490";
	CString strIndex;
	// 종목명//
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
//! Function      : 체결잔고 실시간 처리
//! Param         : LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFidList
//! Return        : void
//! Create        : , 2014/06/02
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveChejanData(LPCTSTR sGubun, long nItemCnt, LPCTSTR sFIdList)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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

						AddMessage(_T("라운드[%d], 종목[%s][%s],단가[%d],수량[%d],잔량[%d], 매수 되었습니다."),
							m_nRoundCount, aItem.m_strCode, aItem.m_strName, lPrice, lQuantity, aItem.m_lQuantity - aItem.m_lBuyQuantity);

						if (aItem.m_lBuyQuantity >= aItem.m_lQuantity)
						{
						//	m_OrderCodeMap.RemoveKey(strOrderCode);
							aItem.m_lholdTime = GetTickCount();
							aItem.m_lholdTimeout = (m_lItemHoldTimeout>0?aItem.m_lholdTime + m_lItemHoldTimeout:0);
							aItem.m_eitemState = eST_HOLDING;
							AddMessage(_T("라운드[%d], 종목[%s][%s],평균단가[%d],수량[%d],재시도[%d], 매수 완료 되었습니다."),
								m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.BuyPrice(), aItem.m_lQuantity, aItem.m_ltryBuyCount);
						}
					}
					else if (aItem.m_eitemState == eST_WAITSELL)
					{
						aItem.m_lSellCost += lPrice*lQuantity;
						aItem.m_lSellQuantity += lQuantity;

						AddMessage(_T("라운드[%d], 종목[%s][%s],단가[%d],수량[%d],잔량[%d], 매도 되었습니다."),
							m_nRoundCount, aItem.m_strCode, aItem.m_strName, lPrice, lQuantity, aItem.m_lQuantity - aItem.m_lSellQuantity);

						if (aItem.m_lSellQuantity >= aItem.m_lQuantity)
						{
						//	m_OrderCodeMap.RemoveKey(strOrderCode);
							aItem.m_eitemState = eST_TRADEDONE;
							AddMessage(_T("라운드[%d], 종목[%s][%s],평균단가[%d],수량[%d],재시도[%d], 거래 완료 되었습니다."),
								m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.SellPrice(), aItem.m_lQuantity, aItem.m_ltryBuyCount);
						}
					}
				}
			}
		}
		else
		{
			strOneData = theApp.m_khOpenApi.GetChejanData(atoi(strFID)).Trim();

			strText.Format(_T("구분[%s] FID[%4s:%s] = [%s]"), strGubun, strFID, strFIDName, strOneData);
			AddMessage(strText);
			strFIDName.Empty();
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveRealDataKhopenapictrl
//! Function      : 실시간 처리
//! Param         : LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveRealData(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
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
	int i, nFieldCnt = sizeof(lstOPTSBFID) / sizeof(*lstOPTSBFID);		// 전체크기 / 원소크기 = 원소개수
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
//! Function      : 문자열 포멧 설정
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
	case DT_DATE:			// 일자
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
	case DT_TIME:			// 시간
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
	case DT_NUMBER:			// 숫자
	case DT_ZERO_NUMBER:	// 숫자(0표시)
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
//! Function      : 그리드 셀 문자색 변경 및 기호 데이터 설정
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
		strData = "↑";
		pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// 지정된 셀의 텍스트 색상 설정
		break;
	case 2:
		strData = "▲";
		pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// 지정된 셀의 텍스트 색상 설정
		break;
	case 3:	strData = "";		break;
	case 4:
		strData = "↓";
		pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// 지정된 셀의 텍스트 색상 설정
		break;
	case 5:
		strData = "▼";
		pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// 지정된 셀의 텍스트 색상 설정
		break;
	}
	pGrid->SetItemText(nRow, nCol, strData);
}

//*******************************************************************/
//! Function Name : SetDataFgColour
//! Function      : 그리드 셀 문자색 변경
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
		pGrid->SetItemFgColour(nRow, nCol, RGB(255, 0, 0));	// 지정된 셀의 텍스트 색상 설정
	}
	else if (atof(strData) < 0)
	{
		pGrid->SetItemFgColour(nRow, nCol, RGB(0, 0, 255));	// 지정된 셀의 텍스트 색상 설정
	}
}


//*******************************************************************/
//! Function Name : SetDataRealAddGrid
//! Function      : 그리드 데이타 설정(관심종목정보 설정)
//! Param         : CStringArray &arrData, CString strRealType/* = ""*/
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::SetDataRealAddGrid(CStringArray &arrData, CString strRealType/* = ""*/)
{
	// 종목코드에 맞는 행 찾기
	CString strData, strTemp;
	strData = arrData.GetAt(0);
	if (!m_mapJongCode.Lookup(strData, strTemp))
	{
		return;
	}

	int i, nRow = atol(strTemp), nCnt = arrData.GetSize();

	// 데이타 설정
	CString strTempData;
	for (i = 0; i < nCnt; i++)
	{
		strData = arrData.GetAt(i);
		/////////////////////////////////////////////
		// 변경 된 데이터 선택 처리↓↓↓↓↓↓↓↓↓
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
		// 변경 된 데이터 선택 처리↑↑↑↑↑↑↑↑↑
		/////////////////////////////////////////////
		if (lstOPTSBFID[i].bTextColor)
		{
			strTemp = arrData.GetAt(3);
			if (strTemp == "1" || strTemp == "2")	// 부호에 따라 색상변경
			{
				strTemp = "1";
			}
			else if (strTemp == "4" || strTemp == "5")	// 부호에 따라 색상변경
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
//! Function      : 그리드 폭을 조정한다.
//! Param         : -
//! Return        : void
//! Create        : 2016/09/13
//! Comment       : 윈도우마다 그리드 모습이 다양하므로 이를 별도 함수로 뺀다.
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
//! Function      : 그리드 폭을 조정한다.
//! Param         : -
//! Return        : void
//! Create        : 2016/09/13
//! Comment       : 윈도우마다 그리드 모습이 다양하므로 이를 별도 함수로 뺀다.
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
//! Function      : 윈도우 버전을 가져옴.
//! Param         : -
//! Return        : void
//! Create        : , 2016/09/12
//! Comment       : 본 함수는 그리드의 모습이 윈도우 버전마다 다른 
//					경우를 처리하기 위해서 추가됨.
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
//! Function      : 조회 응답 처리
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveTrData(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AddMessage(_T("OnReceiveTrData::"));

	CString strRQName = sRQName;
	if (strRQName == _T("예수곰"))		// 주식기본정보 설정
	{
		// 예수금
	//	CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("예수금"));	strData.Trim();
	//	CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("d+1추정예수금"));	strData.Trim();
		CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("d+2추정예수금"));	strData.Trim();
		m_lDepositReceived = atol((LPSTR)(LPCSTR)strData);
		CString strBuf;
		strBuf.Format("%d", m_lDepositReceived);
		((CEdit*)GetDlgItem(IDC_EDIT_DEPOSITRECEIVED))->SetWindowText(strBuf);
		strBuf = strBuf + " KRW";
		AddMessage(strBuf);
	}
	else if (strRQName == _T("주식주문"))		// 주식기본정보 설정
	{
		CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("주문번호"));	strData.Trim();
		AddMessage(strData);
	}
	else if (strRQName.Find("ORDER_")>0)
	{
		CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("주문번호"));	strData.Trim();
		strRQName.Replace("ORDER_","00");
		long nItemIndex = atol((LPSTR)(LPCSTR)strRQName);
		if (0 <= nItemIndex && nItemIndex < _countof(m_Item))
		{
			CABotItem &aItem = m_Item[nItemIndex];
			// 주문 번호
			if (aItem.m_eitemState == eST_WAITBUY)
			{
				aItem.m_strBuyOrder = strData;
			}
			else if (aItem.m_eitemState == eST_WAITSELL)
			{
				aItem.m_strSellOrder = strData;
			}
			m_OrderCodeMap[strData] = nItemIndex;
			AddMessage(_T("라운드[%d], 종목[%s][%s],code[%s] 주문 완료."), m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_strBuyOrder);
		}
	}
	else if (strRQName == _T("대출일조회"))
	{/// 
		CString		strTRCode(sTrCode);
		int		nCnt = theApp.m_khOpenApi.GetRepeatCnt(strTRCode, _T(""));

		if (nCnt < 1)
		{
			AddMessage(_T("대출일 조회 반복데이터 없음."));
			return;
		}

		CString		strSymbol, strSymbolName, strLoanDate, strText;
		for (int nIdx = 0; nIdx < nCnt; nIdx++)
		{
			strSymbol = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), nIdx, _T("종목번호"));
			strSymbolName = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), nIdx, _T("종목명"));
			strLoanDate = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), nIdx, _T("대출일"));

			strSymbol.Trim();
			if (!strSymbol.IsEmpty())
			{
				strSymbol = strSymbol.Mid(1, 6);
			}
			strSymbolName.Trim();
			strLoanDate.Trim();
			strText.Format(_T("대출일[%s]    종목번호[%s]    종목명[%s] \n"), strLoanDate, strSymbol, strSymbolName);
			AddMessage(strText);
		}
	}
	else
	{
		AddMessage("알수없는 메세지 1/3,ScrNo[%s],RQName[%s],TrCode[%s]", sScrNo, sRQName, sTrCode);
		AddMessage("알수없는 메세지 2/3,sRecordName[%s],sPrevNext[%s],nDataLength[%d]", sRecordName, sPrevNext, nDataLength);
		AddMessage("알수없는 메세지 3/3,sErrorCode[%s],sMessage[%s],sSplmMsg[%s]", sErrorCode, sMessage, sSplmMsg);
	}
}



void CABotDlg::OnBnClickedButtonGetfilter()
{
	int nGetConditionRet = theApp.m_khOpenApi.GetConditionLoad();
	if (nGetConditionRet <= 0)
	{
		AddMessage("검색 조건 조회 실패. ret[%d]", nGetConditionRet);
		return;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strConditionName = _T(""), strConditionNameList = _T(""), strCondition = _T(""), strIndex = _T("");

	//조건검색명 리스트를 불러온다.
	strConditionNameList = theApp.m_khOpenApi.GetConditionNameList();

	//받아온 조건검색명을 콤보에 넣는다.
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
			// 검색 조건
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	REQ_ItemSearch();
}

BOOL CABotDlg::REQ_ItemSearch()
{
//	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));
	theApp.m_khOpenApi.SetRealRemove(_T("ALL"), _T("ALL"));

	CString strConditionCodeList = _T(""), strIndex;

	//선택한 조건명의 검색된 종목리스트를 불러온다.
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), m_strConditionName);
	m_mapNameList.Lookup(m_strConditionName, strIndex);
	m_nConditionIndex = _ttoi(strIndex);

	//	m_strPreConditionName = m_strConditionName;
	//	m_nPreConditionIndex = m_nConditionIndex;

	//종목편입, 이탈 로그 삭제
	//	m_listCtl_Insert.ResetContent();
	//	m_listCtl_Delete.ResetContent();

	//조검검색 실시간 조회
	//종목리스트는 OnReceiveTrCondition()을 통하여 들어온다.
	//실시간 조건 검색을 하고 싶으면 마지막 파라메터에 1로 준다.
	//	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 1);
	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 0);
	AddMessage("조건식[%d][%s], 종목 검색 요청 %s.", m_nConditionIndex, m_strConditionName, (bRet ? "성공" : "실패"));
	return bRet;
}


void CABotDlg::OnBnClickedButtonRegtarget()
{
	REQ_ItemRealReg();
}

BOOL CABotDlg::REQ_ItemRealReg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strTargetCodeList;
	int nIndex = 0;
	int nMaxIndex = min(m_ItemCount, _countof(m_Item));
	for (nIndex = 0; nIndex < nMaxIndex; nIndex++)
	{
		strTargetCodeList += m_Item[nIndex].m_strCode + ";";
	}

	long lRet = theApp.m_khOpenApi.SetRealReg(m_strScrNo, strTargetCodeList, FIDLIST, "1");
	AddMessage("검색된 종목들에 대한 실시간 조회 요청 %s. ret[%d]", (lRet >= 0 ? "성공" : "실패"), lRet);
	return (lRet >= 0 ? TRUE : FALSE);
}

//*******************************************************************/
//! Function Name	: OnReceiveTrCondition
//! Function			: 조건검색 종목리스트 조회응답
//! Param				: LPCTSTR sScrNo					- 화면번호
//!						: PCTSTR strCodeList				- 종목리스트
//!						: LPCTSTR strConditionName			- 조건명
//!						: int nConditionIndex				- 조건명 인덱스번호
//!						: int nNext							- 연속조회(2:연속조회, 0:연속조회없음)
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveTrCondition(LPCTSTR sScrNo, LPCTSTR strCodeList, LPCTSTR strConditionName, long nConditionIndex, long nNext)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AddMessage(_T("OnReceiveTrCondition::"));

	CString strCndName, sConditionName;
	m_cmbCon.GetLBText(m_cmbCon.GetCurSel(), strCndName);	//선택된 조건명

	if (strConditionName == strCndName)	//현재 조건명과 조회응답으로 들어온 조건명이 같을때만 처리.
	{
		AddMessage("조건식[%d][%s], 종목 검색 요청 결과,", nConditionIndex, strConditionName);

		if (strlen(strCodeList)>0)
		{
			int nIndex(0);
			CString strConditionCode;
			m_mapJongCode.RemoveAll();		// 종목코드 초기화
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
					SetGridHeight(nIndex, 24); // grid 높이는 윈도우 별로 상이하므로 별도 함수로 호출한다.
					m_grdRealAdd.SetItemText(nIndex, 0, strConditionCode);

					// 종목명
					CString strCodeName;
					strCodeName = theApp.m_khOpenApi.GetMasterCodeName(strConditionCode);
					m_grdRealAdd.SetItemText(nIndex, 1, strCodeName);

					strIndex.Format("%3d", nIndex);
					m_mapJongCode.SetAt(strConditionCode, strIndex);
					AddMessage(_T("[%s][%s][%s] 검색됨."), strIndex, strConditionCode, strCodeName);

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
			m_mapJongCode.RemoveAll();		// 종목코드 초기화
			m_grdRealAdd.SetRowCount(1);

			AddMessage(_T("검색된 종목이 없습니다."));
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_eProcessState == ePST_IDLE)
	{
		SetDisableControls();
		m_nRoundCount = 1;
		m_bDoFinishProcess = FALSE;
		m_eProcessState = ePST_ROUND_START;
		SetTimer(ROUND_TIMER, 1, NULL);
		AddMessage(_T("라운드 시작 요청."));
	}
}


void CABotDlg::OnBnClickedButtonFinishRound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_eProcessState <= ePST_ITEM_WAIT)
	{
		SetEnableControls();
		KillTimer(ROUND_TIMER);
		m_eProcessState = ePST_IDLE;
		AddMessage(_T("라운드 중단."));
	}
	else
	{
		m_bDoFinishProcess = TRUE;
		AddMessage(_T("거래가 이미 시작되었습니다. 라운드 종료 요청."));
	}
}


void CABotDlg::OnBnClickedButtonStopRound()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_eProcessState <= ePST_ITEM_WAIT)
	{
		SetEnableControls();
		KillTimer(ROUND_TIMER);
		m_eProcessState = ePST_IDLE;
		AddMessage(_T("라운드 중단."));
	}
	else
	{
		m_bDoFinishProcess = TRUE;
		AddMessage(_T("거래가 이미 시작되었습니다. 라운드 종료 요청."));
	}
}


void CABotDlg::OnBnClickedButtonSellAllCurCost()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	AddMessage(_T("전체 보유 종목 현재가로 매도 요청."));	
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
	AddMessage(_T("라운드[%d], 운용 금액 총합은 %d[원] 입니다."), m_nRoundCount, m_lProcessDR);

	m_cmbBuyTimeOut.GetLBText(m_cmbBuyTimeOut.GetCurSel(), strBuf);
	m_lItemBuyTimeout = atoi((LPSTR)(LPCSTR)strBuf) * 1000;
	AddMessage(_T("라운드[%d], 종목당 매수 timeout은 %d[clock] 입니다."), m_nRoundCount, m_lItemBuyTimeout);

	m_cmbBuyRetry.GetLBText(m_cmbBuyRetry.GetCurSel(), strBuf);
	m_lItemBuyTryCount = atoi((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("라운드[%d], 종목당 매수 재시도 회수는 %d[회] 입니다."), m_nRoundCount, m_lItemBuyTryCount);

	m_cmbHoldTimeOut.GetLBText(m_cmbHoldTimeOut.GetCurSel(), strBuf);
	m_lItemHoldTimeout = atoi((LPSTR)(LPCSTR)strBuf) * 1000;
	AddMessage(_T("라운드[%d], 종목당 보유 timeout은 %d[clock] 입니다."), m_nRoundCount, m_lItemHoldTimeout);

	m_cmbSellOverThis.GetLBText(m_cmbSellOverThis.GetCurSel(), strBuf);
	m_dSellOverThis = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("라운드[%d], 종목당 매도 Sell'OVER'This는 %f[퍼센트] 입니다."), m_nRoundCount, m_dSellOverThis);

	m_cmbSellOverThis2.GetLBText(m_cmbSellOverThis2.GetCurSel(), strBuf);
	m_dSellOverThis2 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("라운드[%d], 종목당 매도 Sell'OVER'This2는 %f[퍼센트] 입니다."), m_nRoundCount, m_dSellOverThis2);

	m_cmbSellOverThis.GetLBText(m_cmbSellOverThis.GetCurSel(), strBuf);
	m_dSellUnderThis = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("라운드[%d], 종목당 매도 Sell'UNDER'This는 %f[퍼센트] 입니다."), m_nRoundCount, m_dSellUnderThis);

	m_cmbSellOverThis2.GetLBText(m_cmbSellOverThis2.GetCurSel(), strBuf);
	m_dSellUnderThis2 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("라운드[%d], 종목당 매도 Sell'UNDER'This2는 %f[퍼센트] 입니다."), m_nRoundCount, m_dSellUnderThis2);

}

void CABotDlg::ProcessSequence()
{
	switch (m_eProcessState)
	{
	case ePST_ROUND_START:	//라운드 시작 상태.
		m_OrderCodeMap.RemoveAll();
		m_ItemCodeMap.RemoveAll();
		m_nProcessRetryCount = 0;
		LoadProcessCondition();
		AddMessage(_T("라운드[%d], 시작 합니다."), m_nRoundCount);
		m_eProcessState = ePST_WAIT_TIME;
		break;

	case ePST_WAIT_TIME:	//라운드 시작 시작 시간을 기다리는 상태.
		m_nProcessRetryCount++;
		if (IsInRoundTime())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드[%d], 예수금 검색을 시작 합니다."), m_nRoundCount);
			m_eProcessState = ePST_DR_SEARCH;
			break;
		}
		break;

	case  ePST_DR_SEARCH:	//예수금 요청 상태.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) {	m_eProcessState = ePST_ROUND_END; break; }	//라운드 가능 시간 확인

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
			AddMessage(_T("라운드[%d], 예수금 조회에 연속 실패하여 프로세스를 중단합니다."), m_nRoundCount);
			m_eProcessState = ePST_KILL_PROC;
			break;
		}
		break;

	case ePST_DR_WAIT:		//예수금 결과 대기 상태.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//라운드 가능 시간 확인

		if (m_lDepositReceived == 0)
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드[%d], 예수금 %d[원], 라운드를 종료합니다."), m_nRoundCount, m_lDepositReceived);
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
			AddMessage(_T("라운드[%d], 종목 검색을 시작 합니다."), m_nRoundCount);
			m_eProcessState = ePST_ITEM_SEARCH;
			break;
		}
		break;

	case ePST_ITEM_SEARCH:	//종목 검색 요청 상태.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//라운드 가능 시간 확인

		if (m_cmbCon.GetCount()<=0)
		{
			OnBnClickedButtonGetfilter();
			break;
		}

		if (REQ_ItemSearch())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드[%d], 종목 요청 완료. 결과를 기다립니다."), m_nRoundCount);
			m_eProcessState = ePST_ITEM_WAIT;
			break;
		}
		else
		{
			//검색 요청 실패시. 화면 번호도 바꿔보고,. 그래도 소용 없군.
			m_strScrNo.Format("%04d", rand() % 10000);
			Sleep(min(2000, m_nProcessRetryCount*10));
		}
		break;

	case ePST_ITEM_WAIT:	//종목 검색 결과 대기 상태.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//라운드 가능 시간 확인

		if (m_ItemCount == -1)
		{
			break;
		}
		AddMessage(_T("라운드[%d], [%d]개의 종목이 검색되었습니다."), m_nRoundCount, m_ItemCount);

		if (m_ItemCount == 0)
		{
			m_ItemCount = -1;
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드[%d], 종목 검색을 다시 시작 합니다."), m_nRoundCount);
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
				AddMessage(_T("라운드[%d], 종목당 운용 허용 금액은 %d[원] 입니다."), m_nRoundCount, m_lProcessItemDR);

				m_nProcessRetryCount = 0;
				AddMessage(_T("라운드[%d], 종목 거래를 시작 합니다."), m_nRoundCount);
				m_eProcessState = ePST_ITEM_TRADE;
				break;
			}
		}
		break;

	case ePST_ITEM_TRADE:	//종목 거래 상태.
		m_nProcessRetryCount++;
		ProcessTrade();

		if (!IsEndTrade())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드[%d], 종목 거래가 모두 종료 되었습니다."), m_nRoundCount);
			m_eProcessState = ePST_ROUND_END;
			break;
		}
		break;

	case ePST_ROUND_END:	//라운드 1회 종료 상태.
		m_nProcessRetryCount++;
		AddMessage(_T("라운드[%d], 종료 합니다."), m_nRoundCount);
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

	case ePST_IDLE:	//정지 상태.
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
		case eST_NONE:		//아무것도 아닌 상태.
			break;
		case eST_ADDED:		//관심 종목에 추가 되었을때.
			if (aItem.m_lcurPrice>0)
			{
				aItem.m_eitemState = eST_TRYBUY;
				break;
			}
			break;

		case eST_TRYBUY:	//매수 시도 상태.
			if (REQ_ItemBuyOrder(aItem))
			{
				aItem.m_ltryBuyTime = GetTickCount();
				aItem.m_ltryBuyTimeout = GetTickCount() + m_lItemBuyTimeout;
				
				AddMessage(_T("라운드[%d], 종목[%s][%s],단가[%d],수량[%d], 매수가 시도 되었습니다."), 
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lbuyPrice, aItem.m_lQuantity);
				aItem.m_eitemState = eST_WAITBUY;
				break;
			}
			break;

		case eST_WAITBUY:	//매수 체결 대기 상태.
			if (aItem.m_ltryBuyTime + long(GetTickCount()) > aItem.m_ltryBuyTimeout)
			{
				aItem.m_ltryBuyCount++;
				if (aItem.m_ltryBuyCount > m_lItemBuyTryCount)
				{
					AddMessage(_T("라운드[%d], 종목[%s][%s],단가[%d],수량[%d],재시도[%d], 거래를 포기 합니다."),
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

				AddMessage(_T("라운드[%d], 종목[%s][%s],단가[%d],수량[%d], 매수가 재시도[%d] 되었습니다."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_ltryBuyCount);
				aItem.m_eitemState = eST_TRYBUY;
				break;
			}
			break;

		case eST_HOLDING:	//보유 상태.
			if (aItem.m_lcurPrice > long(aItem.BuyPrice() + aItem.BuyPrice()*m_dSellOverThis / 100.))
			{
				AddMessage(_T("라운드[%d], 종목[%s][%s],현재가▲[%d],수량[%d], 매도를 시작합니다. 상승."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity);
				aItem.m_eitemState = eST_TRYSELL;
				break;
			}
			if (aItem.m_lcurPrice < long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis / 100.))
			{
				AddMessage(_T("라운드[%d], 종목[%s][%s],현재가▼[%d],수량[%d], 매도을 시작합니다. 하락."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity);
				aItem.m_eitemState = eST_TRYSELL;
				break;
			}
			if (aItem.m_lholdTimeout>0 && aItem.m_lholdTime + long(GetTickCount()) > aItem.m_lholdTimeout)
			{
				AddMessage(_T("라운드[%d], 종목[%s][%s],현재가[%d],수량[%d], 매도을 시작합니다. 타임아웃."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity);
				aItem.m_eitemState = eST_TRYSELL;
				break;
			}
			break;

		case eST_TRYSELL:	//매도 시도 상태.
			if (REQ_ItemSellOrder(aItem))
			{
				aItem.m_ltrySellTime = GetTickCount();
				aItem.m_ltrySellTimeout = GetTickCount() + 10000;	//팔려고 하는데, 10초이상 안팔리면 이상한거임. 그래서 시장가로 팜.

				AddMessage(_T("라운드[%d], 종목[%s][%s],단가[%d],수량[%d], 매도가 시도 되었습니다."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity - aItem.m_lSellQuantity);
				aItem.m_eitemState = eST_WAITSELL;
				break;
			}
			break;

		case eST_WAITSELL:	//매도 체결 대기 상태.
			if (aItem.m_ltrySellTime + long(GetTickCount()) > aItem.m_ltrySellTimeout)
			{
				AddMessage(_T("라운드[%d], 종목[%s][%s],단가[%d],수량[%d], 매도가 시장가로 재시도 되었습니다."),
					m_nRoundCount, aItem.m_strCode, aItem.m_strName, aItem.m_lcurPrice, aItem.m_lQuantity - aItem.m_lSellQuantity);
				aItem.m_eitemState = eST_TRYSELL;
				break;
			}
			break;

		case eST_TRADEDONE:	//거래 완료 상태.
			break;
		}
	}
}

BOOL CABotDlg::REQ_ItemBuyOrder(CABotItem &aItem)
{
	CString strRQName;
	strRQName.Format("ORDER_%04d", aItem.m_index);

	// 매매구분 취득(1:신규매수, 2:신규매도 3:매수취소, 4:매도취소, 5:매수정정, 6:매도정정)
	long lOrderType = 1;
	if (aItem.m_ltryBuyCount > 0)
	{
		lOrderType = 5;	//5:매수정정
	}

	// 거래구분 취득
	// 00:지정가, 03:시장가, 05:조건부지정가, 06:최유리지정가, 07:최우선지정가, 
	// 10:지정가IOC, 13:시장가IOC, 16:최유리IOC, 20:지정가FOK, 23:시장가FOK, 
	// 26:최유리FOK, 61:장개시전시간외, 62:시간외단일가매매, 81:장후시간외종가
	CString strHogaGb = "00";
	ASSERT(aItem.m_lcurPrice != 0);
	if (aItem.m_lcurPrice == 0){ return FALSE; }

	aItem.m_lbuyPrice = aItem.m_lcurPrice;
	aItem.m_lQuantity = (m_lProcessItemDR - aItem.m_lBuyCost) / aItem.m_lbuyPrice;

	if (aItem.m_lQuantity == 0)	//구매 가능 수량이 0일 경우 예외 처리.
	{
		if (aItem.m_ltryBuyCount > 0)
		{
			lOrderType = 3;	//3:매수취소
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
			AddMessage(_T("라운드[%d], 종목[%s][%s],단가[%d],수량[%d], 거래를 포기 합니다."),
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

	// 매매구분 취득(1:신규매수, 2:신규매도 3:매수취소, 4:매도취소, 5:매수정정, 6:매도정정)
	long lOrderType = 2;
	if (aItem.m_strSellOrder.GetLength() > 0)
	{
		lOrderType = 6;	//6:매도정정
	}

	// 거래구분 취득
	// 00:지정가, 03:시장가, 05:조건부지정가, 06:최유리지정가, 07:최우선지정가, 
	// 10:지정가IOC, 13:시장가IOC, 16:최유리IOC, 20:지정가FOK, 23:시장가FOK, 
	// 26:최유리FOK, 61:장개시전시간외, 62:시간외단일가매매, 81:장후시간외종가
	CString strHogaGb = "00";
	ASSERT(aItem.m_lcurPrice != 0);
	if (aItem.m_lcurPrice == 0){ return FALSE; }

	if (aItem.m_ltrySellTime + long(GetTickCount()) > aItem.m_ltrySellTimeout)
	{
		strHogaGb = "03"; //03:시장가,
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