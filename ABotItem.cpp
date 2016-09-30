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
	m_lsellPrice = 0;		//�ŵ���
	m_lQuantity = 0;		//����.
	m_lupperPrice = 0;		//�Ű� ���Ѱ�. �̰��� �̻����� ������ �Ǵ�.
	m_lunderPrice = 0;		//�Ű� ���Ѱ�. �̰��� ���Ϸ� �������� �Ǵ�.

	m_ltryBuyTimeout = 0;	//���� ���� �õ� ���� �ð�. clock.
	m_strBuyOrder = _T("");	//���� ���� �õ� �ֹ� ��ȣ.
	m_lBuyCost = 0;			//�� ���ſ� ���� ���� �ݾ�.
	m_lBuyQuantity = 0;		//�� ���ŵ� ����.
	m_ltryBuyCount = 0;		//���� �õ� ȸ��.

	m_lholdTime = 0;		//���� �ð�. clock
	m_lholdTimeout = 0;		//������ �ŵ����� Ÿ�Ӿƿ�. clock.

	m_ltrySellTimeout = 0;	//���� �ŵ� �õ� ���� �ð�. clock.
	m_strSellOrder = _T("");	//���� �ŵ� �õ� �ֹ� ��ȣ.
	m_lSellCost = 0;			//�� �ŵ��� ���� �ݾ�.
	m_lSellQuantity = 0;		//�� �ŵ��� ����.
	m_ltrySellCount = 0;		//�ŵ� �õ� ȸ��.
}


CABotItem::~CABotItem()
{
}


CString
CABotItem::GetStateString()
{
	if (m_eitemState == eST_NONE)		//�ƹ��͵� �ƴ� ����.
		return "NONE";
	if (m_eitemState == eST_ADDED)		//���� ���� �߰� �Ǿ�����.
		return "ADDED";
	if (m_eitemState == eST_TRYBUY)		//�ż� �õ� ����.
		return "TRYBUY";
	if (m_eitemState == eST_WAITBUY)	//�ż� ü�� ��� ����.
		return "WAITBUY";
	if (m_eitemState == eST_BUYCANCLE)	//�ż� �ߴ� ����.
		return "BUYCANCLE";
	if (m_eitemState == eST_WAITBUYCANCLE)	//�ż� �ߴ� ��� ����.
		return "WAITBUYCANCLE";
	if (m_eitemState == eST_HOLDING)	//���� ����.
		return "HOLDING";
	if (m_eitemState == eST_TRYSELL)	//�ŵ� �õ� ����.
		return "TRYSELL";
	if (m_eitemState == eST_WAITSELL)	//�ŵ� ü�� ��� ����.
		return "WAITSELL";
	if (m_eitemState == eST_SELLCANCLE)	//�ŵ� �ߴ� ����.
		return "SELLCANCLE";
	if (m_eitemState == eST_WAITSELLCANCLE)	//�ŵ� �ߴ� ��� ����.
		return "WAITSELLCANCLE";
	if (m_eitemState == eST_WAITSELLMARKETVALUE)	//���尡�� �ŵ� ��û�� ���
		return "WAITSELLMARKETVALUE";
	if (m_eitemState == eST_TRADEDONE)	//�ŷ� �Ϸ� ����.
		return "TRADEDONE";

	return "UNKNOWN";
}