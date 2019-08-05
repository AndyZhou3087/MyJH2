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
#include "NewGuideLayer.h"
#include "MopupRewardLayer.h"
#include "SoundManager.h"
#include "EquipDescLayer.h"
#include "SimpleResPopLayer.h"
#include "HintBoxLayer.h"
#include "WaitingProgress.h"
#include "SupperBossRuleLayer.h"
#include "SupperBossRankLayer.h"

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

	std::string mainmapid = m_mapid.substr(0, m_mapid.find_last_of("-"));

	mopupnode = csbnode->getChildByName("mopupnode");

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onBtnClick, this));
	closebtn->setTag(1001);
	//按钮1
	actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onBtnClick, this));
	actionbtn->setTag(1000);

	//按钮1文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("goout_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	Node* mopupbtnnode = csbnode->getChildByName("mopupbtnnode");
	cocos2d::ui::ImageView* mopupbtn = (cocos2d::ui::ImageView*)mopupbtnnode->getChildByName("mopupbtn");
	mopupbtn->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onBtnClick, this));
	mopupbtn->setTag(1002);
	//按钮1文字
	cocos2d::ui::ImageView* mopupbtntxt = (cocos2d::ui::ImageView*)mopupbtn->getChildByName("text");
	mopupbtntxt->loadTexture(ResourcePath::makeTextImgPath("mopup_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	mopupleftcountlbl = (cocos2d::ui::Text*)mopupbtnnode->getChildByName("leftcount");
	j003leftcountlbl = (cocos2d::ui::Text*)mopupbtnnode->getChildByName("j003count");
	packagefoodcountlbl = (cocos2d::ui::Text*)mopupnode->getChildByName("r001count");

	Node* first3starboxnode = csbnode->getChildByName("stagebox1");

	cocos2d::ui::Text* hintlbl = (cocos2d::ui::Text*)csbnode->getChildByName("hintdesc");

	std::string descstr;

	cocos2d::ui::Text* desclbl_1 = (cocos2d::ui::Text*)csbnode->getChildByName("desc_1");

	supperbossleftcountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("supperbossleftcount");
	supperbossleftcountlbl->setPositionX(360);
	supperbosstimelbl = (cocos2d::ui::Text*)csbnode->getChildByName("supperbossopentime");
	supperbosstimelbl->setPositionX(360);

	cocos2d::ui::Widget* rulebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("rulebtn");
	rulebtn->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onBtnClick, this));
	rulebtn->setTag(1003);
	rulebtn->setVisible(false);

	cocos2d::ui::Widget* rankbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("rankbtn");
	rankbtn->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onBtnClick, this));
	rankbtn->setTag(1004);
	rankbtn->setVisible(false);

	GlobalInstance::supperbossinfo.bosslefthp = 0;
	GlobalInstance::challangeType = CH_NORMAL;
	if (mainmapid.compare("m1-5") == 0 || mainmapid.compare("m1-6") == 0)
	{
		descstr = GlobalInstance::map_AllResources[mainmapid].desc;
		std::string strkey = StringUtils::format("%sdesc1", mainmapid.c_str());
		std::string str1 = CommonFuncs::replace_all(ResourceLang::map_lang[strkey], "\\n", "\n");
		desclbl_1->setString(str1);
		Label* ldsc = (Label*)desclbl_1->getVirtualRenderer();
		ldsc->setLineSpacing(20);
		first3starboxnode->setVisible(false);
		csbnode->getChildByName("starnode")->setVisible(false);

		if (GlobalInstance::map_mopuprwds.find(mapid) != GlobalInstance::map_mopuprwds.end())
		{
			strkey = StringUtils::format("%smopuphint", mainmapid.c_str());
			hintlbl->setString(ResourceLang::map_lang[strkey]);

			showMopUpRwd();
			updatelabel(0);
			this->schedule(schedule_selector(StarDescLayer::updatelabel), 1);
			if (!DataSave::getInstance()->getFightWinAllBoss(m_mapid))
			{
				mopupbtnnode->setVisible(false);
				actionbtn->setPositionX(360);
			}
		}
		else
		{
			hintlbl->setVisible(false);
			mopupbtnnode->setVisible(false);
			actionbtn->setPositionX(360);
			actionbtn->setEnabled(false);
			mopupnode->setVisible(false);

			if (m_mapid.compare("m1-6-1") == 0)
			{
				GlobalInstance::challangeType = CH_SUPERBOSS;
				mopupnode->setVisible(true);
				mopupnode->getChildByName("r001count")->setVisible(false);
				mopupnode->getChildByName("r001")->setVisible(false);
				mopupnode->getChildByName("r001desc")->setVisible(false);

				cocos2d::ui::Text* awddesctext = (cocos2d::ui::Text*)mopupnode->getChildByName("awddesctext");
				awddesctext->setString(ResourceLang::map_lang["superbossawdboxdesc"]);
				showSupperBossRwd();

				rulebtn->setVisible(true);
				rankbtn->setVisible(true);

				WaitingProgress* waitbox = WaitingProgress::create(ResourceLang::map_lang["doingtext"]);
				Director::getInstance()->getRunningScene()->addChild(waitbox, 1, "waitbox");
				HttpDataSwap::init(this)->getSupperBossInfo();
			}
		}
	}
	else
	{
		desclbl_1->setVisible(false);
		for (unsigned int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
		{
			if (GlobalInstance::vec_TaskMain[i].place.compare(mapid) == 0)
			{
				descstr = GlobalInstance::vec_TaskMain[i].desc;
				break;
			}
		}

		GlobalInstance::vec_stardata.clear();
		GlobalInstance::curMapFinishStars = 0;

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
			if (cid == SA_NODEATH || cid == SA_GETALLBOX)
				sdata.finishcount = -1;
			else
				sdata.finishcount = 0;

			GlobalInstance::vec_stardata.push_back(sdata);
			std::string key = StringUtils::format("star%d", i);
			star[i] = csbnode->getChildByName("starnode")->getChildByName(key);
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
			cocos2d::ui::Text* ctext = (cocos2d::ui::Text*)csbnode->getChildByName("starnode")->getChildByName(keyname);
			ctext->setString(content);

			if (GlobalInstance::vec_stardata[i].finishcount >= GlobalInstance::vec_stardata[i].needcount)
			{
				star[i]->setVisible(true);
				GlobalInstance::curMapFinishStars++;
			}
		}

		
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)first3starboxnode->getChildByName("resbox");
		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
		cocos2d::ui::Text* rescountlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");
		cocos2d::ui::Text* resname = (cocos2d::ui::Text*)resbox->getChildByName("name");

		if (GlobalInstance::curMapFinishStars >= 3 && GlobalInstance::map_mopuprwds.find(mapid) != GlobalInstance::map_mopuprwds.end())
		{
			first3starboxnode->setVisible(false);

			showMopUpRwd();
			updatelabel(0);
			this->schedule(schedule_selector(StarDescLayer::updatelabel), 1);
		}
		else
		{
			mopupnode->setVisible(false);
			actionbtn->setPositionX(360);
			mopupbtnnode->setVisible(false);
		}

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
		else if (t == T_EPIECE)
		{
			Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
			pieceicon->setAnchorPoint(Vec2(0, 1));
			pieceicon->setPosition(10, resbox->getContentSize().height - 10);
			resbox->addChild(pieceicon);
		}
		resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);

		CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

		std::string residstr = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);// StringUtils::format("ui/%s.png", resid.c_str());
		res->loadTexture(residstr, cocos2d::ui::Widget::TextureResType::PLIST);

		std::string countstr = StringUtils::format("%d", rescount);
		rescountlbl->setString(countstr);

		if (GlobalInstance::map_mopuprwds.find(mapid) != GlobalInstance::map_mopuprwds.end())
		{
			hintlbl->setString(ResourceLang::map_lang["finish3starmopuphint"]);
		}
		else
		{
			hintlbl->setVisible(false);
		}
	}

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setString(descstr);
	Label* ldsc = (Label*)desclbl->getVirtualRenderer();
	ldsc->setLineSpacing(8);

	this->scheduleOnce(schedule_selector(StarDescLayer::delayShowNewerGuide), newguidetime);
	//屏蔽下层点击
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
			if (GlobalInstance::challangeType == CH_SUPERBOSS)
			{
				long long int timeSec = GlobalInstance::servertime + 8 * 60 * 60;
				time_t timep = timeSec;
				tm* timeStuct = gmtime(&timep);

				if (timeStuct->tm_hour < GlobalInstance::supperbossinfo.starthour || timeStuct->tm_hour >= GlobalInstance::supperbossinfo.endhour)
				{
					std::string str = StringUtils::format(ResourceLang::map_lang["supperbosstimetext"].c_str(), GlobalInstance::supperbossinfo.starthour, GlobalInstance::supperbossinfo.endhour);
					MovingLabel::show(str);
					return;
				}

				int count = 0;
				for (int i = 0; i < 6; i++)
				{
					if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getLevel() >= 24)
						count++;
				}
				if (count <= 0)
				{
					MovingLabel::show(ResourceLang::map_lang["supperbossherolvlimit"]);
					return;
				}

				GlobalInstance::supperbossinfo.curhurt = 0;
			}
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
						dv.setValue(GlobalInstance::myCardHeros[i]->getPower().getValue() - needph);
						GlobalInstance::myCardHeros[i]->setPower(dv);
						GlobalInstance::getInstance()->saveHero(GlobalInstance::myCardHeros[i]);

						if (GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
							GlobalInstance::takeoutherocount++;
					}
				}

				btnnode->setEnabled(false);
				GlobalInstance::ishasmazeentry = GlobalInstance::getInstance()->createRandomNum(100) < 50 ? true : false;
				GlobalInstance::starcontinuefightsucccount = 0;
				GlobalInstance::curmapcontinuefightsucccount = 0;
				GlobalInstance::fightwinbosscount = 0;
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
		case 1002:
			mopUpAction();
			break;
		case 1003:
		{
			SupperBossRuleLayer* layer = SupperBossRuleLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
			break;
		case 1004:
		{
			SupperBossRankLayer* layer = SupperBossRankLayer::create(true);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
			break;
		default:
			break;
		}
	}
}

