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
	cocos2d::ui::Widget* actbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actbtn->addTouchEventListener(CC_CALLBACK_2(MyHeroNode::onBtnClick, this));

	silver = (cocos2d::ui::Widget*)csbnode->getChildByName("silver");
	count = (cocos2d::ui::Text*)silver->getChildByName("count");
	std::string s = StringUtils::format("%d", (herodata->getLevel() + 1) * RSILVERCOUNT);
	count->setString(s);

	int langtype = GlobalInstance::getInstance()->getLang();

	//按钮文字
	actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");

	statetag = (cocos2d::ui::ImageView*)csbnode->getChildByName("tag");

	tagtext = (cocos2d::ui::Text*)statetag->getChildByName("text");

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
		bgitem->setTouchEnabled(false);
		statetag->setVisible(false);
		actbtntxt->loadTexture(ResourcePath::makeTextImgPath("cure_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		silver->setVisible(true);
	}

	return true;
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
			CardHeroNode* cardheroNode = (CardHeroNode*)g_mainScene->getChildByName("0outtown")->getChildByTag(selectIndex);
			if (m_heroData->getState() == HS_OWNED)
			{
				//清楚掉之前选择的
				for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
				{
					if (GlobalInstance::vec_myHeros[i]->getPos() == selectheroLayer->getTag() + 1)
					{
						GlobalInstance::vec_myHeros[i]->setState(HS_OWNED);
						GlobalInstance::vec_myHeros[i]->setPos(0);
						selectheroLayer->getMyHeroNode(this->getTag())->setStateTag(HS_OWNED);
						break;
					}
				}
				m_heroData->setState(HS_TAKEON);
				m_heroData->setPos(selectIndex + 1);
				GlobalInstance::myCardHeros[selectIndex] = m_heroData;
				GlobalInstance::getInstance()->saveMyHeros();
				setStateTag(HS_TAKEON);
				
				cardheroNode->setData(m_heroData);
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
					cardheroNode = (CardHeroNode*)g_mainScene->getChildByName("0outtown")->getChildByTag(heroinwhere);
				}
				GlobalInstance::myCardHeros[heroinwhere] = NULL;

				m_heroData->setState(HS_OWNED);
				m_heroData->setPos(0);
				GlobalInstance::getInstance()->saveMyHeros();
				setStateTag(HS_OWNED);
				cardheroNode->setData(NULL);
			}
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