#include "StarDescLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MapBlockScene.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "ResDescLayer.h"
#include "BuyResLayer.h"
#include "DataSave.h"

USING_NS_CC;

StarDescLayer::StarDescLayer()
{
	GlobalInstance::takeoutherocount = 0;
}

StarDescLayer::~StarDescLayer()
{

}


StarDescLayer* StarDescLayer::create(std::string mapid)
{
	StarDescLayer *pRet = new(std::nothrow)StarDescLayer();
	if (pRet && pRet->init(mapid))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}

// on "init" you need to initialize your instance
bool StarDescLayer::init(std::string mapid)
{
	if (!Layer::init())
	{
		return false;
	}

	m_mapid = mapid;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("starDescLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	std::u32string utf32String;
	StringUtils::UTF8ToUTF32(GlobalInstance::map_AllResources[mapid].name, utf32String);

	int u32strlen = utf32String.length();
	cocos2d::ui::ImageView* mapnamebox = (cocos2d::ui::ImageView*)csbnode->getChildByName("addrbox");
	cocos2d::ui::Text* mapnamelbl = (cocos2d::ui::Text*)csbnode->getChildByName("mapname");
	mapnamebox->setContentSize(Size(100 + (mapnamelbl->getFontSize() + 1) * u32strlen, mapnamebox->getContentSize().height));
	mapnamelbl->setString(GlobalInstance::map_AllResources[mapid].name);

	std::string descstr;
	for (unsigned int i = 0;i < GlobalInstance::vec_TaskMain.size(); i++)
	{ 
		if (GlobalInstance::vec_TaskMain[i].place.compare(mapid) == 0)
		{
			descstr = GlobalInstance::vec_TaskMain[i].desc;
			break;
		}
	}
	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setString(descstr);
	Label* ldsc = (Label*)desclbl->getVirtualRenderer();
	ldsc->setLineSpacing(8);

	GlobalInstance::vec_stardata.clear();
	GlobalInstance::curMapFinishStars = 0;
	std::string mainmapid = m_mapid.substr(0, m_mapid.find_last_of("-"));
	
	Node* star[3];

	std::vector<std::string> vec_starc = GlobalInstance::map_mapsdata[mainmapid].map_sublist[m_mapid].vec_starc;

	for (unsigned int i = 0; i < vec_starc.size(); i++)
	{
		std::vector<std::string> vec_one;
		CommonFuncs::split(vec_starc[i], vec_one, "-");

		int cid = atoi(vec_one[0].c_str());
		int count = atoi(vec_one[1].c_str());

		S_StarData sdata;
		sdata.sid = cid;
		sdata.needcount = count;
		if (cid == SA_NODEATH)
			sdata.finishcount = -1;
		else
			sdata.finishcount = 0;

		GlobalInstance::vec_stardata.push_back(sdata);
		std::string key = StringUtils::format("star%d", i);
		star[i] = csbnode->getChildByName(key);
		star[i]->setVisible(false);
	}

	std::vector<std::string> vec_finishstar;

	CommonFuncs::split(DataSave::getInstance()->getFinishStar(mapid), vec_finishstar, ",");

	for (unsigned int i = 0; i < vec_finishstar.size(); i++)
	{
		int ctype = atoi(vec_finishstar[i].c_str());
		for (unsigned int m = 0; m < GlobalInstance::vec_stardata.size(); m++)
		{
			if (ctype == GlobalInstance::vec_stardata[m].sid)
			{
				GlobalInstance::vec_stardata[m].finishcount = GlobalInstance::vec_stardata[m].needcount;
				break;
			}
		}
	}

	std::sort(GlobalInstance::vec_stardata.begin(), GlobalInstance::vec_stardata.end(), sortbyfinishstat);

	for (unsigned int i = 0; i < GlobalInstance::vec_stardata.size(); i++)
	{
		int cid = GlobalInstance::vec_stardata[i].sid;
		int count = GlobalInstance::vec_stardata[i].needcount;

		std::string keyname = StringUtils::format("stagestar%02d", GlobalInstance::vec_stardata[i].sid);
		std::string content;

		if (cid == SA_FIGHTSUCC || cid == SA_GOSTEP)
			content = StringUtils::format(ResourceLang::map_lang[keyname].c_str(), count);
		else
			content = ResourceLang::map_lang[keyname];

		keyname = StringUtils::format("c%d", i);
		cocos2d::ui::Text* ctext = (cocos2d::ui::Text*)csbnode->getChildByName(keyname);
		ctext->setString(content);

		if (GlobalInstance::vec_stardata[i].finishcount >= GlobalInstance::vec_stardata[i].needcount)
		{
			star[i]->setVisible(true);
			GlobalInstance::curMapFinishStars++;
		}
	}

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
	cocos2d::ui::Text* rescountlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");
	cocos2d::ui::Text* resname = (cocos2d::ui::Text*)resbox->getChildByName("name");

	std::vector<std::string> vec_str;
	std::string resstr = GlobalInstance::map_mapsdata[mainmapid].map_sublist[m_mapid].vec_f3starawds[0];
	CommonFuncs::split(resstr, vec_str, "-");
	std::string resid = vec_str[0];
	int rescount = atoi(vec_str[1].c_str());
	resname->setString(GlobalInstance::map_AllResources[resid].name);

	std::string boxstr = "ui/resbox.png";
	int t = 0;
	int qu = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t >= T_ARMOR && t <= T_FASHION)
	{
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_WG && t <= T_NG)
	{
		qu = GlobalInstance::map_GF[resid].qu;
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	else if (t >= T_RENS && t <= T_BOX)
	{
		qu = atoi(resid.substr(1).c_str()) - 1;
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
	}
	resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);

	Node* effectnode = CommonFuncs::playResBoxEffect(resbox, t, qu, 0);
	if (effectnode != NULL)
		effectnode->setScale(0.8f);

	std::string residstr = StringUtils::format("ui/%s.png", resid.c_str());
	res->loadTexture(residstr, cocos2d::ui::Widget::TextureResType::PLIST);

	std::string countstr = StringUtils::format("%d", rescount);
	rescountlbl->setString(countstr);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onBtnClick, this));
	closebtn->setTag(1001);
	//��ť1
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onBtnClick, this));
	actionbtn->setTag(1000);
	//��ť1����
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("goawaybtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//�����²���
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}


