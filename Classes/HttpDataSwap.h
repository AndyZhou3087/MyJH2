/********************************************************************
*���������ݽ���
*********************************************************************/
#ifndef __HTTPDATASWAP__
#define __HTTPDATASWAP__
#include "cocos2d.h"
#include "HttpUtil.h"

class HTTPDataDelegateProtocol;

class HttpDataSwap
{
public:

	static HttpDataSwap* init(HTTPDataDelegateProtocol* delegateProtocol = NULL);

	void getServerTime();

	void setDelegate(HTTPDataDelegateProtocol *delegateProtocol);

private:
	HTTPDataDelegateProtocol *m_pDelegateProtocol;//�ӿڶ���
	static bool isdoing;
	HttpDataSwap();
	~HttpDataSwap();
	HttpDataSwap(HTTPDataDelegateProtocol *pDelegateProtocol);
	void release();
private:

	void httpGetServerTimeCB(std::string retdata, int code, std::string tag);

};

class HTTPDataDelegateProtocol
{
public:
	virtual ~HTTPDataDelegateProtocol(){};

	/**
	* @brief �������
	@return �������0--�ɹ�������--����
	*/
	virtual void onFinish(int code) {};
};

#endif
