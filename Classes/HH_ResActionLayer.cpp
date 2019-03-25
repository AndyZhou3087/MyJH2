#include "HH_ResActionLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "HomeHillLayer2.h"
#include "ResCreator.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "NewGuideLayer.h"
#include "BuyCoinLayer.h"
#include "MainScene.h"
#include "LoadingBarProgressTimer.h"

USING_NS_CC;

HH_ResActionLayer::HH_ResActionLayer()
{

}

HH_ResActionLayer::~HH_ResActionLayer()
{
	if ((!NewGuideLayer::checkifNewerGuide(18)) || (!NewGuideLayer::checkifNewerGuide(19) && NewGuideLayer::checkifNewerGuide(20)))
	{
		HomeHillLayer2* layer = (HomeHillLayer2*)g_mainScene->getChildByName("7homehill");
		if (layer != NULL)
		{
			layer->delayShowNewerGuide(0);
		}
	}
	//else if (!NewGuideLayer::checkifNewerGuide(75) && NewGuideLayer::checkifNewerGuide(76))
	//{
	//	SmithyLayer* layer = (SmithyLayer*)g_mainScene->getChildByName("2smithy");
	//	if (layer != NULL)
	//	{
	//		layer->delayShowNewerGuide(0);
	//	}
	//}
}


