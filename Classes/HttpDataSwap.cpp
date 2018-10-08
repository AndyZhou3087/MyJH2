#include "HttpDataSwap.h"
#include "json.h"
#include "GlobalInstance.h"
#include "DataSave.h"
#include "tinyxml2/tinyxml2.h"

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

	url.append("&pkg=");
	url.append(GlobalInstance::getInstance()->getPackageName());

	url.append("&ver=");
	url.append(GlobalInstance::getInstance()->getVersionCode());

	url.append("&cid=");
	url.append(GlobalInstance::getInstance()->getChannelId());

	url.append("&plat=");
	url.append(GlobalInstance::getInstance()->getPlatForm());

	std::string postdata = GlobalInstance::getInstance()->getUserDefaultXmlString();

	if (postdata.length() > 0)
	{
		//log("zhou postdata = %s", postdata.c_str());
		//rapidjson::Document writedoc;
		//writedoc.SetObject();
		//rapidjson::Document::AllocatorType& allocator = writedoc.GetAllocator();

		//writedoc.AddMember("playerid", rapidjson::Value(GlobalInstance::getInstance()->UUID().c_str(), allocator), allocator);

		//std::string myherosdata;
		//rapidjson::Value dataArray(rapidjson::kArrayType);

		//for (int i = 0; i < 50; i++)
		//{
		//	std::string herokey = StringUtils::format("hero%d", i);
		//	std::string herodatastr = DataSave::getInstance()->getHeroData(herokey);
		//	if (herodatastr.length() > 0)
		//	{
		//		dataArray.PushBack(rapidjson::Value(herodatastr.c_str(), allocator), allocator);
		//	}
		//}
		//writedoc.AddMember("myheros", dataArray, allocator);
		//postdata = JsonWriter(writedoc);
		HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpPostAllDataCB, this), postdata);
	}
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

void HttpDataSwap::vipIsOn()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_takemonthlycard?");
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
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpVipIsOnCB, this));
}

void HttpDataSwap::paySuccNotice(std::string goodsid, int price)
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_buynotify?");
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

	url.append("&goodsid=");
	url.append(goodsid);

	url.append("&price=");
	std::string pricestr = StringUtils::format("%d", price * 100);
	url.append(pricestr);
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpBlankCB, this));
}

void HttpDataSwap::getMessageList(int type)
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

	std::string noticestr = "";
	if (type >= 0)
	{
		url.append("&type=");
		std::string typestr = StringUtils::format("%d", type);
		url.append(typestr);
		noticestr = "notice";
	}

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetMessageListCB, this), "", GET, "", noticestr);
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

void HttpDataSwap::getPlayerId()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_getplayerid?");

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

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetPlayerIdCB, this));
}

void HttpDataSwap::modifyName(int type, std::string nickname)
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_updatenickname?");

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

	url.append("&type=");
	std::string str = StringUtils::format("%d", type);
	url.append(str);

	url.append("&nickname=");
	url.append(nickname);

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpModifyNameCB, this));
}

void HttpDataSwap::report(std::string data)
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_report?");

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

	url.append("&data=");
	url.append(data);

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpReportCB, this));
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

void HttpDataSwap::httpBlankCB(std::string retdata, int code, std::string extdata)
{
	release();
}

void HttpDataSwap::httpVipIsOnCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();
			GlobalInstance::map_buyVipDays.clear();
			GlobalInstance::vec_buyVipIds.clear();
			for (rapidjson::Value::ConstMemberIterator iter = doc.MemberBegin(); iter != doc.MemberEnd(); ++iter)
			{
				std::string strid = iter->name.GetString();

				if (strid.compare("qq") == 0)
				{
					GlobalInstance::qq = iter->value.GetString();
				}
				if (strid.compare("forbidden") == 0)
				{
					int val = atoi(iter->value.GetString());
					if (val == 1)
					{
						MainScene::cheatAction();
						break;
					}
				}
				else if (strid.compare(0, 3, "vip") == 0)
				{
					int val = iter->value.GetInt();
					if (val > 0)
					{
						GlobalInstance::vec_buyVipIds.push_back(strid);
					}
				}
				else
				{
					std::size_t pos = strid.find("vip");
					if (pos != std::string::npos && pos > 0)
					{
						int v = atoi(strid.substr(strid.length() - 1, 1).c_str());
						std::string vipid = StringUtils::format("vip%d", v - 2);
						int val = iter->value.GetInt();
						GlobalInstance::map_buyVipDays[vipid] = val;
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
			if (ret == 0)//解析文件
			{
				if (doc.HasMember("content"))
				{
					tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();
					rapidjson::Value& vc = doc["content"];
					int err = pDoc->Parse(vc.GetString());
					if (err != 0)
						ret = DATA_ERR;
					else
					{
						tinyxml2::XMLElement *rootEle = pDoc->RootElement();
						tinyxml2::XMLElement *element = rootEle->FirstChildElement();
						while (element != NULL)
						{
							if (element->GetText() != NULL)
								UserDefault::getInstance()->setStringForKey(element->Name(), element->GetText());
							element = element->NextSiblingElement();
						}
					}
					delete pDoc;
				}
				else
					ret = DATA_ERR;

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
				GlobalInstance::vec_notice.clear();
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
							if (extdata.length() > 0)
							{
								GlobalInstance::vec_notice.push_back(msgdata);
							}
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

void HttpDataSwap::httpGetPlayerIdCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();

			if (ret == 0)
			{
				rapidjson::Value& v = doc["id"];
				GlobalInstance::getInstance()->setMyID(v.GetString());

				v = doc["nickname"];
				GlobalInstance::getInstance()->setMyNickName(v.GetString());
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

void HttpDataSwap::httpModifyNameCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();

			if (ret == 0)
			{
				if (doc.HasMember("id"))
				{
					rapidjson::Value& v = doc["id"];
					GlobalInstance::getInstance()->setMyID(v.GetString());
				}

				if (doc.HasMember("nickname"))
				{
					rapidjson::Value& v = doc["nickname"];
					GlobalInstance::getInstance()->setMyNickName(v.GetString());
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

void HttpDataSwap::httpReportCB(std::string retdata, int code, std::string extdata)
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