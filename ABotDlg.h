
// ABotDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "GridCtrl\GridCtrl.h"

typedef struct
{
	CString	strKey;				// ��ȸ Ű
	CString	strRealKey;			// ���� Ű
	int		nRow;				// �׸��� ��
	int		nCol;				// �׸��� ��
	int		nDataType;			// ����Ÿ Ÿ��(0:�⺻����, 1:����, 2:�ð�, 3:���� ����, 4:���� ����(0ǥ��), 5:����ȣ)
	BOOL	bTextColor;			// ���ڿ� �� ����(���, �϶���)
	UINT	nAlign;				// ���ڿ� ����(DT_LEFT, DT_CENTER, DT_RIGHT)
	CString	strBeforeData;		// ���ڿ� �� ���� �ֱ�
	CString	strAfterData;		// ���ڿ� �� ���� �ֱ�
} stGRID;


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
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	void LoadSystemFile();
	void SaveSystemFile();
	void InitComboBox();
	void InitFIDName();
	void InitRealAddGrid();
	void SetDataRealAddGrid(CStringArray &arrData, CString strRealType/* = ""*/);
	BOOL AddMessage(char * i_cMsg, ...);
	BOOL AddMessage(CString i_strMsg);

public:
	CString				m_strConfigFile;		//ȯ�� ���� �̸�.
	CString				m_strScrNo;				//��ũ�� ��ȣ
	CString				m_strConditionName;		//���� ���Ǹ�
	int					m_nConditionIndex;		//���� ���Ǹ� �ε���
	CMapStringToString	m_mapFIDName;
	CMapStringToString	m_mapJongCode;			// ���� ��� ����
	CMapStringToString	m_mapNameList;			// ���ǰ˻�����Ʈ
	long				m_lDepositReceived;		// ������ [��]

	CCellID				m_cellSelect;		// ���� �׸��� ���� ��
	CGridCtrl			m_grdRealAdd;		// ���� �׸���
	CComboBox			m_cmbCon;			// �˻� ���� �޺�
	CComboBox			m_cmbRoundCount;	// �ݺ� ȸ��
	CComboBox			m_cmbStartHour;		// ���� �ð��� ��
	CComboBox			m_cmbStartMin;		// ���� �ð��� ��
	CComboBox			m_cmbEndHour;		// ���� �ð��� ��
	CComboBox			m_cmbEndMin;		// ���� �ð��� ��
	CComboBox			m_cmbDpUseRate;		// �ż��� �������� �ִ� ��� �ۼ�Ʈ. [%]
	CComboBox			m_cmbMaxAmount;		// ����� �ִ� ���� ��� �ݾ� [����]
	CComboBox			m_cmbBuyMethod;		// �ż� ���, ���簡, ���尡. ����..�ۼ�Ʈ ����. [���簡, ���尡]
	CComboBox			m_cmbBuyTimeOut;	// �ż� ü�� ��� Ÿ�� �ƿ�. [��]
	CComboBox			m_cmbBuyRetry;		// �ż� ���н� ��õ� ȸ��
	CComboBox			m_cmbSellOverThis;	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]
	CComboBox			m_cmbSellUnderThis;	// ������ ���� ���簡��, �� �ۼ�Ʈ ���� �������� �Ǵ�. [%]

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
};