HH_ResActionLayer* HH_ResActionLayer::create(ResCreator* rcreator, HH_ACTIONTYPE actiontype)
{
	HH_ResActionLayer *pRet = new(std::nothrow)HH_ResActionLayer();
	if (pRet && pRet->init(rcreator, actiontype))
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
bool HH_ResActionLayer::init(ResCreator* rcreator, HH_ACTIONTYPE actiontype)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_actiontype = actiontype;
	m_rescreator = rcreator;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("hhreslvlayer.csb"));
	this->addChild(csbnode);

	Node* lvinfonode = csbnode->getChildByName("lvinfonode");
	Node* newfarmernode = csbnode->getChildByName("newfarmernode");
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("name");

	std::string btn1_text;//按钮1中的文字图片
	std::string btn2_text;//按钮2中的文字图
	std::string titlestr;
	if (actiontype == HH_EMPLOYFARMER)//雇佣工人
	{
		btn1_text = "recruitbtn_text";
		btn2_text = "drrecruitbtn_text";

		titlestr = "hh_addfm_text";

		needresid = "r001";

		lvinfonode->setVisible(false);
		newfarmernode->setVisible(true);

		cocos2d::ui::Text* desc = (cocos2d::ui::Text*)newfarmernode->getChildByName("desc");
		desc->setString(ResourceLang::map_lang["addfarmerdesc"]);

		cocos2d::ui::Text* addfmcount = (cocos2d::ui::Text*)newfarmernode->getChildByName("addfmcount");
		std::string str = StringUtils::format("+%d", 5);
		addfmcount->setString(str);
	}
	else if (actiontype == HH_RESCREATORLVUP)//升级资源容量
	{
		btn1_text = "lvupbtn_text";
		btn2_text = "drlvupbtn_text";

		needresid = "r002";

		titlestr = StringUtils::format("hh_info_%stext", rcreator->getName().c_str());

		lvinfonode->setVisible(true);
		newfarmernode->setVisible(false);
		cocos2d::ui::ImageView* store_text = (cocos2d::ui::ImageView*)lvinfonode->getChildByName("store_text");
		store_text->loadTexture(ResourcePath::makeTextImgPath("hh_info_store_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::ImageView* maxfm_text = (cocos2d::ui::ImageView*)lvinfonode->getChildByName("maxfm_text");
		maxfm_text->loadTexture(ResourcePath::makeTextImgPath("hh_info_maxfm_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

		//int maxcap = rcreator->getMaxCap(RESCREATOREMAXLV - 1).getValue();

		int percent = rcreator->getLv().getValue() / 10 * 20;

		maxcapbar1 = (cocos2d::ui::LoadingBar*)lvinfonode->getChildByName("maxcapbar1");
		//maxcapbar1->setPercent(rcreator->getMaxCap(rcreator->getLv().getValue() + 1).getValue()*100/ maxcap);
		maxcapbar1->setPercent(percent + 10);
		maxcapbar = (cocos2d::ui::LoadingBar*)lvinfonode->getChildByName("maxcapbar");
		//maxcapbar->setPercent(rcreator->getMaxCap(rcreator->getLv().getValue()).getValue() * 100 / maxcap);

		if (percent<= 0)
			maxcapbar->setPercent(5);
		else
			maxcapbar->setPercent(percent);
		int maxfms = rcreator->getMaxFarmersCountByLv(RESCREATOREMAXLV - 1).getValue();

		mafmbar1 = (cocos2d::ui::LoadingBar*)lvinfonode->getChildByName("mafmbar1");
		mafmbar1->setPercent(rcreator->getMaxFarmersCountByLv(rcreator->getLv().getValue() + 1).getValue() * 100 / maxfms);

		mafmbar = (cocos2d::ui::LoadingBar*)lvinfonode->getChildByName("mafmbar");
		mafmbar->setPercent(rcreator->getMaxFarmersCountByLv(rcreator->getLv().getValue()).getValue() * 100 / maxfms);

		maxcapval = (cocos2d::ui::Text*)lvinfonode->getChildByName("maxcapval");
		int s = rcreator->getMaxCap(rcreator->getLv().getValue() + 1).getValue() - rcreator->getMaxCap(rcreator->getLv().getValue()).getValue();
		std::string str = StringUtils::format("%d +%d", rcreator->getMaxCap(rcreator->getLv().getValue()).getValue(), s);
		maxcapval->setString(str);

		storeval = (cocos2d::ui::Text*)lvinfonode->getChildByName("storeval");
		s = rcreator->getMaxFarmersCountByLv(rcreator->getLv().getValue() + 1).getValue() - rcreator->getMaxFarmersCountByLv(rcreator->getLv().getValue()).getValue();
		str = StringUtils::format("%d +%d", rcreator->getMaxFarmersCountByLv(rcreator->getLv().getValue()).getValue(), s);
		storeval->setString(str);

	}
	//标题
	title->loadTexture(ResourcePath::makeTextImgPath(titlestr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	
	cocos2d::ui::ImageView* needres = (cocos2d::ui::ImageView*)csbnode->getChildByName("needres");
	std::string neeresstr = StringUtils::format("ui/%s.png", needresid.c_str());
	needres->loadTexture(neeresstr, cocos2d::ui::Widget::TextureResType::PLIST);
	needres->setScale(0.5f);

	//按钮1
	cocos2d::ui::Widget* lvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvupbtn->setTag(1000);
	lvupbtn->addTouchEventListener(CC_CALLBACK_2(HH_ResActionLayer::onBtnClick, this));
	//按钮1文字
	cocos2d::ui::ImageView* lvupbtntxt = (cocos2d::ui::ImageView*)lvupbtn->getChildByName("text");
	lvupbtntxt->loadTexture(ResourcePath::makeTextImgPath(btn1_text, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* drlvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("drlvupbtn");
	drlvupbtn->setTag(1001);
	drlvupbtn->addTouchEventListener(CC_CALLBACK_2(HH_ResActionLayer::onBtnClick, this));
	//按钮2文字
	cocos2d::ui::ImageView* drlvupbtntxt = (cocos2d::ui::ImageView*)drlvupbtn->getChildByName("text");
	drlvupbtntxt->loadTexture(ResourcePath::makeTextImgPath(btn2_text, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1002);
	closebtn->addTouchEventListener(CC_CALLBACK_2(HH_ResActionLayer::onBtnClick, this));
	
	coincountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");

	neddrescountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("needrescount");

	updateLabelCount();
	updateLabelColor(0);
	this->schedule(schedule_selector(HH_ResActionLayer::updateLabelColor), 1);

	this->scheduleOnce(schedule_selector(HH_ResActionLayer::delayShowNewerGuide), newguidetime);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		//AnimationEffect::closeAniEffect((Layer*)this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void HH_ResActionLayer::delayShowNewerGuide(float dt)
{
	/*if (GlobalInstance::getInstance()->getUnlockHomehillCondition() && !NewGuideLayer::checkifNewerGuide(15))
	{*/
	if (!NewGuideLayer::checkifNewerGuide(15))
	{
		if (NewGuideLayer::checkifNewerGuide(17))
		{
			showNewerGuide(17);
		}
		else if (NewGuideLayer::checkifNewerGuide(19))
		{
			showNewerGuide(19);
		}
	}
	else if (!NewGuideLayer::checkifNewerGuide(74))
	{
		if (NewGuideLayer::checkifNewerGuide(75))
		{
			showNewerGuide(75);
		}
	}
}

void HH_ResActionLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 17 || step == 19 || step == 75)
	{
		nodes.push_back(csbnode->getChildByName("lvupbtn"));
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void HH_ResActionLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 1000://升级
			if (costresdv.getValue() <= MyRes::getMyResCount(needresid))//资源足够
			{
				//减掉资源
				MyRes::Use(needresid, costresdv.getValue());
				action();
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["reslack"]);
			}
			break;
		case 1001://直接升级
			{
				if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= costcoindv.getValue())
				{
					GlobalInstance::getInstance()->costMyCoinCount(costcoindv);
					action();

					GlobalInstance::getInstance()->setPosCostCoin(COSTCOIN_HILL, costcoindv.getValue());
				}
				else
				{
					MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);

					Layer* layer = BuyCoinLayer::create(costcoindv.getValue() - GlobalInstance::getInstance()->getMyCoinCount().getValue());
					Director::getInstance()->getRunningScene()->addChild(layer, 100, "buycoinlayer");
				}
			}
			break;
		case 1002:
			AnimationEffect::closeAniEffect(this);
			break;
		default:
			break;
		}
	}
}

void HH_ResActionLayer::action()
{
	if (m_actiontype == HH_EMPLOYFARMER)
	{
		//工人数增加5
		GlobalInstance::getInstance()->saveTotalFarmers(GlobalInstance::getInstance()->getTotalFarmers() + 5);
		updateLabelCount();
		AnimationEffect::closeAniEffect(this);
	}
	else if (m_actiontype == HH_RESCREATORLVUP)
	{
		ResCreator* rdata = m_rescreator;
		DynamicValueInt dlv;
		dlv.setValue(rdata->getLv().getValue() + 1);
		rdata->setLv(dlv);
		GlobalInstance::getInstance()->saveResCreatorData();
		CommonFuncs::playCommonLvUpAnim(this, "texiao_sjcg");

		if (dlv.getValue() >= RESCREATOREMAXLV - 1)
			AnimationEffect::closeAniEffect(this);
		else
			updateLabelCount();
	}
}

void HH_ResActionLayer::updateLabelColor(float dt)
{
	//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];

	if (costresdv.getValue() > MyRes::getMyResCount(needresid))
	{
		neddrescountlbl->setTextColor(Color4B(255, 0, 0, 255));
	}
	else
	{
		neddrescountlbl->setTextColor(Color4B(0, 0, 0, 255));
	}
	if (costcoindv.getValue() > GlobalInstance::getInstance()->getMyCoinCount().getValue())
	{
		coincountlbl->setTextColor(Color4B(255, 0, 0, 255));
	}
	else
	{
		coincountlbl->setTextColor(Color4B(0, 0, 0, 255));
	}
}

void HH_ResActionLayer::updateLabelCount()
{
	if (m_actiontype == HH_EMPLOYFARMER)//雇佣工人
	{
		int employcount = GlobalInstance::getInstance()->getTotalFarmers();
		costresdv.setValue((GlobalInstance::vec_resCreators[0]->getMaxCap(employcount / 5 - 1).getValue()) * 5 / 10);
		costcoindv.setValue(GlobalInstance::map_AllResources[needresid].silverval * costresdv.getValue() / 100);
	}
	else if (m_actiontype == HH_RESCREATORLVUP)//升级资源容量
	{
		ResCreator* rdata = m_rescreator;
		std::string rdataname = rdata->getName();

		int creatorindex = atoi(rdataname.substr(3).c_str()) - 1;
		costresdv.setValue(rdata->getMaxCap(rdata->getLv().getValue()).getValue() * creatorResLvUpPara[creatorindex]);

		costcoindv.setValue(GlobalInstance::map_AllResources[needresid].silverval * costresdv.getValue() / 100);

		int percent = rdata->getLv().getValue() / 10 * 20;

		maxcapbar1->runAction(LoadingBarProgressTo::create(0.2f, percent + 10));

		if (percent <= 0)
			maxcapbar->runAction(LoadingBarProgressTo::create(0.2f, 5));
		else
			maxcapbar->runAction(LoadingBarProgressTo::create(0.2f, percent));

		int maxfms = rdata->getMaxFarmersCountByLv(RESCREATOREMAXLV - 1).getValue();

		mafmbar1->runAction(LoadingBarProgressTo::create(0.2f, rdata->getMaxFarmersCountByLv(rdata->getLv().getValue() + 1).getValue() * 100 / maxfms));

		mafmbar->runAction(LoadingBarProgressTo::create(0.2f, rdata->getMaxFarmersCountByLv(rdata->getLv().getValue()).getValue() * 100 / maxfms));

		int s = rdata->getMaxCap(rdata->getLv().getValue() + 1).getValue() - rdata->getMaxCap(rdata->getLv().getValue()).getValue();
		std::string str = StringUtils::format("%d +%d", rdata->getMaxCap(rdata->getLv().getValue()).getValue(), s);
		maxcapval->setString(str);

		s = rdata->getMaxFarmersCountByLv(rdata->getLv().getValue() + 1).getValue() - rdata->getMaxFarmersCountByLv(rdata->getLv().getValue()).getValue();
		str = StringUtils::format("%d +%d", rdata->getMaxFarmersCountByLv(rdata->getLv().getValue()).getValue(), s);
		storeval->setString(str);
	}

	std::string coinstr = StringUtils::format("x%d", costcoindv.getValue());
	coincountlbl->setString(coinstr);

	std::string rescountstr = StringUtils::format("x%d", costresdv.getValue());
	neddrescountlbl->setString(rescountstr);
}

void HH_ResActionLayer::showNextLvUi()
{

}

void HH_ResActionLayer::onExit()
{
	Layer::onExit();
}


