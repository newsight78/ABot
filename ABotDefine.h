
#ifndef _ABOTDEFINE_
#define _ABOTDEFINE_

enum eWinVersion
{
	eNotSupportedVersion,
	eWindowsXP,
	eWindows_Vista,
	eWindows_7,
	eWindows_8,
	eWindows_8_1,
	eWindows_10,
	eWinVerMax,
};

enum eProcessState
{
	ePST_IDLE,			//���� ����.
	ePST_ROUND_START,	//���� ���� ����.
	ePST_WAIT_TIME,		//���� ���� ���� �ð��� ��ٸ��� ����.
	ePST_DR_SEARCH,		//������ ��û ����. (DR=Deposite Received=������)
	ePST_DR_WAIT,		//������ ��� ��� ����.
	ePST_ITEM_SEARCH,	//���� �˻� ��û ����.
	ePST_ITEM_WAIT,		//���� �˻� ��� ��� ����.
	ePST_ITEM_TRADE,	//���� �ŷ� ����.
	ePST_ROUND_END,		//���� 1ȸ ���� ����.
	ePST_KILL_PROC,		//���μ��� ���� ��Ű��.
};

typedef struct
{
	CString	strKey;				// ��ȸ Ű
	CString	strRealKey;			// ���� Ű
	long	nRow;				// �׸��� ��
	long	nCol;				// �׸��� ��
	long	nDataType;			// ����Ÿ Ÿ��(0:�⺻����, 1:����, 2:�ð�, 3:���� ����, 4:���� ����(0ǥ��), 5:����ȣ)
	BOOL	bTextColor;			// ���ڿ� �� ����(���, �϶���)
	UINT	nAlign;				// ���ڿ� ����(DT_LEFT, DT_CENTER, DT_RIGHT)
	CString	strBeforeData;		// ���ڿ� �� ���� �ֱ�
	CString	strAfterData;		// ���ڿ� �� ���� �ֱ�
} stGRID;

#define SafeDelete(A) {if(A){delete A; A=NULL;}}
#define SafeDeleteArray(A) {if(A) {delete[] A; A=NULL;}}
#define IsSafeWindow(pwnd) ((pwnd && (pwnd)->GetSafeHwnd()) ? TRUE : FALSE)

#define MAX_ITEM_COUNT	(1000)

#endif