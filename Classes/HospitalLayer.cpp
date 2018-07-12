#include "HospitalLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "MyMenu.h"
#include "GlobalInstance.h"
#include "Resource.h"
#include "MyHeroNode.h"

HospitalLayer::HospitalLayer()
{

}


HospitalLayer::~HospitalLayer()
{

}

bool HospitalLayer::init()
{

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("hospitalLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	int langtype = GlobalInstance::getInstance()->getLang();

	//БъЬт
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("hospital_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(HospitalLayer::onBtnClick, this));

	scrollview = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	cointext = (cocos2d::ui::Text*)m_csbnode->getChildByName("cointext");
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	cointext->setString(str);

	silvertext = (cocos2d::ui::Text*)m_csbnode->getChildByName("silvertext");
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	silvertext->setString(str);

	updateContent();

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void HospitalLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

void HospitalLayer::updateContent()
{
	std::string str = StringUtils::format("%d", GlobalInstance::getInstance()->getMyCoinCount().getValue());
	cointext->setString(str);
	str = StringUtils::format("%d", GlobalInstance::getInstance()->getMySoliverCount().getValue());
	silvertext->setString(str);

	scrollview->removeAllChildrenWithCleanup(true);
	vec_deadheros.clear();
	for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
	{
		Hero* hero = GlobalInstance::vec_myHeros[i];
		if (hero->getState() == HS_DEAD)
		{
			vec_deadheros.push_back(GlobalInstance::vec_myHeros[i]);
		}
	}

	int size = vec_deadheros.size();
	int itemheight = 160;
	int innerheight = itemheight * size;
	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (int i = 0; i < size; i++)
	{
		Hero* hero = vec_deadheros[i];
		MyHeroNode* node = MyHeroNode::create(hero, HS_DEAD);
		scrollview->addChild(node);
		node->setPosition(Vec2(319, innerheight - i*itemheight - itemheight*0.5));
	}
}