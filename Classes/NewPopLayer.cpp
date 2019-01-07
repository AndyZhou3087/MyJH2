#include "NewPopLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"

USING_NS_CC;

NewPopLayer::NewPopLayer()
{

}

NewPopLayer::~NewPopLayer()
{

}


NewPopLayer* NewPopLayer::create(int para)
{
	NewPopLayer *pRet = new(std::nothrow)NewPopLayer();
	if (pRet && pRet->init(para))
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
bool NewPopLayer::init(int unlockchapter)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("newPopLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	cocos2d::ui::Text* titletext = (cocos2d::ui::Text*)csbnode->getChildByName("titletext");
	cocos2d::ui::Text* scenetitle = (cocos2d::ui::Text*)csbnode->getChildByName("scenetitle");
	scenetitle->setString(ResourceLang::map_lang["unlockscenetitle"]);
	cocos2d::ui::Text* scenetext = (cocos2d::ui::Text*)csbnode->getChildByName("scenetext");
	//cocos2d::ui::Text* content = (cocos2d::ui::Text*)csbnode->getChildByName("content");

	cocos2d::ui::ScrollView* contentscoll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");
	contentscoll->setScrollBarEnabled(false);

	Label* contentlbl = Label::createWithTTF("", FONT_NAME, 25);
	contentlbl->setAnchorPoint(Vec2(0, 1));
	contentlbl->setColor(Color3B(100, 100, 100));
	contentlbl->setHorizontalAlignment(TextHAlignment::LEFT);
	//contentlbl->enableShadow(Color4B::BLACK, Size(1, -1));
	contentlbl->setLineBreakWithoutSpace(true);
	contentlbl->setMaxLineWidth(contentscoll->getContentSize().width);
	contentscoll->addChild(contentlbl);

	cocos2d::ui::Button* okbtn = (cocos2d::ui::Button*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(NewPopLayer::onBtnClick, this));
	cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)okbtn->getChildByName("btntext");
	btntext->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	okbtn->setTag(unlockchapter);
	if (unlockchapter != -1)
	{
		scenetitle->setVisible(true);
		scenetext->setVisible(true);
		std::string str;
		std::map<std::string, S_MainMap>::iterator it;
		for (it = GlobalInstance::map_mapsdata.begin(); it != GlobalInstance::map_mapsdata.end(); it++)
		{
			std::string mainmapid = it->first;
			int c = atoi(mainmapid.substr(1, mainmapid.find_first_of("-") - 1).c_str());
			if (c == unlockchapter)
			{
				if (str.length() > 0)
					str.append(ResourceLang::map_lang["dunhao"]);
				str.append(GlobalInstance::map_AllResources[mainmapid].name);
			}
		}
		str = StringUtils::format("%s", str.c_str());
		scenetext->setString(str);

		title->loadTexture(ResourcePath::makeTextImgPath("text_unlock", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		title->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("text_unlock", langtype))->getContentSize());
		titletext->setString(ResourceLang::map_lang["unlocktitletext"]);

		str = StringUtils::format("unlockchapter%d", unlockchapter);
		//content->setString(ResourceLang::map_lang[str]);

		contentlbl->setString(ResourceLang::map_lang[str]);

		int innerheight = contentlbl->getStringNumLines() * 25;//contentlbl->getHeight();
		int contentheight = contentscoll->getContentSize().height;
		if (innerheight < contentheight)
			innerheight = contentheight;
		contentscoll->setInnerContainerSize(Size(contentscoll->getContentSize().width, innerheight));
		contentlbl->setPosition(Vec2(0, innerheight));

	}
	else
	{
		title->loadTexture(ResourcePath::makeTextImgPath("text_notice", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		title->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("text_notice", langtype))->getContentSize());
		titletext->setString(GlobalInstance::vec_notice[0].title);
		scenetitle->setVisible(false);
		scenetext->setVisible(false);
		contentscoll->setPositionY(790);
		contentscoll->setContentSize(Size(contentscoll->getContentSize().width, 380));
		contentlbl->setString(GlobalInstance::vec_notice[0].content);

		int innerheight = contentlbl->getStringNumLines() * 25;//contentlbl->getHeight();
		int contentheight = contentscoll->getContentSize().height;
		if (innerheight < contentheight)
			innerheight = contentheight;
		contentscoll->setInnerContainerSize(Size(contentscoll->getContentSize().width, innerheight));
		contentlbl->setPosition(Vec2(0, innerheight));
	}


	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void NewPopLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (node->getTag() < 0)
			HttpDataSwap::init(NULL)->updateMessageStatus(GlobalInstance::vec_notice[0].id, 1);

		AnimationEffect::closeAniEffect((Layer*)this);
	}
}