#include "HeadInfoLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "DataSave.h"
#include "ChangeHeadLayer.h"
#include "SoundManager.h"

USING_NS_CC;

HeadInfoLayer::HeadInfoLayer()
{

}

HeadInfoLayer::~HeadInfoLayer()
{

}


HeadInfoLayer* HeadInfoLayer::create()
{
	HeadInfoLayer *pRet = new(std::nothrow)HeadInfoLayer();
	if (pRet && pRet->init())
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
bool HeadInfoLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("headInfoLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();


	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(ResourceLang::map_lang["selectheadtext"]);

	cocos2d::ui::Text* attrtext_0 = (cocos2d::ui::Text*)csbnode->getChildByName("attrtext_0");
	int count = GlobalInstance::vec_myHeros.size();
	std::string str = StringUtils::format(ResourceLang::map_lang["haveroles"].c_str(), count);
	attrtext_0->setString(str);

	cocos2d::ui::Text* attrtext_1 = (cocos2d::ui::Text*)csbnode->getChildByName("attrtext_1");
	str = StringUtils::format(ResourceLang::map_lang["havesliver"].c_str(), GlobalInstance::getInstance()->getMySoliverCount().getValue());
	attrtext_1->setString(str);

	cocos2d::ui::Text* attrtext_2 = (cocos2d::ui::Text*)csbnode->getChildByName("attrtext_2");
	str = StringUtils::format(ResourceLang::map_lang["havegold"].c_str(), GlobalInstance::getInstance()->getMyCoinCount().getValue());
	attrtext_2->setString(str);

	cocos2d::ui::Button* actionbtn = (cocos2d::ui::Button*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(HeadInfoLayer::onBtnClick, this));
	actionbtn->setTag(0);

	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("closebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* headimgbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("headbox");
	head = (cocos2d::ui::ImageView*)csbnode->getChildByName("headicon");
	head->setTag(1);
	head->setTouchEnabled(true);
	head->addTouchEventListener(CC_CALLBACK_2(HeadInfoLayer::onBtnClick, this));
	str = StringUtils::format("ui/h_%d_0.png", DataSave::getInstance()->getHeadId());
	head->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	head->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());

	//ClippingNode* m_clippingNode = ClippingNode::create();
	//m_clippingNode->setInverted(false);//设置底板可见
	//m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
	//csbnode->addChild(m_clippingNode, 1);
	//m_clippingNode->setAnchorPoint(Vec2(0.5, 1));
	//m_clippingNode->setPosition(Vec2(headimgbox->getPositionX(), headimgbox->getPositionY() + 45));
	//str = StringUtils::format("images/cardh_%d_0.png", DataSave::getInstance()->getHeadId());
	//head = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::LOCAL);
	//head->addTouchEventListener(CC_CALLBACK_2(HeadInfoLayer::onBtnClick, this));
	//head->setTouchEnabled(true);
	//head->setTag(1);
	//head->setAnchorPoint(Vec2(0.5, 1));
	//head->setPositionY(20);
	//m_clippingNode->addChild(head);
	//Node* stencil = Node::create();
	//Sprite* cnode = Sprite::createWithSpriteFrameName("ui/headclip.png");
	//cnode->setAnchorPoint(Vec2(0.5, 1));
	//stencil->addChild(cnode);
	//m_clippingNode->setStencil(stencil);

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

void HeadInfoLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* node = (Node*)pSender;
	int tag = node->getTag();
	if (tag == 0)
	{
		CommonFuncs::BtnAction(pSender, type);
	}
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 0:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1:
		{
			ChangeHeadLayer* layer = ChangeHeadLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		default:
			break;
		}
	}
}

void HeadInfoLayer::changeHead()
{
	std::string str = StringUtils::format("ui/h_%d_0.png", DataSave::getInstance()->getHeadId());
	head->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	head->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());
}

void HeadInfoLayer::onExit()
{
	Layer::onExit();
}