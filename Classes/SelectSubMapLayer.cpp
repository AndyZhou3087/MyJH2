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

	int i = 0;
	std::map<std::string, S_SubMap>::iterator it;
	for (it = GlobalInstance::map_mapsdata[mainmapid].map_sublist.begin(); it != GlobalInstance::map_mapsdata[mainmapid].map_sublist.end(); it++)
	{
		Node* subnode = CSLoader::createNode(ResourcePath::makePath("selectMapNode.csb"));
		cocos2d::ui::Text* mapname = (cocos2d::ui::Text*)subnode->getChildByName("namelbl");
		mapname->setString(GlobalInstance::map_AllResources[it->first].name);
		subnode->setPosition(Vec2(360, 970 - i * 170));
		this->addChild(subnode);
		cocos2d::ui::Widget* item = (cocos2d::ui::Widget*)subnode->getChildByName("item");
		item->setTag(i+1);
		item->addTouchEventListener(CC_CALLBACK_2(SelectSubMapLayer::onNodeClick, this));
		i++;
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
		showCloudAnim(clicknode->getParent()->getPosition());
		std::string mapid = StringUtils::format("%s-%d", m_mainmapid.c_str(), clicknode->getTag());
		Director::getInstance()->replaceScene(TransitionFade::create(3.0f, MapBlockScene::createScene(mapid)));
	}
}

void SelectSubMapLayer::showCloudAnim(Vec2 pos)
{
	Sprite* cloud1 = Sprite::createWithSpriteFrameName("mapui/submapanimcloud.png");
	cloud1->setPosition(pos);
	this->addChild(cloud1);
	cloud1->runAction(Spawn::create(MoveBy::create(1.0f, Vec2(150, 0)), FadeOut::create(1.5f), NULL));

	Sprite* cloud2 = Sprite::createWithSpriteFrameName("mapui/submapanimcloud.png");
	cloud2->setFlippedX(true);
	cloud2->setPosition(pos);
	this->addChild(cloud2);
	cloud2->runAction(Spawn::create(MoveBy::create(1.0f, Vec2(-150, 0)), FadeOut::create(1.5f), NULL));
}