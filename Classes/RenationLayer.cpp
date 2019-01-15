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

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("renationLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* title = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	title->loadTexture(ResourcePath::makeTextImgPath("renatitontilte", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* tname = (cocos2d::ui::Text*)csbnode->getChildByName("tname");
	tname->setString(ResourceLang::map_lang["renationtname"]);

	npcname = (cocos2d::ui::Text*)csbnode->getChildByName("npcname");

	cocos2d::ui::Text* hintdesc = (cocos2d::ui::Text*)csbnode->getChildByName("hintdesc");
	hintdesc->setString(ResourceLang::map_lang["renationhintdesc"]);

	//°´Å¥1
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(RenationLayer::onBtnClick, this));
	closebtn->setTag(1000);

	createPage();

	cocos2d::ui::Text* totalattrtitle = (cocos2d::ui::Text*)csbnode->getChildByName("totalattrtitle");
	totalattrtitle->setString(ResourceLang::map_lang["totalattrtitle"]);
	
	float totalattrval[] = { 0,0,0,0,0,0 };
	std::map<std::string, NpcFriendly>::iterator it;

	for (int i = 0; i < 6; i++)
	{
		for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); ++it)
		{
			std::string nid = it->first;
			for (unsigned int i = 0; i < it->second.relation.size(); i++)
			{
				if (it->second.relation[i] == NPC_FRIEND)
				{
					totalattrval[i] += GlobalInstance::map_npcrelation[nid].friendratio[i];
				}
				else if (it->second.relation[i] == NPC_MASTER)
				{
					totalattrval[i] += GlobalInstance::map_npcrelation[nid].masterratio[i];
				}
				else if (it->second.relation[i] == NPC_COUPEL)
				{
					totalattrval[i] += GlobalInstance::map_npcrelation[nid].conpelratio[i];
				}
			}
		}
	}

	int tmp[] = {0,1,2,5,4,3};
	for (int i = 0; i < 6; i++)
	{
		std::string str = StringUtils::format("attrtext_%d", i); 
		cocos2d::ui::Text* totalattr = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		std::string strkey = StringUtils::format("addattrtext_%d", tmp[i]);
		str = StringUtils::format(ResourceLang::map_lang[strkey].c_str(), totalattrval[i]);
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
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void RenationLayer::createPage()
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
	int itemheight = 39;
	int perpageitem = 12;
	pageView = cocos2d::ui::PageView::create();
	pageView->setContentSize(Size(270, itemheight*perpageitem));
	pageView->setPosition(Vec2(90, 350));

	int page = size % perpageitem == 0 ? size / perpageitem : size / perpageitem + 1;

	int vecindex = 0;
	for (int m = 0; m < page; m++)
	{
		auto layout = cocos2d::ui::Layout::create();
		layout->setContentSize(Size(pageView->getContentSize().width, itemheight * perpageitem));

		for (int i = 0; i < perpageitem; i++)
		{
			if (vecindex < size)
			{
				Node* itemnode = CSLoader::createNode(ResourcePath::makePath("renationitemNode.csb"));
				itemnode->setPosition(Vec2(layout->getContentSize().width / 2, layout->getContentSize().height - itemheight / 2 - itemheight * i));
				cocos2d::ui::Text* name = (cocos2d::ui::Text*)itemnode->getChildByName("name");
				name->setString(GlobalInstance::map_AllResources[vec_npc[vecindex]].name);
				cocos2d::ui::Text* addr = (cocos2d::ui::Text*)itemnode->getChildByName("addr");
				addr->setString(GlobalInstance::map_AllResources[vec_npc[vecindex]].desc);
				cocos2d::ui::Text* renation = (cocos2d::ui::Text*)itemnode->getChildByName("renation");

				cocos2d::ui::Widget* renationitem = (cocos2d::ui::Widget*)itemnode->getChildByName("renationitem");
				renationitem->addTouchEventListener(CC_CALLBACK_2(RenationLayer::onItemClick, this));
				renationitem->setTag(vecindex);
				renationitem->setSwallowTouches(false);
				
				int ralation = NPC_NORMOL;
				if (GlobalInstance::map_myfriendly.find(vec_npc[vecindex]) != GlobalInstance::map_myfriendly.end())
				{
					for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[vec_npc[vecindex]].relation.size(); i++)
					{
						ralation = GlobalInstance::map_myfriendly[vec_npc[vecindex]].relation[i];
					}
				}

				if (ralation == NPC_FRIEND)
					renation->setTextColor(Color4B(27, 140, 1, 255));
				else if (ralation == NPC_MASTER)
					renation->setTextColor(Color4B(5, 105, 207, 255));
				else if (ralation == NPC_COUPEL)
					renation->setTextColor(Color4B(232,23,23, 255));
				std::string renationname = StringUtils::format("npcrelation_%d", ralation);
				renation->setString(ResourceLang::map_lang[renationname]);
				layout->addChild(itemnode);
			}
			vecindex++;
		}
		pageView->addPage(layout);
	}

	this->addChild(pageView);
}

void RenationLayer::updateNpcAddAttr(std::string npcid)
{
	npcname->setString(GlobalInstance::map_AllResources[npcid].name);

	std::string valueformat[] = {"+%.0f", "+%.0f", "+%.0f", "+%.3f%%", "+%.3f%%", "+%.3f"};
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

			std::string str = StringUtils::format(valueformat[m].c_str(), attrval[m]);
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
				((cocos2d::ui::ImageView*)lastClickNpcItem)->loadTexture(ResourcePath::makePath("ui/renationitem.png"), cocos2d::ui::Widget::TextureResType::PLIST);
			npcitem->loadTexture(ResourcePath::makePath("ui/renationitem1.png"), cocos2d::ui::Widget::TextureResType::PLIST);
			lastClickNpcItem = npcitem;
			updateNpcAddAttr(vec_npc[tag]);
		}
	}
}