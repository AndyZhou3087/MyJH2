#include "TaskLayer.h"
#include <algorithm>
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "TaskMainNode.h"

TaskLayer::TaskLayer()
{
	lastCategoryindex = 0;
}


TaskLayer::~TaskLayer()
{
	GlobalInstance::getInstance()->saveMyTaskMainData();
}

bool TaskLayer::init()
{

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	m_csbnode = CSLoader::createNode(ResourcePath::makePath("taskLayer.csb"));
	this->addChild(m_csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	langtype = GlobalInstance::getInstance()->getLang();

	//БъЬт
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)m_csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("halltitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)m_csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(TaskLayer::onBtnClick, this));

	scrollview = (cocos2d::ui::ScrollView*)m_csbnode->getChildByName("scrollview");
	scrollview->setScrollBarEnabled(false);
	scrollview->setBounceEnabled(true);

	Node* categoryBtnNode = m_csbnode->getChildByName("catanode");
	for (int i = 0; i < categoryBtnNode->getChildrenCount(); i++)
	{
		std::string btnstr = StringUtils::format("btn%d", i);
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)categoryBtnNode->getChildByName(btnstr);
		btn->setTag(i);
		btn->addTouchEventListener(CC_CALLBACK_2(TaskLayer::onCategory, this));
		vec_categoryBtn.push_back(btn);
	}
	updateContent(0);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void TaskLayer::updateContent(int category)
{
	scrollview->removeAllChildrenWithCleanup(true);

	for (unsigned int i = 0; i < vec_categoryBtn.size(); i++)
	{
		cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)vec_categoryBtn[i]->getChildByName("text");

		if (category != i)
		{
			std::string textstr = StringUtils::format("task_text_%d_n", i);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			vec_categoryBtn[i]->setBright(true);
		}
		else
		{
			std::string textstr = StringUtils::format("task_text_%d_s", i);
			vec_categoryBtn[i]->setBright(false);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}

	loadData(category);

	int ressize;
	if (category == 0)
	{
		ressize = GlobalInstance::vec_TaskMain.size();
		sort(GlobalInstance::vec_TaskMain.begin(), GlobalInstance::vec_TaskMain.end(), larger_callback);
	}
	else if (category == 1)
	{
		ressize = 20;
	}
	else
	{
		ressize = 10;
	}

	int itemheight = 140;
	int innerheight = itemheight * ressize;

	int contentheight = scrollview->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollview->setInnerContainerSize(Size(scrollview->getContentSize().width, innerheight));

	for (int i = 0; i < ressize; i++)
	{
		Node* node;
		if (category == 0)
		{
			node = TaskMainNode::create(&GlobalInstance::vec_TaskMain[i],this);
		}
		else if (category == 1)
		{
			
		}
		else
		{
			
		}
		scrollview->addChild(node);
		node->setPosition(Vec2(scrollview->getContentSize().width/2, innerheight - i*itemheight - itemheight*0.5));
	}
	
}

void TaskLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

bool TaskLayer::larger_callback(TaskMainData a, TaskMainData b)
{
	int needcountA = a.isfinish;
	int needcountB = b.isfinish;
	if (needcountA < needcountB)
		return true;
	else
		return false;
}

void TaskLayer::loadData(int category)
{

}


void TaskLayer::onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (lastCategoryindex == node->getTag())
			return;

		lastCategoryindex = node->getTag();

		updateContent(node->getTag());
	}
}