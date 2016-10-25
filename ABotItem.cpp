#include "stdafx.h"
#include "ABotItem.h"


CABotItem::CABotItem()
{
	Init();
}

void CABotItem::Init()
{
	m_eitemState = eST_NONE;
	m_strCode = _T("");
	m_strName = _T("");
	m_lcurPrice = 0;		//현재가. 실시간으로 검색되고 있다면.
	m_lbuyPrice = 0;		//매수가.
	m_lsellPrice = 0;		//매도가.
	m_lQuantity = 0;		//수량.
	m_lupperPrice = 0;		//매각 상한가. 이가격 이상으로 오르면 판다.
	m_lunderPrice = 0;		//매각 하한가. 이가격 이하로 떨어지면 판다.

	m_lfilterBuyAccumTime = 0;	//매수 필터, 데이터 누적 시간. clock.
	m_lfilterBuyTickcount = 0;	//매수 필터, 틱 수량.
	m_lfilterBuyYangcost = 0;	//매수 필터, 양거래금액.

	m_ltryBuyTimeout = 0;	//최종 매수 시도 포기 시각. clock.
	m_strBuyOrder = _T("");	//최종 매수 시도 주문 번호.
	m_lBuyCost = 0;			//실 매수에 사용된 누적 금액.
	m_lBuyQuantity = 0;		//실 매수된 수량.
	m_ltryBuyCount = 0;		//매수 시도 회수.
	m_strBuyTime = _T("");	//최초 매수 시각.

	m_lholdTime = 0;		//보유 시각. clock
	m_lholdTimeout = 0;		//보유후 매도까지 타임아웃. clock.

	m_ltrySellTimeout0 = 0;	//매도 시도 타임 아웃 0 시각. clock.
	m_ltrySellTimeout1 = 0;	//매도 시도 타임 아웃 1 시각. clock.

	m_strSellOrder = _T("");	//최종 매도 시도 주문 번호.
	m_lSellCost = 0;			//실 매도한 누적 금액.
	m_lSellQuantity = 0;		//실 매도된 수량.
	m_ltrySellCount = 0;		//매도 시도 회수.
	m_strSellTime = _T("");		//최초 매도 시각.
}


CABotItem::~CABotItem()
{
}


CString CABotItem::GetStateString()
{
	if (m_eitemState == eST_NONE)		//아무것도 아닌 상태.
		return "NONE";
	else if (m_eitemState == eST_TONONE)	//아무것도 아닌 상태로 되돌리기.
		return "TONONE";
	else if (m_eitemState == eST_ADDED)		//관심 종목에 추가 되었을때.
		return "ADDED";
	else if (m_eitemState == eST_SETFILTERBUY)	//매수 필터 설정.
		return "SETFILTERBUY";
	else if (m_eitemState == eST_WAITFILTERBUY)	//매수 필터 완료 대기.
		return "WAITFILTERBUY";
	else if (m_eitemState == eST_TRYBUY)		//매수 시도 상태.
		return "TRYBUY";
	else if (m_eitemState == eST_WAITBUY)	//매수 체결 대기 상태.
		return "WAITBUY";
	else if (m_eitemState == eST_BUYCANCEL)	//매수 중단 상태.
		return "BUYCANCEL";
	else if (m_eitemState == eST_WAITBUYCANCEL)	//매수 중단 대기 상태.
		return "WAITBUYCANCEL";
	else if (m_eitemState == eST_HOLDING)	//보유 상태.
		return "HOLDING";
	else if (m_eitemState == eST_TRYSELL)	//매도 시도 상태.
		return "TRYSELL";
	else if (m_eitemState == eST_WAITSELL)	//매도 체결 대기 상태.
		return "WAITSELL";
	else if (m_eitemState == eST_SELLCANCEL)	//매도 중단 상태.
		return "SELLCANCEL";
	else if (m_eitemState == eST_WAITSELLCANCEL)	//매도 중단 대기 상태.
		return "WAITSELLCANCEL";
	else if (m_eitemState == eST_SELLMARKETVALUE)		//시장가로 매도 요청
		return "SELLMARKETVALUE";
	else if (m_eitemState == eST_WAITSELLMARKETVALUE)	//시장가로 매도 요청후 대기
		return "WAITSELLMARKETVALUE";
	else if (m_eitemState == eST_TRADECLOSING)	//거래 정산 상태.
		return "TRADECLOSING";
	else if (m_eitemState == eST_TRADEDONE)		//거래 완료 상태.
		return "TRADEDONE";

	return "UNKNOWN";
}
