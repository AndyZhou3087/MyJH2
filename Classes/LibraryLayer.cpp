#include "LibraryLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "GfLibraryLayer.h"
#include "EquipLibraryLayer.h"

USING_NS_CC;

LibraryLayer::LibraryLayer()
{

}

LibraryLayer::~LibraryLayer()
{

}


LibraryLayer* LibraryLayer::create()
{
	LibraryLayer *pRet = new(std::nothrow)LibraryLayer();
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
bool LibraryLayer::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("libraryLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("librarytitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//返回按钮
	cocos2d::ui::Widget* backbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("backbtn");
	backbtn->setTag(1000);
	backbtn->addTouchEventListener(CC_CALLBACK_2(LibraryLayer::onBtnClick, this));

	//返回按钮文字
	cocos2d::ui::ImageView* backbtntxt = (cocos2d::ui::ImageView*)backbtn->getChildByName("text");
	backbtntxt->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//神兵宝甲按钮
	cocos2d::ui::Widget* equipimg = (cocos2d::ui::Widget*)csbnode->getChildByName("equipitem");
	equipimg->setTag(0);
	equipimg->addTouchEventListener(CC_CALLBACK_2(LibraryLayer::onBtnClick, this));

	cocos2d::ui::ImageView* equipimgtext = (cocos2d::ui::ImageView*)equipimg->getChildByName("text");
	equipimgtext->loadTexture(ResourcePath::makeTextImgPath("libraryequiptext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//武功按钮
	cocos2d::ui::Widget* wgimg = (cocos2d::ui::Widget*)csbnode->getChildByName("wgitem");
	wgimg->setTag(1);
	wgimg->addTouchEventListener(CC_CALLBACK_2(LibraryLayer::onBtnClick, this));

	cocos2d::ui::ImageView* wgimgtext = (cocos2d::ui::ImageView*)wgimg->getChildByName("text");
	wgimgtext->loadTexture(ResourcePath::makeTextImgPath("librarywgtext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//内功按钮
	cocos2d::ui::Widget* ngimg = (cocos2d::ui::Widget*)csbnode->getChildByName("ngitem");
	ngimg->setTag(2);
	ngimg->addTouchEventListener(CC_CALLBACK_2(LibraryLayer::onBtnClick, this));

	cocos2d::ui::ImageView* ngimgtext = (cocos2d::ui::ImageView*)ngimg->getChildByName("text");
	ngimgtext->loadTexture(ResourcePath::makeTextImgPath("libraryngtext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    return true;
}


void LibraryLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 0://神兵宝甲
		{
			EquipLibraryLayer* layer = EquipLibraryLayer::create();
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
		break;
		case 1://武功
		{
			GfLibraryLayer* layer = GfLibraryLayer::create(T_WG);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
			break;
		case 2://内功
		{
			GfLibraryLayer* layer = GfLibraryLayer::create(T_NG);
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
}

void LibraryLayer::onExit()
{
	Layer::onExit();
}
