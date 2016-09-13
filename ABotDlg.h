
// ABotDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "ABotItem.h"
#include "GridCtrl\GridCtrl.h"
#include "AbotDefine.h"

enum eProcessState
{
	ePST_IDLE,			//정지 상태.
	ePST_ROUND_START,	//라운드 시작 상태.
	ePST_WAIT_TIME,		//라운드 시작 시작 시간을 기다리는 상태.
	ePST_DR_SEARCH,		//예수금 요청 상태. (DR=Deposite Received=예수금)
	ePST_DR_WAIT,		//예수금 결과 대기 상태.
	ePST_ITEM_SEARCH,	//종목 검색 요청 상태.
	ePST_ITEM_WAIT,		//종목 검색 결과 대기 상태.
	ePST_ITEM_TRADE,	//종목 거래 상태.
	ePST_ROUND_END,		//라운드 1회 종료 상태.
	ePST_KILL_PROC,		//프로세스 종료 시키기.
};

typedef struct
{
	CString	strKey;				// 조회 키
	CString	strRealKey;			// 리얼 키
	int		nRow;				// 그리드 행
	int		nCol;				// 그리드 열
	int		nDataType;			// 데이타 타입(0:기본문자, 1:일자, 2:시간, 3:콤파 숫자, 4:콤파 숫자(0표시), 5:대비기호)
	BOOL	bTextColor;			// 문자열 색 변경(상승, 하락색)
	UINT	nAlign;				// 문자열 정렬(DT_LEFT, DT_CENTER, DT_RIGHT)
	CString	strBeforeData;		// 문자열 앞 문자 넣기
	CString	strAfterData;		// 문자열 뒤 문자 넣기
} stGRID;


// CABotDlg 대화 상자
class CABotDlg : public CDialogEx
{
// 생성입니다.
public:
	CABotDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	BOOL IsError(long lErrCode, CString &strMsg);
	void LoadSystemFile();
	void SaveSystemFile();
	void InitComboBox();
	void InitFIDName();
	void InitRealAddGrid();
	void SetDataRealAddGrid(CStringArray &arrData, CString strRealType/* = ""*/);
	void SetGridHeight(long row, long height);
	void SetGridWidth(long col, long width);
	eWinVersion GetWindowsVersion();
	BOOL AddMessage(char * i_cMsg, ...);
	BOOL AddMessage(CString i_strMsg);

	void ProcessSequence();	// 프로세스 구동 함수.
	void ProcessTrade();	// 거래 구동 함수.
	
	BOOL IsInRoundTime();
	BOOL REQ_DepositReceived();
	BOOL REQ_ItemSearch();
	BOOL REQ_ItemRealReg();
	BOOL REQ_ItemBuyOrder(CABotItem &aItem);
	BOOL REQ_ItemSellOrder(CABotItem &aItem);	
	BOOL IsEndTrade();

	void SetEnableControls() { SetControls(TRUE); };
	void SetDisableControls() { SetControls(FALSE); };
	void SetControls(BOOL bEnable);
	void LoadProcessCondition();

public:
	eProcessState		m_eProcessState;		//프로세스 상태.
	BOOL				m_bDoFinishProcess;		//프로세스 종료 요청.
	int					m_nRoundCount;			//라운드 수.
	int					m_nProcessRetryCount;	//프로세스 재시도 회수.
	long				m_lProcessDR;			//프로세스에서 사용할 예수금 [원]
	long				m_lProcessItemDR;		//종목당 사용할수 있는 예수금 [원]
	long				m_lItemBuyTimeout;		//종목 구매 타임 아웃 [clock]
	long				m_lItemBuyTryCount;		//종목 구매 시도 회수.

	long				m_lItemHoldTimeout;		//종목 보유 타임 아웃 [clock]

	double				m_dSellOverThis;		// 구매후 종목 현재가가, 이 퍼센트 보다 높아지면 판다. [%]
	double				m_dSellOverThis2;		// 구매후 m_cmbSellOverThis 조건을 트리거후, 이 퍼센트 보다 낮아!!!!!지면 시장가로 판다. [%]
	double				m_dSellUnderThis;		// 구매후 종목 현재가가, 이 퍼센트 보다 낮아지면 판다. [%]
	double				m_dSellUnderThis2;		// 구매후 m_cmbSellUnderThis 조건을 트리거후, 이 퍼센트 보다 낮아!!!!!!지면 시장가로 판다. [%]