void StarDescLayer::showMopUpRwd()
{
	for (unsigned int i = 0; i < 8; i++)
	{
		std::string resboxname = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)mopupnode->getChildByName(resboxname);

		if (i < GlobalInstance::map_mopuprwds[m_mapid].vec_rwdstr.size())
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(GlobalInstance::map_mopuprwds[m_mapid].vec_rwdstr[i], vec_tmp, "-");

			std::string resid = vec_tmp[0];


			std::string boxstr = "ui/resbox.png";
			int t = 0;
			int qu = atoi(vec_tmp[2].c_str());
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
			else if (t == T_EPIECE)
			{
				Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
				pieceicon->setAnchorPoint(Vec2(0, 1));
				pieceicon->setPosition(10, resbox->getContentSize().height - 10);
				resbox->addChild(pieceicon);
			}

			resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
			resbox->setTag(i);
			resbox->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onresClick, this));

			CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

			cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
			std::string residstr = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);//StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(residstr, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			resbox->setVisible(false);
		}
	}
}

void StarDescLayer::showSupperBossRwd()
{
	std::string mainmapid = m_mapid.substr(0, m_mapid.find_last_of("-"));
	for (unsigned int i = 0; i < 8; i++)
	{
		std::string resboxname = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)mopupnode->getChildByName(resboxname);

		if (i < GlobalInstance::map_mapsdata[mainmapid].map_sublist[m_mapid].vec_awd.size())
		{
			std::string resid = GlobalInstance::map_mapsdata[mainmapid].map_sublist[m_mapid].vec_awd[i];


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
			else if (t == T_EPIECE)
			{
				Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
				pieceicon->setAnchorPoint(Vec2(0, 1));
				pieceicon->setPosition(10, resbox->getContentSize().height - 10);
				resbox->addChild(pieceicon);
			}

			resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
			resbox->setTag(i);
			resbox->addTouchEventListener(CC_CALLBACK_2(StarDescLayer::onresClick, this));

			CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

			cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
			std::string residstr = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);//StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(residstr, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			resbox->setVisible(false);
		}
	}
}

