
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
	long	nRow;				// 그리드 행
	long	nCol;				// 그리드 열
	long	nDataType;			// 데이타 타입(0:기본문자, 1:일자, 2:시간, 3:콤파 숫자, 4:콤파 숫자(0표시), 5:대비기호)
	BOOL	bTextColor;			// 문자열 색 변경(상승, 하락색)
	UINT	nAlign;				// 문자열 정렬(DT_LEFT, DT_CENTER, DT_RIGHT)
	CString	strBeforeData;		// 문자열 앞 문자 넣기
	CString	strAfterData;		// 문자열 뒤 문자 넣기
} stGRID;

#define SafeDelete(A) {if(A){delete A; A=NULL;}}
#define SafeDeleteArray(A) {if(A) {delete[] A; A=NULL;}}
#define IsSafeWindow(pwnd) ((pwnd && (pwnd)->GetSafeHwnd()) ? TRUE : FALSE)

#define MAX_ITEM_COUNT	(1000)

#endif