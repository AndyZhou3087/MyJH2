#include "ResDescLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "StoreHouseLayer.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "RewardLayer.h"
#include "NewGuideLayer.h"

USING_NS_CC;

ResDescLayer::ResDescLayer()
{
	status = S_OTHER;
}

ResDescLayer::~ResDescLayer()
{
	
}


ResDescLayer* ResDescLayer::create(ResBase* res, int fromwhere)
{
	ResDescLayer *pRet = new(std::nothrow)ResDescLayer();
	if (pRet && pRet->init(res, fromwhere))
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
bool ResDescLayer::init(ResBase* res, int fromwhere)
{
    if ( !Layer::init() )
    {
        return false;
    }
	m_fromwhere = fromwhere;
	m_res = res;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("resDescLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");

	std::string boxstr = "ui/resbox.png";
	int t = 0;
	for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
	{
		if (m_res->getId().compare(0, 1, RES_TYPES_CHAR[t]) == 0)
			break;
	}
	if (t == T_RENS || t == T_DAN || t == T_MIJI || t == T_BOX)
	{
		int qu = atoi(m_res->getId().substr(1).c_str()) - 1;
		boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		CommonFuncs::playResBoxEffect(resbox, qu);
	}

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/%s.png", res->getId().c_str());
	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);


	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[res->getId()].name);

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desclbl");
	std::string descstr = GlobalInstance::map_AllResources[res->getId()].desc;

	Label* descRenderLable = (Label*)desclbl->getVirtualRenderer();
	descRenderLable->setLineSpacing(10);

	str = StringUtils::format(ResourceLang::map_lang["rescount"].c_str(), res->getCount().getValue());

	cocos2d::ui::Text* coutlbl = (cocos2d::ui::Text*)csbnode->getChildByName("coutlbl");
	coutlbl->setString(str);

	//按钮
	actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(ResDescLayer::onBtnClick, this));
	
	std::string btntextstr;

	if (m_fromwhere == 1)
	{
		int r = GlobalInstance::getInstance()->createRandomNum(100);

		int index = 0;
		for (unsigned int i = 0; i < GlobalInstance::map_TBoxs[res->getId()].vec_rnd.size(); i++)
		{
			if (r < GlobalInstance::map_TBoxs[res->getId()].vec_rnd[i])
			{
				index = i;
				break;
			}
		}
		std::string awd = GlobalInstance::map_TBoxs[res->getId()].vec_awds[index];
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(awd, vec_tmp, "-");
		
		std::string awdres = vec_tmp[0];
		int count = atoi(vec_tmp[1].c_str());
		int qu = atoi(vec_tmp[2].c_str());

		std::string str = StringUtils::format("ui/%s.png", awdres.c_str());
		p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

		namelbl->setString(GlobalInstance::map_AllResources[awdres].name);

		str = StringUtils::format(ResourceLang::map_lang["rescount"].c_str(), count);
		coutlbl->setString(str);

		MyRes::Use(res->getId(), 1, MYSTORAGE);

		int stc = GlobalInstance::getInstance()->generateStoneCount(qu);
		MyRes::Add(awdres, count, MYSTORAGE, qu, stc);

		if (g_mainScene != NULL)
		{
			StoreHouseLayer* storeHouseLayer = (StoreHouseLayer*)g_mainScene->getChildByName("3storehouse");
			if (storeHouseLayer != NULL)
				storeHouseLayer->updateUI();
		}

		btntextstr = "closebtn_text";
		descstr = GlobalInstance::map_AllResources[awdres].desc;
	}
	if (m_fromwhere == 0)
	{
		if (res->getType() == T_BOX)
		{
			btntextstr = "usebtn_text";
			status = S_CAN_USE;

			std::string resname;

			int awdszie = GlobalInstance::map_TBoxs[res->getId()].vec_awds.size();
			for (int i = 0; i < awdszie; i++)
			{
				std::string awd = GlobalInstance::map_TBoxs[res->getId()].vec_awds[i];
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(awd, vec_tmp, "-");

				resname.append(GlobalInstance::map_AllResources[vec_tmp[0]].name);
				if (i < awdszie - 1)
					resname.append(ResourceLang::map_lang["dunhao"]);

			}
			descstr = StringUtils::format(ResourceLang::map_lang["tboxdesc"].c_str(), resname.c_str());
		}
		else if (res->getType() == T_TLMED)
		{
			btntextstr = "usebtn_text";
			status = S_CAN_USE;
		}
		else if (res->getType() == T_VSION)
		{
			std::string visonstr = StringUtils::format("%s%d", ResourceLang::map_lang["lvtexts"].c_str(), res->getCount().getValue());
			coutlbl->setString(visonstr);
		}
		else
		{
			if (GlobalInstance::map_AllResources[res->getId()].vec_needres.size() > 0)
			{
				status = S_CAN_DECOMPOSE;
				btntextstr = "decomposebtn_text";
			}
			else
				btntextstr = "closebtn_text";

		}
	}
	else if (m_fromwhere == 2)
	{
		btntextstr = "closebtn_text";
	}

	desclbl->setString(descstr);

	if (btntextstr.length() > 0)
	{
		//按钮文字
		cocos2d::ui::ImageView* btntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
		btntxt->loadTexture(ResourcePath::makeTextImgPath(btntextstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
	else
	{
		actionbtn->setVisible(false);
	}

	this->scheduleOnce(schedule_selector(ResDescLayer::delayShowNewerGuide), newguidetime);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void ResDescLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(89))
	{
		if (NewGuideLayer::checkifNewerGuide(90))
		{
			showNewerGuide(90);
		}
	}
}

void ResDescLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 90)
	{
		nodes.push_back(actionbtn);
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void ResDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (status == S_CAN_DECOMPOSE)
		{
			StoreHouseLayer* storelayer = (StoreHouseLayer*)this->getParent();
			if (storelayer != NULL)
			{
				storelayer->decompose(m_res);
			}
		}
		else if (status == S_CAN_USE)
		{
			if (m_res->getType() == T_TLMED)
			{
				int count = 0;
				for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
				{
					Hero* hero = GlobalInstance::vec_myHeros[i];
					if (hero->getPower().getValue() >= 100)
					{
						count++;
					}
					else if (hero->getPower().getValue() > 50)
					{
						DynamicValueInt dal;
						dal.setValue(100);
						hero->setPower(dal);
					}
					else
					{
						DynamicValueInt dal;
						dal.setValue(hero->getPower().getValue() + 50);
						hero->setPower(dal);
					}
				}
				if (count == GlobalInstance::vec_myHeros.size())
				{
					MovingLabel::show(ResourceLang::map_lang["powerlimit"]);
					return;
				}
				MyRes::Use("p001");

				StoreHouseLayer* storelayer = (StoreHouseLayer*)this->getParent();
				if (storelayer != NULL)
					storelayer->updateUI();
			}
			else if (m_res->getType() == T_BOX)
			{
				StoreHouseLayer* storelayer = (StoreHouseLayer*)this->getParent();

				std::vector<MSGAWDSDATA> vec_rewards;

				int awdszie = GlobalInstance::map_TBoxs[m_res->getId()].vec_awds.size();

				int r = GlobalInstance::getInstance()->createRandomNum(awdszie);

				std::string awd = GlobalInstance::map_TBoxs[m_res->getId()].vec_awds[r];
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(awd, vec_tmp, "-");

				int qu = atoi(vec_tmp[2].c_str());

				MSGAWDSDATA wdata;
				wdata.rid = vec_tmp[0];
				wdata.count = atoi(vec_tmp[1].c_str());
				wdata.qu = qu;
				vec_rewards.push_back(wdata);
				RewardLayer* layer = RewardLayer::create(vec_rewards);
				storelayer->addChild(layer);
				AnimationEffect::openAniEffect(layer);

				MyRes::Use(m_res->getId());

				if (storelayer != NULL)
					storelayer->updateUI();
			}
		}

		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

void ResDescLayer::onExit()
{
	Layer::onExit();
}