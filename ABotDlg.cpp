
// ABotDlg.cpp : 구현 파일
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

//예약어.
#define DEF_CUR_PRICE	_T("현재가")

//참고
//27 (최우선)매도 호가
//28 (최우선)매수 호가
//14 누적거래대금
//16 시가
//17 고가
//18 저가
//21 호가
//26 전일거래량대비(계약,주)
//29 거래대금증감
//30 전일거래량대비(비율)
//31 거래회전율
//32 거래비용
//311 시가총액(억)
//567 상한가발생시간
//568 하한가발생시간

// {조회 키,		리얼 키,	행, 열, 타입,			색 변경, 정렬,		앞 문자, 뒷 문자}
const stGRID lstOPTSBFID[] =
{
	{ "종목코드",		"9001",	-1, 0, DT_NONE,			FALSE,	DT_LEFT,		"", "" },
//	{ "종목명",			"302",	-1, 1, DT_NONE,			FALSE,	DT_LEFT,		"", "" },
	{ DEF_CUR_PRICE,	"10",	-1, 2, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,		"", "" },
	{ "전일대비기호",	"25",	-1, 3, DT_SIGN,			TRUE,	DT_CENTER,		"", "" },
	{ "전일대비",		"11",	-1, 4, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,		"", "" },
	{ "등락율",			"12",	-1, 5, DT_ZERO_NUMBER,	TRUE,	DT_RIGHT,		"", "%" },
	{ "누적거래량",		"13",	-1, 6, DT_ZERO_NUMBER,	FALSE,	DT_RIGHT,		"", "" },
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
		return "라운드 마무리 상태";
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
		char charBuf[256] = { 0 };
		GetCurrentDirectory(_countof(charBuf)-1, charBuf);
		m_strConfigFile.Format("%s\\%s", charBuf, strBuf);
	}

	if (InitLogData() == FALSE) {
		return FALSE;
	}

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
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

