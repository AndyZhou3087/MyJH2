#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "TaskDailyNode.h"
#include "Const.h"
#include "DataSave.h"
#include "TaskLayer.h"
#include "Building.h"
#include "MainScene.h"
#include "RandHeroLayer.h"
#include "SmithyLayer.h"
#include "StoreHouseLayer.h"
#include "Quest.h"
#include "ResCreator.h"
#include "AnimationEffect.h"
#include "OutTownLayer.h"
#include "MarketLayer.h"
#include "SimplePopLayer.h"
#include "SoundManager.h"
#include "RewardLayer.h"
#include "NewGuideLayer.h"
#include "HomeHillLayer.h"
#include "TrainLayer.h"
#include "InnRoomLayer.h"

TaskDailyNode::TaskDailyNode()
{

}


TaskDailyNode::~TaskDailyNode()
{

}

TaskDailyNode* TaskDailyNode::create(DailyTaskData* data)
{
	TaskDailyNode *pRet = new(std::nothrow)TaskDailyNode();
	if (pRet && pRet->init(data))
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

bool TaskDailyNode::init(DailyTaskData* data)
{
	m_Data = data;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("taskDayNode.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* clickimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("clickimg");
	clickimg->addTouchEventListener(CC_CALLBACK_2(TaskDailyNode::onbtnClick, this));
	clickimg->setTag(2000);
	clickimg->setSwallowTouches(false);

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = StringUtils::format("ui/dailytask_%d.png", data->type);
	res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["dailyrlabel"]);

	//名字
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	str = StringUtils::format("dailytype_%d", data->type);
	std::string s = StringUtils::format(ResourceLang::map_lang[str].c_str(), data->count);
	name->setString(s);

	cocos2d::ui::Text* integral = (cocos2d::ui::Text*)csbnode->getChildByName("integral");
	str = StringUtils::format(ResourceLang::map_lang["dailypoint"].c_str(), data->points);
	integral->setString(str);

	finish = (cocos2d::ui::Widget*)csbnode->getChildByName("finish");
	actionbtn = (cocos2d::ui::Button*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(TaskDailyNode::onbtnClick, this));
	actionbtn->setTag(data->type);
	actionbtn->setSwallowTouches(false);

	getbtn = (cocos2d::ui::Button*)csbnode->getChildByName("getbtn");
	getbtn->addTouchEventListener(CC_CALLBACK_2(TaskDailyNode::onbtnClick, this));
	getbtn->setTag(1000);
	getbtn->setSwallowTouches(false);

	taskprobar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("taskprobar");
	taskprotext = (cocos2d::ui::Text*)csbnode->getChildByName("taskprotext");

	//奖励的物品
	for (unsigned int i = 0; i < data->goods.size(); i++)
	{
		std::vector<std::string> vec = data->goods[i];
		int count = atoi(vec[1].c_str());
		int qu = 0;
		if (vec.size() > 2 && vec[2].length() > 0)
		{
			qu = atoi(vec[2].c_str());
		}
		std::string str = GlobalInstance::getInstance()->getResUIFrameName(vec[0], qu);
		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(0 + i * 80, 0));
		this->addChild(res);
		res->setScale(0.4f);

		str = StringUtils::format("x%d", count);
		Label *namelbl = Label::createWithTTF(str, FONT_NAME, 16);
		namelbl->setColor(Color3B(0, 0, 0));
		namelbl->setAnchorPoint(Vec2(0, 0.5));
		namelbl->setPosition(Vec2(18 + i * 80, -7));
		this->addChild(namelbl);
	}

	updateData(0);

	this->schedule(schedule_selector(TaskDailyNode::updateData), 1.0f);
	return true;
}

