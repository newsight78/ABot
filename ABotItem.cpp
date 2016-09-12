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
	m_lbuyPrice = 0;		//구입가.
	m_lQuantity = 0;		//수량.
	m_lupperPrice = 0;		//매각 상한가. 이가격 이상으로 오르면 판다.
	m_lunderPrice = 0;		//매각 하한가. 이가격 이하로 떨어지면 판다.

	m_ltryBuyTime = 0;		//최종 구매 시도 시각. clock.
	m_ltryBuyTimeout = 0;	//최종 구매 시도 포기 시각. clock.
	m_strBuyOrder = _T("");	//최종 구매 시도 주문 번호.
	m_lBuyCost = 0;			//실 구매에 사용된 누적 금액.
	m_lBuyQuantity = 0;		//실 구매된 수량.
	m_ltryBuyCount = 0;		//구매 시도 회수.

	m_lholdTime = 0;		//보유 시각. clock
	m_lholdTimeout = 0;		//보유후 매도까지 타임아웃. clock.

	m_ltrySellTime = 0;		//최종 매도 시도 시각. clock.
	m_ltrySellTimeout = 0;	//최종 매도 시도 포기 시각. clock.
	m_strSellOrder = _T("");	//최종 매도 시도 주문 번호.
	m_lSellCost = 0;			//실 매도한 누적 금액.
	m_lSellQuantity = 0;		//실 매도된 수량.
	m_ltrySellCount = 0;		//매도 시도 회수.
}


CABotItem::~CABotItem()
{
}
