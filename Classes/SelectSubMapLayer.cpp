#include "SelectSubMapLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MapBlockScene.h"

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
	this->addChild(color);
    
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

		subnode->setPosition(Vec2(scrollView->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
		//subnode->setPosition(Vec2(360, 970 - i * 170));
		scrollView->addChild(subnode);

		cocos2d::ui::Text* mapname = (cocos2d::ui::Text*)subnode->getChildByName("namelbl");
		mapname->setString(GlobalInstance::map_AllResources[it->first].name);

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
				tmp.append(ResourceLang::map_lang["zhdunhao"]);
		}
		strdesc = StringUtils::format(ResourceLang::map_lang["awarddesc"].c_str(), tmp.c_str());
		awarddesc->setString(strdesc);

		cocos2d::ui::Widget* item = (cocos2d::ui::Widget*)subnode->getChildByName("item");
		item->setTag(i+1);
		item->addTouchEventListener(CC_CALLBACK_2(SelectSubMapLayer::onNodeClick, this));
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
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		showCloudAnim(clicknode->getParent()->getParent(), clicknode->getParent()->getPosition());
		std::string mapid = StringUtils::format("%s-%d", m_mainmapid.c_str(), clicknode->getTag());
		int needph = GlobalInstance::map_mapsdata[m_mainmapid].map_sublist[mapid].ph;
		bool isphok = true;
		for (int i=0;i<6;i++)
		{
			if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getPower().getValue() < needph)
				isphok = false;
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

				}
			}
			Director::getInstance()->replaceScene(TransitionFade::create(3.0f, MapBlockScene::createScene(mapid)));
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