void StarDescLayer::mopUpAction()
{
	int leftcount = GlobalInstance::map_mopuprwds[m_mapid].leftcount;
	if (leftcount <= 0)
	{
		if (GlobalInstance::map_mopuprwds[m_mapid].resetcount <= 0)
		{
			MovingLabel::show(ResourceLang::map_lang["mopupnoresetcount"]);
		}
		else
		{
			std::string str = StringUtils::format(ResourceLang::map_lang["mopupresetcount"].c_str(), GlobalInstance::map_mopuprwds[m_mapid].resetcount);

			Layer* layer = HintBoxLayer::create(str, 14);
			layer->setUserData((void*)m_mapid.c_str());
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		return;
	}

	if (MyRes::getMyResCount("j003") <= 0)
	{
		MovingLabel::show(ResourceLang::map_lang["mopupj003less"]);

		std::vector< MSGAWDSDATA> vec_res;
		MSGAWDSDATA rdata;
		rdata.rid = "j003";
		rdata.count = 5;
		rdata.qu = 0;
		vec_res.push_back(rdata);
		BuyResLayer* layer = BuyResLayer::create(vec_res, MYSTORAGE, false);
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);

		return;
	}

	if (MyRes::getMyResCount("r001", MYPACKAGE) < GlobalInstance::map_mopuprwds[m_mapid].costfood)
	{
		std::string str = StringUtils::format(ResourceLang::map_lang["mopupfoodless"].c_str(), GlobalInstance::map_mopuprwds[m_mapid].costfood);
		MovingLabel::show(str);
		return;
	}

	int herocount = 0;
	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getState() != HS_DEAD)
			herocount++;
	}
	if (herocount < GlobalInstance::map_mopuprwds[m_mapid].needherocount)
	{
		std::string str = StringUtils::format(ResourceLang::map_lang["mopupcountless"].c_str(), GlobalInstance::map_mopuprwds[m_mapid].needherocount);
		MovingLabel::show(str);
		return;
	}

	if (!checkMopupHeroPower())
		return;

	std::vector<MSGAWDSDATA> vec_selrwd;

	for (unsigned int i = 0; i < GlobalInstance::map_mopuprwds[m_mapid].vec_rwdstr.size(); i++)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(1000);

		if (r < GlobalInstance::map_mopuprwds[m_mapid].vec_rnd[i])
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(GlobalInstance::map_mopuprwds[m_mapid].vec_rwdstr[i], vec_tmp, "-");

			MSGAWDSDATA data;
			data.rid = vec_tmp[0];
			data.qu = atoi(vec_tmp[2].c_str());

			std::vector<std::string> vec_;
			CommonFuncs::split(vec_tmp[1], vec_, ",");
			int minc = atoi(vec_[0].c_str());
			int maxc = atoi(vec_[1].c_str());
			data.count = GlobalInstance::getInstance()->createRandomNum(maxc + 1 - minc) + minc;
			vec_selrwd.push_back(data);
		}
	}

	MyRes::Use("j003", 1);
	GlobalInstance::map_mopuprwds[m_mapid].leftcount--;
	GlobalInstance::getInstance()->saveMopupLeftData(m_mapid);

	MyRes::Use("r001", GlobalInstance::map_mopuprwds[m_mapid].costfood, MYPACKAGE);

	MopupRewardLayer* layer = MopupRewardLayer::create(vec_selrwd, GlobalInstance::map_mopuprwds[m_mapid].bnsexp / herocount);
	this->addChild(layer);
	AnimationEffect::openAniEffect(layer);
}

