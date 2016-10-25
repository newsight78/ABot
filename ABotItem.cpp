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
	m_lcurPrice = 0;		//���簡. �ǽð����� �˻��ǰ� �ִٸ�.
	m_lbuyPrice = 0;		//�ż���.
	m_lsellPrice = 0;		//�ŵ���.
	m_lQuantity = 0;		//����.
	m_lupperPrice = 0;		//�Ű� ���Ѱ�. �̰��� �̻����� ������ �Ǵ�.
	m_lunderPrice = 0;		//�Ű� ���Ѱ�. �̰��� ���Ϸ� �������� �Ǵ�.

	m_lfilterBuyAccumTime = 0;	//�ż� ����, ������ ���� �ð�. clock.
	m_lfilterBuyTickcount = 0;	//�ż� ����, ƽ ����.
	m_lfilterBuyYangcost = 0;	//�ż� ����, ��ŷ��ݾ�.

	m_ltryBuyTimeout = 0;	//���� �ż� �õ� ���� �ð�. clock.
	m_strBuyOrder = _T("");	//���� �ż� �õ� �ֹ� ��ȣ.
	m_lBuyCost = 0;			//�� �ż��� ���� ���� �ݾ�.
	m_lBuyQuantity = 0;		//�� �ż��� ����.
	m_ltryBuyCount = 0;		//�ż� �õ� ȸ��.
	m_strBuyTime = _T("");	//���� �ż� �ð�.

	m_lholdTime = 0;		//���� �ð�. clock
	m_lholdTimeout = 0;		//������ �ŵ����� Ÿ�Ӿƿ�. clock.

	m_ltrySellTimeout0 = 0;	//�ŵ� �õ� Ÿ�� �ƿ� 0 �ð�. clock.
	m_ltrySellTimeout1 = 0;	//�ŵ� �õ� Ÿ�� �ƿ� 1 �ð�. clock.

	m_strSellOrder = _T("");	//���� �ŵ� �õ� �ֹ� ��ȣ.
	m_lSellCost = 0;			//�� �ŵ��� ���� �ݾ�.
	m_lSellQuantity = 0;		//�� �ŵ��� ����.
	m_ltrySellCount = 0;		//�ŵ� �õ� ȸ��.
	m_strSellTime = _T("");		//���� �ŵ� �ð�.
}


CABotItem::~CABotItem()
{
}


CString CABotItem::GetStateString()
{
	if (m_eitemState == eST_NONE)		//�ƹ��͵� �ƴ� ����.
		return "NONE";
	else if (m_eitemState == eST_TONONE)	//�ƹ��͵� �ƴ� ���·� �ǵ�����.
		return "TONONE";
	else if (m_eitemState == eST_ADDED)		//���� ���� �߰� �Ǿ�����.
		return "ADDED";
	else if (m_eitemState == eST_SETFILTERBUY)	//�ż� ���� ����.
		return "SETFILTERBUY";
	else if (m_eitemState == eST_WAITFILTERBUY)	//�ż� ���� �Ϸ� ���.
		return "WAITFILTERBUY";
	else if (m_eitemState == eST_TRYBUY)		//�ż� �õ� ����.
		return "TRYBUY";
	else if (m_eitemState == eST_WAITBUY)	//�ż� ü�� ��� ����.
		return "WAITBUY";
	else if (m_eitemState == eST_BUYCANCEL)	//�ż� �ߴ� ����.
		return "BUYCANCEL";
	else if (m_eitemState == eST_WAITBUYCANCEL)	//�ż� �ߴ� ��� ����.
		return "WAITBUYCANCEL";
	else if (m_eitemState == eST_HOLDING)	//���� ����.
		return "HOLDING";
	else if (m_eitemState == eST_TRYSELL)	//�ŵ� �õ� ����.
		return "TRYSELL";
	else if (m_eitemState == eST_WAITSELL)	//�ŵ� ü�� ��� ����.
		return "WAITSELL";
	else if (m_eitemState == eST_SELLCANCEL)	//�ŵ� �ߴ� ����.
		return "SELLCANCEL";
	else if (m_eitemState == eST_WAITSELLCANCEL)	//�ŵ� �ߴ� ��� ����.
		return "WAITSELLCANCEL";
	else if (m_eitemState == eST_SELLMARKETVALUE)		//���尡�� �ŵ� ��û
		return "SELLMARKETVALUE";
	else if (m_eitemState == eST_WAITSELLMARKETVALUE)	//���尡�� �ŵ� ��û�� ���
		return "WAITSELLMARKETVALUE";
	else if (m_eitemState == eST_TRADECLOSING)	//�ŷ� ���� ����.
		return "TRADECLOSING";
	else if (m_eitemState == eST_TRADEDONE)		//�ŷ� �Ϸ� ����.
		return "TRADEDONE";

	return "UNKNOWN";
}
