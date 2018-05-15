#include "MyHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "HeroAttrLayer.h"
#include "MainScene.h"
#include "InnRoomLayer.h"
#include "GlobalInstance.h"

MyHeroNode::MyHeroNode()
{

}


MyHeroNode::~MyHeroNode()
{

}

MyHeroNode* MyHeroNode::create(Hero* herodata)
{
	MyHeroNode *pRet = new(std::nothrow)MyHeroNode();
	if (pRet && pRet->init(herodata))
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

bool MyHeroNode::init(Hero* herodata)
{

	m_heroData = herodata;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("myHeroNode.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Widget* bgitem = (cocos2d::ui::Widget*)csbnode->getChildByName("itembg");
	bgitem->addTouchEventListener(CC_CALLBACK_2(MyHeroNode::onbgClick, this));

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

	//解雇按钮
	cocos2d::ui::Widget* actbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actbtn->addTouchEventListener(CC_CALLBACK_2(MyHeroNode::onBtnClick, this));

	int langtype = GlobalInstance::getInstance()->getLang();

	//解雇按钮文字
	cocos2d::ui::ImageView* actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");
	actbtntxt->loadTexture(ResourcePath::makeTextImgPath("herofire_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	updateData();

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
}


void MyHeroNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		InnRoomLayer* innroomLayer = (InnRoomLayer*)g_mainScene->getChildByName("innroom");
		innroomLayer->fireHero();
	}
}

void MyHeroNode::onbgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Layer* layer = HeroAttrLayer::create(MYHERO, m_heroData);
		g_mainScene->getChildByName("innroom")->addChild(layer, 0, this->getTag());
	}
}