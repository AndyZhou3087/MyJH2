#include "ConsumeResActionLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Building.h"
#include "Const.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "InnRoomLayer.h"
#include "HomeHillLayer.h"
#include "ResCreator.h"
#include "SmithyLayer.h"

USING_NS_CC;

#define COINREFRESH_NUM 100

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

	std::string titlestr;		//标题
	std::string btn1_text;//按钮1中的文字图片
	std::string btn2_text;//按钮2中的文字图
	DynamicValueInt coincount; //需要的元宝
	if (actiontype == CA_BUILDINGLVUP)//建筑物升级
	{
		btn1_text = "lvupbtn_text";
		btn2_text = "drlvupbtn_text";
		Building* bdata = (Building*)data;
		titlestr = StringUtils::format("%s%s", GlobalInstance::map_AllResources[bdata->name].name.c_str(), ResourceLang::map_lang["lvuptext"].c_str());
		vec_res = bdata->lvupres[bdata->level.getValue()];
		coincount.setValue((bdata->level.getValue() + 1) * 100);
	}
	else if (actiontype == CA_EMPLOYFARMER)//雇佣工人
	{
		btn1_text = "recruitbtn_text";
		btn2_text = "drrecruitbtn_text";
		titlestr = ResourceLang::map_lang["employ5farmertext"];

		int employcount = GlobalInstance::getInstance()->getTotalFarmers();
		DynamicValueInt dint;
		dint.setValue((employcount / 5 * 100 + GlobalInstance::vec_resCreators[0]->getMaxCap(GlobalInstance::vec_resCreators[0]->getLv().getValue()).getValue()) * 8 / 10);
		if (dint.getValue() > 100000)
			dint.setValue(100000);
		std::map<std::string, int> map_res;
		map_res["r001"] = dint.getValue();
		vec_res.push_back(map_res);
		coincount.setValue(dint.getValue()/10);
	}
	else if (actiontype == CA_RESCREATORLVUP)//升级资源容量
	{
		btn1_text = "lvupbtn_text";
		btn2_text = "drlvupbtn_text";
		titlestr = ResourceLang::map_lang["lvrescreatortitle"];
		int homehilllv = DataSave::getInstance()->getBuildLv("7homehill");
		ResCreator* rdata = (ResCreator*)m_data;
		std::string rdataname = rdata->getName();
		titlestr = StringUtils::format(titlestr.c_str(), GlobalInstance::map_AllResources[rdata->getName()].name.c_str(), rdata->getMaxCap(rdata->getLv().getValue() + 1).getValue());
		DynamicValueInt dint;
		int creatorindex = atoi(rdataname.substr(3).c_str()) - 1;
		dint.setValue(rdata->getMaxCap(rdata->getLv().getValue()).getValue() * creatorResLvUpPara[creatorindex]);
		std::map<std::string, int> map_res;
		map_res["r002"] = dint.getValue();
		vec_res.push_back(map_res);
		coincount.setValue(dint.getValue()/10);
	}
	else if (actiontype == CA_MAKERES)//合成资源
	{
		btn1_text = "makeresbtn_text";
		btn2_text = "drmakeresbtn_text";
		std::string resid = (char*)m_data;
		titlestr = GlobalInstance::map_AllResources[resid].name;

		vec_res = GlobalInstance::map_AllResources[resid].vec_needres;
		DynamicValueInt dint;
		dint.setValue(100);
		coincount.setValue(dint.getValue() / 10);
	}
	//标题
	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("titlename");
	title->setString(titlestr);

	//3个资源的展示
	for (unsigned int i = 0; i < 3; i++)
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

			if (vec_res.size() == 1)
			{
				resbox->setPositionX(360);
			}
			else if (vec_res.size() == 2)
			{
				resbox->setPositionX(240 + i*230);
			}
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
	lvupbtn->setTag(1000);
	lvupbtn->addTouchEventListener(CC_CALLBACK_2(ConsumeResActionLayer::onBtnClick, this));
	//按钮1文字
	cocos2d::ui::ImageView* lvupbtntxt = (cocos2d::ui::ImageView*)lvupbtn->getChildByName("text");
	lvupbtntxt->loadTexture(ResourcePath::makeTextImgPath(btn1_text, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* drlvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("drlvupbtn");
	drlvupbtn->setTag(1001);
	drlvupbtn->addTouchEventListener(CC_CALLBACK_2(ConsumeResActionLayer::onBtnClick, this));
	//按钮2文字
	cocos2d::ui::ImageView* drlvupbtntxt = (cocos2d::ui::ImageView*)drlvupbtn->getChildByName("text");
	drlvupbtntxt->loadTexture(ResourcePath::makeTextImgPath(btn2_text, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	coincountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");
	std::string coinstr = StringUtils::format("x%d", coincount.getValue());
	coincountlbl->setString(coinstr);

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
		case 1000://升级
			if (checkResIsEnough())//资源足够
			{	//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];
				//减掉资源
				for (unsigned int i = 0; i < vec_res.size(); i++)
				{
					std::map<std::string, int> map_res = vec_res[i];
					std::map<std::string, int>::iterator map_it = map_res.begin();
					std::string resid = map_it->first;
					MyRes::Use(resid, map_res[resid]);
				}
				action();

				this->removeFromParentAndCleanup(true);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["reslack"]);
			}
			break;
		case 1001://直接升级
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= COINREFRESH_NUM)
			{
				action();
				this->removeFromParentAndCleanup(true);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomorecoin"]);
			}
			break;
		default:
			break;
		}
	}
}

void ConsumeResActionLayer::action()
{
	if (m_actiontype == CA_BUILDINGLVUP)
	{
		Building* bdata = (Building*)m_data;
		bdata->level.setValue(bdata->level.getValue() + 1);
		DataSave::getInstance()->setBuildingLv(bdata->name, bdata->level.getValue());

		if (bdata->name.compare("7homehill") == 0)
		{
			InnRoomLayer* innroomLayer = (InnRoomLayer*)this->getParent();
			innroomLayer->lvup();
		}
		else if (bdata->name.compare("7homehill") == 0)
		{
			HomeHillLayer* homeHillLayer = (HomeHillLayer*)this->getParent();
			homeHillLayer->lvup();
		}
		else if (bdata->name.compare("2smithy") == 0)
		{
			SmithyLayer* smithyLayer = (SmithyLayer*)this->getParent();
			smithyLayer->lvup();
		}
	}
	else if (m_actiontype == CA_EMPLOYFARMER)
	{
		//工人数增加5
		GlobalInstance::getInstance()->saveTotalFarmers(GlobalInstance::getInstance()->getTotalFarmers() + 5);
	}
	else if (m_actiontype == CA_RESCREATORLVUP)
	{
		ResCreator* rdata = (ResCreator*)m_data;
		DynamicValueInt dlv;
		dlv.setValue(rdata->getLv().getValue() + 1);
		rdata->setLv(dlv);
		GlobalInstance::getInstance()->saveResCreatorData();
	}
	else if (m_actiontype == CA_MAKERES)
	{
		std::string rid = (char*)m_data;
		SmithyLayer* smithyLayer = (SmithyLayer*)this->getParent();
		smithyLayer->makeRes(rid);
	}
}

void ConsumeResActionLayer::updateUI(float dt)
{
	//std::vector<std::map<std::string, int>> lvupres = m_building->lvupres[m_building->level.getValue()];

	for (unsigned int i = 0; i < vec_res.size(); i++)
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

	for (unsigned int i = 0; i < vec_res.size(); i++)
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


