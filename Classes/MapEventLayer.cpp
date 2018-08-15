#include "MapEventLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"

USING_NS_CC;

MapEventLayer::MapEventLayer()
{

}

MapEventLayer::~MapEventLayer()
{
	
}


MapEventLayer* MapEventLayer::create(int eventindex)
{
	MapEventLayer *pRet = new(std::nothrow)MapEventLayer();
	if (pRet && pRet->init(eventindex))
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
bool MapEventLayer::init(int eventindex)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_eventindex = eventindex;

	LayerColor* color = LayerColor::create(Color4B(0, 0, 0, 255));
	this->addChild(color,0,"colorLayer");
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mapEventLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	std::string str = StringUtils::format("images/event%d.jpg", eventindex);
	eventimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("eventimg");
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	eventnode_1 = csbnode->getChildByName("eventnode_1");
	eventnode_2 = csbnode->getChildByName("eventnode_2");
	eventnode_3 = csbnode->getChildByName("eventnode_3");
	eventnode_1->setVisible(true);
	eventnode_2->setVisible(false);
	eventnode_3->setVisible(false);

	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_1->getChildByName("text");
	str = StringUtils::format("event%d_0", eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_1->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	closebtn->setTag(0);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	str = StringUtils::format("mapeventtext_%d_0", eventindex);
	text->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	text->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath(str, langtype))->getContentSize());

	cocos2d::ui::Button* actionbtn = (cocos2d::ui::Button*)eventnode_1->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	actionbtn->setTag(1);
	cocos2d::ui::ImageView* text2 = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	str = StringUtils::format("mapeventtext_%d_1", eventindex);
	text2->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	text2->setContentSize(Sprite::createWithSpriteFrameName(ResourcePath::makeTextImgPath(str, langtype))->getContentSize());

	loadPrData();

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	/*listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		this->removeFromParentAndCleanup(true);
	};*/
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}

void MapEventLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		switch (tag)
		{
		case 0:
			this->removeFromParentAndCleanup(true);
			break;
		case 1:
			if (m_eventindex == MAP_JUMP || m_eventindex == POS_CAVE)
			{
				/*int r = GlobalInstance::getInstance()->createRandomNum(10) + 1;
				if (r <= 1)
				{*/
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(true);
					eventnode_3->setVisible(false);
					boxEventNode();
				/*}
				else if (r <= 5)
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(false);
					eventnode_3->setVisible(true);
					eventNothing();
				}
				else
				{
					eventnode_1->setVisible(false);
					eventnode_2->setVisible(false);
					eventnode_3->setVisible(true);
					eventHurt();
				}*/
			}
			break;
		default:
			break;
		}
	}
}

void MapEventLayer::loadPrData()
{
	std::map<std::string, EventData>::iterator it;
	for (it = GlobalInstance::map_eventdata.begin(); it != GlobalInstance::map_eventdata.end(); it++)
	{
		vec_eventdata.push_back(it);
	}
	sort(vec_eventdata.begin(), vec_eventdata.end(), larger_callback);
}

bool MapEventLayer::larger_callback(EventData a, EventData b)
{
	int prA = a.pr;
	int prB = b.pr;
	if (prA < prB)
		return true;
	else
		return false;
}

int MapEventLayer::getAllPr()
{
	int allpr = 0;
	for (unsigned int i = 0; i < vec_eventdata.size(); i++)
	{
		pr += vec_eventdata[i].pr;
		allpr += pr;
	}
	/*std::map<std::string, EventData>::iterator it;
	for (it = GlobalInstance::map_eventdata.begin(); it != GlobalInstance::map_eventdata.end(); it++)
	{
		int pr = it->second.pr;
		allpr += pr;
	}*/
	return allpr;
}

std::string MapEventLayer::getDataIdByPr()
{
	int r = GlobalInstance::getInstance()->createRandomNum(getAllPr()) + 1;
	std::string id = "";
	int pr = 0;
	for (unsigned int i = 0; i < vec_eventdata.size(); i++)
	{
		pr += vec_eventdata[i].pr;
		if (pr >= r)
		{
			id = vec_eventdata[i].id;
			vec_eventdata.erase(vec_eventdata.begin() + i);
			return id;
		}
	}
	return id;
}

