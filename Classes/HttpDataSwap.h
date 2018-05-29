/********************************************************************
*服务器数据交互
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
	HTTPDataDelegateProtocol *m_pDelegateProtocol;//接口对象
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
	* @brief 数据完成
	@return 错误代码0--成功，其它--错误
	*/
	virtual void onFinish(int code) {};
};

#endif