BOOL CABotDlg::InitLogData()
{
	CRect rect(0, 0, 0, 0);
	CString strBuf;
	long n = 0;

	// 로그 파일 경로 얻기.
	ReadFromIniFile_String(m_strConfigFile, "SYSTEM", "logfoldername", ".\\", strBuf);
	strBuf += _T("\\"); strBuf.Replace(_T("\\\\"), _T("\\"));
	m_strLogFolderName = strBuf;
	g_logger.Initial(m_strLogFolderName + _T("ABotLog_"));
	ReadFromIniFile_Int(m_strConfigFile, "SYSTEM", "dowritelog", 1, n);
	g_logger.SetEnable(n > 0 ? TRUE : FALSE);

	//메세지 디스플레이를 가장 먼저 만들고,
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
	//콤보 박스 기초 데이터.
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

	// 계좌 번호.
	ReadFromIniFile_String(m_strConfigFile, "ACCOUNT", "number", "-", strBuf);
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->SetWindowText(strBuf);
	AddMessage("     계좌 번호[%s]", strBuf);

	// 반복 회수
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "count", "1", strBuf);
	if ("반복" == strBuf)
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
	AddMessage("     반복 회수[%s].", strBuf);

	CString strBuf2 = "시작 ";
	// 시작 시각의 시
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "start_hour", "9", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	m_cmbStartHour.SetCurSel(n);
	strBuf2 += strBuf + "시 ";

	// 시작 시각의 분
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "start_min", "0", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	m_cmbStartMin.SetCurSel(n);
	strBuf2 += strBuf + "분,  종료 ";

	// 종료 시각의 시
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "end_hour", "15", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	m_cmbEndHour.SetCurSel(n);
	strBuf2 += strBuf + "시 ";

	// 종료 시각의 분
	ReadFromIniFile_String(m_strConfigFile, "ROUND", "end_min", "0", strBuf);
	n = atol((LPSTR)(LPCSTR)strBuf);
	m_cmbEndMin.SetCurSel(n);
	strBuf2 += strBuf + "분";
	AddMessage("     동작 시간[%s].", strBuf2);

	// 매수시 예수금의 최대 사용 퍼센트. [%]
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
	AddMessage("     예수금 사용 [%s]퍼센트.", strBuf);

	// 매수시 예수금의 최대 사용 금액. [만원]
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
	AddMessage("     최대 예수금 사용 허용 금액 [%s]만원.", strBuf);

	// 종목당 최대 투자 허용 금액
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
	AddMessage("     종목당 운용 금액 [%s]만원.", strBuf);

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
	AddMessage("     매수 체결 대기 타임 아웃 [%s]초.", strBuf);

	// 매수 실패시 재시도 회수
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
	AddMessage("     매수 실패시 재시도 회수 [%s]회.", strBuf);

	// 주식 보유 타임 아웃.
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
	AddMessage("     보유 대기 타임 아웃 [%s]분.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 높아지면 판다.
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
	AddMessage("     매도 시점 #0. 매수 금액의 [%s] 퍼센트 이상 상승시.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 낮아지면 판다.
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
	AddMessage("     매도 시점 #0. 매수 금액의 [%s] 퍼센트 이상 하락시 시장가 매도.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 높아지면 판다.
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
	AddMessage("     매도 시점 #1. 매도 타임아웃0 후 매수 금액의 [%s] 퍼센트 이상 상승시  매도.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 낮아지면 판다.
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
	AddMessage("     매도 시점 #1. 매도 타임아웃0 후 매수 금액의 [%s] 퍼센트 이상 하락시 시장가 매도.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 높아지면 판다.
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
	AddMessage("     매도 시점 #2. 매도 타임아웃0+1 후 매수 금액의 [%s] 퍼센트 이상 상승시 매도.", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 낮아지면 판다.
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
	AddMessage("     매도 시점 #2. 매도 타임아웃0+1 후 매수 금액의 [%s] 퍼센트 이하 도달시 시장가 매도.", strBuf);

	// 매도 타임 아웃.
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
	AddMessage("     매도 대기 타임 아웃 #0 [%s]분.", strBuf);

	// 매도 타임 아웃.
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
	AddMessage("     매도 대기 타임 아웃 #1 [%s]분.", strBuf);

	// 매수 수수료
	ReadFromIniFile_String(m_strConfigFile, "BUY", "tradefee", "0.015", strBuf);
	m_dBuyTradeFee = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage("     매수 거래 수수료(세금 포함)매수 금액의 [%s] 퍼센트.", strBuf);

	// 매도 수수료
	ReadFromIniFile_String(m_strConfigFile, "SELL", "tradefee", "0.315", strBuf);
	m_dSellTradeFee = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage("     매도 거래 수수료(세금 포함)매도 금액의 [%s] 퍼센트.", strBuf);

	// 라운드 종료후 미채결 종목 시장가로 팔기.
	ReadFromIniFile_String(m_strConfigFile, "SELL", "sell_item_market_value_at_round_end", "0", strBuf);
	m_bDoSellItemMarketValueAtRoundEnd = (atoi((LPSTR)(LPCSTR)strBuf) != 0 ? TRUE : FALSE);
	m_checkDoSellItemMarketValueAtRoundEnd.SetCheck(m_bDoSellItemMarketValueAtRoundEnd ? BST_CHECKED : BST_UNCHECKED);
	AddMessage("     라운드 종료 후, 미채결 종목 시장가로 팔기 [%s].", m_bDoSellItemMarketValueAtRoundEnd?"함":"안함");	
}

void CABotDlg::SaveSystemFile()
{
	long i = 0, n = 0;
	double d = 0.;
	CString strBuf, strCombo;

	// 계좌 번호.
	((CEdit*)GetDlgItem(IDC_EDIT_ACCNO))->GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ACCOUNT", "number", strBuf);

	// 반복 회수
	m_cmbItemCount.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "count", strBuf);

	// 시작 시각의 시
	m_cmbStartHour.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "start_hour", strBuf);

	// 시작 시각의 분
	m_cmbStartMin.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "start_min", strBuf);

	// 종료 시각의 시
	m_cmbEndHour.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "end_hour", strBuf);

	// 종료 시각의 분
	m_cmbEndMin.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "ROUND", "end_min", strBuf);

	// 검색 조건
	m_cmbCondtion.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "CONDITION", "name", strBuf);

	// 매수시 예수금의 최대 사용 퍼센트. [%]
	m_cmbDpUseRate.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "use_rate", strBuf);

	// 매수시 예수금의 최대 사용 금액. [만원]
	m_cmbMaxTotalAmount.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "max_total_amount", strBuf);	

	// 종목당 최대 투자 허용 금액
	m_cmbMaxAmount.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "max_amount", strBuf);

	// 매수 방법, 현재가, 시장가. 향후..퍼센트 지정.
	m_cmbBuyMethod.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "method", strBuf);

	// 매수 체결 대기 타임 아웃.
	m_cmbBuyTimeOut.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "timeout", strBuf);

	// 매수 실패시 재시도 회수
	m_cmbBuyRetry.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "BUY", "retry", strBuf);

	// 주식 보유 타임 아웃.
	m_cmbHoldTimeOut.GetWindowText(strBuf);
	WriteToIniFile_String(m_strConfigFile, "HOLD", "timeout", strBuf);

	// 구매후 종목 현재가가, 이 퍼센트 보다 높아/낮지면 판다.
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
	
	// 매도 타임 아웃.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	SetDlgItemText(IDC_BUTTON_EDIT_INROUND, "비율 수정");

	CString strBuf;

	CString strIndex;
	m_cmbCondtion.GetLBText(m_cmbCondtion.GetCurSel(), m_strConditionName);
	m_mapNameList.Lookup(m_strConditionName, strIndex);
	m_nConditionIndex = _ttoi(strIndex);
	AddMessage(_T("_____::검색식[%d][%s]을 사용합니다."), m_nConditionIndex, m_strConditionName);

	m_cmbItemCount.GetWindowText(strBuf);
	m_nProcessItemCount = atol((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::운용 종목수는 %d[개] 입니다."), m_nProcessItemCount);

	m_cmbMaxAmount.GetWindowText(strBuf);
	m_lProcessItemDR = atol((LPSTR)(LPCSTR)strBuf) * 10000;
	if (m_lProcessItemDR > 100 * 10000)
	{
		m_lProcessItemDR = 100 * 10000;
	}
	AddMessage(_T("_____::종목당 운용 금액은 %s[원] 입니다."), GetCurrencyString(m_lProcessItemDR));

	m_cmbBuyTimeOut.GetWindowText(strBuf);
	m_lItemBuyTimeout = atol((LPSTR)(LPCSTR)strBuf) * 1000;
	AddMessage(_T("_____::종목당 매수 timeout은 %d[초] 입니다."), m_lItemBuyTimeout / 1000);

	m_cmbBuyRetry.GetWindowText(strBuf);
	m_lItemBuyTryCount = atol((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::종목당 매수 재시도 회수는 %d[회] 입니다."), m_lItemBuyTryCount);

	m_cmbHoldTimeOut.GetWindowText(strBuf);
	m_lItemHoldTimeout = atol((LPSTR)(LPCSTR)strBuf) * 1000 * 60;
	AddMessage(_T("_____::종목당 보유 timeout은 %d[분] 입니다."), m_lItemHoldTimeout / (1000 * 60));

	m_cmbSellOverThis0.GetWindowText(strBuf);
	m_dSellOverThis0 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::종목당 매도 Sell'OVER'This0는 %f[퍼센트] 입니다."), m_dSellOverThis0);

	m_cmbSellUnderThis0.GetWindowText(strBuf);
	m_dSellUnderThis0 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::종목당 매도 Sell'UNDER'This0는 %f[퍼센트] 입니다."), m_dSellUnderThis0);

	m_cmbSellOverThis1.GetWindowText(strBuf);
	m_dSellOverThis1 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::종목당 타임아웃#0후 매도 Sell'OVER'This1는 %f[퍼센트] 입니다."), m_dSellOverThis1);

	m_cmbSellUnderThis1.GetWindowText(strBuf);
	m_dSellUnderThis1 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::종목당 타임아웃#0후 매도 Sell'UNDER'This1는 %f[퍼센트] 입니다."), m_dSellUnderThis1);

	m_cmbSellOverThis2.GetWindowText(strBuf);
	m_dSellOverThis2 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::종목당 타임아웃#0+ #1후 매도 Sell'OVER'This2는 %f[퍼센트] 입니다."), m_dSellOverThis2);

	m_cmbSellUnderThis2.GetWindowText(strBuf);
	m_dSellUnderThis2 = atof((LPSTR)(LPCSTR)strBuf);
	AddMessage(_T("_____::종목당 타임아웃#0+ #1후 매도 Sell'UNDER'This2는 %f[퍼센트] 입니다."), m_dSellUnderThis2);

	m_cmbSellTimeOut0.GetWindowText(strBuf);
	m_lItemSellTimeout0 = long(atof((LPSTR)(LPCSTR)strBuf) * 1000.0 * 60.0);
	AddMessage(_T("_____::종목당 매도 타임아웃#0은 %s[분] 입니다."), strBuf);

	m_cmbSellTimeOut1.GetWindowText(strBuf);
	m_lItemSellTimeout1 = long(atof((LPSTR)(LPCSTR)strBuf) * 1000.0 * 60.0);
	AddMessage(_T("_____::종목당 매도 타임아웃#1은 %s[분] 입니다."), strBuf);

	m_bDoSellItemMarketValueAtRoundEnd = (m_checkDoSellItemMarketValueAtRoundEnd.GetCheck() == BST_CHECKED);
	AddMessage(_T("_____::라운드 종료 후, 미채결 종목 시장가로 팔기 [%s]."), m_bDoSellItemMarketValueAtRoundEnd ? "함" : "안함");

	theApp.m_khOpenApi.SetRealRemove(_T("ALL"), _T("ALL"));
	AddMessage(_T("_____::현재 등록된 실시간 조회 요청 종목을 모두 삭제합니다."));
}


void CABotDlg::InitRealAddGrid()
{
	COLORREF clr = RGB(215, 227, 241);
	long i = 0, nWidth[] = { 50, 101, 56, 30, 56, 56, 80 };
	long nCnt = sizeof(nWidth) / sizeof(*nWidth);		// 전체크기 / 원소크기 = 원소개수
	CString strHeader[] = { "코드", "종목명", "현재가", "기호", "전일대비", "등락율", "거래량" };

	m_grdRealAdd.SetEditable(FALSE);				//cell을 에디트 못하게 함.
//	m_grdRealAdd.EnableScrollBars(SB_BOTH, FALSE);
	m_grdRealAdd.EnableScrollBars(SB_VERT, TRUE);

	m_grdRealAdd.SetFixedRowCount(1); // 고정 행/열 설정
	m_grdRealAdd.SetRowCount(1); // 행/열 갯수 설정
	SetGridHeight(m_grdRealAdd, 0, 24); // grid 높이는 윈도우 별로 상이하므로 별도 함수로 호출한다.
	m_grdRealAdd.SetColumnCount(nCnt); // 열의 개수 설정

	for (i = 0; i < nCnt; i++)
	{
		SetGridWidth(m_grdRealAdd, i, nWidth[i]); // grid 폭은 윈도우 별로 상이하므로 별도 함수로 호출한다.
		m_grdRealAdd.SetItemFormat(0, i, DT_CENTER);
		m_grdRealAdd.SetItemText(0, i, strHeader[i]);
		m_grdRealAdd.SetItemBkColour(0, i, clr);	// 지정된 셀의 배경색 설정
	}

	m_grdRealAdd.SetRowCount(21);
	m_grdRealAdd.Invalidate();
}

void CABotDlg::InitBuyItemGrid()
{
	COLORREF clr = RGB(215, 227, 241);
	long i = 0, nWidth[] = { 101, 56, 80, 80, 56, 56, 56 };
	long nCnt = sizeof(nWidth) / sizeof(*nWidth);		// 전체크기 / 원소크기 = 원소개수
	CString strHeader[] = { "종목명", "매입가", "평가손익", "매입금액", "수익율", "가능수량", "보유수량" };

	m_grdBuyItem.SetEditable(FALSE);				//cell을 에디트 못하게 함.
	m_grdBuyItem.EnableScrollBars(SB_VERT, FALSE);
	m_grdBuyItem.SetFixedRowCount(1); // 고정 행/열 설정
	m_grdBuyItem.SetRowCount(1); // 행/열 갯수 설정
	m_grdBuyItem.SetColumnCount(nCnt);
	
	for (i = 0; i < nCnt; i++)
	{
		SetGridWidth(m_grdBuyItem, i, nWidth[i]); // grid 폭은 윈도우 별로 상이하므로 별도 함수로 호출한다.
		m_grdBuyItem.SetItemFormat(0, i, DT_CENTER);
		m_grdBuyItem.SetItemText(0, i, strHeader[i]);
		m_grdBuyItem.SetItemBkColour(0, i, clr);	// 지정된 셀의 배경색 설정
	}
	m_grdBuyItem.SetRowCount(21);
	m_grdBuyItem.Invalidate();
}

void CABotDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.

}

void CABotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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
		long cxIcon = GetSystemMetrics(SM_CXICON);
		long cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		long x = (rect.Width() - cxIcon + 1) / 2;
		long y = (rect.Height() - cyIcon + 1) / 2;

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

	SafeDelete(g_pMsgDisp);	//메세지 디스플레이를 가장 나중에 소멸 시켜야 한다.
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
	ON_EVENT(CABotDlg, IDC_KHOPENAPICTRL1, 7, CABotDlg::OnReceiveRealCondition, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
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
		long nGetConditionRet = theApp.m_khOpenApi.GetConditionLoad();
		AddMessage("검색 조건 조회 %s. [%d]", (nGetConditionRet>=0?"성공":"실패"), nGetConditionRet);
		SetTimer(CONDITION_TIMER, 1000, NULL);
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

	//아래 메시지 같은것이 날라온다... 어케 처리해야 할까.
	//[2016/09/27][09:07:10:466] : 조회에러,스크린번호[6484],RQName[주식주문],TrCode[주식주문],메시지[[571552] 주문단가가 하한가보다 낮습니다.]

}


void CABotDlg::OnBnClickedBalanceQuery()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	REQ_DepositReceived();
}

BOOL CABotDlg::REQ_DepositReceived()
{
	if (getAccountData() == FALSE) {
		return FALSE;
	}
	theApp.m_khOpenApi.SetInputValue("계좌번호", m_strAccNo);
	//	theApp.m_khOpenApi.SetInputValue("비밀번호", _T("0000"));
	theApp.m_khOpenApi.SetInputValue("비밀번호입력매체구분", _T("00"));
	theApp.m_khOpenApi.SetInputValue("조회구분", _T("1"));

	long lRet = theApp.m_khOpenApi.CommRqData(_T("예수금"), _T("OPW00001"), 0, m_strScrNo);
	AddMessage("예수금 조회 요청 %s. [%d]", (lRet >= 0 ? "성공" : "실패"), lRet);
	return (lRet >= 0 ? TRUE : FALSE);
}

/*
void CABotDlg::RealData()
{
	static BOOL btoggle = TRUE;
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
	btoggle = btoggle?FALSE:TRUE;
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

		strText.Format(_T("__________::구분[%s] FID[%4s:%s] = [%s]"), strGubun, strFID, strFIDName, strOneData);
	//	if (!IsInRound()) 
			AddMessage(strText);

		strFIDName.Empty();

		//구분[0] FID[9201:계좌번호] = [5088156710]
		//구분[0] FID[9203:주문번호] = [0067777]
		//구분[0] FID[9205:9205] = []
		//구분[0] FID[9001:종목코드] = [A009180]
		//구분[0] FID[912:912] = [JJ]
		//구분[0] FID[913:주문상태] = [체결]
		//구분[0] FID[302:종목명] = [한솔로지스틱스]
		//구분[0] FID[900:주문수량] = [90]
		//구분[0] FID[901:주문가격] = [3300]
		//구분[0] FID[902:미체결수량] = [0]
		//구분[0] FID[903:체결누계금액] = [296670]
		//구분[0] FID[904:원주문번호] = [0000000]
		//구분[0] FID[905:주문구분] = [+매수]
		//구분[0] FID[906:매매구분] = [보통]
		//구분[0] FID[907:매도수구분] = [2]
		//구분[0] FID[908:주문 / 체결시간] = [124500]
		//구분[0] FID[909:체결번호] = [14728]
		//구분[0] FID[910:체결가] = [3296]
		//구분[0] FID[911:체결량] = [90]
		//구분[0] FID[10:현재가] = [+3300]
		//구분[0] FID[27:(최우선)매도호가] = [+3300]
		//구분[0] FID[28:(최우선)매수호가] = [+3290]
		//구분[0] FID[914:단위체결가] = [3300]
		//구분[0] FID[915:단위체결량] = [24]
		//구분[0] FID[938:938] = [40]
		//구분[0] FID[939:939] = [0]
		//구분[0] FID[919:거부사유] = [0]
		//구분[0] FID[920:화면번호] = [7198]
		//구분[0] FID[921:921] = [Y3011168]
		//구분[0] FID[922:922] = [00]
		//구분[0] FID[923:923] = [00000000]

	//	if (IsInRound())// && strGubun=="0")
		{
			if (strFID == "9203")	//주문번호
			{
				strOrderCode = strOneData;
			}
			if (strFID == "9001")	//종목코드
			{
				strItemCode = strOneData.Right(6);
			}
			else if (strFID == "913")	//주문상태
			{
				if (strOneData.Find("접수") >= 0)
				{
					bREQDone = TRUE;
				}
				else if (strOneData.Find("체결") >= 0)
				{
					bTRADEDone = TRUE;
				}
			}
			else if (strFID == "905" && strGubun == "0")	//주문구분
			{
				if (strOneData.Find("매수") >= 0)
				{
					bBUYDone = TRUE;
				}
				else if (strOneData.Find("매도") >= 0)
				{
					bSELLDone = TRUE;
				}
				else
				{
					long k = 0;//이게 취소인가?
				}
			}
			else if (strFID == "911")	//체결량
			{
				lTradeQuantity = atol(strOneData);
			}
			else if (strFID == "902")	//미 체결량
			{
				lRemainQuantity = atol(strOneData);
			}
			else if (strFID == "910")	//체결가
			{
				lTradePrice = atol(strOneData);
			}
			else if (strFID == "10")	//현재가
			{
				lCurPrice = atol(strOneData);
			}
			else if (strFID == "900")	//요청수량
			{
				lReqQuantity = atol(strOneData);
			}
			else if (strFID == "901")	//요청가
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
				// 주문 번호
				if (aItem.m_eitemState == eST_WAITBUY)
				{
					aItem.m_ltryBuyTimeout = GetTickCount() + m_lItemBuyTimeout;

					if (lRemainQuantity != 0)
					{
						aItem.m_strBuyOrder = strOrderCode;
						AddMessage(_T("__________::[%s][%s][%s],단가[%d],수량[%d],잔량[%d],코드[%s] 매수 주문이 완료 되었습니다."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lReqPrice, lReqQuantity, aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_strBuyOrder);
					}
				}
				else if (aItem.m_eitemState == eST_WAITSELL)
				{
				//	aItem.m_ltrySellTimeout = (m_lItemHoldTimeout > 0 ? GetTickCount() + m_lItemHoldTimeout : 0);
					if (lRemainQuantity != 0)
					{ 
						aItem.m_strSellOrder = strOrderCode; 
						AddMessage(_T("__________::[%s][%s][%s],단가[%d],수량[%d],잔량[%d],코드[%s] 매도 주문이 완료 되었습니다."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lReqPrice, lReqQuantity, aItem.m_lQuantity - aItem.m_lSellQuantity, aItem.m_strSellOrder);
					}
				}
				else if (aItem.m_eitemState == eST_WAITBUYCANCEL ||
					aItem.m_eitemState == eST_BUYCANCEL)
				{
					if (aItem.m_lBuyQuantity > 0)
					{
						AddMessage(_T("__________::[%s][%s][%s],단가[%d],수량[%d],잔량[%d], 매수 취소 주문이 완료 되었습니다."),
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
						AddMessage(_T("__________::[%s][%s][%s] 거래 완료 처리 되었습니다."),
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
						AddMessage(_T("__________::[%s][%s][%s],단가[%d],수량[%d],잔량[%d], 매도 취소 주문이 완료 되었습니다."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lReqPrice, lReqQuantity, aItem.m_lQuantity - aItem.m_lSellQuantity);
						aItem.m_eitemState = eST_HOLDING;
					//	return;
					}
					else
					{
						AddMessage(_T("__________::[%s][%s][%s] 매도 취소가 완료 되었습니다."),
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
						AddMessage(_T("__________::[%s][%s][%s],매수 잔여 수량이 [0]으로 정정 되었습니다."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString());
					}
					AddMessage(_T("__________::[%s][%s][%s],단가[%d],수량[%d],잔량[%d], 매수 되었습니다."),
						aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lTradePrice, lTradeQuantity, aItem.m_lQuantity - aItem.m_lBuyQuantity);

					if (aItem.m_lBuyQuantity >= aItem.m_lQuantity)
					{
					//	m_mapOrderCode.RemoveKey(strOrderCode);
						aItem.m_lholdTime = GetTickCount();
						aItem.m_lholdTimeout	= (m_lItemHoldTimeout > 0 ? aItem.m_lholdTime + m_lItemHoldTimeout : 0);
						aItem.m_ltrySellTimeout0 = (m_lItemSellTimeout0 > 0 ? aItem.m_lholdTime + m_lItemSellTimeout0 : 0);
						aItem.m_ltrySellTimeout1 = ((m_lItemSellTimeout0 > 0 && m_lItemSellTimeout1 > 0) ? aItem.m_lholdTime + m_lItemSellTimeout0 + m_lItemSellTimeout1 : 0);
						aItem.m_eitemState = eST_HOLDING;
						AddMessage(_T("__________::[%s][%s][%s],평균단가[%d],수량[%d],재시도회수[%d], 매수 완료 되었습니다."),
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
						AddMessage(_T("__________::[%s][%s][%s],매도 잔여 수량이 [0]으로 정정 되었습니다."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString());
					}
					AddMessage(_T("__________::[%s][%s][%s],단가[%d],수량[%d],잔량[%d], 매도 되었습니다."),
						aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lTradePrice, lTradeQuantity, aItem.m_lQuantity - aItem.m_lSellQuantity);

					if (aItem.m_lSellQuantity >= aItem.m_lQuantity)
					{
						//	m_mapOrderCode.RemoveKey(strOrderCode);

						theApp.m_khOpenApi.SetRealRemove(m_strScrNo, aItem.m_strCode);
						AddMessage(_T("__________::[%s][%s][%s],재시도회수_매수[%d]매도[%d], 실시간 관심 종목 제외."),
							aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_ltryBuyCount, aItem.m_ltrySellCount);

						aItem.m_eitemState = eST_TRADECLOSING;
						AddMessage(_T("__________::[%s][%s][%s],이익[%d],매수가[%d],매도가[%d], 거래 완료 되었습니다."),
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
	// 종목명/
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
	long i=0, nFieldCnt = sizeof(lstOPTSBFID) / sizeof(*lstOPTSBFID);		// 전체크기 / 원소크기 = 원소개수
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
				//	AddMessage(_T("OnReceiveRealData::종목[%s][%s][%s] 현재가[%s], grid index[%s], item index[%d]"),
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
//! Function      : 문자열 포멧 설정
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
//! Function      : 그리드 셀 문자색 변경 및 기호 데이터 설정
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

	long i=0, nRow = atol(strTemp), nCnt = arrData.GetSize();

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
			long nCol = lstOPTSBFID[i].nCol;
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
void CABotDlg::SetGridHeight(CGridCtrl& gridCtrl, long row, long height)
{
	if (GetWindowsVersion() > eWindows_7) {
		height *= 2;
	}
	gridCtrl.SetRowHeight(row, height);
}

//*******************************************************************/
//! Function Name : SetGridWidth
//! Function      : 그리드 폭을 조정한다.
//! Param         : -
//! Return        : void
//! Create        : 2016/09/13
//! Comment       : 윈도우마다 그리드 모습이 다양하므로 이를 별도 함수로 뺀다.
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
//! Function      : 조회 응답 처리
//! Param         : LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrcode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveTrData(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AddMessage("OnReceiveTrData::ScrNo[%s],RQName[%s],TrCode[%s]", sScrNo, sRQName, sTrCode);

	CString strRQName = sRQName;
	if (strRQName == _T("예수금"))		// 주식기본정보 설정
	{
		CString strBuff;
		// 예수금
	//	strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("예수금"));	strBuff.Trim();
	//	strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("d+1추정예수금"));	strBuff.Trim();
		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("d+2추정예수금"));	strBuff.Trim();
		
		m_lDepositReceived = atol((LPSTR)(LPCSTR)strBuff);
		
		strBuff = GetCurrencyString(m_lDepositReceived);
		
		((CEdit*)GetDlgItem(IDC_EDIT_DEPOSITRECEIVED))->SetWindowText(strBuff);

		AddMessage(strBuff + " [원]");
	}
	else if (strRQName == _T("잔고"))
	{
		double temp = 0.0;
		CString strBuff;
		
		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("총매입금액"));	
		strBuff = GetCurrencyString(strBuff);
		((CEdit*)GetDlgItem(IDC_EDIT_GROSS_PURCHASE))->SetWindowText(strBuff);
		AddMessage("총매입" + strBuff + " KRW");

		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("총평가금액"));
		strBuff = GetCurrencyString(strBuff);
		((CEdit*)GetDlgItem(IDC_EDIT_ASSESSMENT_TOTAL))->SetWindowText(strBuff);
		AddMessage("총평가" + strBuff + " KRW");

		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("총평가손익금액"));
		strBuff = GetCurrencyString(strBuff);
		((CEdit*)GetDlgItem(IDC_EDIT_PROFIT_LOSS_TOTAL))->SetWindowText(strBuff);
		AddMessage("총손익" + strBuff + " KRW");

		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("총수익률(%)"));
		temp = atof(strBuff);
		temp = temp / (double)100;
		strBuff.Format("%.2f", temp);
		((CEdit*)GetDlgItem(IDC_EDIT_EARNING_RATIO_TOTAL))->SetWindowText(strBuff);
		AddMessage(CString("총수익률" + strBuff + " 퍼센트")); // %나 %%의 경우 동작하지 않고 도중에 에러 발생.

		CString	strTRCode(sTrCode);
		long rowCount = theApp.m_khOpenApi.GetRepeatCnt(strTRCode, _T(""));

		m_grdBuyItem.SetRowCount(rowCount + 1);

		if (rowCount >= 1)
		{
			CString strData;

			for (long row = 0; row < rowCount; row++)
			{
				strBuff.Empty();
				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("종목명")); strData.Trim();
				strBuff += CString("종목명" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 0, DT_CENTER);
				m_grdBuyItem.SetItemText(row + 1, 0, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("매입가")); strData.Trim();
				strData = GetCurrencyString(strData);
				strBuff += CString("매입가" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 1, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 1, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("평가손익")); strData.Trim();
				strData = GetCurrencyString(strData);
				m_grdBuyItem.SetItemFgColour(row + 1, 2, GetFGColor(strData));
				m_grdBuyItem.SetItemFormat(row + 1, 2, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 2, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("매입금액")); strData.Trim();
				strData = GetCurrencyString(strData);
				strBuff += CString("매입금액" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 3, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 3, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("수익률(%)")); strData.Trim();
				temp = atof(strData);
				temp = temp / (double)100;
				strData.Format("%.2f", temp);
				strBuff += CString("수익률" + strData + ", ");
				m_grdBuyItem.SetItemFgColour(row + 1, 4, GetFGColor(strData));
				m_grdBuyItem.SetItemFormat(row + 1, 4, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 4, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("매매가능수량")); strData.Trim();
				strData.Format("%d", atol(strData));
				strBuff += CString("가능수량" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 5, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 5, strData);

				strData = theApp.m_khOpenApi.CommGetData(strTRCode, _T(""), _T(""), row, _T("보유수량")); strData.Trim();
				strData.Format("%d", atol(strData));
				strBuff += CString("보유수량" + strData + ", ");
				m_grdBuyItem.SetItemFormat(row + 1, 6, DT_RIGHT);
				m_grdBuyItem.SetItemText(row + 1, 6, strData);
				
				AddMessage(strBuff);
			}
			m_grdBuyItem.Invalidate();
		}
	}
	else if (strRQName == _T("일자별실현손익요청"))
	{
		CString strBuff;

		strBuff = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("실현손익"));
		strBuff = GetCurrencyString(strBuff);
		((CEdit*)GetDlgItem(IDC_EDIT_REALIZATION_PROFIT_LOSS))->SetWindowText(strBuff);
		AddMessage("실현손익" + strBuff + " [원]");
	}
	else if (strRQName == _T("주식주문"))		// 주식기본정보 설정
	{
		CString strData = theApp.m_khOpenApi.GetCommData(sTrCode, sRQName, 0, _T("주문번호"));	strData.Trim();
		AddMessage("주식주문 주문번호 [" + strData + "]");
	}
	else if (strRQName == _T("대출일조회"))
	{/// 
		CString		strTRCode(sTrCode);
		long nCnt = theApp.m_khOpenApi.GetRepeatCnt(strTRCode, _T(""));

		if (nCnt < 1)
		{
			AddMessage(_T("대출일 조회 반복데이터 없음."));
			return;
		}

		CString		strSymbol, strSymbolName, strLoanDate, strText;
		for (long nIdx = 0; nIdx < nCnt; nIdx++)
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



void CABotDlg::OnBnClickedButtonGetCondition()
{
	long nGetConditionRet = theApp.m_khOpenApi.GetConditionLoad();
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
			// 검색 조건
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	REQ_ItemSearch();
}

BOOL CABotDlg::REQ_ItemSearch()
{
//	DWORD pId = GetCurrentProcessId();
//	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, _T("ALL"));
	theApp.m_khOpenApi.SetRealRemove(_T("ALL"), _T("ALL"));

	CString strConditionCodeList = _T(""), strIndex;

	//선택한 조건명의 검색된 종목리스트를 불러온다.
	m_cmbCondtion.GetLBText(m_cmbCondtion.GetCurSel(), m_strConditionName);
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
	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 1);
//	BOOL bRet = theApp.m_khOpenApi.SendCondition(m_strScrNo, m_strConditionName, m_nConditionIndex, 0);
	AddMessage("검색식[%d][%s], 실시간 종목 검색 요청 %s.", m_nConditionIndex, m_strConditionName, (bRet ? "성공" : "실패"));
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	long ret = 0;
	ret = CalcBuyAndSellPrice(5300, 1.0, FALSE);
}

