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
	contentscoll->setBounceEnabled(true);

	Label* contentlbl = Label::createWithTTF("", FONT_NAME, 25);
	contentlbl->setAnchorPoint(Vec2(0, 1));
	contentlbl->setColor(Color3B(100, 100, 100));
	contentlbl->setHorizontalAlignment(TextHAlignment::LEFT);
	//contentlbl->enableShadow(Color4B::BLACK, Size(1, -1));
	contentlbl->setLineBreakWithoutSpace(true);
	contentlbl->setLineSpacing(8);
	contentlbl->setMaxLineWidth(contentscoll->getContentSize().width);
	contentscoll->addChild(contentlbl);

	cocos2d::ui::Button* okbtn = (cocos2d::ui::Button*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(NewPopLayer::onBtnClick, this));
	cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)okbtn->getChildByName("btntext");
	btntext->ignoreContentAdaptWithSize(true);
	btntext->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	okbtn->setTag(unlockchapter);
	okbtn->setPositionX(360);

	cocos2d::ui::Button* cancelbtn = (cocos2d::ui::Button*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(NewPopLayer::onBtnClick, this));
	cancelbtn->setTag(1001);
	cocos2d::ui::ImageView* cancelbtntext = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("btntext");
	cancelbtntext->ignoreContentAdaptWithSize(true);
	cancelbtntext->loadTexture(ResourcePath::makeTextImgPath("nexttime_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	cancelbtn->setVisible(false);

	if (unlockchapter != -1)
	{
		scenetitle->setVisible(true);
		scenetext->setVisible(true);
		Label* scenetextlbl = (Label*)scenetext->getVirtualRenderer();
		scenetextlbl->setLineSpacing(10);
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

		if (unlockchapter >= 5)
		{
			str.append("\n");
			int maxlv = BASEMAXHEROLV;
			for (int i = 0; i < sizeof(ADDMAXHEROLV) / sizeof(ADDMAXHEROLV[0]); i++)
			{
				if (i < unlockchapter)
					maxlv += ADDMAXHEROLV[i];
			}
			std::string unlocklvstr = StringUtils::format(ResourceLang::map_lang["unlockmaxherolv"].c_str(), maxlv);
			str.append(unlocklvstr);
		}

		scenetext->setString(str);

		title->loadTexture(ResourcePath::makeTextImgPath("text_unlock", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		title->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath("text_unlock", langtype))->getContentSize());
		titletext->setString(ResourceLang::map_lang["unlocktitletext"]);

		str = StringUtils::format("unlockchapter%d", unlockchapter);
		//content->setString(ResourceLang::map_lang[str]);
		std::string contentstr = ResourceLang::map_lang[str];

		contentlbl->setString(contentstr);

		int innerheight = contentlbl->getStringNumLines() * 33;//contentlbl->getHeight();
		int contentheight = contentscoll->getContentSize().height;
		if (innerheight < contentheight)
		{
			innerheight = contentheight;
			contentscoll->setBounceEnabled(false);
		}
		contentscoll->setInnerContainerSize(Size(contentscoll->getContentSize().width, innerheight));
		contentlbl->setPosition(Vec2(0, innerheight));

		cocos2d::ui::Text* tbookdesc = (cocos2d::ui::Text*)csbnode->getChildByName("tbookdesc");

		std::string bookidstr = StringUtils::format("ts%02d", unlockchapter - 1);
		std::string bookstr = StringUtils::format(ResourceLang::map_lang["getbookdesc"].c_str(), GlobalInstance::map_AllResources[bookidstr].name.c_str());
		tbookdesc->setString(bookstr);
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

		int innerheight = contentlbl->getStringNumLines() * 33;//contentlbl->getHeight();
		int contentheight = contentscoll->getContentSize().height;
		if (innerheight < contentheight)
		{
			innerheight = contentheight;
			contentscoll->setBounceEnabled(false);
		}

		contentscoll->setInnerContainerSize(Size(contentscoll->getContentSize().width, innerheight));
		contentlbl->setPosition(Vec2(0, innerheight));
		
		int subtype = GlobalInstance::vec_notice[0].subtype;
		if (subtype > 0)
		{
			btntext->loadTexture(ResourcePath::makeTextImgPath("upgrade_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			if (subtype == 2)
			{
				okbtn->setPositionX(230);
				cancelbtn->setVisible(true);
				cancelbtn->setTag(-1000);
				cancelbtn->setPositionX(480);
			}
		}
	}


	//屏蔽下层点击
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

		if (node->getTag() < 0)//公告类型
		{
			int subtype = GlobalInstance::vec_notice[0].subtype;

			if (node->getTag() == -1)
			{
				if (subtype == 0)//纯公告
				{
					GlobalInstance::vec_notice[0].status = 1;
					HttpDataSwap::init(NULL)->updateMessageStatus(GlobalInstance::vec_notice[0].id, 1);
					AnimationEffect::closeAniEffect((Layer*)this);
				}
				else if (subtype == 1 || subtype == 2)//更新
				{
					GlobalInstance::getInstance()->upgradeApp(GlobalInstance::upgradeurl);
				}
			}
			else
			{
				if (subtype == 2)
				{
					GlobalInstance::vec_notice[0].status = 1;
					HttpDataSwap::init(NULL)->updateMessageStatus(GlobalInstance::vec_notice[0].id, 1);
				}
				AnimationEffect::closeAniEffect((Layer*)this);
			}
		}
		else
		{
			AnimationEffect::closeAniEffect((Layer*)this);
		}
	}
}