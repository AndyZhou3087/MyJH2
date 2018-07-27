#include "TrainSelectLayer.h"
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
#include "Quest.h"
#include "TrainLayer.h"

USING_NS_CC;

const static int resCount[3] = { 100, 500, 800 };
const static int timeCount[3] = { 3600, 18000, 28800 };

TrainSelectLayer::TrainSelectLayer()
{
	lastSelectIndex = 0;
}

TrainSelectLayer::~TrainSelectLayer()
{

}


TrainSelectLayer* TrainSelectLayer::create(Hero* herodata, MyHeroNode* myNode)
{
	TrainSelectLayer *pRet = new(std::nothrow)TrainSelectLayer();
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
bool TrainSelectLayer::init(Hero* herodata, MyHeroNode* myNode)
{
	if (!Layer::init())
	{
		return false;
	}

	m_data = herodata;
	myheroNode = myNode;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("trainSelectLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	coincount.setValue((GlobalInstance::map_AllResources["r001"].silverval * 100 + GlobalInstance::map_AllResources["c001"].silverval)/10);//需要的元宝

	//标题
	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("titlename");
	title->setString(ResourceLang::map_lang["timeselect"].c_str());

	for (int i = 0; i < 3; i++)
	{
		std::string nstr = StringUtils::format("name%d", i);
		cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName(nstr);
		std::string tstr = StringUtils::format("hour%d", i);
		name->setString(ResourceLang::map_lang[tstr].c_str());

		nstr = StringUtils::format("res%d", i);
		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName(nstr);
		res->addTouchEventListener(CC_CALLBACK_2(TrainSelectLayer::onImgClick, this));
		res->setTag(i);

		nstr = StringUtils::format("train_select_%d", i);
		cocos2d::ui::ImageView* sel = (cocos2d::ui::ImageView*)csbnode->getChildByName(nstr);
		selArr[i] = sel;

		nstr = StringUtils::format("countlbl%d", i);
		cocos2d::ui::Text* count = (cocos2d::ui::Text*)csbnode->getChildByName(nstr);
		countlbl[i] = count;

		std::map<std::string, int> resarr;
		resarr["r001"] = resCount[i];
		resarr["c001"] = 1;
		vec_res.push_back(resarr);
	}

	//按钮1
	cocos2d::ui::Widget* lvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvupbtn->setTag(1000);
	lvupbtn->addTouchEventListener(CC_CALLBACK_2(TrainSelectLayer::onBtnClick, this));
	//按钮1文字
	cocos2d::ui::ImageView* lvupbtntxt = (cocos2d::ui::ImageView*)lvupbtn->getChildByName("text");
	lvupbtntxt->loadTexture(ResourcePath::makeTextImgPath("trainstart_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮2
	cocos2d::ui::Widget* drlvupbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("drlvupbtn");
	drlvupbtn->setTag(1001);
	drlvupbtn->addTouchEventListener(CC_CALLBACK_2(TrainSelectLayer::onBtnClick, this));
	//按钮2文字
	cocos2d::ui::ImageView* drlvupbtntxt = (cocos2d::ui::ImageView*)drlvupbtn->getChildByName("text");
	drlvupbtntxt->loadTexture(ResourcePath::makeTextImgPath("traincoin_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	coincountlbl = (cocos2d::ui::Text*)csbnode->getChildByName("countlbl");
	std::string coinstr = StringUtils::format("x%d", coincount.getValue());
	coincountlbl->setString(coinstr);

	updateUI();

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

void TrainSelectLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (tag)
		{
		case 1000://训练
			if (checkResIsEnough(lastSelectIndex))//资源足够
			{	
				//减掉资源
				for (unsigned int i = 0; i < vec_res.size(); i++)
				{
					if (i == lastSelectIndex)
					{
						std::map<std::string, int>::iterator map_it;
						for (map_it = vec_res[lastSelectIndex].begin(); map_it != vec_res[lastSelectIndex].end(); map_it++)
						{
							std::map<std::string, int> map_res = vec_res[lastSelectIndex];
							std::string resid = map_it->first;
							MyRes::Use(resid, map_res[resid]);
						}
					}
				}
				action();
				this->removeFromParentAndCleanup(true);
			}
			else
			{
				if (MyRes::getMyResCount("r001") < vec_res[lastSelectIndex]["r001"] && MyRes::getMyResCount("c001") < vec_res[lastSelectIndex]["c001"])
				{
					std::string astr = StringUtils::format("%s,%s", GlobalInstance::map_AllResources["r001"].name.c_str(), GlobalInstance::map_AllResources["c001"].name.c_str());
					std::string str = StringUtils::format(ResourceLang::map_lang["notenouph"].c_str(), astr.c_str());
					MovingLabel::show(str);
				}
				else if (MyRes::getMyResCount("r001") < vec_res[lastSelectIndex]["r001"])
				{
					std::string str = StringUtils::format(ResourceLang::map_lang["notenouph"].c_str(), GlobalInstance::map_AllResources["r001"].name.c_str());
					MovingLabel::show(str);
				}
				else
				{
					std::string str = StringUtils::format(ResourceLang::map_lang["notenouph"].c_str(), GlobalInstance::map_AllResources["c001"].name.c_str());
					MovingLabel::show(str);
				}
			}
			break;
		case 1001://直接训练
			if (GlobalInstance::getInstance()->getMyCoinCount().getValue() >= coincount.getValue())
			{
				DynamicValueInt dval;
				dval.setValue(coincount.getValue());
				GlobalInstance::getInstance()->costMyCoinCount(dval);
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

void TrainSelectLayer::onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (lastSelectIndex == tag)
		{
			return;
		}
		lastSelectIndex = tag;
		updateUI();
	}
}

void TrainSelectLayer::action()
{
	m_data->setState(HS_TRAINING);
	m_data->setTrainHour(timeCount[lastSelectIndex]);
	m_data->setTrainTime(GlobalInstance::servertime);
	myheroNode->changeBtnContent();
}

void TrainSelectLayer::updateUI()
{
	for (int i = 0; i < 3; i++)
	{
		std::string str = StringUtils::format("%d/%d", MyRes::getMyResCount("r001"), resCount[i]);
		countlbl[i]->setString(str);
		if (lastSelectIndex == i)
		{
			selArr[i]->setVisible(true);
		}
		else
		{
			selArr[i]->setVisible(false);
		}
		if (MyRes::getMyResCount("r001") < resCount[i])
		{
			countlbl[i]->setTextColor(Color4B(255, 0, 0, 255));
		}
		else
		{
			countlbl[i]->setTextColor(Color4B(255, 255, 255, 255));
		}
	}

	coincount.setValue((GlobalInstance::map_AllResources["r001"].silverval * resCount[lastSelectIndex] + GlobalInstance::map_AllResources["c001"].silverval)/10);//需要的元宝
	std::string coinstr = StringUtils::format("x%d", coincount.getValue());
	coincountlbl->setString(coinstr);
}


//资源足够训练
bool TrainSelectLayer::checkResIsEnough(int index)
{
	for (unsigned int i = 0; i < vec_res.size(); i++)
	{
		if (i == index)
		{
			std::map<std::string, int>::iterator map_it;
			for (map_it = vec_res[index].begin(); map_it != vec_res[index].end(); map_it++)
			{
				std::map<std::string, int> map_res = vec_res[index];
				std::string resid = map_it->first;
				int mycount = MyRes::getMyResCount(resid);

				if (mycount < map_res[resid])
					return false;
			}
		}
	}
	return true;
}

void TrainSelectLayer::onExit()
{
	Layer::onExit();
}


