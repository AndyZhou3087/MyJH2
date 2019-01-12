#include "MoreBtnNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "SoundManager.h"
#include "SettingLayer.h"
#include "AnimationEffect.h"
#include "LoginRewardV2Layer.h"

MoreBtnNode::MoreBtnNode()
{

}

MoreBtnNode::~MoreBtnNode()
{

}

MoreBtnNode* MoreBtnNode::create()
{
	MoreBtnNode *pRet = new(std::nothrow)MoreBtnNode();
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

bool MoreBtnNode::init()
{
	if (!Node::init())
		return false;

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("morebtnNode.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* btnbg = (cocos2d::ui::ImageView*)csbnode->getChildByName("morebg");
	btnbg->setSwallowTouches(true);

	for (int i = 0; i < btnbg->getChildrenCount(); i++)
	{
		std::string name = btnbg->getChildren().at(i)->getName();
		cocos2d::ui::Widget* clickwidget = (cocos2d::ui::Widget*)btnbg->getChildByName(name);
		clickwidget->setTag(i);
		clickwidget->addTouchEventListener(CC_CALLBACK_2(MoreBtnNode::onBtnClick, this));

		cocos2d::ui::ImageView* textimg = (cocos2d::ui::ImageView*)clickwidget->getChildByName("text");
		std::string textname = StringUtils::format("main_%s_text", name.c_str());
		textimg->loadTexture(ResourcePath::makeTextImgPath(textname, langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	}
	return true;
}

void MoreBtnNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case SETBTN:
		{
			SettingLayer* layer = SettingLayer::create();
			g_mainScene->addChild(layer, 0, "settinglayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case LOGINBTN:
		{
			LoginRewardV2Layer* layer = LoginRewardV2Layer::create();
			g_mainScene->addChild(layer, 0, "loginrewardv2Layer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		default:
			break;
		}
	}
}