#pragma once
enum eItemState
{
	eST_NONE,				//�ƹ��͵� �ƴ� ����.
	eST_ADDED,				//���� ���� �߰� �Ǿ�����.
	eST_TRYBUY,				//�ż� �õ� ����.
	eST_WAITBUY,			//�ż� ü�� ��� ����.
	eST_BUYCANCEL,			//�ż� �ߴ� ����.
	eST_WAITBUYCANCEL,		//�ż� �ߴ� ��� ����.
	eST_HOLDING,			//���� ����.
	eST_TRYSELL,			//�ŵ� �õ� ����.
	eST_WAITSELL,			//�ŵ� ü�� ��� ����.
	eST_SELLCANCEL,			//�ŵ� �ߴ� ����.
	eST_WAITSELLCANCEL,		//�ŵ� �ߴ� ��� ����.
	eST_SELLMARKETVALUE,	//���尡�� �ŵ� ��û.
	eST_WAITSELLMARKETVALUE,//���尡�� �ŵ� ��û�� ���
	eST_TRADECLOSING,		//�ŷ� ���� ����.
	eST_TRADEDONE,			//�ŷ� �Ϸ� ����.
};

class CABotItem
{
public:
	CABotItem();
	~CABotItem();
	void Init();

	CString GetStateString();

public:
	long			m_index;			//����.
	eItemState		m_eitemState;		//���μ��� ����.
	CString			m_strCode;			//�ڵ�
	CString			m_strName;			//�̸�
	long			m_lcurPrice;		//���簡. �ǽð����� �˻��ǰ� �ִٸ�.
	long			m_lbuyPrice;		//������ �ż���.
	long			m_lsellPrice;		//������ �ŵ���.
	long			m_lQuantity;		//����.
	long			m_lupperPrice;		//�Ű� ���Ѱ�. �̰��� �̻����� ������ �Ǵ�.
	long			m_lunderPrice;		//�Ű� ���Ѱ�. �̰��� ���Ϸ� �������� �Ǵ�.

	long			m_ltryBuyTimeout;	//���� ���� �õ� ���� �ð�. clock.
	CString			m_strBuyOrder;		//���� ���� �õ� �ֹ� ��ȣ.
	long			m_lBuyCost;			//�� ���ſ� ���� ���� �ݾ�.
	long			m_lBuyQuantity;		//�� ���ŵ� ����.
	long			BuyPrice()			{ return (m_lBuyQuantity > 0 ? m_lBuyCost / m_lBuyQuantity : 0); };
	long			m_ltryBuyCount;		//���� �õ� ȸ��.

	long			m_lholdTime;		//���� �ð�. clock
	long			m_lholdTimeout;		//������ �ŵ����� Ÿ�Ӿƿ�. clock.

	long			m_ltrySellTimeout;	//���� �ŵ� �õ� ���� �ð�. clock.
	CString			m_strSellOrder;		//���� �ŵ� �õ� �ֹ� ��ȣ.
	long			m_lSellCost;		//�� �ŵ��� ���� �ݾ�.
	long			m_lSellQuantity;	//�� �ŵ��� ����.
	long			SellPrice()			{ return (m_lSellQuantity > 0 ? m_lSellCost / m_lSellQuantity : 0); };
	long			m_ltrySellCount;	//�ŵ� �õ� ȸ��.
};

