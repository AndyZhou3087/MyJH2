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

	void paySuccNotice(std::string orderid, std::string goodsid, int price/*��Ϊ��λ*/);

	void getMonthlyReward(std::string vipid);

	void report(std::string data);

	void checklegalword(std::string words);

	void postMyMatchHeros();

	void getMyMatchHeros();

	void getMatchPairData(std::string player ="");

	void sendMatchResult(int score);

	void getMatchRankHeros();

	void getRewardMatch();

	void getLoginData();

	void getLoginAward();

	void addNews(std::string content, int type = 2);

	void getNews();

	void askApraise();

	void praiseRankList();//���ް�

	void askApraiseRankList();//������

	void praise(std::string toplayerid);//����

	void exchangePraise(int amount);//�һ�����

	void getSupperBossInfo();//��ȡ����Boss��Ϣ

	void postSupperBossHurt(int usefree);

	void getSupperBossRankList(int type);

	void getExchangeInvitationcode(std::string code);

private:
	HTTPDataDelegateProtocol *m_pDelegateProtocol;//�ӿڶ���
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

	void httpReportCB(std::string retdata, int code, std::string extdata);

	void httpGetMonthlyRewardCB(std::string retdata, int code, std::string extdata);

	void httpPostMyMatchHerosCB(std::string retdata, int code, std::string extdata);

	void httpGetMyMatchHerosCB(std::string retdata, int code, std::string extdata);

	void httpGetMatchPairDataCB(std::string retdata, int code, std::string extdata);

	void httpBlankCB(std::string retdata, int code, std::string extdata);

	void httpChecklegalwordCB(std::string retdata, int code, std::string extdata);

	void httpSendMatchResultCB(std::string retdata, int code, std::string extdata);

	void httpGetMyMatchRankingCB(std::string retdata, int code, std::string extdata);

	void httpGetMyRewardCB(std::string retdata, int code, std::string extdata);

	void httpGetLoginDataCB(std::string retdata, int code, std::string extdata);

	void httpGetLoginAwardCB(std::string retdata, int code, std::string extdata);

	void httpAddNewsCB(std::string retdata, int code, std::string extdata);

	void httpGetNewsCB(std::string retdata, int code, std::string extdata);

	void httpBuyNoticeCB(std::string retdata, int code, std::string extdata);

	void httpAskApraiseCB(std::string retdata, int code, std::string extdata);

	void httpPraiseRankListCB(std::string retdata, int code, std::string extdata);

	void httpAskApraiseRankListCB(std::string retdata, int code, std::string extdata);

	void httpPraiseCB(std::string retdata, int code, std::string extdata);

	void httpExchangePraiseCB(std::string retdata, int code, std::string extdata);

	void httpGetSupperBossInfoCB(std::string retdata, int code, std::string extdata);

	void httpPostSupperBossHurtCB(std::string retdata, int code, std::string extdata);
	
	void httpGetSupperBossRankListCB(std::string retdata, int code, std::string extdata);

	void httpGetExchangeInvitationcodeCB(std::string retdata, int code, std::string extdata);
	
	
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
