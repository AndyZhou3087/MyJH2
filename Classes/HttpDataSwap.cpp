#include "HttpDataSwap.h"
#include "json.h"
#include "GlobalInstance.h"

#define HTTPURL "https://www.stormnet.cn/jhapi/"


HttpDataSwap::HttpDataSwap()
{
	m_pDelegateProtocol = NULL;
}

HttpDataSwap::~HttpDataSwap()
{
	m_pDelegateProtocol = NULL;
}
HttpDataSwap::HttpDataSwap(HTTPDataDelegateProtocol *pDelegateProtocol)
{
	m_pDelegateProtocol = pDelegateProtocol;
}

HttpDataSwap* HttpDataSwap::init(HTTPDataDelegateProtocol* delegateProtocol)
{
	HttpDataSwap *httpDataSwap = new  HttpDataSwap(delegateProtocol);

	return httpDataSwap;
}

void HttpDataSwap::setDelegate(HTTPDataDelegateProtocol *delegateProtocol)
{
	m_pDelegateProtocol = delegateProtocol;
}

void HttpDataSwap::release()
{
	delete this;
}

void HttpDataSwap::getServerTime()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_getservertime");
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetServerTimeCB, this));
}

void HttpDataSwap::httpGetServerTimeCB(std::string retdata, int code, std::string tag)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();

			rapidjson::Value& timev = doc["time"];
			
			GlobalInstance::servertime = timev.GetInt();
		}
		else
		{
			ret = JSON_ERR;
		}
	}

	if (m_pDelegateProtocol != NULL)
	{
		m_pDelegateProtocol->onFinish(ret);
	}
	release();
}