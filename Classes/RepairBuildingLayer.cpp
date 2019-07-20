#include "RepairBuildingLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "MainScene.h"
#include "DataSave.h"

USING_NS_CC;

RepairBuildingLayer::RepairBuildingLayer()
{

}

RepairBuildingLayer::~RepairBuildingLayer()
{

}


RepairBuildingLayer* RepairBuildingLayer::create(std::string buildingname, int type)
{
	RepairBuildingLayer*pRet = new(std::nothrow)RepairBuildingLayer();
	if (pRet && pRet->init(buildingname, type))
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
bool RepairBuildingLayer::init(std::string buildingname, int type)
{
	if (!Layer::init())
	{
		return false;
	}

	m_type = type;

	m_buildingname = buildingname;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("repairBuildingNode.csb"));
	csbnode->setPosition(Vec2(360, 128));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Text* titletext = (cocos2d::ui::Text*)csbnode->getChildByName("title");

	//按钮1
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(RepairBuildingLayer::onBtnClick, this));
	actionbtn->setTag(1000);
	//按钮1文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("btntext");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("repairbuildbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtntxt->ignoreContentAdaptWithSize(true);

	int rewardsize = 1;
	if (type < 2)
		rewardsize = GlobalInstance::map_buildingrepairdata[buildingname].vec_repairres.size();

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(RepairBuildingLayer::onBtnClick, this));
	closebtn->setTag(1001);

	timelbl = (cocos2d::ui::Text*)csbnode->getChildByName("timelbl");
	timelbl->setString("");

	std::string str;

	std::vector<std::string> vec_showres;
	if (type == 0)
		str = StringUtils::format(ResourceLang::map_lang["repairbuilding"].c_str(), GlobalInstance::map_AllResources[buildingname].name.c_str());
	else if (type == 1)
	{
		closebtn->setVisible(false);
		actionbtn->setTag(1002);
		str = StringUtils::format(ResourceLang::map_lang["repairbuildingawddesc"].c_str(), GlobalInstance::map_AllResources[buildingname].name.c_str());
		timelbl->setVisible(false); 
		actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("allgetbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		actionbtntxt->setScale(0.8f);
	}
	else if (type == 2)
	{
		actionbtn->setTag(1003);
		str = ResourceLang::map_lang["beggargivetext"].c_str();
		timelbl->setVisible(false);
		actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("givebeggar_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		std::vector<std::string> vec_res;
		for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
		{
			if ((MyRes::vec_MyResources[i]->getType() == T_RENS && (MyRes::vec_MyResources[i]->getId().compare("r001") >= 0 && MyRes::vec_MyResources[i]->getId().compare("r006") != 0 && MyRes::vec_MyResources[i]->getId().compare("r011") <= 0))
				|| MyRes::vec_MyResources[i]->getType() == T_RENS
				|| MyRes::vec_MyResources[i]->getType() == T_QH
				|| MyRes::vec_MyResources[i]->getType() == T_DAN
				|| MyRes::vec_MyResources[i]->getType() == T_MIJI
				|| MyRes::vec_MyResources[i]->getType() == T_BOX
				)
			{
				int count = 1;

				if (MyRes::vec_MyResources[i]->getCount().getValue() < 10)
					count = 1;
				else
					count = MyRes::vec_MyResources[i]->getCount().getValue() / 10;

				if (count > 10)
					count = 10;
				std::string str = StringUtils::format("%s-%d", MyRes::vec_MyResources[i]->getId().c_str(), count);
				vec_res.push_back(str);
			}
		}
		int solilercount = 1;

		if (GlobalInstance::getInstance()->getMySoliverCount().getValue() >= 1000)
			vec_res.push_back("r006-1000");
		if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= 10)
			vec_res.push_back("r012-10");

		int r = GlobalInstance::getInstance()->createRandomNum(vec_res.size());
		vec_showres.push_back(vec_res[r]);
	}
	titletext->setString(str);


	for (int i = 0; i < 3; i++)
	{
		std::string str = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");

		cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");
		cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)resbox->getChildByName("name");

		if (i < rewardsize)
		{
			std::string boxstr = "ui/resbox.png";
			int t = 0;
			int qu = 0;
			std::vector<std::string> vec_ret;
			if (type == 0)
				CommonFuncs::split(GlobalInstance::map_buildingrepairdata[buildingname].vec_repairres[i], vec_ret, "-");
			else if (type == 1)
				CommonFuncs::split(GlobalInstance::map_buildingrepairdata[buildingname].vec_adws[i], vec_ret, "-");
			else if (type == 2)
				CommonFuncs::split(vec_showres[i], vec_ret, "-");

			std::string resid = vec_ret[0];
			int count = atoi(vec_ret[1].c_str());

			MSGAWDSDATA resdata;
			resdata.rid = resid;
			resdata.count = count;
			vec_repairres.push_back(resdata);

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

			CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

			resbox->setPositionX(-240 + 140*i);
			resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
			std::string resstr = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);//StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

			namelbl->setString(GlobalInstance::map_AllResources[resid].name);
			std::string countstr = StringUtils::format("%d", count);
			countlbl->setString(countstr);
		}
		else
		{
			resbox->setVisible(false);
		}
	}

	if (m_type == 0 && GlobalInstance::map_buildingrepairdata[buildingname].state == 3)
	{
		actionbtn->setEnabled(false);
		upatetimelbl(0);
		this->schedule(schedule_selector(RepairBuildingLayer::upatetimelbl), 1.0f);
	}

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

void RepairBuildingLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btnnode = (cocos2d::ui::Button*)pSender;
		int tag = btnnode->getTag();

		if (tag == 1001)
			removeSelf();
		else if (tag == 1000)
		{
			bool isresenough = true;

			std::string laskresstr;
			for (unsigned int i = 0; i < vec_repairres.size(); i++)
			{
				std::string resid = vec_repairres[i].rid;
				int count = vec_repairres[i].count;
				if (resid.compare("r006") == 0)
				{
					if (vec_repairres[i].count > GlobalInstance::getInstance()->getMySoliverCount().getValue())
					{
						if (laskresstr.length() > 0)
							laskresstr.append(ResourceLang::map_lang["dunhao"]);
						laskresstr.append(GlobalInstance::map_AllResources[resid].name);
						isresenough = false;
					}
				}
				else if (resid.compare("r012") == 0)
				{
					if (vec_repairres[i].count > GlobalInstance::getInstance()->getMyCoinCount().getValue())
					{
						if (laskresstr.length() > 0)
							laskresstr.append(ResourceLang::map_lang["dunhao"]);
						laskresstr.append(GlobalInstance::map_AllResources[resid].name);
						isresenough = false;
					}
				}
				else if (vec_repairres[i].count > MyRes::getMyResCount(vec_repairres[i].rid))
				{
					if (laskresstr.length() > 0)
						laskresstr.append(ResourceLang::map_lang["dunhao"]);
					laskresstr.append(GlobalInstance::map_AllResources[resid].name);
					isresenough = false;
				}
			}
			if (!isresenough)
			{
				std::string showstr = StringUtils::format(ResourceLang::map_lang["notenouph"].c_str(), laskresstr.c_str());
				MovingLabel::show(showstr);
				return;
			}

			for (unsigned int i = 0; i < vec_repairres.size(); i++)
			{
				std::string resid = vec_repairres[i].rid;
				int count = vec_repairres[i].count;
				if (resid.compare("r006") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(count);
					GlobalInstance::getInstance()->costMySoliverCount(dvint);
				}
				else if (resid.compare("r012") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(count);
					GlobalInstance::getInstance()->costMyCoinCount(dvint);
				}
				else
				{
					MyRes::Use(resid, count);
				}
			}

			GlobalInstance::map_buildingrepairdata[m_buildingname].state = 3;
			GlobalInstance::map_buildingrepairdata[m_buildingname].repairtime = GlobalInstance::servertime;
			GlobalInstance::getInstance()->setBuildingBroken();
			btnnode->setEnabled(false);
			upatetimelbl(0);
			this->schedule(schedule_selector(RepairBuildingLayer::upatetimelbl), 1.0f);
		}

		else if (tag == 1002)
		{
			for (unsigned int i = 0; i < vec_repairres.size(); i++)
			{
				std::string resid = vec_repairres[i].rid;
				int count = vec_repairres[i].count;
				if (resid.compare("r006") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(count);
					GlobalInstance::getInstance()->addMySoliverCount(dvint);
				}
				else if (resid.compare("r012") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(count);
					GlobalInstance::getInstance()->addMyCoinCount(dvint);
				}
				else
				{
					MyRes::Add(resid, count);
				}
			}
			removeSelf();
		}
		else if (tag == 1003)
		{
			if (g_mainScene != NULL)
				g_mainScene->hideBeggar();

			DataSave::getInstance()->setHasBeggar(false);

			for (unsigned int i = 0; i < vec_repairres.size(); i++)
			{
				std::string resid = vec_repairres[i].rid;
				int count = vec_repairres[i].count;
				if (resid.compare("r006") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(count);
					GlobalInstance::getInstance()->costMySoliverCount(dvint);
				}
				else if (resid.compare("r012") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(count);
					GlobalInstance::getInstance()->costMyCoinCount(dvint);
				}
				else
				{
					MyRes::Use(resid, count);
				}
			}
			removeSelf();
		}
	}
}

void RepairBuildingLayer::upatetimelbl(float dt)
{
	int repairstate = GlobalInstance::map_buildingrepairdata[m_buildingname].state;
	if (repairstate > 0)
	{
		if (repairstate == 3)
		{
			int pasttime = GlobalInstance::servertime - GlobalInstance::map_buildingrepairdata[m_buildingname].repairtime;

			if (pasttime >= REPAIRTIME)
			{
				removeSelf();
			}
			else
			{
				timelbl->setVisible(true);
				int lefttime = REPAIRTIME - pasttime;
				std::string strlbl = StringUtils::format("%02d:%02d", lefttime / 60, lefttime % 60);
				timelbl->setString(strlbl);
			}
		}
	}
	else
	{
		removeSelf();
	}
}

void RepairBuildingLayer::removeSelf()
{
	timelbl->setVisible(false);
	AnimationEffect::closeAniEffect((Layer*)this);
}