#include "HttpDataSwap.h"
#include "json.h"
#include "GlobalInstance.h"
#include "DataSave.h"

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

void HttpDataSwap::postAllData()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_saveplayerfile?");

	url.append("playerid=");
	url.append(GlobalInstance::getInstance()->UUID());

	std::string postdata;

	rapidjson::Document writedoc;
	writedoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = writedoc.GetAllocator();

	writedoc.AddMember("playerid", rapidjson::Value(GlobalInstance::getInstance()->UUID().c_str(), allocator), allocator);

	std::string myherosdata;
	rapidjson::Value dataArray(rapidjson::kArrayType);

	for (int i = 0; i < 50; i++)
	{
		std::string herokey = StringUtils::format("hero%d", i);
		std::string herodatastr = DataSave::getInstance()->getHeroData(herokey);
		if (herodatastr.length() > 0)
		{
			dataArray.PushBack(rapidjson::Value(herodatastr.c_str(), allocator), allocator);
		}
	}
	writedoc.AddMember("myheros", dataArray, allocator);
	postdata = JsonWriter(writedoc);
	//postdata
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpPostAllDataCB, this), postdata);
}

void HttpDataSwap::getAllData()
{

	std::string url;
	url.append(HTTPURL);
	url.append("jh_getplayerfile?");

	url.append("playerid=");
	url.append(GlobalInstance::getInstance()->UUID());

	url.append("&pkg=");
	url.append(GlobalInstance::getInstance()->getPackageName());

	url.append("&ver=");
	url.append(GlobalInstance::getInstance()->getVersionCode());

	url.append("&cid=");
	url.append(GlobalInstance::getInstance()->getChannelId());

	url.append("&plat=");
	url.append(GlobalInstance::getInstance()->getPlatForm());

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetAllDataCB, this));
}

void HttpDataSwap::getMessageList()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_getmessage?");

	url.append("playerid=");
	url.append(GlobalInstance::getInstance()->UUID());

	url.append("&pkg=");
	url.append(GlobalInstance::getInstance()->getPackageName());

	url.append("&ver=");
	url.append(GlobalInstance::getInstance()->getVersionCode());

	url.append("&cid=");
	url.append(GlobalInstance::getInstance()->getChannelId());

	url.append("&plat=");
	url.append(GlobalInstance::getInstance()->getPlatForm());

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetMessageListCB, this));
}

void HttpDataSwap::updateMessageStatus(std::string id, int changestatus)
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_updatemessage?");

	url.append("playerid=");
	url.append(GlobalInstance::getInstance()->UUID());

	url.append("&pkg=");
	url.append(GlobalInstance::getInstance()->getPackageName());

	url.append("&ver=");
	url.append(GlobalInstance::getInstance()->getVersionCode());

	url.append("&cid=");
	url.append(GlobalInstance::getInstance()->getChannelId());

	url.append("&plat=");
	url.append(GlobalInstance::getInstance()->getPlatForm());

	url.append("&id=");
	url.append(id);

	url.append("&status=");
	std::string statusstr = StringUtils::format("%d", changestatus);
	url.append(statusstr);

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpUpdateMessageStatusCB, this));
}

void HttpDataSwap::httpGetServerTimeCB(std::string retdata, int code, std::string extdata)
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

void HttpDataSwap::httpPostAllDataCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();
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

void HttpDataSwap::httpGetAllDataCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();
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

void HttpDataSwap::httpGetMessageListCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == SUCCESS)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();
			if (ret == SUCCESS)
			{
				GlobalInstance::vec_messsages.clear();

				if (doc.HasMember("data"))
				{
					rapidjson::Value& data = doc["data"];
					if (data.IsArray())
					{
						for (unsigned int i = 0; i < data.Size(); i++)
						{
							MessageData msgdata;
							rapidjson::Value& onedata = data[i];
							rapidjson::Value& dataval = onedata["id"];
							msgdata.id = dataval.GetString();
							dataval = onedata["title"];
							msgdata.title = dataval.GetString();
							dataval = onedata["content"];
							msgdata.content = dataval.GetString();
							dataval = onedata["type"];
							msgdata.type = atoi(dataval.GetString());
							dataval = onedata["status"];
							msgdata.status = atoi(dataval.GetString());
							GlobalInstance::vec_messsages.push_back(msgdata);
						}
					}
				}
			}
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

void HttpDataSwap::httpUpdateMessageStatusCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();
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