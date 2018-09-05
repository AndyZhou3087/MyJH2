#include "SelectSubMapLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MapBlockScene.h"
#include "Resource.h"
#include "MovingLabel.h"
#include "SoundManager.h"

USING_NS_CC;

SelectSubMapLayer::SelectSubMapLayer()
{

}

SelectSubMapLayer::~SelectSubMapLayer()
{
	
}


SelectSubMapLayer* SelectSubMapLayer::create(std::string mapid)
{
	SelectSubMapLayer *pRet = new(std::nothrow)SelectSubMapLayer();
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
bool SelectSubMapLayer::init(std::string mainmapid)
{
    if ( !Layer::init() )
    {
        return false;
    }
	m_mainmapid = mainmapid;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("selectSubMapLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("submaptitle_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮
	cocos2d::ui::Widget* backbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("backbtn");
	backbtn->addTouchEventListener(CC_CALLBACK_2(SelectSubMapLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* backbtntxt = (cocos2d::ui::ImageView*)backbtn->getChildByName("text");
	backbtntxt->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	cocos2d::ui::Widget* morearrow = (cocos2d::ui::Widget*)csbnode->getChildByName("morearrow");

	int size = GlobalInstance::map_mapsdata[mainmapid].map_sublist.size();

	if (size > 5)
	{
		morearrow->setVisible(true);
		morearrow->runAction(RepeatForever::create(Blink::create(1, 8)));
	}
	else
	{
		morearrow->setVisible(false);
	}

	int itemheight = 170;
	int innerheight = itemheight * size;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(720, innerheight));

	int i = 0;
	std::map<std::string, S_SubMap>::iterator it;
	for (it = GlobalInstance::map_mapsdata[mainmapid].map_sublist.begin(); it != GlobalInstance::map_mapsdata[mainmapid].map_sublist.end(); it++)
	{
		Node* subnode = CSLoader::createNode(ResourcePath::makePath("selectMapNode.csb"));

		subnode->setPosition(Vec2(scrollView->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
		subnode->runAction(EaseSineIn::create(MoveBy::create(0.15f + i*0.07f, Vec2(-scrollView->getContentSize().width / 2 - 600, 0))));

		//subnode->setPosition(Vec2(scrollView->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		scrollView->addChild(subnode);

		cocos2d::ui::Text* mapname = (cocos2d::ui::Text*)subnode->getChildByName("namelbl");
		mapname->setString(GlobalInstance::map_AllResources[it->first].name);

		if (GlobalInstance::myCurMainData.place.compare(it->first) == 0 && GlobalInstance::myCurMainData.isfinish == QUEST_ACC)
		{
			cocos2d::ui::ImageView* taskicon = (cocos2d::ui::ImageView*)subnode->getChildByName("taskicon");
			taskicon->setVisible(true);
			taskicon->setPositionX(mapname->getPositionX() + mapname->getContentSize().width / 2 + 30);
		}

		cocos2d::ui::Text* hpdesc = (cocos2d::ui::Text*)subnode->getChildByName("hpdesc");
		std::string strdesc = StringUtils::format(ResourceLang::map_lang["hpdesc"].c_str(), GlobalInstance::map_mapsdata[mainmapid].map_sublist[it->first].ph);
		hpdesc->setString(strdesc);

		cocos2d::ui::Text* awarddesc = (cocos2d::ui::Text*)subnode->getChildByName("awarddesc");
		std::string tmp;
		int awdsize = GlobalInstance::map_mapsdata[mainmapid].map_sublist[it->first].vec_awd.size();
		for (int i= 0; i < awdsize; i++)
		{
			std::string resid = GlobalInstance::map_mapsdata[mainmapid].map_sublist[it->first].vec_awd[i];

			tmp.append(GlobalInstance::map_AllResources[resid].name);
			if (i < awdsize - 1)
				tmp.append(ResourceLang::map_lang["dunhao"]);
		}
		strdesc = StringUtils::format(ResourceLang::map_lang["awarddesc"].c_str(), tmp.c_str());
		awarddesc->setString(strdesc);

		cocos2d::ui::ImageView* item = (cocos2d::ui::ImageView*)subnode->getChildByName("item");
		item->setTag(i+1);
		item->addTouchEventListener(CC_CALLBACK_2(SelectSubMapLayer::onNodeClick, this));
		item->setSwallowTouches(false);
		i++;
	}

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setString(GlobalInstance::map_AllResources[mainmapid].desc);

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

void SelectSubMapLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

void SelectSubMapLayer::onNodeClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::ImageView* clicknode = (cocos2d::ui::ImageView*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		clickflag = true;
		beginTouchPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));
	}
	else if (type == ui::Widget::TouchEventType::MOVED)
	{
		Vec2 movedPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));

		if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP)
			clickflag = false;
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		showCloudAnim(clicknode->getParent()->getParent(), clicknode->getParent()->getPosition());
		std::string mapid = StringUtils::format("%s-%d", m_mainmapid.c_str(), clicknode->getTag());
		int needph = GlobalInstance::map_mapsdata[m_mainmapid].map_sublist[mapid].ph;
		bool isphok = true;

		std::string nohpherostr;
		for (int i=0;i<6;i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getPower().getValue() < needph)
			{
				if (nohpherostr.length() > 0)
					nohpherostr.append(ResourceLang::map_lang["dunhao"]);
				nohpherostr.append(GlobalInstance::myCardHeros[i]->getName());
				isphok = false;
			}
		}
		if (isphok)
		{

			DynamicValueInt dv;
			for (int i = 0; i<6; i++)
			{
				if (GlobalInstance::myCardHeros[i] != NULL)
				{
					if (GlobalInstance::myCardHeros[i]->getPower().getValue() >= 100)
						GlobalInstance::myCardHeros[i]->setPowerTime(GlobalInstance::servertime);

					dv.setValue(GlobalInstance::myCardHeros[i]->getPower().getValue() - needph);
					GlobalInstance::myCardHeros[i]->setPower(dv);
					GlobalInstance::getInstance()->saveHero(GlobalInstance::myCardHeros[i]);
				}
			}
			clicknode->setTouchEnabled(false);
			Director::getInstance()->replaceScene(TransitionFade::create(2.2f, MapBlockScene::createScene(mapid, GlobalInstance::map_mapsdata[m_mainmapid].map_sublist[mapid].bgtype)));
		}
		else
		{
			nohpherostr = nohpherostr.substr(0, nohpherostr.length());
			nohpherostr.append(ResourceLang::map_lang["nomorehp"]);
			MovingLabel::show(nohpherostr);
		}
	}
}

void SelectSubMapLayer::showCloudAnim(Node* target, Vec2 pos)
{
	Sprite* cloud1 = Sprite::createWithSpriteFrameName("mapui/submapanimcloud.png");
	cloud1->setPosition(pos);
	target->addChild(cloud1);
	cloud1->runAction(Spawn::create(MoveBy::create(1.0f, Vec2(150, 0)), FadeOut::create(1.5f), NULL));

	Sprite* cloud2 = Sprite::createWithSpriteFrameName("mapui/submapanimcloud.png");
	cloud2->setFlippedX(true);
	cloud2->setPosition(pos);
	target->addChild(cloud2);
	cloud2->runAction(Spawn::create(MoveBy::create(1.0f, Vec2(-150, 0)), FadeOut::create(1.5f), NULL));
}