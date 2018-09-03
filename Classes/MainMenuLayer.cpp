#include "MainMenuLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MaterialDescLayer.h"
#include "TaskTalkLayer.h"
#include "Quest.h"
#include "MessageLayer.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "SettingLayer.h"
#include "SoundManager.h"
#include "AchieveLayer.h"
#include "DataSave.h"
#include "HeadInfoLayer.h"
#include "NewPopLayer.h"
#include "ShopLayer.h"
#include "GiftContentLayer.h"

USING_NS_CC;

MainMenuLayer::MainMenuLayer()
{

}

MainMenuLayer::~MainMenuLayer()
{
	
}


// on "init" you need to initialize your instance
bool MainMenuLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("MainMenuLayer.csb"));
	this->addChild(csbnode);

	for (int i = 0; i < csbnode->getChildrenCount(); i++)
	{
		std::string name = csbnode->getChildren().at(i)->getName();
		cocos2d::ui::Widget* clickwidget = (cocos2d::ui::Widget*)csbnode->getChildByName(name);
		clickwidget->setTag(i);
		clickwidget->addTouchEventListener(CC_CALLBACK_2(MainMenuLayer::onClick, this));

		if (i >= R001BTN && i <= R005BTN)
		{
			reslbl[i - R001BTN] = (cocos2d::ui::Text*)clickwidget->getChildByName("countlbl");
		}
		else if (i == ACHBTN)
		{
			achredpoint = (cocos2d::ui::Widget*)clickwidget->getChildByName("redpoint");
			achredpoint->setVisible(false);
		}
		else if (i == MAILBTN)
		{
			mailredpoint = (cocos2d::ui::Widget*)clickwidget->getChildByName("redpoint");
			mailredpoint->setVisible(false);
		}
		else if (i == SILVERBOX)
		{
			mysilverlbl = (cocos2d::ui::Text*)clickwidget->getChildByName("countlbl");
		}
		else if (i == COINBOX)
		{
			mycoinlbl = (cocos2d::ui::Text*)clickwidget->getChildByName("countlbl");
		}

		if (i >= SETBTN && i <= SHOPBTN)
		{
			cocos2d::ui::ImageView* textimg =  (cocos2d::ui::ImageView*)clickwidget->getChildByName("text");
			std::string textname = StringUtils::format("main_%s_text", name.c_str());
			textimg->loadTexture(ResourcePath::makeTextImgPath(textname, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}

	cocos2d::ui::Widget* headimgbox = (cocos2d::ui::Widget*)csbnode->getChildByName("headimgbox");
	head = (cocos2d::ui::ImageView*)headimgbox->getChildByName("headimg");
	std::string str = StringUtils::format("ui/h_%d_0.png", DataSave::getInstance()->getHeadId());
	head->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	head->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());

	//ClippingNode* m_clippingNode = ClippingNode::create();
	//m_clippingNode->setInverted(false);//设置底板可见
	//m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
	//csbnode->addChild(m_clippingNode, 1);
	//m_clippingNode->setAnchorPoint(Vec2(0.5, 1));
	//m_clippingNode->setPosition(Vec2(headimgbox->getPositionX(), headimgbox->getPositionY() + 80));
	//std::string str = StringUtils::format("images/cardh_%d_0.png", DataSave::getInstance()->getHeadId());
	//head = cocos2d::ui::ImageView::create(str, cocos2d::ui::Widget::TextureResType::LOCAL);
	//head->setAnchorPoint(Vec2(0.5, 1));
	//head->setPositionY(0);
	//m_clippingNode->addChild(head);
	//Node* stencil = Node::create();
	//Sprite* cnode = Sprite::createWithSpriteFrameName("ui/headclip.png");
	//cnode->setAnchorPoint(Vec2(0.5, 1));
	//stencil->addChild(cnode);
	//m_clippingNode->setStencil(stencil);

	updateUI(0);
	this->schedule(schedule_selector(MainMenuLayer::updateUI), 1.0f);

	HttpDataSwap::init(this)->vipIsOn();
	HttpDataSwap::init(this)->getMessageList(0);

    return true;
}

void MainMenuLayer::onFinish(int code)
{
	if (code == SUCCESS)
	{
		if (GlobalInstance::map_buyVipDays.size()>0)
		{
			std::map<std::string, int>::iterator it;
			for (it = GlobalInstance::map_buyVipDays.begin(); it != GlobalInstance::map_buyVipDays.end(); ++it)
			{
				for (unsigned int i = 0; i < GlobalInstance::vec_shopdata.size(); i++)
				{
					if (GlobalInstance::vec_shopdata[i].icon.compare(it->first) == 0)
					{
						GiftContentLayer* layer = GiftContentLayer::create(&GlobalInstance::vec_shopdata[i], i);
						this->addChild(layer);
						AnimationEffect::openAniEffect((Layer*)layer);
						break;
					}
				}
			}
		}

		if (GlobalInstance::vec_notice.size() <= 0)
		{
			return;
		}
		if (GlobalInstance::noticeID.compare(GlobalInstance::vec_notice[0].id) != 0)
		{
			NewPopLayer* unlock = NewPopLayer::create();
			this->addChild(unlock);
			AnimationEffect::openAniEffect((Layer*)unlock);
		}
	}
}

void MainMenuLayer::updateUI(float dt)
{
	std::string str;
	for (int i = 0; i < 5; i++)
	{
		std::string rid = StringUtils::format("r%03d", i + 1);

		str = StringUtils::format("%d", MyRes::getMyResCount(rid));
		reslbl[i]->setString(str);

	}
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	mycoinlbl->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	mysilverlbl->setString(str);
	for (unsigned int i = 0; i < GlobalInstance::vec_messsages.size(); i++)
	{
		int type = GlobalInstance::vec_messsages[i].type;
		int s = GlobalInstance::vec_messsages[i].status;
		if ((type == 0 && s == 0) || (type != 0 && s < 2))
		{
			mailredpoint->setVisible(true);
			break;
		}
	}
}

void MainMenuLayer::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int menuType = clicknode->getTag();
	if (menuType >= SETBTN && menuType <= SHOPBTN)
		CommonFuncs::BtnAction(pSender, type);
	else if (type == ui::Widget::TouchEventType::ENDED)
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		switch (menuType)
		{
		case HEADBOX:
		{
			HeadInfoLayer* layer = HeadInfoLayer::create();
			this->addChild(layer, 0, "HeadInfoLayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case R001BTN:
		case R002BTN:
		case R003BTN:
		case R004BTN:
		case R005BTN:
		{
			Layer * malayer = MaterialDescLayer::create(clicknode->getName());
			this->addChild(malayer, 0, "MaterialDescLayer");
		}
			break;
		case SETBTN:
		{
			SettingLayer* layer = SettingLayer::create();
			g_mainScene->addChild(layer, 0, "settinglayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case ACHBTN:
		{
			AchieveLayer* layer = AchieveLayer::create();
			g_mainScene->addChild(layer, 0, "achieveLayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case RANKBTN:
			break;
		case MAILBTN:
		{
			MessageLayer* layer = MessageLayer::create();
			g_mainScene->addChild(layer, 0, "messagelayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case SHOPBTN:
		{
			ShopLayer* layer = ShopLayer::create();
			g_mainScene->addChild(layer, 0, "ShopLayer");
			AnimationEffect::openAniEffect((Layer*)layer);
		}
			break;
		case SILVERBOX:
		case ADDSILVERBTN:
			break;
		case COINBOX:
		case ADDCOINBTN:
			break;
		default:
			break;
		}
	}
}

void MainMenuLayer::changeHead()
{
	std::string str = StringUtils::format("ui/h_%d_0.png", DataSave::getInstance()->getHeadId());
	head->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
	head->setContentSize(Sprite::createWithSpriteFrameName(str)->getContentSize());
}

void MainMenuLayer::onExit()
{
	Layer::onExit();
}