void TaskDailyNode::updateData(float dt)
{
	if (m_Data->state == DAILY_UNFINISHED)
	{
		finish->setVisible(false);
		getbtn->setVisible(false);
		actionbtn->setVisible(true);
	}
	else if (m_Data->state == DAILY_FINISHED)
	{
		finish->setVisible(false);
		getbtn->setVisible(true);
		actionbtn->setVisible(false);
	}
	else
	{
		finish->setVisible(true);
		getbtn->setVisible(false);
		actionbtn->setVisible(false);
	}

	int c = Quest::map_DailyTypeCount[m_Data->type];
	if (c >= m_Data->count)
	{
		c = m_Data->count;
	}
	float h = c * 100 / m_Data->count;
	taskprobar->setPercent(h);
	std::string str = StringUtils::format("%d/%d", c, m_Data->count);
	taskprotext->setString(str);

	TaskLayer* layer = (TaskLayer*)g_mainScene->getChildByName("9assemblyhall");
	if (layer != NULL)
	{
		layer->updateDailyTip();
	}
}

void TaskDailyNode::onbtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();

	if (tag != 2000)
		CommonFuncs::BtnAction(pSender, type);

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

		if (tag == 2000)
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
			std::string str = StringUtils::format("dailytype_%d", m_Data->type);
			std::string s = StringUtils::format(ResourceLang::map_lang[str].c_str(), m_Data->count);
			SimplePopLayer* simplelayer = SimplePopLayer::create(s);
			g_mainScene->addChild(simplelayer);
			AnimationEffect::openAniEffect(simplelayer);
		}
		else if (tag == 1000)
		{
			std::vector<MSGAWDSDATA> vec_rewards;
			//物品
			for (unsigned int i = 0; i < m_Data->goods.size(); i++)
			{
				std::vector<std::string> one_res = m_Data->goods[i];
				std::string resid = one_res[0];
				int count = atoi(one_res[1].c_str());
				int qu = 0;
				int stc = 0;
				if (one_res.size()>2 && one_res[2].length()>0)
				{
					qu = atoi(one_res[2].c_str());
				}

				for (unsigned int i = 0; i < GlobalInstance::vec_resCreators.size(); i++)
				{
					ResCreator* rescreator = GlobalInstance::vec_resCreators[i];
					if (rescreator->getName().compare(resid) == 0)
					{
						int maxcount = rescreator->getMaxCap(rescreator->getLv().getValue()).getValue();
						if (MyRes::getMyResCount(resid)+count>maxcount)
						{
							std::string hintstr = StringUtils::format(ResourceLang::map_lang["nomoreres"].c_str(), GlobalInstance::map_AllResources[rescreator->getName()].name.c_str());
							MovingLabel::show(hintstr);
							return;
						}
					}
				}
				MSGAWDSDATA wdata;
				wdata.rid = resid;
				wdata.count = count;
				wdata.qu = qu;
				vec_rewards.push_back(wdata);
			}

			if (vec_rewards.size() > 0)
			{
				RewardLayer* layer = RewardLayer::create(vec_rewards);
				g_mainScene->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}

			cocos2d::ui::Button* btn = (cocos2d::ui::Button*)pSender;
			btn->setTouchEnabled(false);

			m_Data->state = DAILY_RECEIVE;
			GlobalInstance::getInstance()->saveMyDailyTaskData();
			//积分
			int m_point = DataSave::getInstance()->getMyyDailyPoint();
			m_point += m_Data->points;
			DataSave::getInstance()->setMyDailyPoint(m_point);
		}
		else
		{
			switch (tag)
			{
			case FINISH_BRANCH:
			{
				TaskLayer* layer = (TaskLayer*)this->getParent()->getParent()->getParent()->getParent();
				if (layer != NULL)
				{
					layer->skipContent();
				}
			}
			break;
			case FRESH_PUBENLIST:
			{
				RandHeroLayer* layer = RandHeroLayer::create();
				g_mainScene->addChild(layer, 0, "RandHeroLayer");
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			break;
			case UPGRADE_HERO:
			{
				InnRoomLayer* layer = InnRoomLayer::create(Building::map_buildingDatas["6innroom"]);
				g_mainScene->addChild(layer, 0, "6innroom");
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			break;
			case UPGRADE_BUILDING:
			{
				if (Building::map_buildingDatas["7homehill"]->level.getValue() < Building::map_buildingDatas["7homehill"]->maxlevel.getValue() - 1)
				{
					HomeHillLayer* layer = HomeHillLayer::create(Building::map_buildingDatas["7homehill"]);
					g_mainScene->addChild(layer, 0, "7homehill");
					AnimationEffect::openAniEffect((Layer*)layer);
				}
				else if (Building::map_buildingDatas["6innroom"]->level.getValue() < Building::map_buildingDatas["6innroom"]->maxlevel.getValue() - 1)
				{
					RandHeroLayer* layer = RandHeroLayer::create();
					g_mainScene->addChild(layer, 0, "RandHeroLayer");
					AnimationEffect::openAniEffect((Layer*)layer);
				}
				else if (Building::map_buildingDatas["5market"]->level.getValue() < Building::map_buildingDatas["5market"]->maxlevel.getValue() - 1)
				{
					MarketLayer* layer = MarketLayer::create(Building::map_buildingDatas["5market"]);
					g_mainScene->addChild(layer, 0, "5market");
					AnimationEffect::openAniEffect((Layer*)layer);
				}
				else if (Building::map_buildingDatas["2smithy"]->level.getValue() < Building::map_buildingDatas["2smithy"]->maxlevel.getValue() - 1)
				{
					SmithyLayer* layer = SmithyLayer::create(Building::map_buildingDatas["2smithy"]);
					g_mainScene->addChild(layer, 0, "2smithy");
					AnimationEffect::openAniEffect((Layer*)layer);
				}
				else
				{
					TrainLayer* layer = TrainLayer::create(Building::map_buildingDatas["4trainigroom"]);
					g_mainScene->addChild(layer, 0, "4trainigroom");
					AnimationEffect::openAniEffect((Layer*)layer);
				}
			}
			break;
			case STRENG_EQUIP:
			{
				StoreHouseLayer* layer = StoreHouseLayer::create();
				g_mainScene->addChild(layer, 0, "3storehouse");
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			break;
			case STRENG_WG:
			{
				InnRoomLayer* layer = InnRoomLayer::create(Building::map_buildingDatas["6innroom"]);
				g_mainScene->addChild(layer, 0, "6innroom");
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			break;
			case DECOMPOSE_EQUIP:
			{
				StoreHouseLayer* layer = StoreHouseLayer::create();
				g_mainScene->addChild(layer, 0, "3storehouse");
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			break;
			case SET_GEM:
			{
				InnRoomLayer* layer = InnRoomLayer::create(Building::map_buildingDatas["6innroom"]);
				g_mainScene->addChild(layer, 0, "6innroom");
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			break;
			case ACHIEVE_FIGHT:
			{
				OutTownLayer* layer = OutTownLayer::create();
				g_mainScene->addChild(layer, 0, "0outtown");
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			break;
			case ACHIEVE_GOODS:
			{
				doMainMarket();
			}
			break;
			case ACHIEVE_GOLD:
			{
				doMainMarket();
			}
			break;
			case ACHIEVE_SLIVER:
			{
				doMainMarket();
			}
			break;
			default:
				break;
			}
		}
	}
}

void TaskDailyNode::doMainMarket()
{
	if (g_mainScene != NULL && g_mainScene->buildingIsClickOn(6))
	{
		MovingLabel::show(ResourceLang::map_lang["unlockmain_6"]);
	}
	else
	{
		MarketLayer* layer = MarketLayer::create(Building::map_buildingDatas["5market"]);
		g_mainScene->addChild(layer, 0, "5market");
		AnimationEffect::openAniEffect((Layer*)layer);
	}
}
