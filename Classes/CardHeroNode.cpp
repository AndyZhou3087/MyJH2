#include "CardHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "SelectMyHerosLayer.h"

CardHeroNode::CardHeroNode()
{

}


CardHeroNode::~CardHeroNode()
{

}

CardHeroNode* CardHeroNode::create()
{
	CardHeroNode *pRet = new(std::nothrow)CardHeroNode();
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

bool CardHeroNode::init()
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("cardHeroNode.csb"));
	this->addChild(csbnode);

	desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");

	//头像框
	headbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	headbox->addTouchEventListener(CC_CALLBACK_2(CardHeroNode::onClick, this));

	//头像
	headimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("head");

	//名字
	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	//职业

	vocationlbl = (cocos2d::ui::Text*)csbnode->getChildByName("vocation");

	//等级

	lvlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");

	return true;
}

void CardHeroNode::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Layer* layer = SelectMyHerosLayer::create(HS_TAKEON);
		layer->setTag(this->getTag());
		this->getParent()->addChild(layer, 0, "selectmyheroslayer");
	}
}

void CardHeroNode::setData(Hero* herodata)
{
	if (herodata != NULL)
	{
		std::string str = StringUtils::format("cardh_%d_%d.png", herodata->getVocation(), herodata->getSex());
		headimg->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);
		headimg->setVisible(true);

		str = StringUtils::format("cardherobox_%d.png", herodata->getPotential());
		headbox->loadTexture(ResourcePath::makeImagePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

		str = StringUtils::format("vocation_%d", herodata->getVocation());
		vocationlbl->setString(ResourceLang::map_lang[str]);
		vocationlbl->setVisible(true);

		str = StringUtils::format("lv.%d", herodata->getLevel()+1);
		lvlbl->setString(str);
		lvlbl->setVisible(true);

		namelbl->setString(herodata->getName());
		namelbl->setVisible(true);

		desclbl->setVisible(false);
	}
	else
	{
		headbox->loadTexture(ResourcePath::makeImagePath("cardherobox_.png"), cocos2d::ui::Widget::TextureResType::LOCAL);
		headimg->setVisible(false);
		vocationlbl->setVisible(false);
		lvlbl->setVisible(false);
		namelbl->setVisible(false);
		desclbl->setVisible(true);
		std::string indexstr = StringUtils::format("selheronum%d", this->getTag() + 1);
		std::string descstr = StringUtils::format(ResourceLang::map_lang["selherohinttext"].c_str(), ResourceLang::map_lang[indexstr].c_str());
		desclbl->setString(descstr);
	}
}
