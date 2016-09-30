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
	m_lsellPrice = 0;		//매도가
	m_lQuantity = 0;		//수량.
	m_lupperPrice = 0;		//매각 상한가. 이가격 이상으로 오르면 판다.
	m_lunderPrice = 0;		//매각 하한가. 이가격 이하로 떨어지면 판다.

	m_ltryBuyTimeout = 0;	//최종 구매 시도 포기 시각. clock.
	m_strBuyOrder = _T("");	//최종 구매 시도 주문 번호.
	m_lBuyCost = 0;			//실 구매에 사용된 누적 금액.
	m_lBuyQuantity = 0;		//실 구매된 수량.
	m_ltryBuyCount = 0;		//구매 시도 회수.

	m_lholdTime = 0;		//보유 시각. clock
	m_lholdTimeout = 0;		//보유후 매도까지 타임아웃. clock.

	m_ltrySellTimeout = 0;	//최종 매도 시도 포기 시각. clock.
	m_strSellOrder = _T("");	//최종 매도 시도 주문 번호.
	m_lSellCost = 0;			//실 매도한 누적 금액.
	m_lSellQuantity = 0;		//실 매도된 수량.
	m_ltrySellCount = 0;		//매도 시도 회수.
}


CABotItem::~CABotItem()
{
}


CString
CABotItem::GetStateString()
{
	if (m_eitemState == eST_NONE)		//아무것도 아닌 상태.
		return "NONE";
	if (m_eitemState == eST_ADDED)		//관심 종목에 추가 되었을때.
		return "ADDED";
	if (m_eitemState == eST_TRYBUY)		//매수 시도 상태.
		return "TRYBUY";
	if (m_eitemState == eST_WAITBUY)	//매수 체결 대기 상태.
		return "WAITBUY";
	if (m_eitemState == eST_BUYCANCLE)	//매수 중단 상태.
		return "BUYCANCLE";
	if (m_eitemState == eST_WAITBUYCANCLE)	//매수 중단 대기 상태.
		return "WAITBUYCANCLE";
	if (m_eitemState == eST_HOLDING)	//보유 상태.
		return "HOLDING";
	if (m_eitemState == eST_TRYSELL)	//매도 시도 상태.
		return "TRYSELL";
	if (m_eitemState == eST_WAITSELL)	//매도 체결 대기 상태.
		return "WAITSELL";
	if (m_eitemState == eST_SELLCANCLE)	//매도 중단 상태.
		return "SELLCANCLE";
	if (m_eitemState == eST_WAITSELLCANCLE)	//매도 중단 대기 상태.
		return "WAITSELLCANCLE";
	if (m_eitemState == eST_WAITSELLMARKETVALUE)	//시장가로 매도 요청후 대기
		return "WAITSELLMARKETVALUE";
	if (m_eitemState == eST_TRADEDONE)	//거래 완료 상태.
		return "TRADEDONE";

	return "UNKNOWN";
}