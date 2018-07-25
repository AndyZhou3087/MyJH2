#include "MyHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "HeroAttrLayer.h"
#include "MainScene.h"
#include "InnRoomLayer.h"
#include "GlobalInstance.h"
#include "SelectMyHerosLayer.h"
#include "CardHeroNode.h"
#include "HospitalLayer.h"
#include "MovingLabel.h"
#include "Const.h"
#include "TrainSelectLayer.h"
#include "TrainHintLayer.h"
#include "TrainLayer.h"
#include "Building.h"
#include "OutTownLayer.h"

#define RSILVERCOUNT 100

MyHeroNode::MyHeroNode()
{

}


MyHeroNode::~MyHeroNode()
{

}

MyHeroNode* MyHeroNode::create(Hero* herodata, int showtype)
{
	MyHeroNode *pRet = new(std::nothrow)MyHeroNode();
	if (pRet && pRet->init(herodata, showtype))
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

bool MyHeroNode::init(Hero* herodata, int showtype)
{

	m_heroData = herodata;
	m_showtype = showtype;
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("myHeroNode.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Widget* bgitem = (cocos2d::ui::Widget*)csbnode->getChildByName("itembg");
	bgitem->addTouchEventListener(CC_CALLBACK_2(MyHeroNode::onbgClick, this));
	bgitem->setSwallowTouches(false);

	//头像框
	headbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("herobox");

	//头像
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("heroimg");

	//名字
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	//职业
	vocationtextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("vocationtext");
	vocationtextlbl->setString(ResourceLang::map_lang["vocationtext"]);

	vocationlbl = (cocos2d::ui::Text*)csbnode->getChildByName("vocation");

	//等级
	lvtextlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lvtext");
	lvtextlbl->setString(ResourceLang::map_lang["lvtexts"]);

	lvlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");

	//按钮
	actbtn = (cocos2d::ui::ImageView*)csbnode->getChildByName("actionbtn");
	actbtn->addTouchEventListener(CC_CALLBACK_2(MyHeroNode::onBtnClick, this));

	silver = (cocos2d::ui::Widget*)csbnode->getChildByName("silver");
	count = (cocos2d::ui::Text*)silver->getChildByName("count");
	std::string s = StringUtils::format("%d", (herodata->getLevel() + 1) * RSILVERCOUNT);
	count->setString(s);

	langtype = GlobalInstance::getInstance()->getLang();

	//按钮文字
	actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");

	statetag = (cocos2d::ui::ImageView*)csbnode->getChildByName("tag");

	tagtext = (cocos2d::ui::Text*)statetag->getChildByName("text");

	countdown = (cocos2d::ui::Text*)csbnode->getChildByName("countdown");

	hpdesc = (cocos2d::ui::Text*)csbnode->getChildByName("hpdesc");

	setStateTag(herodata->getState());

	for (int i = 0; i < 5; i++)
	{
		std::string starstr = StringUtils::format("star%d", i+1);
		stars[i] = (cocos2d::ui::ImageView*)csbnode->getChildByName(starstr);
		stars[i]->setVisible(false);
	}

	updateData();

	if (m_showtype == HS_DEAD)
	{
		hpdesc->setVisible(false);
		bgitem->setTouchEnabled(false);
		statetag->setVisible(false);
		actbtntxt->loadTexture(ResourcePath::makeTextImgPath("cure_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		silver->setVisible(true);
	}
	else if (m_showtype == HS_TRAINING)
	{
		bgitem->setTouchEnabled(false);
		updateContent();

	}
	if (m_heroData->getPower().getValue() >= 100)
	{
		std::string hpstr = StringUtils::format(ResourceLang::map_lang["powerdesc"].c_str(), m_heroData->getPower().getValue(), ResourceLang::map_lang["powermaxtext"].c_str());;
		hpdesc->setString(hpstr);
	}

	updateTime(0);
	this->schedule(schedule_selector(MyHeroNode::updateTime), 1.0f);

	return true;
}

void MyHeroNode::updateContent()
{
	if (m_heroData->getState() == HS_TRAINING)
	{
		actbtntxt->loadTexture(ResourcePath::makeTextImgPath("herocancel_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		actbtn->loadTexture("ui/actionbtn_yellow.png", cocos2d::ui::Widget::TextureResType::PLIST);
		countdown->setVisible(true);
	}
	else
	{
		actbtntxt->loadTexture(ResourcePath::makeTextImgPath("training_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		actbtn->loadTexture("ui/actionbtn_blue.png", cocos2d::ui::Widget::TextureResType::PLIST);
		countdown->setVisible(false);
	}
}

void MyHeroNode::updateTime(float dt)
{
	if (m_heroData->getState() == HS_TRAINING)
	{
		int lefttime = 0;
		int refreshtime = m_heroData->getTrainTime();
		int pasttime = GlobalInstance::servertime - refreshtime;
		if (pasttime >= m_heroData->getTrainHour())
		{
			int lv = Building::map_buildingDatas["4trainigroom"]->level.getValue();
			int bexp = Building::map_buildingDatas["4trainigroom"]->vec_exdatatrain[lv];
			DynamicValueInt dexp;
			dexp.setValue(m_heroData->getExp().getValue() + m_heroData->getTrainHour() / 3600 * bexp);
			m_heroData->setExp(dexp);

			m_heroData->setTrainHour(0);
			m_heroData->setTrainTime(0);
			m_heroData->setState(HS_OWNED);
			countdown->setVisible(false);
		}
		else
		{
			lefttime = m_heroData->getTrainHour() - pasttime;
		}
		std::string timestr = StringUtils::format("%02d:%02d:%02d", lefttime / 3600, lefttime % 3600 / 60, lefttime % 3600 % 60);
		countdown->setString(timestr);
	}
	else
	{
		if (m_heroData->getPower().getValue() < 100)
		{
			int pasttime = GlobalInstance::servertime - m_heroData->getPowerTime();
			int lefttime = HEROPOWER_RESETTIME - pasttime % HEROPOWER_RESETTIME;
			std::string timestr = StringUtils::format("%02d:%02d", lefttime / 60, lefttime % 60);

			DynamicValueInt dv;
			int count = pasttime / HEROPOWER_RESETTIME;
			if (count > 0)
			{
				m_heroData->setPowerTime(GlobalInstance::servertime);
				if (count + m_heroData->getPower().getValue() > 100)
					count = 100;
				else
					count += m_heroData->getPower().getValue();
				dv.setValue(count);

				m_heroData->setPower(dv);
			}

			std::string hpstr;
			if (count >= 100)
				timestr = ResourceLang::map_lang["powermaxtext"].c_str();

			hpstr = StringUtils::format(ResourceLang::map_lang["powerdesc"].c_str(), m_heroData->getPower().getValue(), timestr.c_str());
			hpdesc->setString(hpstr);
			GlobalInstance::getInstance()->saveHero(m_heroData);
		}
	}
}

void MyHeroNode::updateData()
{
	std::string str = StringUtils::format("ui/h_%d_%d.png", m_heroData->getVocation(), m_heroData->getSex());
	headimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("ui/herobox_%d.png", m_heroData->getPotential());
	headbox->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("vocation_%d", m_heroData->getVocation());
	vocationlbl->setString(ResourceLang::map_lang[str]);

	str = StringUtils::format("Lv.%d", m_heroData->getLevel() + 1);
	lvlbl->setString(str);

	namelbl->setString(m_heroData->getName());

	for (int i = 0; i < m_heroData->getBreakUpper(); i++)
	{
		stars[i]->setVisible(true);
	}
}


void MyHeroNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_showtype == HS_OWNED)
		{
			GlobalInstance::getInstance()->fireHero(this->getTag());
			InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
			if (innroomLayer != NULL)
				innroomLayer->refreshMyHerosUi();
		}
		else if (m_showtype == HS_TAKEON)
		{
			SelectMyHerosLayer* selectheroLayer = (SelectMyHerosLayer*)g_mainScene->getChildByName("0outtown")->getChildByName("selectmyheroslayer");
			int selectIndex = selectheroLayer->getTag();
			OutTownLayer* outTownLayer = (OutTownLayer*)g_mainScene->getChildByName("0outtown");
			CardHeroNode* cardheroNode = (CardHeroNode*)outTownLayer->getChildByTag(selectIndex);
			if (m_heroData->getState() == HS_OWNED)
			{
				//清楚掉之前选择的
				for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
				{
					if (GlobalInstance::vec_myHeros[i]->getPos() == selectheroLayer->getTag() + 1)
					{
						GlobalInstance::vec_myHeros[i]->setState(HS_OWNED);
						GlobalInstance::vec_myHeros[i]->setPos(0);
						//selectheroLayer->getMyHeroNode(this->getTag())->setStateTag(HS_OWNED);
						break;
					}
				}
				m_heroData->setState(HS_TAKEON);
				m_heroData->setPos(selectIndex + 1);
				GlobalInstance::myCardHeros[selectIndex] = m_heroData;
				GlobalInstance::getInstance()->saveMyHeros();
				setStateTag(HS_TAKEON);
				
				cardheroNode->setData(m_heroData);
				selectheroLayer->removeFromParentAndCleanup(true);
			}
			else if (m_heroData->getState() == HS_TAKEON)
			{
				int heroinwhere = 0;
				if (selectIndex + 1 == m_heroData->getPos())//取消框里的那个英雄
				{
					heroinwhere = selectIndex;
				}
				else//取消另外一个框的英雄
				{
					heroinwhere = m_heroData->getPos() - 1;
					cardheroNode = (CardHeroNode*)outTownLayer->getChildByTag(heroinwhere);
				}
				GlobalInstance::myCardHeros[heroinwhere] = NULL;

				m_heroData->setState(HS_OWNED);
				m_heroData->setPos(0);
				GlobalInstance::getInstance()->saveMyHeros();
				setStateTag(HS_OWNED);
				cardheroNode->setData(NULL);
			}
			outTownLayer->updateHeroCarry();
		}
		else if (m_showtype == HS_DEAD)
		{
			DynamicValueInt dval = GlobalInstance::getInstance()->getMySoliverCount();
			if (dval.getValue() >= (m_heroData->getLevel() + 1) * RSILVERCOUNT)
			{
				DynamicValueInt dva;
				dva.setValue((m_heroData->getLevel() + 1) * RSILVERCOUNT);
				GlobalInstance::getInstance()->costMySoliverCount(dva);

				m_heroData->setState(HS_OWNED);
				m_heroData->setHp(m_heroData->getMaxHp());
				GlobalInstance::getInstance()->saveMyHeros();
				HospitalLayer* hospitalLayer = (HospitalLayer*)g_mainScene->getChildByName("1hospital");
				if (hospitalLayer != NULL)
				{
					hospitalLayer->updateContent();
				}
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["nomoresilver"]);
			}
		}
		else if (m_showtype == HS_TRAINING)
		{
			if (m_heroData->getState() == HS_OWNED)
			{
				TrainSelectLayer* layer = TrainSelectLayer::create(m_heroData,this);
				g_mainScene->addChild(layer, 1, "TrainSelectLayer");
			}
			else if (m_heroData->getState() == HS_TRAINING)
			{
				TrainHintLayer* layer = TrainHintLayer::create(m_heroData,this);
				g_mainScene->addChild(layer, 1, "TrainHintLayer");
			}
			else if (m_heroData->getState() == HS_TAKEON)
			{
				MovingLabel::show(ResourceLang::map_lang["canttrain"]);
			}
		}
	}
}

void MyHeroNode::onbgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Layer* layer = HeroAttrLayer::create(m_heroData);
		g_mainScene->addChild(layer, 0, this->getTag());
	}
}

void MyHeroNode::setStateTag(int state)
{
	int langtype = GlobalInstance::getInstance()->getLang();

	std::string btntextstr;
	if (m_showtype == HS_TAKEON)
	{
		if (state == HS_TAKEON)
		{
			statetag->setVisible(true);
			statetag->loadTexture(ResourcePath::makePath("ui/herotag_0.png"), cocos2d::ui::Widget::TextureResType::PLIST);
			btntextstr = "herocancel_text";

		}
		else if (state == HS_OWNED)
		{
			statetag->setVisible(false);
			btntextstr = "herofight_text";
		}
	}
	else
	{
		if (state == HS_TAKEON)
		{
			statetag->setVisible(true);
			statetag->loadTexture(ResourcePath::makePath("ui/herotag_0.png"), cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
			statetag->setVisible(false);
		btntextstr = "firebtn_text";
	}
	if (statetag->isVisible())
	{
		std::string tagtextstr = StringUtils::format("%d", m_heroData->getPos());
		tagtext->setString(tagtextstr);
	}
	actbtntxt->loadTexture(ResourcePath::makeTextImgPath(btntextstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
}