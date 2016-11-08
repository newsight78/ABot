
// ABotDlg.h : ��� ����
//

#pragma once

//#define USE_THREAD	//�����带 ���ǰ�(�ƴϸ� Ÿ�̸Ӹ� ����.)
//#define CRITICAL_LOG	//Critical section ���� �α� ���⳪?
//#define LICENSE_LOG		//���̼��� �α� ���⳪?

#include "afxwin.h"
#include "ABotItem.h"
#include "GridCtrl\GridCtrl.h"
#include "AbotDefine.h"

#ifdef USE_THREAD
#include "AflThread.h"
#endif//USE_THREAD

// CABotDlg ��ȭ ����
class CABotDlg : public CDialogEx
#ifdef USE_THREAD 
				, public CAflThread
#endif//USE_THREAD
{
// �����Դϴ�.
public:
	CABotDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	CRITICAL_SECTION m_criticalItemProcess;
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	BOOL IsError(long lErrCode, CString &strMsg);
	void LoadSystemFile();
	void SaveSystemFile();
	BOOL InitLogData();
	void InitComboBox();
	void InitFIDName();
	void InitRealAddGrid();
	void InitBuyItemGrid();
	void SetDataRealAddGrid(CStringArray &arrData, CString strRealType/* = ""*/);
	void SetGridHeight(CGridCtrl& gridCtrl, long row, long height);
	void SetGridWidth(CGridCtrl& gridCtrl, long col, long width);
	BOOL AddMessage(char * i_cMsg, ...);
	BOOL AddMessage(CString i_strMsg);

	void ProcessSequence();				// ���μ��� ���� �Լ�.
	void ProcessTradeAll();				// ����� ���� �ŷ� ���� �Լ�.
	void ProcessTradeItem(int nItemId, BOOL bFromAllTrade=FALSE);	// Ư�� ���� �ŷ� ���� �Լ�.

	BOOL IsInRound()	{ return (m_eProcessState != ePST_IDLE?TRUE:FALSE); };
	BOOL IsInRoundTime();
	BOOL REQ_DepositReceived();
	BOOL REQ_ItemSearch();
	BOOL REQ_ItemRealReg();
	BOOL REQ_ItemBuyOrder(CABotItem &aItem, BOOL bFromAllTrade);
	BOOL REQ_ItemBuyCancel(CABotItem &aItem, BOOL bFromAllTrade);
	BOOL REQ_ItemSellOrder(CABotItem &aItem, BOOL bMarketVale, BOOL bFromAllTrade);
	BOOL REQ_ItemSellCancel(CABotItem &aItem, BOOL bFromAllTrade);
	BOOL REQ_BalanceInfo();
	BOOL REQ_DateInfo();
	BOOL IsEndTrade();
	void ReportAllTrade();

	void SetEnableControls() { SetControls(TRUE); };
	void SetDisableControls() { SetControls(FALSE); };
	void SetControls(BOOL bEnable);
	void InitProcessCondition();

	BOOL getAccountData();
	CString GetOrderTypeString(long lOrderType);

#ifdef USE_THREAD
protected:
	volatile BOOL bDoThreadRun;

public:
	virtual void ThreadEntry(void);
	virtual void ThreadExit(void);
	virtual void ThreadRun(void);

	void ThreadEnd(void);
#endif//USE_THREAD

public:
	BOOL				m_bTradeAllowed;		//Ʈ���̵尡 ����Ǿ� �ִ°�?
	eProcessState		m_eProcessState;		//���μ��� ����.
	BOOL				m_bDoFinishProcess;		//���μ��� ���� ��û.
	long				m_nProcessItemCount;	//���忡�� ����� �ִ� �����.
	long				m_nProcessRetryCount;	//���μ��� ��õ� ȸ��.
	long				m_lProcessDR;			//���μ������� ����� ������ [��]
	long				m_lProcessItemDR;		//����� ����Ҽ� �ִ� ������ [��]
	long				m_lItemBuyTimeout;		//���� ���� Ÿ�� �ƿ� [clock]
	long				m_lItemBuyTryCount;		//���� ���� �õ� ȸ��.
	long				m_lItemHoldTimeout;		//���� ���� Ÿ�� �ƿ� [��]

	long				m_lItemSellTimeout0;	// ���� �ŵ� Ÿ�� �ƿ� 0 [��]
	long				m_lItemSellTimeout1;	// ���� �ŵ� Ÿ�� �ƿ� 1 [��]
	double				m_dSellOverThis0;		// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	double				m_dSellUnderThis0;		// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	double				m_dSellOverThis1;		// ������ m_lItemSellTimeout_0 �� ������ ���ʹ�, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	double				m_dSellUnderThis1;		// ������ m_lItemSellTimeout_0 �� ������ ���ʹ�, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	double				m_dSellOverThis2;		// ������ m_lItemSellTimeout_0+m_lItemSellTimeout_1 �� ������ ���ʹ�, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	double				m_dSellUnderThis2;		// ������ m_lItemSellTimeout_0+m_lItemSellTimeout_1 �� ������ ���ʹ�, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]

	double				m_dBuyTradeFee;			// �ż� ������ �ż� ������ �� �ۼ�Ʈ ��ŭ ���ٴ�. [%]
	double				m_dSellTradeFee;		// �ŵ� ������ �ŵ� ������ �� �ۼ�Ʈ ��ŭ ���ٴ�. [%]

	long				m_lfilterBuyType;			//�ż� ���� ����, [0, ���� ����]. [1, ���� �̰�].
	double				m_dfilterBuyAccumTime;		//�ż� ����, ������ ���� �ð�. [��].
	double				m_dfilterBuyTickspeedLimit;	//�ż� ����, ƽ �ӵ� ����, �̰ͺ��� ������ �ż�����. [ƽ/��].
	double				m_dfilterBuyYangcostLimit;	//�ż� ����, ��ŷ��ݾ� ����, �̰ͺ��� ������ �ż�����. [����/��]

	CABotItem			m_Item[MAX_ITEM_COUNT];	//����.
	long				m_ItemCount;			//�����.
	CABotItem			m_TradeDoneItem[MAX_ITEM_COUNT];	//�ŷ� �Ϸ�� ����.
	long				m_TradeDoneItemCount;				//�ŷ� �Ϸ�� �����.
	CMap<CString, LPCSTR, long, long>	m_mapItemCode;			// CodeString => m_Item's index;
	CMap<CString, LPCSTR, long, long>	m_mapOrderCode;			// CodeString => m_Item's index;
	CMap<CString, LPCSTR, long, long>	m_mapUsedItemCode;		// CodeString => m_Item's index;
	
	CString				m_strConfigFile;		//ȯ�� ���� �̸�.
	CString				m_strLogFolderName;		//�α� ���� ���.
	CString				m_strScrNo;				//��ũ�� ��ȣ
	CString				m_strAccNo;				//���� ��ȣ
	CString				m_strConditionName;		//���� ���Ǹ�
	long				m_nConditionIndex;		//���� ���Ǹ� �ε���
	CMapStringToString	m_mapFIDName;
	CMapStringToString	m_mapJongCode;			// ���� ��� ����
	CMapStringToString	m_mapNameList;			// ���ǰ˻�����Ʈ
	long				m_lDepositReceived;		// ������ [��]

	CCellID				m_cellSelect;		// ���� �׸��� ���� ��
	CGridCtrl			m_grdRealAdd;		// ���� �׸���
	CGridCtrl			m_grdBuyItem;		// �ż��� ���� ���� �׸���
	CComboBox			m_cmbCondtion;		// �˻� ���� �޺�
	CComboBox			m_cmbItemCount;		// ��� ���� ����
	CComboBox			m_cmbStartHour;		// ���� �ð��� ��
	CComboBox			m_cmbStartMin;		// ���� �ð��� ��
	CComboBox			m_cmbEndHour;		// ���� �ð��� ��
	CComboBox			m_cmbEndMin;		// ���� �ð��� ��
	CComboBox			m_cmbDpUseRate;		// �ż��� �������� �ִ� ��� �ۼ�Ʈ. [%]
	CComboBox			m_cmbMaxTotalAmount;// �ż��� �������� �ִ� ��� �ݾ�. [����]
	CComboBox			m_cmbMaxAmount;		// ����� ��� �ݾ� [����]
	CComboBox			m_cmbBuyMethod;		// �ż� ���, ���簡, ���尡. ����..�ۼ�Ʈ ����. [���簡, ���尡]
	CComboBox			m_cmbBuyTimeOut;	// �ż� ü�� ��� Ÿ�� �ƿ�. [��]
	CComboBox			m_cmbBuyRetry;		// �ż� ���н� ��õ� ȸ��
	CComboBox			m_cmbHoldTimeOut;	// ������ Ÿ�Ӿƿ� ���� ����� ���簡 �ŵ�(�ŵ� �ɶ�����). [��]

	CComboBox			m_cmbSellTimeOut0;	// �ŵ� �õ���(==���� ��) ü�� ��� Ÿ�� �ƿ� 0. [��]
	CComboBox			m_cmbSellTimeOut1;	// �ŵ� �õ���(==���� ��) ü�� ��� Ÿ�� �ƿ� 1. [��]
	CComboBox			m_cmbSellOverThis0;	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%] >>> ��.
	CComboBox			m_cmbSellUnderThis0;// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%] >>> �ϰ�.
	CComboBox			m_cmbSellOverThis1;	// �ŵ� Ÿ�� �ƿ� 0 ��, �󰡸� �̰����� �ٲ۴�. [%]
	CComboBox			m_cmbSellUnderThis1;// �ŵ� Ÿ�� �ƿ� 0 ��, �ϰ��� �̰����� �ٲ۴�. [%]
	CComboBox			m_cmbSellOverThis2;	// �ŵ� Ÿ�� �ƿ� 1 ��, �󰡸� �̰����� �ٲ۴�. [%]
	CComboBox			m_cmbSellUnderThis2;// �ŵ� Ÿ�� �ƿ� 1 ��, �ϰ��� �̰����� �ٲ۴�. [%]

	BOOL				m_bDoSellItemMarketValueAtRoundEnd;		// ���� ���� ������ ��ä�� ������ ���尡�� �ȱ�.
	CButton				m_checkDoSellItemMarketValueAtRoundEnd;	// ���� ���� ������ ��ä�� ������ ���尡�� �ȱ�.

	BOOL				m_bDoReuseItemAfterTradeDone;		// �ŷ��ߴ� ���� �ٽ� �˻��Ǹ� �ŷ��� �ϱ�.
	CButton				m_checkDoReuseItemAfterTradeDone;	// �ŷ��ߴ� ���� �ٽ� �˻��Ǹ� �ŷ��� �ϱ�.

