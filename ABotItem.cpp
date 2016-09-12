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
	m_lbuyPrice = 0;		//���԰�.
	m_lQuantity = 0;		//����.
	m_lupperPrice = 0;		//�Ű� ���Ѱ�. �̰��� �̻����� ������ �Ǵ�.
	m_lunderPrice = 0;		//�Ű� ���Ѱ�. �̰��� ���Ϸ� �������� �Ǵ�.

	m_ltryBuyTime = 0;		//���� ���� �õ� �ð�. clock.
	m_ltryBuyTimeout = 0;	//���� ���� �õ� ���� �ð�. clock.
	m_strBuyOrder = _T("");	//���� ���� �õ� �ֹ� ��ȣ.
	m_lBuyCost = 0;			//�� ���ſ� ���� ���� �ݾ�.
	m_lBuyQuantity = 0;		//�� ���ŵ� ����.
	m_ltryBuyCount = 0;		//���� �õ� ȸ��.

	m_lholdTime = 0;		//���� �ð�. clock
	m_lholdTimeout = 0;		//������ �ŵ����� Ÿ�Ӿƿ�. clock.

	m_ltrySellTime = 0;		//���� �ŵ� �õ� �ð�. clock.
	m_ltrySellTimeout = 0;	//���� �ŵ� �õ� ���� �ð�. clock.
	m_strSellOrder = _T("");	//���� �ŵ� �õ� �ֹ� ��ȣ.
	m_lSellCost = 0;			//�� �ŵ��� ���� �ݾ�.
	m_lSellQuantity = 0;		//�� �ŵ��� ����.
	m_ltrySellCount = 0;		//�ŵ� �õ� ȸ��.
}


CABotItem::~CABotItem()
{
}
