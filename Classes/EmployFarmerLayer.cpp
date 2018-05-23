#include "EmployFarmerLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "Const.h"
#include "MyRes.h"
#include "MovingLabel.h"

USING_NS_CC;

EmployFarmerLayer::EmployFarmerLayer()
{

}

EmployFarmerLayer::~EmployFarmerLayer()
{
	
}


EmployFarmerLayer* EmployFarmerLayer::create()
{
	EmployFarmerLayer *pRet = new(std::nothrow)EmployFarmerLayer();
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
bool EmployFarmerLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("employFarmers.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	std::string resid = "r001";
	//标题
	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("titlename");
	title->setString(ResourceLang::map_lang["employ5farmertext"]);

	needlbl = (cocos2d::ui::Text*)csbnode->getChildByName("needtext");

	lacklbl = (cocos2d::ui::Text*)csbnode->getChildByName("lacktext");
	lacklbl->setVisible(false);

	//招募按钮
	cocos2d::ui::Widget* getbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("getbtn");
	getbtn->setTag(0);
	getbtn->addTouchEventListener(CC_CALLBACK_2(EmployFarmerLayer::onBtnClick, this));

	//招募按钮文字
	cocos2d::ui::ImageView* getbtntxt = (cocos2d::ui::ImageView*)getbtn->getChildByName("text");
	getbtntxt->loadTexture(ResourcePath::makeTextImgPath("recruitbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//立即招募按钮
	cocos2d::ui::Widget* drgetbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("drgetbtn");
	drgetbtn->setTag(1);
	drgetbtn->addTouchEventListener(CC_CALLBACK_2(EmployFarmerLayer::onBtnClick, this));

	//立即招募按钮文字
	cocos2d::ui::ImageView* drgetbtntxt = (cocos2d::ui::ImageView*)drgetbtn->getChildByName("text");
	drgetbtntxt->loadTexture(ResourcePath::makeTextImgPath("drrecruitbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	countlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");

	updateUI(0);
	this->schedule(schedule_selector(EmployFarmerLayer::updateUI), 1);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		this->removeFromParentAndCleanup(true);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void EmployFarmerLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 0://招募
			if (MyRes::getMyResCount("r001") >= dint.getValue())
			{
				GlobalInstance::getInstance()->saveTotalFarmers(GlobalInstance::getInstance()->getTotalFarmers() + 5);
				MyRes::Add("r001", -dint.getValue());
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["reslack"]);
			}
			break;
		case 1://立即招募
			break;
		case 2://关闭
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void EmployFarmerLayer::updateUI(float dt)
{
	int employcount = GlobalInstance::getInstance()->getTotalFarmers();

	dint.setValue((employcount / 5 * 100 + GlobalInstance::vec_resCreators[0]->getMaxCap(GlobalInstance::vec_resCreators[0]->getLv().getValue()).getValue()) * 8 / 10);
	if (dint.getValue() > 100000)
		dint.setValue(100000);

	std::string str = StringUtils::format(ResourceLang::map_lang["needfoodtext"].c_str(), dint.getValue());

	needlbl->setString(str);

	if (MyRes::getMyResCount("r001") >= dint.getValue())
	{
		lacklbl->setVisible(false);
	}
	else
	{
		std::string str = StringUtils::format(ResourceLang::map_lang["lackfoodtext"].c_str(), dint.getValue() - MyRes::getMyResCount("r001"));
		lacklbl->setVisible(true);
		lacklbl->setString(str);
	}

	str = StringUtils::format("x%d", dint.getValue() / 10);
	countlbl->setString(str);
}

void EmployFarmerLayer::onExit()
{
	Layer::onExit();
}


