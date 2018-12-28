#include "HttpDataSwap.h"
#include "json.h"
#include "GlobalInstance.h"
#include "DataSave.h"
#include "tinyxml2/tinyxml2.h"
#include "Equip.h"
#include "GongFa.h"
#include "MD5.h"

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

	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	url.append("&coin=");

	url.append(str);

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

	url.append("&sign=");
	std::string md5ostr = GlobalInstance::getInstance()->UUID() + goodsid;

	std::string signstr = md5(md5ostr + "key=zhoujian-87");
	url.append(signstr);

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpBlankCB, this));
}

void HttpDataSwap::getMonthlyReward(std::string vipid)
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_getmonthlycard?");
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
	url.append(vipid);

	url.append("&sign=");
	std::string md5ostr = GlobalInstance::getInstance()->UUID() + vipid;

	std::string signstr = md5(md5ostr + "key=zhoujian-87");
	url.append(signstr);

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetMonthlyRewardCB, this));
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

void HttpDataSwap::checklegalword(std::string words)
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_checklegalword?");

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

	url.append("&nickname=");
	url.append(words);

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpChecklegalwordCB, this));
}

void HttpDataSwap::postMyMatchHeros()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_matchmatchsave?");

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

	rapidjson::Document writedoc;
	writedoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = writedoc.GetAllocator();

	//rapidjson::Value object(rapidjson::kObjectType);

	for (int i = 0; i < 6; i++)
	{
		std::string herokey = StringUtils::format("hero%d", i);
		std::string herodata;
		if (GlobalInstance::myOnChallengeHeros[i] != NULL)
		{
			Hero* hero = GlobalInstance::myOnChallengeHeros[i];
			herodata = StringUtils::format("%s-%d-%d-%d-%d-%s-%d;", hero->getName().c_str(), hero->getExp().getValue(), hero->getVocation(), hero->getPotential(), hero->getSex(), hero->getId().c_str(), hero->getChangeCount());

			std::string equipstr;

			for (int k = T_ARMOR; k <= T_NG; k++)
			{
				ResBase* eqres = hero->getEquipable(k);
				if (eqres != NULL)
				{
					std::string onestr;
					if (eqres->getType() >= T_ARMOR && eqres->getType() <= T_FASHION)
					{
						Equip* eres = (Equip*)eqres;
						std::string stonestr;
						int estonesize = eres->vec_stones.size();
						for (int i = 0; i < estonesize; i++)
						{
							if (i < estonesize - 1)
							{
								stonestr.append(eres->vec_stones[i] + ",");
							}
							else
								stonestr.append(eres->vec_stones[i]);
						}
						onestr = StringUtils::format("%s-%d-%d-%s#", eres->getId().c_str(), eres->getQU().getValue(), eres->getLv().getValue(), stonestr.c_str());
					}
					else if (eqres->getType() >= T_WG && eqres->getType() <= T_NG)
					{
						GongFa* gres = (GongFa*)eqres;

						onestr = StringUtils::format("%s-%d-%d#", gres->getId().c_str(), gres->getQU().getValue(), gres->getExp().getValue());
					}
					equipstr.append(onestr);
				}
			}

			if (equipstr.length() > 0)
			{
				equipstr = equipstr.substr(0, equipstr.length() - 1);
			}
			herodata.append(equipstr);
		}

		writedoc.AddMember(rapidjson::Value(herokey.c_str(), allocator), rapidjson::Value(herodata.c_str(), allocator), allocator);
	}

	std::string friendlystr;
	std::map<std::string, NpcFriendly>::iterator it;
	for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); ++it)
	{
		std::string nid = it->first;
		int ralation = 0;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			if (it->second.relation[i] == NPC_FRIEND)
			{
				ralation += NPC_FRIEND;
			}
			else if (it->second.relation[i] == NPC_MASTER)
			{
				ralation += NPC_MASTER;
			}
			else if (it->second.relation[i] == NPC_COUPEL)
			{
				ralation += NPC_COUPEL;
			}
		}
		if (ralation == NPC_COUPEL && it->second.relation.size() == 2)//关系数值1，2，3，(1,2),(1,3)
			ralation = 5;
		if (ralation > 0)
		{
			std::string fonestr = StringUtils::format("%s-%d;", nid.substr(1).c_str(), ralation);
			friendlystr.append(fonestr);
		}
	}

	if (friendlystr.length() > 0)
	{
		friendlystr = friendlystr.substr(0, friendlystr.length() - 1);
	}
	writedoc.AddMember("friendly", rapidjson::Value(friendlystr.c_str(), allocator), allocator);

	std::string jsonstr = JsonWriter(writedoc);
	jsonstr = StringUtils::format("data=%s", jsonstr.c_str());
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpPostMyMatchHerosCB, this), jsonstr);
}

void HttpDataSwap::getMyMatchHeros()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_matchmatchselfinfo?");
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
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetMyMatchHerosCB, this));
}

void HttpDataSwap::getMatchRankHeros()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_matchmatchranklist?");
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
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetMyMatchRankingCB, this));
}

void HttpDataSwap::getMatchPairData()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_matchmatchinfo?");
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
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetMatchPairDataCB, this));
}

void HttpDataSwap::sendMatchResult(int score)
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_matchmatchresult?");
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
	url.append("&matchplayerid=");
	url.append(GlobalInstance::myMatchInfo.pairplayerid);
	url.append("&score=");
	std::string scorestr = StringUtils::format("%d", score);
	url.append(scorestr);
	url.append("&sign=");
	std::string md5ostr = GlobalInstance::getInstance()->UUID() + scorestr + GlobalInstance::myMatchInfo.pairplayerid;

	std::string signstr = md5(md5ostr + "key=zhoujian-87");
	url.append(signstr);
	

	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpSendMatchResultCB, this));
}

