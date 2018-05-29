#include "ConsumeResActionLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "Const.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "HomeHillLayer.h"

USING_NS_CC;

ConsumeResActionLayer::ConsumeResActionLayer()
{

}

ConsumeResActionLayer::~ConsumeResActionLayer()
{
	
}


ConsumeResActionLayer* ConsumeResActionLayer::create(void* data, int actiontype)
{
	ConsumeResActionLayer *pRet = new(std::nothrow)ConsumeResActionLayer();
	if (pRet && pRet->init(data, actiontype))
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
bool ConsumeResActionLayer::init(void* data, int actiontype)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_data = data;
	m_actiontype = actiontype;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("consumeResActionLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	std::string titlestr;
	std::string btn1_text;
	std::string btn2_text;
	int count = 0;
	if (actiontype == CA_BUILDINGLVUP)
	{
		btn1_text = "lvupbtn_text";
		btn2_text = "drlvupbtn_text";
		Building* bdata = (Building*)data;
		titlestr = StringUtils::format("%s%s", bdata->cname.c_str(), ResourceLang::map_lang["lvuptext"].c_str());
		vec_res = bdata->lvupres[bdata->level.getValue()];
	}
	else if (actiontype == CA_EMPLOYFARMER)
	{
		btn1_text = "recruitbtn_text";
		btn2_text = "drrecruitbtn_text";
		titlestr = ResourceLang::map_lang["employ5farmertext"];

		int employcount = GlobalInstance::getInstance()->getTotalFarmers();

		std::map<std::string, int> map_res;
		map_res["r001"] = (employcount / 5 * 100 + GlobalInstance::vec_resCreators[0]->getMaxCap(GlobalInstance::vec_resCreators[0]->getLv().getValue()).getValue()) * 8 / 10;
vec_res.p
	}
	//标题
	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("titlename");
	title->setString(titlestr);

	
	for (int i = 0; i < 3; i++)
	{
		if (i < vec_res.size())
		{
			std::map<std::string, int> map_res = vec_res[i];
			std::map<std::string, int>::iterator map_it = map_res.begin();

			std::string resid = map_it->first;
			std::string str = StringUtils::format("res%d", i);
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

			str = StringUtils::format("resbox%d", i);
			cocos2d::ui::Widget* resbox = (cocos2d::ui::Widget*)csbnode->getChildByName(str);
			resbox->setVisible(false);

			str = StringUtils::format("resbox%d_qu", i);
			cocos2d::ui::Widget* resboxqu = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			resboxqu->setVisible(false);


			if (vec_res.size() == 1)
			{
				resbox->setPositionX(360);
			}
			else if (vec_res.size() == 2)
			{
				resbox->setPositionX(240 + i*230);
			}
			resboxqu->setPositionX(resbox->getPositionX());
			res->setPositionX(resbox->getPositionX());
			namelbl[i]->setPositionX(resbox->getPositionX());
			countlbl[i]->setPositionX(resbox->getPositionX());
		}
		else
		{
			std::string str = StringUtils::format("resbox%d", i);
			cocos2d::ui::Widget* widget = (cocos2d::ui::Widget*)csbnode->getChildByName(str);
			widget->setVisible(false);
			str = StringUtils::format("resbox%d_qu", i);
			widget = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			widget->setVisible(false);

			str = StringUtils::format("res%d", i);
			widget = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			widget->setVisible(false);

			str = StringUtils::format("name%d", i);
			widget = (cocos2d::ui::Widget*)csbnode->getChildByName(str);
			widget->setVisible(false);

			str = StringUtils::format("countlbl%d", i);
			widget = (cocos2d::ui::Widget*)csbnode->getChildByName(str);
			widget->setVisible(false);
			
		}

	}
	//按钮1
	cocos2d::ui::Widget* lvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvupbtn->setTag(0);
	lvupbtn->addTouchEventListener(CC_CALLBACK_2(ConsumeResActionLayer::onBtnClick, this));
	//按钮1文字
	cocos2d::ui::ImageView* lvupbtntxt = (cocos2d::ui::ImageView*)lvupbtn->getChildByName("text");
	lvupbtntxt->loadTexture(ResourcePath::makeTextImgPath(btn1_text, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* drlvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("drlvupbtn");
	drlvupbtn->setTag(1);
	drlvupbtn->addTouchEventListener(CC_CALLBACK_2(ConsumeResActionLayer::onBtnClick, this));
	//按钮2文字
	cocos2d::ui::ImageView* drlvupbtntxt = (cocos2d::ui::ImageView*)drlvupbtn->getChildByName("text");
	drlvupbtntxt->loadTexture(ResourcePath::makeTextImgPath(btn2_text, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	coincountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");

	updateUI(0);
	this->schedule(schedule_selector(ConsumeResActionLayer::updateUI), 1);

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

void ConsumeResActionLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 0://升级
			if (checkResIsEnough())
			{
				if (m_actiontype = CA_BUILDINGLVUP)
				{
					//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];
					for (int i = 0; i < 3; i++)
					{
						std::map<std::string, int> map_res = vec_res[i];
						std::map<std::string, int>::iterator map_it = map_res.begin();

						std::string resid = map_it->first;
						int mycount = MyRes::getMyResCount(resid);
						MyRes::Add(resid, -map_res[resid]);
					}
					Building* bdata = (Building*)m_data;
					bdata->level.setValue(bdata->level.getValue() + 1);
					DataSave::getInstance()->setBuildingLv(bdata->name, bdata->level.getValue());
					if (bdata->name.compare("7homehill") == 0)
					{
						HomeHillLayer* homeHillLayer = (HomeHillLayer*)this->getParent();
						homeHillLayer->lvup();
					}
					else
					{

					}
				}
				else
				{

				}
				this->removeFromParentAndCleanup(true);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["reslack"]);
			}
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

void ConsumeResActionLayer::updateUI(float dt)
{
	//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];

	for (int i = 0; i < 3; i++)
	{
		std::map<std::string, int> map_res = vec_res[i];
		std::map<std::string, int>::iterator map_it = map_res.begin();

		std::string resid = map_it->first;
		int mycount = MyRes::getMyResCount(resid);
		std::string str = StringUtils::format("%d/%d", MyRes::getMyResCount(resid), map_res[resid]);

		countlbl[i]->setString(str);
		if (mycount < map_res[resid])
			countlbl[i]->setTextColor(Color4B(255, 0, 0, 255));
		else
			countlbl[i]->setTextColor(Color4B(255, 255, 255, 255));
	}

	//std::string str = StringUtils::format("x%d", (m_building->level.getValue() + 1) * 100);
	//coincountlbl->setString(str);
}


//资源足够升级
bool ConsumeResActionLayer::checkResIsEnough()
{
	//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];

	for (int i = 0; i < 3; i++)
	{
		std::map<std::string, int> map_res = vec_res[i];
		std::map<std::string, int>::iterator map_it = map_res.begin();

		std::string resid = map_it->first;
		int mycount = MyRes::getMyResCount(resid);

		if (mycount < map_res[resid])
			return false;
	}
	return true;
}

void ConsumeResActionLayer::onExit()
{
	Layer::onExit();
}


