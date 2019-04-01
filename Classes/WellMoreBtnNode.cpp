#include "WellMoreBtnNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "WellGiftLayer.h"
#include "RebatLayer.h"

WellMoreBtnNode::WellMoreBtnNode()
{

}

WellMoreBtnNode::~WellMoreBtnNode()
{

}

WellMoreBtnNode* WellMoreBtnNode::create()
{
	WellMoreBtnNode *pRet = new(std::nothrow)WellMoreBtnNode();
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

bool WellMoreBtnNode::init()
{
	if (!Node::init())
		return false;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("wellmorebtnnode.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* btnbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("bg");
	btnbg->setSwallowTouches(true);

	for (int i = 0; i < btnbg->getChildrenCount(); i++)
	{
		std::string name = btnbg->getChildren().at(i)->getName();
		cocos2d::ui::Widget* clickwidget = (cocos2d::ui::Widget*)btnbg->getChildByName(name);
		clickwidget->setTag(i);
		clickwidget->addTouchEventListener(CC_CALLBACK_2(WellMoreBtnNode::onBtnClick, this));

		cocos2d::ui::ImageView* textimg = (cocos2d::ui::ImageView*)clickwidget->getChildByName("text");
		std::string textname = StringUtils::format("main_%s_text", name.c_str());
		textimg->loadTexture(ResourcePath::makeTextImgPath(textname, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	}
	return true;
}

void WellMoreBtnNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case REBATEBTN:
		{
			RebatLayer* layer = RebatLayer::create();
			g_mainScene->addChild(layer, 0, "rebatlayer");
			AnimationEffect::openAniEffect(layer);
		}
			break;
		case WELLCARDBTN:
		{
			WellGiftLayer* layer = WellGiftLayer::create();
			g_mainScene->addChild(layer, 0, "wellgiftlayer");
			AnimationEffect::openAniEffect(layer);
		}
			break;
		default:
			break;
		}
	}
}