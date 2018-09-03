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

	void postAllData();

	void getAllData();

	void getMessageList(int type = -1);

	void updateMessageStatus(std::string id, int changestatus);

	void getPlayerId();

	void modifyName(int type, std::string nickname = "");

	void setDelegate(HTTPDataDelegateProtocol *delegateProtocol);

	void vipIsOn();

	void vipSuccNotice(std::string vipid);

private:
	HTTPDataDelegateProtocol *m_pDelegateProtocol;//�ӿڶ���
	static bool isdoing;
	HttpDataSwap();
	~HttpDataSwap();
	HttpDataSwap(HTTPDataDelegateProtocol *pDelegateProtocol);
	void release();
private:

	void httpGetServerTimeCB(std::string retdata, int code, std::string extdata);

	void httpPostAllDataCB(std::string retdata, int code, std::string extdata);

	void httpGetAllDataCB(std::string retdata, int code, std::string extdata);

	void httpGetMessageListCB(std::string retdata, int code, std::string extdata);

	void httpUpdateMessageStatusCB(std::string retdata, int code, std::string extdata);

	void httpGetPlayerIdCB(std::string retdata, int code, std::string extdata);

	void httpModifyNameCB(std::string retdata, int code, std::string extdata);

	void httpVipIsOnCB(std::string retdata, int code, std::string extdata);

	void httpBlankCB(std::string retdata, int code, std::string extdata);
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
