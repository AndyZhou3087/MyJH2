#include "BuyResLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MyRes.h"

USING_NS_CC;

BuyResLayer::BuyResLayer()
{

}

BuyResLayer::~BuyResLayer()
{

}


BuyResLayer* BuyResLayer::create(std::vector<MSGAWDSDATA> vec_rewards)
{
	BuyResLayer *pRet = new(std::nothrow)BuyResLayer();
	if (pRet && pRet->init(vec_rewards))
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
bool BuyResLayer::init(std::vector<MSGAWDSDATA> vec_res)
{
	if (!Layer::init())
	{
		return false;
	}

	m_vecres = vec_res;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 128));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("buyResLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["reslack"]);

	//��ť1
	cocos2d::ui::Widget* okbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(BuyResLayer::onBtnClick, this));
	okbtn->setTag(1);
	//��ť1����
	cocos2d::ui::ImageView* okbtntxt = (cocos2d::ui::ImageView*)okbtn->getChildByName("text");
	okbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//��ť2
	cocos2d::ui::Widget* cancelbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("cancelbtn");
	cancelbtn->addTouchEventListener(CC_CALLBACK_2(BuyResLayer::onBtnClick, this));
	cancelbtn->setTag(0);
	//��ť2����
	cocos2d::ui::ImageView* cancelbtntxt = (cocos2d::ui::ImageView*)cancelbtn->getChildByName("text");
	cancelbtntxt->loadTexture(ResourcePath::makeTextImgPath("cancelbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	int startx[] = { 360, 270 ,210 };
	int offsetx[] = { 0, 180, 150 };
	int rewardsize = vec_res.size();

	for (int i = 0; i < 3; i++)
	{
		std::string str = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");

		cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");
		cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)resbox->getChildByName("name");

		if (i < rewardsize)
		{
			std::string boxstr = "ui/resbox.png";
			int t = 0;
			int qu = vec_res[i].qu;
			std::string resid = vec_res[i].rid;
			int count = vec_res[i].count;
			for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
			{
				if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
					break;
			}
			if (t >= T_ARMOR && t <= T_NG)
			{
				boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (t >= T_WG && t <= T_NG)
			{
				qu = GlobalInstance::map_GF[resid].qu;
				boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (t == T_RENS || t == T_DAN || t == T_MIJI || t == T_BOX)
			{
				qu = atoi(resid.substr(1).c_str()) - 1;
				boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}

			CommonFuncs::playResBoxEffect(resbox, qu);

			resbox->setPositionX(startx[rewardsize - 1] + offsetx[rewardsize - 1] * i);
			resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
			std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

			namelbl->setString(GlobalInstance::map_AllResources[resid].name);
			std::string countstr = StringUtils::format("%d/%d", MyRes::getMyResCount(resid, MYSTORAGE), count);
			countlbl->setString(countstr);
			needcoincount.setValue(needcoincount.getValue() + (count - MyRes::getMyResCount(resid, MYSTORAGE))*GlobalInstance::map_AllResources[resid].silverval / 10);
		}
		else
		{
			resbox->setVisible(false);
		}
	}

	cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)csbnode->getChildByName("cnumbl");
	std::string str = StringUtils::format("%d", needcoincount.getValue());
	countlbl->setString(str);


	//�����²���
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect(this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void BuyResLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		switch (node->getTag())
		{
		case 0:
			AnimationEffect::closeAniEffect(this);
			break;
		case 1:
		{
			if (m_vecres.size() == 1 && m_vecres[0].rid.compare("p001") == 0)//����ҩˮ
			{
				for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
				{
					Hero* hero = GlobalInstance::vec_myHeros[i];
					if (hero->getPower().getValue() > 50)
					{
						DynamicValueInt dal;
						dal.setValue(100);
						hero->setPower(dal);
					}
					else
					{
						DynamicValueInt dal;
						dal.setValue(hero->getPower().getValue() + 50);
						hero->setPower(dal);
					}
				}
			}
			AnimationEffect::closeAniEffect(this);
		}
			break;
		default:
			break;
		}
	}
}