	CABotItem			m_Item[100];			//종목.
	int					m_ItemCount;			//종목수.
	CMap<CString, LPCSTR, int, int>		m_ItemCodeMap;			// CodeString => m_Item's index;
	CMap<CString, LPCSTR, int, int>		m_OrderCodeMap;			// CodeString => m_Item's index;

	CString				m_strConfigFile;		//환경 파일 이름.
	CString				m_strLogFolderName;		//로그 파일 경로.
	CString				m_strScrNo;				//스크린 번호
	CString				m_strAccNo;				//계좌 번호
	CString				m_strConditionName;		//현재 조건명
	int					m_nConditionIndex;		//현재 조건명 인덱스
	CMapStringToString	m_mapFIDName;
	CMapStringToString	m_mapJongCode;			// 리얼 등록 종목
	CMapStringToString	m_mapNameList;			// 조건검색명리스트
	long				m_lDepositReceived;		// 예수금 [원]

	CCellID				m_cellSelect;		// 종목 그리드 선택 행
	CGridCtrl			m_grdRealAdd;		// 종목 그리드
	CComboBox			m_cmbCon;			// 검색 조건 콤보
	CComboBox			m_cmbRoundCount;	// 반복 회수
	CComboBox			m_cmbStartHour;		// 시작 시각의 시
	CComboBox			m_cmbStartMin;		// 시작 시각의 분
	CComboBox			m_cmbEndHour;		// 종료 시각의 시
	CComboBox			m_cmbEndMin;		// 종료 시각의 분
	CComboBox			m_cmbDpUseRate;		// 매수시 예수금의 최대 사용 퍼센트. [%]
	CComboBox			m_cmbMaxTotalAmount;// 매수시 예수금의 최대 사용 금액. [만원]
	CComboBox			m_cmbMaxAmount;		// 종목당 최대 투자 허용 금액 [만원]
	CComboBox			m_cmbBuyMethod;		// 매수 방법, 현재가, 시장가. 향후..퍼센트 지정. [현재가, 시장가]
	CComboBox			m_cmbBuyTimeOut;	// 매수 체결 대기 타임 아웃. [초]
	CComboBox			m_cmbBuyRetry;		// 매수 실패시 재시도 회수
	CComboBox			m_cmbHoldTimeOut;	// 구매후 타임아웃 동안 대기후 현재가 매도(매도 될때까지). [초]
	CComboBox			m_cmbSellOverThis;	// 구매후 종목 현재가가, 이 퍼센트 보다 높아지면 판다. [%]
	CComboBox			m_cmbSellOverThis2;	// 구매후 m_cmbSellOverThis 조건을 트리거후, 이 퍼센트 보다 낮아!!!!!지면 시장가로 판다. [%]
	CComboBox			m_cmbSellUnderThis;	// 구매후 종목 현재가가, 이 퍼센트 보다 낮아지면 판다. [%]
	CComboBox			m_cmbSellUnderThis2;// 구매후 m_cmbSellUnderThis 조건을 트리거후, 이 퍼센트 보다 낮아!!!!!!지면 시장가로 판다. [%]

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedShowMessage();
	afx_msg void OnBnClickedBalanceQuery();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonGetfilter();
	afx_msg void OnBnClickedButtonGettarget();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSaveconfig();
	DECLARE_EVENTSINK_MAP()
	void OnEventConnect(long nErrCode);
	void OnReceiveErrorMsg(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	void OnReceiveChejanData(LPCTSTR sGubun, long nItemCnt, LPCTSTR sFIdList);
	void OnReceiveRealData(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData);
	void OnReceiveTrData(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg);
	void OnReceiveTrCondition(LPCTSTR sScrNo, LPCTSTR strCodeList, LPCTSTR strConditionName, long nConditionIndex, long nNext);
	afx_msg void OnBnClickedButtonStartRound();
	afx_msg void OnBnClickedButtonFinishRound();
	afx_msg void OnBnClickedButtonStopRound();
	afx_msg void OnBnClickedButtonSellAllCurCost();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonRegtarget();
};
