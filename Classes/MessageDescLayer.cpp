#include "MessageDescLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "HttpDataSwap.h"
#include "MessageLayer.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"

MessageDescLayer::MessageDescLayer()
{

}


MessageDescLayer::~MessageDescLayer()
{

}

MessageDescLayer* MessageDescLayer::create(int index)
{
	MessageDescLayer *pRet = new(std::nothrow)MessageDescLayer();
	if (pRet && pRet->init(index))
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

bool MessageDescLayer::init(int index)
{
	if (!Layer::init())
	{
		return false;
	}

	MessageData data = GlobalInstance::vec_messsages[index];

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("messageDescLayer.csb"));
	this->addChild(m_csbnode);

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)m_csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::Widget* awdnode = (cocos2d::ui::Widget*)m_csbnode->getChildByName("awdnode");

	int langtype = GlobalInstance::getInstance()->getLang();

	std::string actiontextstr;
	int btntag = 0;

	int textscrollheight = 0;
	std::string contentstr = data.content;
	if (data.type == 0 || data.type == 3)
	{
		actiontextstr = "closebtn_text";
		btntag = 1000;
		textscrollheight = 500;
		awdnode->setVisible(false);

		if (data.subtype == 1 || data.subtype == 2 || data.subtype == 3)
		{
			btntag = 1003;
			actiontextstr = "upgrade_text";
		}
	}
	else
	{
		actiontextstr = "msgallget_text";
		btntag = 1001;
		textscrollheight = 350;

		contentstr = data.content;


		int pos = data.content.find("$$");
		if (pos >= 0)
		{
			contentstr = contentstr.substr(0, pos);
			std::string awdstr = data.content.substr(pos + 2);
			std::vector<std::string> vec_str;
			CommonFuncs::split(awdstr, vec_str, ";");
			for (unsigned int n = 0; n < vec_str.size(); n++)
			{
				if (vec_str[n].length() > 0)
				{
					std::vector<std::string> one_str;
					CommonFuncs::split(vec_str[n], one_str, "-");

					if (one_str.size() > 1)
					{
						MSGAWDSDATA adata;
						adata.rid = one_str[0];
						adata.count = atoi(one_str[1].c_str());
						if (one_str.size() > 2)
							adata.qu = atoi(one_str[2].c_str());

						if (GlobalInstance::map_AllResources.find(adata.rid) != GlobalInstance::map_AllResources.end())
							awdslist.push_back(adata);
					}
				}
			}
		}

		int asize = awdslist.size();
		for (int m = 0; m < 5; m++)
		{

			std::string str = StringUtils::format("resbox%d", m);
			cocos2d::ui::ImageView*  resbox = (cocos2d::ui::ImageView*)awdnode->getChildByName(str);


			if (m < asize)
			{
				std::string resid = awdslist[m].rid;
				int qu = awdslist[m].qu;

				std::string qustr = "ui/resbox.png";
				int k = 0;
				for (; k < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); k++)
				{
					if (resid.compare(0, 1, RES_TYPES_CHAR[k]) == 0)
						break;
				}
				if (k >= T_ARMOR && k <= T_FASHION)
					qustr = StringUtils::format("ui/resbox_qu%d.png", qu);

				else if (k >= T_WG && k <= T_NG)
				{
					qu = GlobalInstance::map_GF[resid].qu;
					qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
				}
				else if (k >= T_RENS && k <= T_BOX)
				{
					qu = atoi(resid.substr(1).c_str()) - 1;
					qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
				}
				else if (k >= T_HEROCARD && k <= T_ARMCARD)
				{
					qu = atoi(resid.substr(1).c_str()) + 2;
					qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
				}
				else if(k == T_EPIECE)
				{
					Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
					pieceicon->setAnchorPoint(Vec2(0, 1));
					pieceicon->setPosition(10, resbox->getContentSize().height - 10);
					resbox->addChild(pieceicon);
				}

				resbox->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

				cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");
				str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

				res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
				cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");
				str = StringUtils::format("%d", awdslist[m].count);
				countlbl->setString(str);

				cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)resbox->getChildByName("name");
				namelbl->setString(GlobalInstance::map_AllResources[resid].name);
			}
			else
			{
				resbox->setVisible(false);
			}
		}
	}
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(MessageDescLayer::onBtnClick, this));
	actionbtn->setTag(btntag);
	//按钮1文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->ignoreContentAdaptWithSize(true);
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath(actiontextstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	actionbtntxt->ignoreContentAdaptWithSize(true);

	//标题
	cocos2d::ui::Text* title = (cocos2d::ui::Text*)m_csbnode->getChildByName("title");
	title->setString(data.title);

	cocos2d::ui::ScrollView* contentscroll = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("contentscroll");

	contentscroll->setContentSize(Size(contentscroll->getContentSize().width, textscrollheight));
	contentscroll->setBounceEnabled(true);

	int contentwidth = 500;
	Label* contentlbl = Label::createWithTTF(contentstr, FONT_NAME, 25);
	contentlbl->setAnchorPoint(Vec2(0, 1));
	contentlbl->setColor(Color3B(255, 255, 255));
	contentlbl->setHorizontalAlignment(TextHAlignment::LEFT);
	contentlbl->setLineBreakWithoutSpace(true);
	contentlbl->setLineSpacing(8);
	contentlbl->setMaxLineWidth(contentwidth);
	contentscroll->addChild(contentlbl);
	int innerheight = contentlbl->getStringNumLines() * 35;
	int contentheight = contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	contentscroll->setInnerContainerSize(Size(contentscroll->getContentSize().width, innerheight));
	contentlbl->setPosition(Vec2((contentscroll->getContentSize().width - contentwidth)/2, innerheight));

	if (data.status == 0)
	{
		HttpDataSwap::init(NULL)->updateMessageStatus(data.id, 1);
		this->scheduleOnce(schedule_selector(MessageDescLayer::delay), 0.5f);
	}
	else if (data.status == 3)
	{
		actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("msggeted_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		actionbtn->setBright(false);
		actionbtn->setTag(1002);
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void MessageDescLayer::delay(float dt)
{
	int tag = this->getTag();
	GlobalInstance::vec_messsages[tag].status = 1;
	MessageLayer* mlayer = (MessageLayer*)this->getParent();
	mlayer->updateStatus(tag);
}

void MessageDescLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* node = (cocos2d::ui::Button*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 1000:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1001:
		{
			node->setEnabled(false);
			for (unsigned int i = 0; i < awdslist.size(); i++)
			{
				int qu = awdslist[i].qu;
				std::string resid = awdslist[i].rid;
				int stc = GlobalInstance::getInstance()->generateStoneCount(qu);

				int addcount = awdslist[i].count;

				if (resid.compare("r006") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(addcount);
					GlobalInstance::getInstance()->addMySoliverCount(dvint);
				}
				else if (resid.compare("r012") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(addcount);
					GlobalInstance::getInstance()->addMyCoinCount(dvint);
				}
				else
					MyRes::Add(awdslist[i].rid, addcount, MYSTORAGE, qu, stc);
			}
			MovingLabel::show(ResourceLang::map_lang["msgawdsucc"]);

			int tag = this->getTag();
			GlobalInstance::vec_messsages[tag].status = 3;
			MessageLayer* mlayer = (MessageLayer*)this->getParent();
			mlayer->updateStatus(tag);

			HttpDataSwap::init(NULL)->updateMessageStatus(GlobalInstance::vec_messsages[tag].id, 3);

			AnimationEffect::closeAniEffect((Layer*)this);
		}
			break;
		case 1002:
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 1003:
			GlobalInstance::getInstance()->upgradeApp(GlobalInstance::upgradeurl);
			break;
		default:
			break;
		}
	}
}