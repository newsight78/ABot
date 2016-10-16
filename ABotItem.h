#pragma once
enum eItemState
{
	eST_NONE,				//아무것도 아닌 상태.
	eST_ADDED,				//관심 종목에 추가 되었을때.
	eST_TRYBUY,				//매수 시도 상태.
	eST_WAITBUY,			//매수 체결 대기 상태.
	eST_BUYCANCEL,			//매수 중단 상태.
	eST_WAITBUYCANCEL,		//매수 중단 대기 상태.
	eST_HOLDING,			//보유 상태.
	eST_TRYSELL,			//매도 시도 상태.
	eST_WAITSELL,			//매도 체결 대기 상태.
	eST_SELLCANCEL,			//매도 중단 상태.
	eST_WAITSELLCANCEL,		//매도 중단 대기 상태.
	eST_SELLMARKETVALUE,	//시장가로 매도 요청.
	eST_WAITSELLMARKETVALUE,//시장가로 매도 요청후 대기
	eST_TRADECLOSING,		//거래 정산 상태.
	eST_TRADEDONE,			//거래 완료 상태.
};

class CABotItem
{
public:
	CABotItem();
	~CABotItem();
	void Init();

	CString GetStateString();

public:
	long			m_index;			//순번.
	eItemState		m_eitemState;		//프로세스 상태.
	CString			m_strCode;			//코드
	CString			m_strName;			//이름
	long			m_lcurPrice;		//현재가. 실시간으로 검색되고 있다면.
	long			m_lbuyPrice;		//마지막 매수가.
	long			m_lsellPrice;		//마지막 매도가.
	long			m_lQuantity;		//수량.
	long			m_lupperPrice;		//매각 상한가. 이가격 이상으로 오르면 판다.
	long			m_lunderPrice;		//매각 하한가. 이가격 이하로 떨어지면 판다.

	long			m_ltryBuyTimeout;	//최종 구매 시도 포기 시각. clock.
	CString			m_strBuyOrder;		//최종 구매 시도 주문 번호.
	long			m_lBuyCost;			//실 구매에 사용된 누적 금액.
	long			m_lBuyQuantity;		//실 구매된 수량.
	long			BuyPrice()			{ return (m_lBuyQuantity > 0 ? m_lBuyCost / m_lBuyQuantity : 0); };
	long			m_ltryBuyCount;		//구매 시도 회수.

	long			m_lholdTime;		//보유 시각. clock
	long			m_lholdTimeout;		//보유후 매도까지 타임아웃. clock.

	long			m_ltrySellTimeout;	//최종 매도 시도 포기 시각. clock.
	CString			m_strSellOrder;		//최종 매도 시도 주문 번호.
	long			m_lSellCost;		//실 매도한 누적 금액.
	long			m_lSellQuantity;	//실 매도된 수량.
	long			SellPrice()			{ return (m_lSellQuantity > 0 ? m_lSellCost / m_lSellQuantity : 0); };
	long			m_ltrySellCount;	//매도 시도 회수.
};