void HttpDataSwap::getRewardMatch()
{
	std::string url;
	url.append(HTTPURL);
	url.append("jh_matchmatchgetaward?");

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
	HttpUtil::getInstance()->doData(url, httputil_calback(HttpDataSwap::httpGetMyRewardCB, this));
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
				else if (strid.compare("giftopen") == 0)
				{
					GlobalInstance::serverTimeGiftData.isopen = iter->value.GetInt() == 1 ? true : false;
				}
				else if (strid.compare("turntime") == 0)
				{
					GlobalInstance::serverTimeGiftData.lefttime = iter->value.GetInt();
				}
				else if (strid.compare("turn") == 0)
				{
					GlobalInstance::serverTimeGiftData.turn = iter->value.GetInt();
				}
				else if (strid.compare(0, 3, "vip") == 0)
				{
					int val = iter->value.GetInt();
					if (val > 0)
					{
						int v = atoi(strid.substr(strid.length() - 1, 1).c_str());
						std::string vipid = StringUtils::format("vip%d", v - 2);
						GlobalInstance::vec_buyVipIds.push_back(vipid);
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


void HttpDataSwap::httpGetMonthlyRewardCB(std::string retdata, int code, std::string extdata)
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

void HttpDataSwap::httpChecklegalwordCB(std::string retdata, int code, std::string extdata)
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

void HttpDataSwap::httpPostMyMatchHerosCB(std::string retdata, int code, std::string extdata)
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

void HttpDataSwap::httpGetMyMatchRankingCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();
			if (doc.HasMember("data"))
			{
				rapidjson::Value& mydatav = doc["data"];

				GlobalInstance::myRankInfo.vec_rankData.clear();
				for (unsigned int m = 0; m < mydatav.Size(); m++)
				{
					MyRankData data;
					data.rank = m;
					rapidjson::Value& myd = mydatav[m]["playerid"];
					data.playerid = myd.GetString();
					myd = mydatav[m]["nickname"];
					data.nickname = myd.GetString();
					myd = mydatav[m]["matchscore"];
					data.matchscore = atoi(myd.GetString());

					for (int i = 0; i < 6; i++)
					{
						std::string herokey = StringUtils::format("hero%d", i);
						std::string herodata = mydatav[m][herokey.c_str()].GetString();
						data.map_otherheros[herokey] = herodata;
					}

					if (mydatav[m].HasMember("friendly"))
					{
						myd = mydatav[m]["friendly"];
						data.friendly = myd.GetString();
					}
						

					GlobalInstance::myRankInfo.vec_rankData.push_back(data);
				}
			}

			if (doc.HasMember("myrank"))
			{
				rapidjson::Value& v = doc["myrank"];
				GlobalInstance::myRankInfo.myrank = atoi(v.GetString());
			}
			
			if (ret == 2)
				ret = SUCCESS;

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

void HttpDataSwap::httpGetMyRewardCB(std::string retdata, int code, std::string extdata)
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

void HttpDataSwap::httpGetMyMatchHerosCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();

			if (doc.HasMember("data"))
			{
				rapidjson::Value& mydatav = doc["data"];

				rapidjson::Value& myd = mydatav["isgetaward"];
				GlobalInstance::myMatchInfo.getrewardstate = atoi(myd.GetString());
				myd = mydatav["matchscore"];
				GlobalInstance::myMatchInfo.matchscore = atoi(myd.GetString());
				myd = mydatav["awardindex"];
				GlobalInstance::myMatchInfo.awardindex = atoi(myd.GetString());
				GlobalInstance::myMatchInfo.map_myheros.clear();
				for (int i = 0; i < 6; i++)
				{
					std::string herokey = StringUtils::format("hero%d", i);
					std::string herodata = mydatav[herokey.c_str()].GetString();
					GlobalInstance::myMatchInfo.map_myheros[herokey] = herodata;
				}
			}

			if (doc.HasMember("endday"))
			{
				rapidjson::Value& v = doc["endday"];
				GlobalInstance::myMatchInfo.endtime = v.GetString();
			}

			if (doc.HasMember("matchaward"))
			{
				rapidjson::Value& v = doc["matchaward"];
				GlobalInstance::myMatchInfo.rewardstr = v.GetString();
			}
			if (ret == 2)
				ret = SUCCESS;

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

void HttpDataSwap::httpGetMatchPairDataCB(std::string retdata, int code, std::string extdata)
{
	int ret = code;
	if (code == 0)
	{
		rapidjson::Document doc;
		if (JsonReader(retdata, doc))
		{
			GlobalInstance::myMatchInfo.map_pairfriendly.clear();

			rapidjson::Value& retv = doc["ret"];
			ret = retv.GetInt();

			if (doc.HasMember("data"))
			{
				rapidjson::Value& mydatav = doc["data"];

				for (int i = 0; i < 6; i++)
				{
					std::string herokey = StringUtils::format("hero%d", i);
					std::string herodata = mydatav[herokey.c_str()].GetString();
					GlobalInstance::myMatchInfo.map_pairheros[herokey] = herodata;
				}
				GlobalInstance::myMatchInfo.pairnickname = mydatav["nickname"].GetString();
				GlobalInstance::myMatchInfo.pairplayerid = mydatav["playerid"].GetString();
				GlobalInstance::myMatchInfo.pairscore = atoi(mydatav["matchscore"].GetString());

				if (mydatav.HasMember("friendly"))
					GlobalInstance::getInstance()->parsePairFriendly(mydatav["friendly"].GetString());
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

void HttpDataSwap::httpSendMatchResultCB(std::string retdata, int code, std::string extdata)
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