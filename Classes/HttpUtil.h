#ifndef __HTTPUTIL_H__
#define __HTTPUTIL_H__
#include "cocos2d.h"
#include "network/HttpClient.h"
USING_NS_CC;
using namespace network;
typedef enum
{
	GET = 0,
	POST
}HTTPTYPE;

typedef enum
{
	JSON_ERR = -3,
	CONN_FAIL = -2,//连接失败，可能网络没开
	DATA_ERR = -1,//数据错误
	SUCCESS = 0
}HTTP_ERR;

typedef std::function<void(std::string retdata, int code, std::string extenddata)> HttpCBFunc;

#define httputil_calback(__selector__,__target__) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

class HttpUtil: public Ref
{
public:
	static HttpUtil* getInstance();
	//请求服务器数据，完整函数, 默认GET方式
	void doData(std::string url, HttpCBFunc http_cb, std::string filename/*数据写入文件，不需要写文件传""*/, HTTPTYPE method, std::string postdata, std::string extenddata = "");
	//GET方法服务器数据，
	void doData(std::string url, HttpCBFunc http_cb);
	//POST方法服务器数据，
	void doData(std::string url, HttpCBFunc http_cb, std::string postdata);
private:
	HttpCBFunc http_cbfunc;
	std::string m_filename;
	std::string m_extdata;
private:
	HttpUtil();

	~HttpUtil();
	void onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *resp);
};
#endif