void StarDescLayer::onresClick(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType type)
{

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		int tag = node->getTag();

		std::string resid;
		int qu = 0;
		if (GlobalInstance::challangeType == CH_SUPERBOSS)
		{
			std::string mainmapid = m_mapid.substr(0, m_mapid.find_last_of("-"));
			resid = GlobalInstance::map_mapsdata[mainmapid].map_sublist[m_mapid].vec_awd[tag];
		}
		else
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(GlobalInstance::map_mopuprwds[m_mapid].vec_rwdstr[tag], vec_tmp, "-");
			resid = vec_tmp[0];
			qu = atoi(vec_tmp[2].c_str());
		}

		int t = -1;
		for (int k = 0; k < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); k++)
		{
			if (resid.compare(0, 1, RES_TYPES_CHAR[k]) == 0)
			{
				t = k;
				break;
			}
		}

		if (t >= T_ARMOR && t <= T_NG)
		{
			Layer* layer = EquipDescLayer::create(resid, qu, 1);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		else
		{
			SimpleResPopLayer* layer = SimpleResPopLayer::create(resid, 3);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
	}
}

void StarDescLayer::updatelabel(float dt)
{
	std::string str = StringUtils::format(ResourceLang::map_lang["mopupleftcount"].c_str(), GlobalInstance::map_mopuprwds[m_mapid].leftcount);
	mopupleftcountlbl->setString(str);
	str = StringUtils::format("1/%d", MyRes::getMyResCount("j003"));
	j003leftcountlbl->setString(str);

	str = StringUtils::format("%d/%d", GlobalInstance::map_mopuprwds[m_mapid].costfood, MyRes::getMyResCount("r001", MYPACKAGE));
	packagefoodcountlbl->setString(str);

	if (MyRes::getMyResCount("r001", MYPACKAGE) >= GlobalInstance::map_mopuprwds[m_mapid].costfood)
	{
		packagefoodcountlbl->setTextColor(Color4B(92,46,13,255));
	}
	else
	{
		packagefoodcountlbl->setTextColor(Color4B::RED);
	}
}

