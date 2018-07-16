#include "TrainLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "ConsumeResActionLayer.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "Hero.h"
#include "MyHeroNode.h"

USING_NS_CC;

TrainLayer::TrainLayer()
{

}

TrainLayer::~TrainLayer()
{

}


TrainLayer* TrainLayer::create(Building* buidingData)
{
	TrainLayer *pRet = new(std::nothrow)TrainLayer();
	if (pRet && pRet->init(buidingData))
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
bool TrainLayer::init(Building* buidingData)
{
	if (!Layer::init())
	{
		return false;
	}

	m_buidingData = buidingData;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("trainLayer.csb"));
	this->addChild(csbnode);

	langtype = GlobalInstance::getInstance()->getLang();
	//����
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("train_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//�ȼ�
	lvUIlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	//������ť
	cocos2d::ui::Widget* lvUpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvUpbtn->setTag(1000);
	lvUpbtn->addTouchEventListener(CC_CALLBACK_2(TrainLayer::onBtnClick, this));

	//������ť����
	cocos2d::ui::ImageView* lvUpbtntxt = (cocos2d::ui::ImageView*)lvUpbtn->getChildByName("text");
	lvUpbtntxt->loadTexture(ResourcePath::makeTextImgPath("trainlv_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//�رհ�ť
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1001);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TrainLayer::onBtnClick, this));

	//�����ؼ�
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	loadData();
	updateContent();

	//�����²���
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void TrainLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://����
			if (m_buidingData->level.getValue() < m_buidingData->maxlevel.getValue() - 1)
			{
				ConsumeResActionLayer* layer = ConsumeResActionLayer::create(m_buidingData, CA_BUILDINGLVUP);
				this->addChild(layer);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["maxlv"]);
			}
			break;
		case 1001://�ر�
			this->removeFromParentAndCleanup(true);
			break;
		default:
			break;
		}
	}
}

void TrainLayer::loadData()
{
	
}

void TrainLayer::updateContent()
{
	m_contentscroll->removeAllChildrenWithCleanup(true);

	int itemheight = 160;

	int size = GlobalInstance::vec_myHeros.size();
	int innerheight = itemheight * size;
	int contentheight = m_contentscroll->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	m_contentscroll->setInnerContainerSize(Size(720, innerheight));

	std::vector<Hero*> vec_trainheros;
	for (int i = 0; i < size; i++)
	{
		Hero* hero = GlobalInstance::vec_myHeros[i];
		if (hero->getState()!=HS_DEAD)
		{
			vec_trainheros.push_back(hero);
		}
	}

	for (unsigned int i = 0; i < vec_trainheros.size(); i++)
	{
		Hero* hero = vec_trainheros[i];
		MyHeroNode* node = MyHeroNode::create(hero, HS_TRAINING);
		m_contentscroll->addChild(node);
		node->setPosition(Vec2(319, innerheight - i*itemheight - itemheight*0.5));
	}
}

void TrainLayer::lvup()
{
	std::string str = StringUtils::format("%d%s", m_buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	loadData();
	updateContent();
}