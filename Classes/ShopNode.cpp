#include "ShopNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "MovingLabel.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "ShopLayer.h"
#include "GiftContentLayer.h"
#include "MainScene.h"

ShopNode::ShopNode()
{

}


ShopNode::~ShopNode()
{

}

ShopNode* ShopNode::create(ShopData* shopdata)
{
	ShopNode *pRet = new(std::nothrow)ShopNode();
	if (pRet && pRet->init(shopdata))
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

bool ShopNode::init(ShopData* shopdata)
{
	m_shopdata = shopdata;
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("shopNode.csb"));
	this->addChild(csbnode);

	cocos2d::ui::Widget* shopitembg = (cocos2d::ui::Widget*)csbnode->getChildByName("shopitembg");
	shopitembg->setSwallowTouches(false);

	int langtype = GlobalInstance::getInstance()->getLang();

	//icon
	cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)csbnode->getChildByName("icon");
	std::string str = StringUtils::format("ui/%s.png", shopdata->icon.c_str());
	icon->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	//Ãû×Ö
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(shopdata->name);
	Label *namelabel = (Label*)namelbl->getVirtualRenderer();
	namelabel->enableBold();
	/*namelbl->setVisible(false);

	Label *namelabel = Label::createWithTTF(shopdata->name, FONT_NAME, 23);
	namelabel->setPosition(namelbl->getPosition());
	namelabel->setAnchorPoint(Vec2(0, 0.5));
	namelabel->setColor(Color3B(236, 132, 78));
	namelabel->enableBold();
	csbnode->addChild(namelabel);*/

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(shopdata->desc);
	desc->setFontSize(19);

	cocos2d::ui::Button* buybtn = (cocos2d::ui::Button*)csbnode->getChildByName("buybtn");
	buybtn->addTouchEventListener(CC_CALLBACK_2(ShopNode::onBtnClick, this));
	buybtn->setSwallowTouches(false);
	cocos2d::ui::ImageView* buybtntext = (cocos2d::ui::ImageView*)buybtn->getChildByName("text");
	buybtntext->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_6_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* coin = (cocos2d::ui::Widget*)csbnode->getChildByName("coin");
	cocos2d::ui::Text* count = (cocos2d::ui::Text*)coin->getChildByName("count");
	str = StringUtils::format("x%d", shopdata->count);
	count->setString(str);
	if (shopdata->type == COIN)
	{
		coin->setVisible(true);
	}
	else
	{
		coin->setVisible(false);
	}

	cocos2d::ui::Text* price = (cocos2d::ui::Text*)csbnode->getChildByName("price");
	str = StringUtils::format(ResourceLang::map_lang["shoppricetext"].c_str(), shopdata->price);
	price->setString(str);

	return true;
}

void ShopNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
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
		if (m_shopdata->type == COIN)
		{
			ShopLayer::beginPay(this->getTag());
		}
		else
		{
			GiftContentLayer* layer = GiftContentLayer::create(m_shopdata, this->getTag());
			ShopLayer* shop = (ShopLayer*)g_mainScene->getChildByName("ShopLayer");
			if (shop != NULL)
			{
				shop->addChild(layer);
				AnimationEffect::openAniEffect((Layer*)layer);
			}
		}
	}
}