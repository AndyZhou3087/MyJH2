#include "BuildingLvUpLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "Const.h"
#include "MyRes.h"

USING_NS_CC;

BuildingLvUpLayer::BuildingLvUpLayer()
{

}

BuildingLvUpLayer::~BuildingLvUpLayer()
{
	
}


BuildingLvUpLayer* BuildingLvUpLayer::create(Building* building)
{
	BuildingLvUpLayer *pRet = new(std::nothrow)BuildingLvUpLayer();
	if (pRet && pRet->init(building))
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
bool BuildingLvUpLayer::init(Building* building)
{
    if ( !Layer::init() )
    {
        return false;
    }
	m_building = building;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("buildingLvUpLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	std::string str = StringUtils::format("%s%s", m_building->cname.c_str(), ResourceLang::map_lang["lvuptext"].c_str());
	//标题
	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("titlename");
	title->setString(str);

	std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];

	for (int i = 0; i < 3; i++)
	{
		std::map<std::string, int> map_res = lvupres[i];
		std::map<std::string, int>::iterator map_it = map_res.begin();

		std::string resid = StringUtils::format("%s", map_it->first.c_str());
		str = StringUtils::format("res%d", i);
		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		str = StringUtils::format("ui/%s.png", resid.c_str());
		res->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

		str = StringUtils::format("name%d", i);
		namelbl[i] = (cocos2d::ui::Text*)csbnode->getChildByName(str);

		namelbl[i]->setString(GlobalInstance::map_AllResources[resid].name);

		str = StringUtils::format("countlbl%d", i);
		countlbl[i] = (cocos2d::ui::Text*)csbnode->getChildByName(str);


		str = StringUtils::format("%d/%d", MyRes::getMyResCount(resid), map_res[resid]);

		countlbl[i]->setString(str);

	}
	//升级按钮
	cocos2d::ui::Widget* lvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvupbtn->setTag(0);
	lvupbtn->addTouchEventListener(CC_CALLBACK_2(BuildingLvUpLayer::onBtnClick, this));
	//升级按钮文字
	cocos2d::ui::ImageView* lvupbtntxt = (cocos2d::ui::ImageView*)lvupbtn->getChildByName("text");
	lvupbtntxt->loadTexture(ResourcePath::makeTextImgPath("lvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//直接升级按钮
	cocos2d::ui::Widget* drlvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("drlvupbtn");
	drlvupbtn->setTag(1);
	drlvupbtn->addTouchEventListener(CC_CALLBACK_2(BuildingLvUpLayer::onBtnClick, this));
	//直接升级按钮文字
	cocos2d::ui::ImageView* drlvupbtntxt = (cocos2d::ui::ImageView*)drlvupbtn->getChildByName("text");
	drlvupbtntxt->loadTexture(ResourcePath::makeTextImgPath("drlvupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");
	str = StringUtils::format("x%d", (m_building->level.getValue() + 1) * 100);
	countlbl->setString(str);

	this->schedule(schedule_selector(BuildingLvUpLayer::updateUI), 1);

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

void BuildingLvUpLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 0://升级

			break;
		case 1://直接升级
			break;
		case 2://关闭
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void BuildingLvUpLayer::updateUI(float dt)
{
	std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];

	for (int i = 0; i < 3; i++)
	{
		std::map<std::string, int> map_res = lvupres[i];
		std::map<std::string, int>::iterator map_it = map_res.begin();

		std::string resid = StringUtils::format("%s", map_it->first.c_str());
		int mycount = MyRes::getMyResCount(resid);
		std::string str = StringUtils::format("%d/%d", MyRes::getMyResCount(resid), map_res[resid]);

		countlbl[i]->setString(str);
		if (mycount < map_res[resid])
			countlbl[i]->setTextColor(Color4B(255, 0, 0, 255));
		else
			countlbl[i]->setTextColor(Color4B(255, 255, 255, 255));
	}
}

void BuildingLvUpLayer::onExit()
{
	Layer::onExit();
}