BOOL CABotDlg::REQ_ItemRealReg()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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
	AddMessage("검색된 종목들에 대한 실시간 조회 요청 %s. ret[%d]", (lRet >= 0 ? "성공" : "실패"), lRet);
	return (lRet >= 0 ? TRUE : FALSE);
}

//*******************************************************************/
//! Function Name	: OnReceiveTrCondition
//! Function			: 조건검색 종목리스트 조회응답
//! Param				: LPCTSTR sScrNo					- 화면번호
//!						: PCTSTR strCodeList				- 종목리스트
//!						: LPCTSTR strConditionName			- 조건명
//!						: long nConditionIndex				- 조건명 인덱스번호
//!						: long nNext							- 연속조회(2:연속조회, 0:연속조회없음)
//! Return        : void
//! Create        : , 2015/04/01
//! Comment       : 
//******************************************************************/
void CABotDlg::OnReceiveTrCondition(LPCTSTR sScrNo, LPCTSTR strCodeList, LPCTSTR strConditionName, long nConditionIndex, long nNext)
{
//	DWORD pId = GetCurrentProcessId();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	AddMessage(_T("OnReceiveTrCondition::"));

	CString strCndName, sConditionName;
	m_cmbCondtion.GetLBText(m_cmbCondtion.GetCurSel(), strCndName);	//선택된 조건명

	if (strConditionName == strCndName)	//현재 조건명과 조회응답으로 들어온 조건명이 같을때만 처리.
	{
		AddMessage("검색식[%d][%s], 종목 검색 요청 결과,", nConditionIndex, strConditionName);

		if (strlen(strCodeList)>0)
		{
			long nIndex(0);
			CString strConditionCode;
			m_mapJongCode.RemoveAll();		// 종목코드 초기화
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
					SetGridHeight(m_grdRealAdd, nIndex, 24); // grid 높이는 윈도우 별로 상이하므로 별도 함수로 호출한다.
					m_grdRealAdd.SetItemText(nIndex, 0, strConditionCode);

					// 종목명
					CString strCodeName;
					strCodeName = theApp.m_khOpenApi.GetMasterCodeName(strConditionCode);
					m_grdRealAdd.SetItemText(nIndex, 1, strCodeName);

					strIndex.Format("%3d", nIndex);
					m_mapJongCode.SetAt(strConditionCode, strIndex);
					AddMessage(_T("[%s][%s][%s] 검색됨."), strIndex, strConditionCode, strCodeName);

					if ((IsInRound() && IsInRoundTime() && 0 <= m_ItemCount && m_ItemCount < min(long(_countof(m_Item)), m_nProcessItemCount)) ||
						(!IsInRound() && 0 <= m_ItemCount && m_ItemCount < long(_countof(m_Item))))
					{
						AddMessage("[%3d][%s][%s] 운용 종목들에 추가", m_ItemCount, strConditionCode, strCodeName);
						m_Item[m_ItemCount].m_eitemState = eST_ADDED;
						m_Item[m_ItemCount].m_strName = strCodeName;
						m_Item[m_ItemCount].m_strCode = strConditionCode;
						m_mapItemCode[m_Item[m_ItemCount].m_strCode] = m_ItemCount;
						m_mapUsedItemCode[m_Item[m_ItemCount].m_strCode] = m_ItemCount;
						m_ItemCount++;
					}
				}
			}
		//	m_ItemCount = 1;	//디버그용.
		}
		else
		{
			m_mapJongCode.RemoveAll();		// 종목코드 초기화
			m_grdRealAdd.SetRowCount(1);

			m_ItemCount = 0;
			m_mapItemCode.RemoveAll();
			m_mapUsedItemCode.RemoveAll();

			AddMessage(_T("검색된 종목이 없습니다."));
		}
	}
}

