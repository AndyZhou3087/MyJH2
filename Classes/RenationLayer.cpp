#include "RenationLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"

USING_NS_CC;

RenationLayer::RenationLayer()
{
	lastClickNpcItem = NULL;
}

RenationLayer::~RenationLayer()
{

}


RenationLayer* RenationLayer::create()
{
	RenationLayer *pRet = new(std::nothrow)RenationLayer();
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

// on "init" you need to initialize your instance
bool RenationLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 220));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("renationLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	title->loadTexture(ResourcePath::makeTextImgPath("renatitontilte", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	addrname = (cocos2d::ui::Text*)csbnode->getChildByName("addrname");
	addrname->setString("");

	cocos2d::ui::Text* addrtext = (cocos2d::ui::Text*)csbnode->getChildByName("addrtext");
	addrtext->setString(ResourceLang::map_lang["renationaddrtext"]);

	//°´Å¥1
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(RenationLayer::onBtnClick, this));
	closebtn->setTag(1000);

	scrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);
	createScrollview();

	cocos2d::ui::Text* totalattrtitle = (cocos2d::ui::Text*)csbnode->getChildByName("totalattrtitle");
	totalattrtitle->setString(ResourceLang::map_lang["totalattrtitle"]);
	
	float totalattrval[] = { 0,0,0,0,0,0 };
	std::map<std::string, NpcFriendly>::iterator it;

	for (int m = 0; m < 6; m++)
	{
		for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); ++it)
		{
			std::string nid = it->first;
			for (unsigned int i = 0; i < it->second.relation.size(); i++)
			{
				if (it->second.relation[i] == NPC_FRIEND)
				{
					totalattrval[m] += GlobalInstance::map_npcrelation[nid].friendratio[m];
				}
				else if (it->second.relation[i] == NPC_MASTER)
				{
					totalattrval[m] += GlobalInstance::map_npcrelation[nid].masterratio[m];
				}
				else if (it->second.relation[i] == NPC_COUPEL)
				{
					totalattrval[m] += GlobalInstance::map_npcrelation[nid].conpelratio[m];
				}
			}
		}
	}

	for (int i = 0; i < 6; i++)
	{
		std::string str = StringUtils::format("attrtext_%d", i); 
		cocos2d::ui::Text* totalattrtext = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		std::string strkey = StringUtils::format("reattrtext_%d", i);
		totalattrtext->setString(ResourceLang::map_lang[strkey]);

		str = StringUtils::format("attrval_%d", i);
		cocos2d::ui::Text* totalattr = (cocos2d::ui::Text*)csbnode->getChildByName(str);

		std::string valueformat[] = { "+%.2f%%", "+%.2f%%", "+%.2f%%", "+%.3f%%", "+%.3f%%", "+%.3f%%" };

		str = StringUtils::format(valueformat[i].c_str(), totalattrval[i] * 100);
		totalattr->setString(str);

		str = StringUtils::format("renationitem_%d", i);

		cocos2d::ui::Text* npcattrtext = (cocos2d::ui::Text*)csbnode->getChildByName(str)->getChildByName("attrtext");

		npcattrlbl[i] = (cocos2d::ui::Text*)csbnode->getChildByName(str)->getChildByName("value");

		str = StringUtils::format("reattrtext_%d", i);
		npcattrtext->setString(ResourceLang::map_lang[str]);
	}
	//ÆÁ±ÎÏÂ²ãµã»÷
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		beginTouchPoint = touch->getLocation();

		return true;
	};
	listener->onTouchMoved = [=](Touch *touch, Event *event)
	{

	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void RenationLayer::createScrollview()
{
	std::map<std::string, NPCData>::iterator it;
	
	for (it = GlobalInstance::map_Npcs.begin(); it != GlobalInstance::map_Npcs.end(); it++)
	{
		std::string npcaddr = GlobalInstance::map_AllResources[it->first].desc;
		if (npcaddr.length() > 0)
		{
			vec_npc.push_back(it->first);
		}
	}

	int size = vec_npc.size();

	int itemwidth = 160;
	int itemheight = 230;
	int row = size % 4 == 0 ? size / 4 : (size / 4 + 1);
	int innerheight = itemheight * row;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (int n = 0; n < size; n++)
	{
		std::string npcid = vec_npc[n];
		Node* itemnode = CSLoader::createNode(ResourcePath::makePath("renationitemNode.csb"));

		itemnode->setPosition(Vec2(itemwidth/2 + n % 4 * itemwidth, innerheight - itemheight / 2 - n / 4 * itemheight + 5));
		scrollview->addChild(itemnode);

		cocos2d::ui::Text* name = (cocos2d::ui::Text*)itemnode->getChildByName("name");
		name->setString(GlobalInstance::map_AllResources[npcid].name);

		cocos2d::ui::ImageView* npcicon = (cocos2d::ui::ImageView*)itemnode->getChildByName("icon");
		std::string npcpath = StringUtils::format("ui/%s.png", GlobalInstance::map_Npcs[npcid].icon.c_str());
		npcicon->loadTexture(npcpath, cocos2d::ui::Widget::TextureResType::PLIST);

		cocos2d::ui::ImageView* renationitem = (cocos2d::ui::ImageView*)itemnode->getChildByName("renationitem");
		renationitem->addTouchEventListener(CC_CALLBACK_2(RenationLayer::onItemClick, this));
		renationitem->setTag(n);
		renationitem->ignoreContentAdaptWithSize(true);
		renationitem->setSwallowTouches(false);

		int rcount = 0;
		int ralation = NPC_NORMOL;
		if (GlobalInstance::map_myfriendly.find(npcid) != GlobalInstance::map_myfriendly.end())
		{
			for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[npcid].relation.size(); i++)
			{
				ralation = GlobalInstance::map_myfriendly[npcid].relation[i];

				Sprite* rbox = NULL;
				std::string renationsimptext;
				if (ralation == NPC_FRIEND)
				{
					rbox = Sprite::createWithSpriteFrameName("ui/renationitemr0.png");
					renationsimptext = ResourceLang::map_lang["renationtextsimp0"];
				}
				else if (ralation == NPC_MASTER)
				{
					rbox = Sprite::createWithSpriteFrameName("ui/renationitemr1.png");
					renationsimptext = ResourceLang::map_lang["renationtextsimp1"];
				}
				else if (ralation == NPC_COUPEL)
				{
					rbox = Sprite::createWithSpriteFrameName("ui/renationitemr2.png");
					renationsimptext = ResourceLang::map_lang["renationtextsimp2"];
				}
				if (rbox != NULL)
				{
					rbox->setPosition(Vec2(-50, 72 - 32 * rcount));
					itemnode->addChild(rbox);

					Label* rlbl = Label::createWithTTF(renationsimptext, FONT_NAME, 22);
					rlbl->setColor(Color3B(255, 255, 255));
					rlbl->setPosition(rbox->getPosition());
					itemnode->addChild(rlbl);
				}

				rcount++;
			}
		}

	}
}

