#include "AchieveLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "Quest.h"
#include "AnimationEffect.h"
#include "AchieveNode.h"

AchieveLayer::AchieveLayer()
{
	
}


AchieveLayer::~AchieveLayer()
{
	
}

bool AchieveLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("achieveLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//БъЬт
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("achieve_title", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(AchieveLayer::onBtnClick, this));

	scrollview = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("contentscroll");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	int itemheight = 140;
	int innerheight = itemheight * GlobalInstance::vec_achievedata.size();

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));


	std::vector<AchieveData*> vec_fin;
	std::vector<AchieveData*> vec_unfin;
	std::vector<AchieveData*> vec_get;

	for (unsigned int i = 0; i < GlobalInstance::vec_achievedata.size(); i++)
	{
		AchieveData* data = &GlobalInstance::vec_achievedata[i];
		if (data->state == DAILY_RECEIVE)
		{
			vec_get.push_back(data);
		}
		else if (data->state == DAILY_FINISHED)
		{
			vec_fin.push_back(data);
		}
		else
		{
			vec_unfin.push_back(data);
		}
	}
	std::vector<AchieveData*> vec_all;
	for (unsigned int i = 0; i < vec_fin.size(); i++)
	{
		vec_all.push_back(vec_fin[i]);

	}
	for (unsigned int i = 0; i < vec_unfin.size(); i++)
	{
		vec_all.push_back(vec_unfin[i]);
	}
	for (unsigned int i = 0; i < vec_get.size(); i++)
	{
		vec_all.push_back(vec_get[i]);
	}
	for (unsigned int i = 0; i < vec_all.size(); i++)
	{
		Node* node = AchieveNode::create(vec_all[i]);
		scrollview->addChild(node);
		node->setPosition(Vec2(scrollview->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
		node->runAction(EaseSineIn::create(MoveBy::create(0.10f + i*0.05f, Vec2(-scrollview->getContentSize().width / 2 - 600, 0))));
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void AchieveLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}