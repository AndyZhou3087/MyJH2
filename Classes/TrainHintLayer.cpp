#include "TrainHintLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MovingLabel.h"

USING_NS_CC;

TrainHintLayer::TrainHintLayer()
{

}

TrainHintLayer::~TrainHintLayer()
{

}


TrainHintLayer* TrainHintLayer::create(Hero* herodata, MyHeroNode* myNode)
{
	TrainHintLayer *pRet = new(std::nothrow)TrainHintLayer();
	if (pRet && pRet->init(herodata, myNode))
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
bool TrainHintLayer::init(Hero* herodata, MyHeroNode* myNode)
{
	if (!Layer::init())
	{
		return false;
	}

	m_herodata = herodata;
	myheroNode = myNode;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("trainCancelLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	//按钮1
	cocos2d::ui::Widget* okbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(TrainHintLayer::onBtnClick, this));
	okbtn->setTag(1);
	//按钮1文字
	cocos2d::ui::ImageView* okbtntxt = (cocos2d::ui::ImageView*)okbtn->getChildByName("text");
	okbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(TrainHintLayer::onBtnClick, this));
	cancelbtn->setTag(0);
	//按钮2文字
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* content = (cocos2d::ui::Text*)csbnode->getChildByName("content");
	int refreshtime = m_herodata->getTrainTime();
	int pasttime = GlobalInstance::servertime - refreshtime;
	int lefttime = m_herodata->getTrainHour() - pasttime;
	std::string str = StringUtils::format(ResourceLang::map_lang["traintipstext"].c_str(), lefttime / 3600, lefttime % 3600 / 60, lefttime % 3600 % 60);
	content->setString(str);
	

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

void TrainHintLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 0:
			this->removeFromParentAndCleanup(true);
			break;
		case 1:
			if (m_herodata->getState() == HS_TRAINING)
			{
				m_herodata->setTrainHour(0);
				m_herodata->setTrainTime(0);
				m_herodata->setState(HS_OWNED);
				myheroNode->updateContent();
			}
			this->removeFromParentAndCleanup(true);
		default:
			break;
		}
	}
}