bool StarDescLayer::sortbyfinishstat(S_StarData a, S_StarData b)
{
	if (a.finishcount >= a.needcount && b.finishcount < b.needcount)
		return true;
	return false;
}

void StarDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btnnode = (cocos2d::ui::Button*)pSender;
		switch (btnnode->getTag())
		{
		case 1000:
		{
			std::string mainmapid = m_mapid.substr(0, m_mapid.find_last_of("-"));

			int needph = GlobalInstance::map_mapsdata[mainmapid].map_sublist[m_mapid].ph;
			bool isphok = true;

			std::string nohpherostr;
			int nohpcount = 0;
			for (int i = 0; i < 6; i++)
			{
				if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getPower().getValue() < needph)
				{
					nohpcount++;
					if (nohpherostr.length() > 0)
						nohpherostr.append(ResourceLang::map_lang["dunhao"]);
					if (nohpcount <= 3)
						nohpherostr.append(GlobalInstance::myCardHeros[i]->getName());
					else
					{
						nohpherostr.append("...");
						break;
					}
					isphok = false;
				}
			}
			if (isphok)
			{

				DynamicValueInt dv;
				for (int i = 0; i < 6; i++)
				{
					if (GlobalInstance::myCardHeros[i] != NULL)
					{
						if (GlobalInstance::myCardHeros[i]->getPower().getValue() >= 100)
							GlobalInstance::myCardHeros[i]->setPowerTime(GlobalInstance::servertime);

						dv.setValue(GlobalInstance::myCardHeros[i]->getPower().getValue() - needph);
						GlobalInstance::myCardHeros[i]->setPower(dv);
						GlobalInstance::getInstance()->saveHero(GlobalInstance::myCardHeros[i]);

						if (GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
							GlobalInstance::takeoutherocount++;
					}
				}

				btnnode->setEnabled(false);
				GlobalInstance::ishasmazeentry = GlobalInstance::getInstance()->createRandomNum(100) < 50 ? true : false;
				Director::getInstance()->replaceScene(TransitionFade::create(1.5f, MapBlockScene::createScene(m_mapid, GlobalInstance::map_mapsdata[mainmapid].map_sublist[m_mapid].bgtype)));
			}
			else
			{
				nohpherostr = nohpherostr.substr(0, nohpherostr.length());
				nohpherostr.append(ResourceLang::map_lang["nomorehp"]);
				MovingLabel::show(nohpherostr);

				if (MyRes::getMyResCount("p001", MYSTORAGE) > 0)
				{
					ResDescLayer* layer = ResDescLayer::create(MyRes::getMyRes("p001", MYSTORAGE), 0);
					this->addChild(layer, 0, 1111);
					AnimationEffect::openAniEffect((Layer*)layer);
				}
				else
				{
					std::vector< MSGAWDSDATA> vec_res;
					MSGAWDSDATA rdata;
					rdata.rid = "p001";
					rdata.count = 1;
					rdata.qu = 0;
					vec_res.push_back(rdata);
					BuyResLayer* layer = BuyResLayer::create(vec_res);
					this->addChild(layer);
				}
			}
		}
			break;
		case 1001:
			btnnode->setEnabled(false);
			AnimationEffect::closeAniEffect(this);
			break;
		default:
			break;
		}
	}
}