//*******************************************************************/
//! Function Name : OnReceiveRealCondition
//! Function      : 조건검색 실시간 종목 삽입/삭제
//! Param         : LPCTSTR strCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex
//! Return        : void
//! Create        : , 2015/04/20
//! Comment       : 
//******************************************************************/

void CABotDlg::OnReceiveRealCondition(LPCTSTR sTrCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex)
{
//	DWORD pId = GetCurrentProcessId();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
//	AddMessage(_T("OnReceiveRealCondition::"));

	CString strName, strMsg, strIndex, sType, sCode;
	sType = strType;	//종목 편입, 이탈 구분
	sCode = sTrCode;	//종목코드
	m_cmbCondtion.GetLBText(m_cmbCondtion.GetCurSel(), strName);	//현재 선택된 조건명
	if (IsInRound()) { strName = m_strConditionName; }

	// 종목명/
	CString strCodeName;
	strCodeName = theApp.m_khOpenApi.GetMasterCodeName(sCode);

	AddMessage(_T("OnReceiveRealCondition::검색식[%s],[%s][%s]종목이 %s되었습니다."), strConditionName, sCode, strCodeName, (sType=="I"?"편입":"이탈"));

	if (strName == strConditionName)	//현재 선택된 조건명과 실시간으로 들어온 조건명이 같은지 비교.
	{
		if (sType == "I")	//종목 편입
		{
			long aIndex = 0;
			if (m_mapItemCode.Lookup(sCode, aIndex)) // 이미 있는 종목의 경우 다시 매수 종목 리스트로 옮기지 않는다.
			{
				return;
			}

			m_grdRealAdd.InsertRow(sCode, -1);

			SetGridHeight(m_grdRealAdd, m_ItemCount + 1, 24); // grid 높이는 윈도우 별로 상이하므로 별도 함수로 호출한다.
			m_grdRealAdd.SetItemText(m_ItemCount + 1, 0, sCode);

			m_grdRealAdd.SetItemText(m_ItemCount + 1, 1, strCodeName);

			strMsg.Format(_T("______________________::[%s][%s]종목이 편입되었습니다."), sCode, strCodeName);
			AddMessage(strMsg);

			//기존조회된 종목의 시세가 들어오고 있는 상황에서 추가로 편입된 종목의 시세를 보여줄때 
			// 마지막 파라메터에 "1"을 넣어주어야 기존 종목과 함께 편입된 종목의 시세가 들어온다.
			// 만약 "1" 이외의 "0"을 넣어주면 기존에 등록한 종목들에 대한 시세가 끊어진다.
			//실시간등록 함수 호출
			long lRet = theApp.m_khOpenApi.SetRealReg(m_strScrNo, sCode, FIDLIST, "1");
			AddMessage("______________________::[%s][%s]종목 실시간 조회 요청 %s. ret[%d]", sCode, strCodeName, (lRet >= 0 ? "성공" : "실패"), lRet);

			if (lRet >= 0)
			{
				aIndex = 0;
				if (!m_mapUsedItemCode.Lookup(sCode, aIndex))
				{
					if ((IsInRound() && IsInRoundTime() && 0 <= m_ItemCount && m_ItemCount < min(long(_countof(m_Item)), m_nProcessItemCount)) ||
						(!IsInRound() && 0 <= m_ItemCount && m_ItemCount < long(_countof(m_Item))))
					{
						AddMessage("______________________::[%s][%s]종목 운용종목[%3d]에 추가", sCode, strCodeName, m_ItemCount);
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
		else if (sType == "D")	//종목 이탈
		{
			//기존에 있던 종목인지 확인.
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
		//			//인덱스 정렬을 해야 하는구나.
		//		}
		//	}

			//이탈 종목 삭제.
		//	m_grdRealAdd.DeleteRow(_ttoi(strIndex));

			//이탈된 종목의 시세를 받지 않을려면 사용한다.
		//	theApp.m_khOpenApi.SetRealRemove(m_strScrNo, sCode);

			strMsg.Format(_T("______________________::[%s][%s]종목이 이탈되었습니다."), sCode, strCodeName);
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!IsInRound())
	{
		if (REQ_DepositReceived())
		{
			SetDisableControls();
			m_bDoFinishProcess = FALSE;
			m_eProcessState = ePST_ROUND_START;
			SetTimer(ROUND_TIMER, 10, NULL);
			AddMessage(_T("라운드 시작 요청."));
		}
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
		SetWindowText("ABot");
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
	if (TRUE)//m_eProcessState <= ePST_ITEM_WAIT)
	{
		SetEnableControls();
		KillTimer(ROUND_TIMER);
		m_eProcessState = ePST_IDLE;
		AddMessage(_T("라운드 중단."));
		SetWindowText("ABot");
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


void CABotDlg::OnBnClickedButtonEditInround()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*
	if (IsInRound())
	{
		CString strBuf;
		GetDlgItemText(IDC_BUTTON_EDIT_INROUND, strBuf);
		if (strBuf == "비율 수정")
		{
			m_cmbSellOverThis.EnableWindow(TRUE);
			m_cmbSellUnderThis.EnableWindow(TRUE);
			SetDlgItemText(IDC_BUTTON_EDIT_INROUND, "비율 적용");
		}
		else if (strBuf == "비율 적용")
		{
			m_cmbSellOverThis.EnableWindow(FALSE);
			m_cmbSellUnderThis.EnableWindow(FALSE);
			SetDlgItemText(IDC_BUTTON_EDIT_INROUND, "비율 수정");

			m_cmbSellOverThis.GetWindowText(strBuf);
			double dSellOverThis = atof((LPSTR)(LPCSTR)strBuf);

			m_cmbSellUnderThis.GetWindowText(strBuf);
			double dSellUnderThis = atof((LPSTR)(LPCSTR)strBuf);

			if (m_dSellOverThis != dSellOverThis || m_dSellUnderThis != dSellUnderThis)
			{
				m_dSellOverThis = dSellOverThis;
				m_dSellUnderThis = dSellUnderThis;
				AddMessage(_T("_____::[정정]종목당 매도 Sell'OVER'This는 %f[퍼센트] 입니다."), m_dSellOverThis);
				AddMessage(_T("_____::[정정]종목당 매도 Sell'UNDER'This는 %f[퍼센트] 입니다."), m_dSellUnderThis);

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
		AfxMessageBox("계좌번호 10자를 입력 해 주세요~!");
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
	case ePST_ROUND_START:	//라운드 시작 상태.
		m_mapOrderCode.RemoveAll();
		m_mapItemCode.RemoveAll();
		m_mapUsedItemCode.RemoveAll();
		m_grdRealAdd.SetRowCount(1);
		m_nProcessRetryCount = 0;
		InitProcessCondition();
		AddMessage(_T("라운드::시작 합니다."));
		m_eProcessState = ePST_WAIT_TIME;
		break;

	case ePST_WAIT_TIME:	//라운드 시작 시작 시간을 기다리는 상태.
		m_nProcessRetryCount++;
		if (IsInRoundTime())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드::예수금 검색을 시작 합니다."));
			m_eProcessState = ePST_DR_SEARCH;
			break;
		}
		break;

	case ePST_DR_SEARCH:	//예수금 요청 상태.
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
			AddMessage(_T("라운드::예수금 조회에 연속 실패하여 프로세스를 중단합니다."));
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
			AddMessage(_T("라운드::예수금 %d[원], 라운드를 종료합니다."), m_lDepositReceived);
			m_eProcessState = ePST_ROUND_END;
			break;
		}
		else if (m_lDepositReceived > 0)
		{
			CString strBuf;
			m_cmbMaxTotalAmount.GetLBText(m_cmbMaxTotalAmount.GetCurSel(), strBuf);
			long lMaxTotalAmount = atol((LPSTR)(LPCSTR)strBuf) * 10000;	// 단위가 만원이므로 10000을 곱한다.

			m_cmbDpUseRate.GetLBText(m_cmbDpUseRate.GetCurSel(), strBuf);
			long nUseRate = atol((LPSTR)(LPCSTR)strBuf);

			m_lProcessDR = min(long(m_lDepositReceived*(nUseRate / 100.0)), lMaxTotalAmount);
			AddMessage(_T("라운드::운용 금액 총합은 %s[원] 입니다."), GetCurrencyString(m_lProcessDR));

			if (m_lProcessDR < m_lProcessItemDR)
			{
				m_nProcessRetryCount = 0;
				AddMessage(_T("라운드::전체 운용 금액 %s[원]이 종목 운용 금액(수수료 포함) %s[원]보다 작습니다. 라운드를 종료합니다."), GetCurrencyString(m_lProcessDR), GetCurrencyString(m_lProcessItemDR));
				m_eProcessState = ePST_ROUND_END;
				break;
			}
	
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드::종목 검색을 시작 합니다."));
			m_eProcessState = ePST_ITEM_SEARCH;
			break;
		}
		break;

	case ePST_ITEM_SEARCH:	//종목 검색 요청 상태.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//라운드 가능 시간 확인

		if (m_cmbCondtion.GetCount()<=0)
		{
			OnBnClickedButtonGetCondition();
			break;
		}

		if (REQ_ItemSearch())
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드::종목 요청 완료. 결과를 기다립니다."));
			m_eProcessState = ePST_ITEM_WAIT;
			break;
		}
	//	else
	//	{
	//		//검색 요청 실패시. 화면 번호도 바꿔보고,. 그래도 소용 없군.
	//		m_strScrNo.Format("%04d", rand() % 10000);
	//		Sleep(min(2000, m_nProcessRetryCount*10));
	//	}
		break;

	case ePST_ITEM_WAIT:	//종목 검색 결과 대기 상태.
		m_nProcessRetryCount++;
		if (!IsInRoundTime()) { m_eProcessState = ePST_ROUND_END; break; }	//라운드 가능 시간 확인

		if (m_ItemCount == -1)
		{
			break;
		}
		AddMessage(_T("라운드::[%d]개의 종목이 검색되었습니다."), m_ItemCount);
		bGoTrade = FALSE;
		if (m_ItemCount == 0)
		{
			AddMessage(_T("라운드::종목이 검색될때 거래를 시작합니다."));
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
			AddMessage(_T("라운드::종목 거래를 시작 합니다."));
			m_eProcessState = ePST_ITEM_TRADE;
			break;
		}
		break;

	case ePST_ITEM_TRADE:	//종목 거래 상태.
		m_nProcessRetryCount++;
		ProcessTradeAll();
		if (IsEndTrade() && 
			(m_nProcessItemCount <= m_ItemCount || !IsInRoundTime()) )
		{
			m_nProcessRetryCount = 0;
			AddMessage(_T("라운드::종목 거래가 모두 종료 되었습니다."));
			m_eProcessState = ePST_ROUND_END;
			break;
		}
		m_nProcessRetryCount--;
		break;

	case ePST_ROUND_END:	//라운드 마무리 상태.
		m_nProcessRetryCount++;
		AddMessage(_T("라운드::종료 합니다."));

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

	case ePST_IDLE:	//정지 상태.
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
	//	if (aItem.m_lQuantity != 0 && aItem.m_lQuantity - aItem.m_lBuyQuantity <= 0)
	//	{
	//		aItem.m_eitemState = eST_HOLDING;
	//		break;
	//	}
		if (aItem.m_ltryBuyCount == 0 && m_lProcessDR < m_lProcessItemDR)
		{
			//실시간 정보를 보여주는 그리드에서 지운다
			CString strIndex;
			if (strIndex.GetLength()>0)
			{
				m_grdRealAdd.DeleteRow(_ttoi(strIndex));
			}
			//실시간 정보를 보여주는 그리드의 순서에서 지운다
			m_mapJongCode.RemoveKey(aItem.m_strCode);

			//m_Item[MAX_ITEM_COUNT]에서의 item Index를 멥에서 지운다.
			m_mapItemCode.RemoveKey(aItem.m_strCode);

			//종목의 시세를 받지 않는다.
			theApp.m_khOpenApi.SetRealRemove(m_strScrNo, aItem.m_strCode);

			//다시 종목으로 검색될때 까지 안녕~
			AddMessage(_T("라운드::[%s][%s][%s], 잔여 운용 금액 %s[원]이 종목 운용 금액(수수료 포함) %s[원]보다 작습니다. 다시 관심 종목에 진입할때까지 거래를 하지 않습니다."), 
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), GetCurrencyString(m_lProcessDR), GetCurrencyString(m_lProcessItemDR));
			aItem.m_eitemState = eST_NONE;
			break;
		}

		if (REQ_ItemBuyOrder(aItem, bFromAllTrade))
		{
			AddMessage(_T("라운드::[%s][%s][%s],단가[%d],수량[%d], 매수가 시도 되었습니다."), 
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_WAITBUY;

			if (aItem.m_ltryBuyCount == 0)
			{
				m_lProcessDR -= m_lProcessItemDR;
				AddMessage(_T("라운드::운용 금액 총합은 %s[원]이 되었습니다."), GetCurrencyString(m_lProcessDR));
			}
			break;
		}
		break;

	case eST_WAITBUY:	//매수 체결 대기 상태.
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
				AddMessage(_T("라운드::[%s][%s][%s],단가[%d],수량[%d],재시도[%d], 거래를 포기 합니다."),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity, aItem.m_ltryBuyCount);
				aItem.m_eitemState = eST_BUYCANCEL;
				break;
			}

			AddMessage(_T("라운드::[%s][%s][%s],단가[%d],수량[%d], 매수가 재시도[%d] 되었습니다."),
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

	case eST_HOLDING:	//보유 상태.
		if (true)//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		{
			aItem.m_lsellPrice = CalcBuyAndSellPrice(aItem.BuyPrice(), m_dSellOverThis0, TRUE);
			aItem.m_lunderPrice = long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis0 / 100.);
			AddMessage(_T("라운드::[%s][%s][%s],목표상가[%d],목표하가[%d],수량[%d], 매도에 도전합니다."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lunderPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_TRYSELL;
		//	break;
		}
		/*
		else if (aItem.m_lcurPrice > long(aItem.BuyPrice() + aItem.BuyPrice()*m_dSellOverThis / 100.))
		{
			aItem.m_lsellPrice = aItem.m_lcurPrice;
			AddMessage(_T("라운드::[%s][%s][%s],현재가▲[%d],수량[%d], 매도를 시도합니다. 상승."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_TRYSELL;
		//	break;
		}
		else if (aItem.m_lcurPrice < long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis / 100.))
		{
			aItem.m_lsellPrice = aItem.m_lcurPrice;
			AddMessage(_T("라운드::[%s][%s][%s],현재가▼[%d],수량[%d], 매도을 시도합니다. 하락."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_TRYSELL;
		//	break;
		}
		else if (aItem.m_lholdTimeout>0 && aItem.m_lholdTime + long(GetTickCount()) > aItem.m_lholdTimeout)
		{
			aItem.m_lsellPrice = aItem.m_lcurPrice;
			AddMessage(_T("라운드::[%s][%s][%s],현재가~[%d],수량[%d], 매도을 시도합니다. 타임아웃."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_TRYSELL;
		//	break;
		}
		*/
		if (aItem.m_eitemState == eST_TRYSELL && aItem.m_lBuyQuantity != aItem.m_lQuantity)
		{
			AddMessage(_T("라운드::[%s][%s][%s],매도 수량 정정, 최초수량[%d]<==매수수량[%d]."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lQuantity, aItem.m_lBuyQuantity);
			aItem.m_lQuantity = aItem.m_lBuyQuantity;
		}

		if (aItem.m_eitemState != eST_TRYSELL)
		{
			break;
		}

	case eST_TRYSELL:	//매도 시도 상태.
		if (aItem.m_lQuantity != 0 && aItem.m_lQuantity - aItem.m_lSellQuantity <= 0)
		{
			aItem.m_eitemState = eST_TRADECLOSING;
			break;
		}
		if (REQ_ItemSellOrder(aItem, FALSE, bFromAllTrade))
		{
		//	aItem.m_ltrySellTimeout = 0;

			AddMessage(_T("라운드::[%s][%s][%s],단가[%d],수량[%d], 매도가 시도 되었습니다."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity - aItem.m_lSellQuantity);
			aItem.m_eitemState = eST_WAITSELL;
			break;
		}
		break;

	case eST_WAITSELL:	//매도 체결 대기 상태.
		if (aItem.m_lQuantity != 0 && aItem.m_lQuantity - aItem.m_lSellQuantity <= 0)
		{
			aItem.m_eitemState = eST_TRADECLOSING;
			break;
		}

		if (m_bDoSellItemMarketValueAtRoundEnd && !IsInRoundTime())
		{
			AddMessage(_T("라운드::[%s][%s][%s],현재가[%d],수량[%d], 라운드 시간 후 시장가 매도 옵션에 의해, 매도가 시장가로 시도합니다."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_SELLMARKETVALUE;
			break;
		}
		else if (aItem.m_lcurPrice <= aItem.m_lunderPrice)
		{
			AddMessage(_T("라운드::[%s][%s][%s],현재가▼[%d],수량[%d], 매도가 시장가로 시도합니다. 하락."),
				aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			aItem.m_eitemState = eST_SELLMARKETVALUE;
			break;
		}
		else if (aItem.m_ltrySellTimeout0 > 0 && long(GetTickCount()) > aItem.m_ltrySellTimeout0)
		{
			aItem.m_ltrySellTimeout0 = 0;
			long aunderPrice = long(aItem.BuyPrice() - aItem.BuyPrice()*m_dSellUnderThis1 / 100.);
			long asellPrice = CalcBuyAndSellPrice(aItem.BuyPrice(), m_dSellOverThis1, TRUE);
			
			//지금 가격이 손해가 아니면 현재가로 매도 시도한다.
			long lestimatedSellprice = long(aItem.m_lcurPrice*(1 - m_dSellTradeFee / 100.) - long(aItem.BuyPrice()*(m_dBuyTradeFee / 100.)));
			if (aItem.BuyPrice() >= lestimatedSellprice)
			{
				aItem.m_lsellPrice = aItem.m_lcurPrice;
				AddMessage(_T("라운드::[%s][%s][%s],현재가[%d],수량[%d], 매도 시간 타임 아웃#0, 현재가로 매도 시도합니다."),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			}

			if (aItem.m_lsellPrice != asellPrice || aItem.m_lunderPrice != aunderPrice)
			{
				aItem.m_lunderPrice = aunderPrice;
				AddMessage(_T("라운드::[%s][%s][%s],목표상가[%d],목표하가[%d],수량[%d], 매도 시간 타임 아웃#0, 상가/하가 변경 되었습니다."),
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

			//지금 가격이 손해가 아니면 현재가로 매도 시도한다.
			long lestimatedSellprice = long(aItem.m_lcurPrice*(1 - m_dSellTradeFee / 100.) - long(aItem.BuyPrice()*(m_dBuyTradeFee / 100.)));
			if (aItem.BuyPrice() >= lestimatedSellprice)
			{
				aItem.m_lsellPrice = aItem.m_lcurPrice;
				AddMessage(_T("라운드::[%s][%s][%s],현재가[%d],수량[%d], 매도 시간 타임 아웃#1, 현재가로 매도 시도합니다."),
					aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity);
			}

			if (aItem.m_lsellPrice != asellPrice || aItem.m_lunderPrice != aunderPrice)
			{
				aItem.m_lunderPrice = aunderPrice;
				AddMessage(_T("라운드::[%s][%s][%s],목표상가[%d],목표하가[%d],수량[%d], 매도 시간 타임 아웃#1, 상가/하가 변경 되었습니다."),
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
			AddMessage(_T("결산 =================================================================================================================="));
			AddMessage(_T("      [%s][%s],실 매입금[%s], 실 매도금[%s], 실현 이익[%s], 실 이익율[%4.2f]퍼센트 입니다."),
				aItem.m_strCode, aItem.m_strName, GetCurrencyString(lactBuyCost), GetCurrencyString(lactSellCost), GetCurrencyString(lactSellCost - lactBuyCost), double(lactSellCost - lactBuyCost) / double(lactBuyCost)*100.0);
			AddMessage(_T("결산 =================================================================================================================="));

			m_lProcessDR += m_lProcessItemDR;

			m_lProcessDR -= lactBuyCost;
			m_lProcessDR += lactSellCost;
			AddMessage(_T("운용 금액 총합은 %s[원]이 되었습니다."), GetCurrencyString(m_lProcessDR));
		}
		aItem.m_eitemState = eST_TRADEDONE;
		break;

	case eST_TRADEDONE:	//거래 완료 상태.
		break;
	}
}

BOOL CABotDlg::REQ_ItemBuyOrder(CABotItem &aItem, BOOL bFromAllTrade)
{
	CString strRQName = "주식주문";

	// 매매구분 취득(1:신규매수, 2:신규매도 3:매수취소, 4:매도취소, 5:매수정정, 6:매도정정)
	long lOrderType = 1;
//	if (aItem.m_ltryBuyCount > 0)
//	{
//		lOrderType = 5;	//5:매수정정
//	}

	// 거래구분 취득
	// 00:지정가, 03:시장가, 05:조건부지정가, 06:최유리지정가, 07:최우선지정가, 
	// 10:지정가IOC, 13:시장가IOC, 16:최유리IOC, 20:지정가FOK, 23:시장가FOK, 
	// 26:최유리FOK, 61:장개시전시간외, 62:시간외단일가매매, 81:장후시간외종가
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

	if (nextQuantity <= 0)	//구매 가능 수량이 0일 경우 예외 처리.
	{
		AddMessage(_T("BUY_ORDER::[%s],[%s][%s][%s],단가[%d],수량[%d], 거래를 포기 합니다."),
			GetOrderTypeString(lOrderType), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity);
		if (aItem.m_lBuyQuantity > 0)
		{
			//매수 취소.
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
		//매수 취소.
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

	AddMessage(_T("BUY_ORDER::[%s],[%s][%s][%s],단가[%d],수량[%d],거래구분[%s],원주문번호[%s], 매수 요청[%s] %s!"),
		GetOrderTypeString(lOrderType), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lbuyPrice, aItem.m_lQuantity - aItem.m_lBuyQuantity, strHogaGb, aItem.m_strBuyOrder, (bFromAllTrade ? "타이머" : "이벤트"), (lRet >= 0 ? "성공" : "실패"));

	return (lRet >= 0 ? TRUE : FALSE);
}

BOOL CABotDlg::REQ_ItemBuyCancel(CABotItem &aItem, BOOL bFromAllTrade)
{
	CString strRQName = "주식주문";

	// 매매구분 취득(1:신규매수, 2:신규매도 3:매수취소, 4:매도취소, 5:매수정정, 6:매도정정)
	long lCANCELOrder = 3; //3:매수취소

	// 거래구분 취득
	// 00:지정가, 03:시장가, 05:조건부지정가, 06:최유리지정가, 07:최우선지정가, 
	// 10:지정가IOC, 13:시장가IOC, 16:최유리IOC, 20:지정가FOK, 23:시장가FOK, 
	// 26:최유리FOK, 61:장개시전시간외, 62:시간외단일가매매, 81:장후시간외종가
	CString strHogaGb = "00";

	long lCANCELQuantity = aItem.m_lQuantity - aItem.m_lBuyQuantity;
	long lCANCELPrice = 0;
	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lCANCELOrder, aItem.m_strCode,
		lCANCELQuantity, lCANCELPrice, strHogaGb, aItem.m_strBuyOrder);

	AddMessage(_T("BUY_CANCEL::[%s],[%s][%s][%s],단가[%d],수량[%d],거래구분[%s],원주문번호[%s], 매수 취소 요청[%s] %s!"),
		GetOrderTypeString(lCANCELOrder), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), lCANCELPrice, lCANCELQuantity, strHogaGb, aItem.m_strBuyOrder, (bFromAllTrade ? "타이머" : "이벤트"), (lRet >= 0 ? "성공" : "실패"));

	return (lRet >= 0 ? TRUE : FALSE);
}

BOOL CABotDlg::REQ_ItemSellOrder(CABotItem &aItem, BOOL bMarketVale, BOOL bFromAllTrade)
{
	CString strRQName = "주식주문";

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

	if (bMarketVale)
	{
		if (aItem.m_strSellOrder.GetLength() > 0)
		{
			if (!REQ_ItemSellCancel(aItem, bFromAllTrade))
			{
				return FALSE;
			}
		}

		lOrderType = 2;					//취소 후, 신규 매도로 해야 한다.
		strHogaGb = "03";				//03:시장가,

		aItem.m_strSellOrder = "";		//원주문 번호 제거.
		aItem.m_lsellPrice = 0;			//시장가는 가격이 0이다.
		aItem.m_ltrySellTimeout0 = 0;	//타임 아웃 없앤다.
		aItem.m_ltrySellTimeout1 = 0;	//타임 아웃 없앤다.
	}
	else
	{
		ASSERT(aItem.m_lsellPrice != 0);
		if (aItem.m_lsellPrice == 0){ return FALSE; }
	}

	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lOrderType, aItem.m_strCode,
		aItem.m_lQuantity - aItem.m_lSellQuantity, aItem.m_lsellPrice, strHogaGb, aItem.m_strSellOrder);

	AddMessage(_T("SELL_ORDER::[%s],[%s][%s][%s],단가[%d],수량[%d],거래구분[%s],원주문번호[%s], 매도 요청[%s] %s!"),
		GetOrderTypeString(lOrderType), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity - aItem.m_lSellQuantity, strHogaGb, aItem.m_strSellOrder, (bFromAllTrade ? "타이머" : "이벤트"), (lRet >= 0 ? "성공" : "실패"));

	if (lRet >= 0)
	{
		aItem.m_strSellOrder = "";
	}
	return (lRet >= 0 ? TRUE : FALSE);
}

BOOL CABotDlg::REQ_ItemSellCancel(CABotItem &aItem, BOOL bFromAllTrade)
{
	CString strRQName = "주식주문";

	// 매매구분 취득(1:신규매수, 2:신규매도 3:매수취소, 4:매도취소, 5:매수정정, 6:매도정정)
	long lCANCELOrder = 4;

	// 거래구분 취득
	// 00:지정가, 03:시장가, 05:조건부지정가, 06:최유리지정가, 07:최우선지정가, 
	// 10:지정가IOC, 13:시장가IOC, 16:최유리IOC, 20:지정가FOK, 23:시장가FOK, 
	// 26:최유리FOK, 61:장개시전시간외, 62:시간외단일가매매, 81:장후시간외종가
	CString strHogaGb = "00";

	long lRet = theApp.m_khOpenApi.SendOrder(strRQName, m_strScrNo, m_strAccNo,
		lCANCELOrder, aItem.m_strCode,
		aItem.m_lQuantity - aItem.m_lSellQuantity, aItem.m_lsellPrice, strHogaGb, aItem.m_strSellOrder);

	AddMessage(_T("SELL_CANCEL::[%s],[%s][%s][%s],단가[%d],수량[%d],거래구분[%s],원주문번호[%s], 매도 취소 요청[%s] %s!"),
		GetOrderTypeString(lCANCELOrder), aItem.m_strCode, aItem.m_strName, aItem.GetStateString(), aItem.m_lsellPrice, aItem.m_lQuantity - aItem.m_lSellQuantity, strHogaGb, aItem.m_strSellOrder, (bFromAllTrade ? "타이머" : "이벤트"), (lRet >= 0 ? "성공" : "실패"));

	return (lRet >= 0 ? TRUE : FALSE);
}

CString CABotDlg::GetOrderTypeString(long lOrderType)
{
	// 매매구분 취득(1:신규매수, 2:신규매도 3:매수취소, 4:매도취소, 5:매수정정, 6:매도정정)

	if (lOrderType == 1)
		return "신규매수";
	if (lOrderType == 2)
		return "신규매도";
	if (lOrderType == 3)
		return "매수취소";
	if (lOrderType == 4)
		return "매도취소";
	if (lOrderType == 5)
		return "매수정정";
	if (lOrderType == 6)
		return "매도정정";

	CString strBuf;
	strBuf.Format("모름(%d)", lOrderType);
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

	theApp.m_khOpenApi.SetInputValue("계좌번호", m_strAccNo);
	theApp.m_khOpenApi.SetInputValue("비밀번호입력매체구분", _T("00"));
	theApp.m_khOpenApi.SetInputValue("조회구분", _T("1")); // 1:합산, 2:개별
	
	lRet = theApp.m_khOpenApi.CommRqData(_T("잔고"), _T("OPW00018"), 0, m_strScrNo);
	AddMessage("잔고 조회 요청 %s. [%d]", (lRet >= 0 ? "성공" : "실패"), lRet);
	if (lRet < 0) {
		return FALSE;
	}

	theApp.m_khOpenApi.SetInputValue("계좌번호", m_strAccNo);
	strBuff = startDay.Format("%Y%m%d");	theApp.m_khOpenApi.SetInputValue("시작일자", strBuff);
	strBuff = endDay.Format("%Y%m%d");		theApp.m_khOpenApi.SetInputValue("종료일자", strBuff);

	lRet = theApp.m_khOpenApi.CommRqData(_T("일자별실현손익요청"), _T("opt10074"), 0, m_strScrNo);
	AddMessage("일자별실현손익요청 %s. [%d]", (lRet >= 0 ? "성공" : "실패"), lRet);
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
	AddMessage(_T("총결산 ==============================================================================================================="));
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
				strMark = "▲__";
			}
			else if (lactSellCost < lactBuyCost)
			{
				strMark = "__▼";
			}

			if (lactBuyCost == 0) {	lactBuyCost = 1; }
			AddMessage(_T("       ,[%s][%3d],[%s][%s],실매입금[%d],실매도금[%d],실이익율[%4.2f]퍼센트 입니다."),
				strMark, i + 1, m_Item[i].m_strCode, m_Item[i].m_strName, lactBuyCost, lactSellCost, double(lactSellCost - lactBuyCost) / double(lactBuyCost)*100.0);
			laccBuyCost += lactBuyCost;
			laccSellCost += lactSellCost;
		}
	}
	AddMessage(_T("총결산 ==============================================================================================================="));

	AddMessage(_T("총결산   총매입금[%s],총매도금[%s],총이익금[%s],총이익율[%4.2f]퍼센트"),
		GetCurrencyString(laccBuyCost), GetCurrencyString(laccSellCost), GetCurrencyString(laccSellCost - laccBuyCost), double(laccSellCost - laccBuyCost) / double(laccBuyCost)*100.0);

	AddMessage(_T("총결산 ==============================================================================================================="));
}