public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedShowMessage();
	afx_msg void OnBnClickedBalanceQuery();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonGetCondition();
	afx_msg void OnBnClickedButtonGetCondItems();
	afx_msg void OnBnClickedButtonRegItems();
	afx_msg void OnBnClickedButtonGetBalance();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSaveconfig();
	afx_msg void OnBnClickedButtonStartRound();
	afx_msg void OnBnClickedButtonFinishRound();
	afx_msg void OnBnClickedButtonStopRound();
	afx_msg void OnBnClickedButtonSellAllCurCost();
	afx_msg void OnBnClickedButtonDebugTest();
	afx_msg void OnBnClickedButtonEditInround();
	DECLARE_EVENTSINK_MAP()
	void OnEventConnect(long nErrCode);
	void OnReceiveErrorMsg(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	void OnReceiveChejanData(LPCTSTR sGubun, long nItemCnt, LPCTSTR sFIdList);
	void OnReceiveRealData(LPCTSTR sJongmokCode, LPCTSTR sRealType, LPCTSTR sRealData);
	void OnReceiveTrData(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg);
	void OnReceiveTrCondition(LPCTSTR sScrNo, LPCTSTR strCodeList, LPCTSTR strConditionName, long nConditionIndex, long nNext);
	void OnReceiveRealCondition(LPCTSTR sTrCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