void MapEventLayer::boxEventNode()
{
	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_2->getChildByName("text");
	std::string str = StringUtils::format("event%d_1", m_eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Button* actionbtn = (cocos2d::ui::Button*)eventnode_2->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	actionbtn->setTag(2);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("allgetbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* resbox_0 = (cocos2d::ui::ImageView*)eventnode_2->getChildByName("resbox_0");
	cocos2d::ui::ImageView* resbox_1 = (cocos2d::ui::ImageView*)eventnode_2->getChildByName("resbox_1");
	cocos2d::ui::ImageView* resbox_2 = (cocos2d::ui::ImageView*)eventnode_2->getChildByName("resbox_2");

	int r = GlobalInstance::getInstance()->createRandomNum(3) + 1;
	std::string arr[r];
	for (int i = 0; i < r; i++)
	{
		arr[i] = getDataIdByPr();
	}
	if (r == 1)
	{
		resbox_0->setVisible(false);
		resbox_2->setVisible(false);
		cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)resbox_1->getChildByName("icon");
	}
	else if (r == 2)
	{
		resbox_1->setVisible(false);
		resbox_0->setPositionX(-134);
		resbox_2->setPositionX(118);
		cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)resbox_1->getChildByName("icon");
	}
	else
	{
		cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)resbox_1->getChildByName("icon");
	}
}

void MapEventLayer::eventNothing()
{
	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_3->getChildByName("text");
	std::string str = StringUtils::format("event%d_2", m_eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_3->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	closebtn->setTag(0);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("images/event%d-2.jpg", m_eventindex);
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);
}

void MapEventLayer::eventHurt()
{
	cocos2d::ui::Text* textdesc = (cocos2d::ui::Text*)eventnode_3->getChildByName("text");
	std::string str = StringUtils::format("event%d_3", m_eventindex);
	textdesc->setString(ResourceLang::map_lang[str]);
	textdesc->setVisible(false);

	Label* m_wordlbl = Label::createWithTTF(ResourceLang::map_lang[str], FONT_NAME, 25);
	m_wordlbl->setLineBreakWithoutSpace(true);
	m_wordlbl->setMaxLineWidth(600);
	m_wordlbl->setPosition(textdesc->getPosition());
	m_wordlbl->setLineSpacing(6.0f);
	m_wordlbl->setAlignment(TextHAlignment::CENTER);
	m_wordlbl->setVerticalAlignment(TextVAlignment::CENTER);
	eventnode_3->addChild(m_wordlbl);
	std::vector<std::string> xstr;
	CommonFuncs::split(ResourceLang::map_lang[str], xstr, ResourceLang::map_lang["juhao"]);
	std::string temp = ResourceLang::map_lang[str];
	std::string gfname = xstr[xstr.size() - 2];
	int findpos = temp.find(gfname);
	if (findpos != std::string::npos)
	{
		temp = temp.substr(findpos + gfname.length(), temp.length() - gfname.length());
		int sindex = (ResourceLang::map_lang[str].length() - temp.length() - gfname.length()) / 3;
		int len = gfname.length() / 3;
		for (int i = sindex; i < sindex + len; i++)
		{
			m_wordlbl->getLetter(i)->setColor(Color3B(255, 0, 0));
		}
	}

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)eventnode_3->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(MapEventLayer::onBtnClick, this));
	closebtn->setTag(0);
	cocos2d::ui::ImageView* text = (cocos2d::ui::ImageView*)closebtn->getChildByName("text");
	text->loadTexture(ResourcePath::makeTextImgPath("mapeventtext_lk", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	str = StringUtils::format("images/event%d-3.jpg", m_eventindex);
	eventimg->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::LOCAL);

	for (int i = 0; i < 6; i++)
	{
		if (GlobalInstance::myCardHeros[i] != NULL)
		{
			float hp = GlobalInstance::myCardHeros[i]->getHp();
			hp -= GlobalInstance::myCardHeros[i]->getMaxHp()*0.3;
			if (hp <= 0)
			{
				hp = 0;
				GlobalInstance::myCardHeros[i]->setState(HS_DEAD);
				GlobalInstance::myCardHeros[i]->setPos(0);
			}
			GlobalInstance::myCardHeros[i]->setHp(hp);
		}
	}
}

void MapEventLayer::onExit()
{
	Layer::onExit();
}