#include "StarResultLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MovingLabel.h"
#include "MyTransitionScene.h"
#include "StarFrist3AwdLayer.h"

USING_NS_CC;

StarResultLayer::StarResultLayer()
{

}

StarResultLayer::~StarResultLayer()
{

}


StarResultLayer* StarResultLayer::create(std::string mapid, int towherescene)
{
	StarResultLayer *pRet = new(std::nothrow)StarResultLayer();
	if (pRet && pRet->init(mapid, towherescene))
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
bool StarResultLayer::init(std::string mapid, int towherescene)
{
	if (!Layer::init())
	{
		return false;
	}

	m_mapid = mapid;
	m_cwhere = towherescene;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("stargresultLayer.csb"));
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

	std::string mainmapid = m_mapid.substr(0, m_mapid.find_last_of("-"));
	
	Node* star[3];

	for (unsigned int i = 0; i < 3; i++)
	{
		std::string key = StringUtils::format("star%d", i);
		star[i] = csbnode->getChildByName(key);
		star[i]->setVisible(false);
	}

	int starcount = 0;

	for (unsigned int i = 0; i < GlobalInstance::map_stardata[m_mapid].size(); i++)
	{
		int cid = GlobalInstance::map_stardata[m_mapid][i].sid;
		int count = GlobalInstance::map_stardata[m_mapid][i].needcount;

		std::string keyname = StringUtils::format("stagestar%02d", cid);
		std::string content;

		if (GlobalInstance::map_stardata[m_mapid][i].needid.compare("0") == 0)
			content = StringUtils::format(ResourceLang::map_lang[keyname].c_str(), count);
		else
			content = StringUtils::format(ResourceLang::map_lang[keyname].c_str(), GlobalInstance::map_AllResources[GlobalInstance::map_stardata[m_mapid][i].needid].name.c_str(), count);


		keyname = StringUtils::format("c%d", i);
		cocos2d::ui::Text* ctext = (cocos2d::ui::Text*)csbnode->getChildByName(keyname);
		ctext->setString(content);

		if (GlobalInstance::map_stardata[m_mapid][i].status == 1)
		{
			star[i]->setVisible(true);
			starcount++;
		}
	}


	cocos2d::ui::ImageView* retimg =(cocos2d::ui::ImageView*)csbnode->getChildByName("retimg");
	retimg->ignoreContentAdaptWithSize(true);

	std::string retstr = StringUtils::format("starscore%d_text", starcount);
	retimg->loadTexture(ResourcePath::makeTextImgPath(retstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	besttextimg = csbnode->getChildByName("besttext");
	besttextimg->setVisible(false);
	besttextimg->setScale(7);

	//按钮1
	actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(StarResultLayer::onBtnClick, this));
	actionbtn->setTag(1000);


	//按钮1文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtntxt->setContentSize(Size(104, 50));

	if (starcount >= 3)
	{
		actionbtn->setVisible(false);
		this->scheduleOnce(schedule_selector(StarResultLayer::showAnim), 0.5f);
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

void StarResultLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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

			if (m_cwhere == 0 || m_cwhere == 1 || m_cwhere == 2)
			{
#if USE_TRANSCENE
				Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAIN)));
#else
				Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
#endif
			}
			else
			{
#if USE_TRANSCENE
				Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MyTransitionScene::createScene(TO_MAP)));
#else
				Director::getInstance()->replaceScene(TransitionFade::create(0.5f, MainMapScene::createScene()));
#endif
			}

		}
			break;
		default:
			break;
		}
	}
}

void StarResultLayer::showAnim(float dt)
{
	besttextimg->setVisible(true);

	besttextimg->runAction(ScaleTo::create(0.3f, 1));

	this->scheduleOnce(schedule_selector(StarResultLayer::showf3starAwd), 1.5f);
}

void StarResultLayer::showf3starAwd(float dt)
{
	actionbtn->setVisible(true);
	std::string mainmapid = m_mapid.substr(0, m_mapid.find_last_of("-"));

	std::string resstr = GlobalInstance::map_mapsdata[mainmapid].map_sublist[m_mapid].vec_f3starawds[0];
	StarFrist3AwdLayer* layer = StarFrist3AwdLayer::create(resstr);
	this->addChild(layer);
	AnimationEffect::openAniEffect(layer);
}