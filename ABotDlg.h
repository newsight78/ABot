
// ABotDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "ABotItem.h"
#include "GridCtrl\GridCtrl.h"
#include "AbotDefine.h"

// CABotDlg ��ȭ ����
class CABotDlg : public CDialogEx
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
	CCriticalSection m_criticalItemProcess;
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
	BOOL REQ_ItemBuyOrder(CABotItem &aItem);
	BOOL REQ_ItemBuyCancle(CABotItem &aItem);
	BOOL REQ_ItemSellOrder(CABotItem &aItem, BOOL bMarketVale, BOOL bFromAllTrade);
	BOOL REQ_ItemSellCancle(CABotItem &aItem);
	BOOL REQ_BalanceInfo();
	BOOL IsEndTrade();
	void ReportAllTrade();

	void SetEnableControls() { SetControls(TRUE); };
	void SetDisableControls() { SetControls(FALSE); };
	void SetControls(BOOL bEnable);
	void InitProcessCondition();

	BOOL getAccountData();
	CString GetOrderTypeString(long lOrderType);

public:
	eProcessState		m_eProcessState;		//���μ��� ����.
	BOOL				m_bDoFinishProcess;		//���μ��� ���� ��û.
	long				m_nRoundCount;			//���� ��.
	long				m_nProcessItemCount;	//���� �� ����� �ִ� Item ��.	//��а� �ִ� ���� ���� ����Ѵ�.
	long				m_nProcessRetryCount;	//���μ��� ��õ� ȸ��.
	long				m_lProcessDR;			//���μ������� ����� ������ [��]
	long				m_lProcessItemDR;		//����� ����Ҽ� �ִ� ������ [��]
	long				m_lItemBuyTimeout;		//���� ���� Ÿ�� �ƿ� [clock]
	long				m_lItemBuyTryCount;		//���� ���� �õ� ȸ��.
	long				m_lItemHoldTimeout;		//���� ���� Ÿ�� �ƿ� [��]

	double				m_dSellOverThis;		// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	double				m_dSellOverThis2;		// ������ m_cmbSellOverThis ������ Ʈ������, �� �ۼ�Ʈ ���� ����!!!!!���� ���尡�� �Ǵ�. [%]
	double				m_dSellUnderThis;		// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	double				m_dSellUnderThis2;		// ������ m_cmbSellUnderThis ������ Ʈ������, �� �ۼ�Ʈ ���� ����!!!!!!���� ���尡�� �Ǵ�. [%]

	double				m_dBuyTradeFee;			// �ż� ������ �ż� ������ �� �ۼ�Ʈ ��ŭ ���ٴ�. [%]
	double				m_dSellTradeFee;		// �ŵ� ������ �ŵ� ������ �� �ۼ�Ʈ ��ŭ ���ٴ�. [%]

	CABotItem			m_Item[1000];			//����.
	long				m_ItemCount;			//�����.
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
	CComboBox			m_cmbSellOverThis;	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	CComboBox			m_cmbSellOverThis2;	// ������ m_cmbSellOverThis ������ Ʈ������, �� �ۼ�Ʈ ���� ����!!!!!���� ���尡�� �Ǵ�. [%]
	CComboBox			m_cmbSellUnderThis;	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	CComboBox			m_cmbSellUnderThis2;// ������ m_cmbSellUnderThis ������ Ʈ������, �� �ۼ�Ʈ ���� ����!!!!!!���� ���尡�� �Ǵ�. [%]

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
	afx_msg void OnBnClickedButtonDebugTest();
	void OnReceiveRealCondition(LPCTSTR sTrCode, LPCTSTR strType, LPCTSTR strConditionName, LPCTSTR strConditionIndex);
};