void RenationLayer::updateNpcAddAttr(std::string npcid)
{
	addrname->setString(GlobalInstance::map_AllResources[GlobalInstance::map_AllResources[npcid].desc].name);

	std::string valueformat[] = {"+%.2f%%", "+%.2f%%", "+%.2f%%", "+%.3f%%", "+%.3f%%", "+%.3f%%"};
	float attrval[] = { 0,0,0,0,0,0 };
	if (GlobalInstance::map_myfriendly.find(npcid) != GlobalInstance::map_myfriendly.end())
	{
		for (int m = 0; m < 6; m++)
		{
			for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[npcid].relation.size(); i++)
			{
				if (GlobalInstance::map_myfriendly[npcid].relation[i] == NPC_FRIEND)
				{
					attrval[m] += GlobalInstance::map_npcrelation[npcid].friendratio[m];
				}
				else if (GlobalInstance::map_myfriendly[npcid].relation[i] == NPC_MASTER)
				{
					attrval[m] += GlobalInstance::map_npcrelation[npcid].masterratio[m];
				}
				else if (GlobalInstance::map_myfriendly[npcid].relation[i] == NPC_COUPEL)
				{
					attrval[m] += GlobalInstance::map_npcrelation[npcid].conpelratio[m];
				}
			}

			std::string str = StringUtils::format(valueformat[m].c_str(), attrval[m]*100);
			npcattrlbl[m]->setString(str);
		}
	}
}

void RenationLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (tag == 1000)
		{
			AnimationEffect::closeAniEffect(this);
		}
	}
}

void RenationLayer::onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
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

		Node* node = (Node*)pSender;
		int tag = node->getTag();
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		cocos2d::ui::ImageView* npcitem = (cocos2d::ui::ImageView*)pSender;
		if (lastClickNpcItem != npcitem)
		{
			if (lastClickNpcItem != NULL)
				((cocos2d::ui::ImageView*)lastClickNpcItem)->loadTexture(ResourcePath::makePath("ui/renationitem_n.png"), cocos2d::ui::Widget::TextureResType::PLIST);
			npcitem->loadTexture(ResourcePath::makePath("ui/renationitem_s.png"), cocos2d::ui::Widget::TextureResType::PLIST);
			lastClickNpcItem = npcitem;
			updateNpcAddAttr(vec_npc[tag]);
		}
	}
}