bool StarDescLayer::checkMopupHeroPower()
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
				dv.setValue(GlobalInstance::myCardHeros[i]->getPower().getValue() - needph);
				GlobalInstance::myCardHeros[i]->setPower(dv);
				GlobalInstance::getInstance()->saveHero(GlobalInstance::myCardHeros[i]);
			}
		}
		return true;
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

		return false;
	}
}

void StarDescLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(FIRSTGUIDESTEP))
	{
		if (NewGuideLayer::checkifNewerGuide(6))
		{
			showNewerGuide(6);
		}
	}
}

void StarDescLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 6)
	{
		nodes.push_back(actionbtn);
	}
	if (g_NewGuideLayer == NULL)
	{
		g_NewGuideLayer = NewGuideLayer::create(step, nodes);
		this->addChild(g_NewGuideLayer, 10);
	}
}

void StarDescLayer::onFinish(int errcode)
{
	Director::getInstance()->getRunningScene()->removeChildByName("waitbox");

	if (errcode == 0)
	{
		std::string str = StringUtils::format(ResourceLang::map_lang["supperbossleftcounttext"].c_str(), GlobalInstance::supperbossinfo.leftfreecount);
		supperbossleftcountlbl->setString(str);

		std::vector<std::string> vec_;
		CommonFuncs::split(GlobalInstance::supperbossinfo.bossdata, vec_, "-");

		GlobalInstance::supperbossinfo.bosslv = atoi(vec_[0].c_str());
		GlobalInstance::supperbossinfo.bossmaxhp = atoi(vec_[1].c_str());

		GlobalInstance::supperbossinfo.bossdodge = atoi(vec_[2].c_str());
		vec_.clear();
		CommonFuncs::split(GlobalInstance::supperbossinfo.starendtime, vec_, "-");
		GlobalInstance::supperbossinfo.starthour = atoi(vec_[0].c_str());

		GlobalInstance::supperbossinfo.endhour = atoi(vec_[1].c_str());

		str = StringUtils::format(ResourceLang::map_lang["supperbosstimetext"].c_str(), GlobalInstance::supperbossinfo.starthour, GlobalInstance::supperbossinfo.endhour);
		supperbosstimelbl->setString(str);
		actionbtn->setEnabled(true);
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["matchnetworkerr"]);

		AnimationEffect::closeAniEffect(this);
	}

}