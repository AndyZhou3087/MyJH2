#include "MainMenuLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MaterialDescLayer.h"
#include "TaskTalkLayer.h"
#include "TaskBranchTalkLayer.h"
#include "Quest.h"
#include "MessageLayer.h"
#include "MainScene.h"
#include "AnimationEffect.h"
#include "SettingLayer.h"
#include "SoundManager.h"

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
	updateUI(0);
	this->schedule(schedule_selector(MainMenuLayer::updateUI), 1.0f);
    return true;
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
			break;
		case R001BTN:
		case R002BTN:
		case R003BTN:
		case R004BTN:
		case R005BTN:
		{
			Layer * malayer = MaterialDescLayer::create(clicknode->getName());
			this->addChild(malayer, 0, "MaterialDescLayer");
			AnimationEffect::openAniEffect((Layer*)malayer);
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

void MainMenuLayer::onExit()
{
	Layer::onExit();
}
