#include "LibraryLayer2.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "GfLibraryLayer.h"
#include "EquipLibraryLayer.h"
#include "SoundManager.h"

USING_NS_CC;

LibraryLayer2::LibraryLayer2()
{

}

LibraryLayer2::~LibraryLayer2()
{

}


LibraryLayer2* LibraryLayer2::create()
{
	LibraryLayer2 *pRet = new(std::nothrow)LibraryLayer2();
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
bool LibraryLayer2::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("libraryLayer2.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* backClick = (cocos2d::ui::Widget*)csbnode->getChildByName("backclick");
	backClick->addTouchEventListener(CC_CALLBACK_2(LibraryLayer2::onBackClick, this));

	cocos2d::ui::ScrollView* ScrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("ScrollView");
	ScrollView->setScrollBarEnabled(false);
	for (int i = 0; i <= 3; i++)
	{
		std::string name = StringUtils::format("click_%d_n", i);
		cocos2d::ui::Widget* click_n = (cocos2d::ui::Widget*)ScrollView->getChildByName(name);
		click_n->addTouchEventListener(CC_CALLBACK_2(LibraryLayer2::onBtnClick, this));

		name = StringUtils::format("click_%d_s", i);
		cocos2d::ui::Widget* click_s = (cocos2d::ui::Widget*)ScrollView->getChildByName(name);
		click_s->setVisible(false);
		click_n->setTag(i);
		click_n->setUserData((void*)click_s);
		
	}

	cocos2d::ui::ImageView* ngimgtext = (cocos2d::ui::ImageView*)ScrollView->getChildByName("libraryngtextbg")->getChildByName("text");
	ngimgtext->loadTexture(ResourcePath::makeTextImgPath("libraryng_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* wgimgtext = (cocos2d::ui::ImageView*)ScrollView->getChildByName("librarywgtextbg")->getChildByName("text");
	wgimgtext->loadTexture(ResourcePath::makeTextImgPath("librarywg_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* equipimgtext = (cocos2d::ui::ImageView*)ScrollView->getChildByName("libraryequiptextbg")->getChildByName("text");
	equipimgtext->loadTexture(ResourcePath::makeTextImgPath("libraryequip_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* armimgtext = (cocos2d::ui::ImageView*)ScrollView->getChildByName("libraryarmtextbg")->getChildByName("text");
	armimgtext->loadTexture(ResourcePath::makeTextImgPath("libraryarm_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		m_isDraging = false;
		m_startClickX = touch->getLocation().x;
		m_startClickY = touch->getLocation().y;
		return true;
	};

	listener->onTouchMoved = [=](Touch *touch, Event *event)
	{
		if (fabsf(m_startClickX - touch->getLocation().x) > CLICKOFFSETP || fabsf(m_startClickY - touch->getLocation().y) > CLICKOFFSETP)
		{
			m_isDraging = true;
		}
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}


void LibraryLayer2::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::ImageView* clicknode = (cocos2d::ui::ImageView*)pSender;
	Node* snode = (Node*)clicknode->getUserData();
	int tag = clicknode->getTag();
	switch (type)
	{
		case cocos2d::ui::Widget::TouchEventType::BEGAN:
		{
			snode->setVisible(true);
		}
		break;
		case cocos2d::ui::Widget::TouchEventType::MOVED:
			break;
		case ui::Widget::TouchEventType::ENDED:
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
			Node* clicknode = (Node*)pSender;
			int tag = clicknode->getTag();
			switch (tag)
			{
			case 0://武功
			{
				GfLibraryLayer* layer = GfLibraryLayer::create(T_WG);
				this->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
			break;
			case 1://内功
			{
				GfLibraryLayer* layer = GfLibraryLayer::create(T_NG);
				this->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
			break;
			case 2://神兵
			{
				EquipLibraryLayer* layer = EquipLibraryLayer::create(1);
				this->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
			break;
			case 3://宝甲
			{
				EquipLibraryLayer* layer = EquipLibraryLayer::create(0);
				this->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
			break;
			case 1000://关闭
				AnimationEffect::closeAniEffect(this);
				break;
			default:
				break;
			}
		}
		case cocos2d::ui::Widget::TouchEventType::CANCELED:
		{
			snode->setVisible(false);

			break;
		}
	}
}

void LibraryLayer2::onBackClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect(this);
	}
}

void LibraryLayer2::onExit()
{
	Layer::onExit();
}
