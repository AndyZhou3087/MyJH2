#include "FirstChargeLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "ShopLayer.h"
#include "ResDescLayer.h"
#include "EquipDescLayer.h"

USING_NS_CC;

FirstChargeLayer::FirstChargeLayer()
{
	clickres = NULL;
}

FirstChargeLayer::~FirstChargeLayer()
{
	if (clickres != NULL)
	{
		delete clickres;
		clickres = NULL;
	}
}


FirstChargeLayer* FirstChargeLayer::create(ShopData* data)
{
	FirstChargeLayer *pRet = new(std::nothrow)FirstChargeLayer();
	if (pRet && pRet->init(data))
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
bool FirstChargeLayer::init(ShopData* data)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("firstchargeLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("firstchargetitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮1
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(FirstChargeLayer::onBtnClick, this));
	actionbtn->setTag(1000);

	//按钮1文字
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("firstchargebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(FirstChargeLayer::onBtnClick, this));
	closebtn->setTag(1001);

	cocos2d::ui::ImageView* descimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("desc");
	descimg->loadTexture(ResourcePath::makeTextImgPath("firstchargedesc", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	for (unsigned int i = 0; i < data->res.size(); i++)
	{
		MSGAWDSDATA msgdata;
		std::vector<std::string> vec_res = data->res[i];
		msgdata.rid = vec_res[0];
		msgdata.count = atoi(vec_res[1].c_str());
		if (vec_res.size() >= 3)
			msgdata.qu = atoi(vec_res[2].c_str());
		else
			msgdata.qu = 0;
		vec_rewards.push_back(msgdata);
	}


	int rewardsize = vec_rewards.size();

	for (int i = 0; i < rewardsize; i++)
	{
		std::string str = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
		resbox->addTouchEventListener(CC_CALLBACK_2(FirstChargeLayer::onBtnClick, this));
		resbox->setTag(i);
		resbox->setUserData((void*)&vec_rewards[i]);

		cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");

		cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)resbox->getChildByName("count");
		cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)resbox->getChildByName("name");

		std::string boxstr = "ui/resbox.png";
		int t = 0;
		int qu = vec_rewards[i].qu;
		std::string resid = vec_rewards[i].rid;
		int count = vec_rewards[i].count;
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (vec_rewards[i].rid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
				break;
		}
		if (t >= T_ARMOR && t <= T_FASHION)
		{
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t >= T_WG && t <= T_NG)
		{
			qu = GlobalInstance::map_GF[resid].qu;
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (t >= T_RENS && t <= T_BOX)
		{
			qu = atoi(resid.substr(1).c_str()) - 1;
			boxstr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}

		resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
		std::string resstr = StringUtils::format("ui/%s.png", vec_rewards[i].rid.c_str());
		res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

		namelbl->setString(GlobalInstance::map_AllResources[vec_rewards[i].rid].name);
		std::string countstr = StringUtils::format("%d", vec_rewards[i].count);
		countlbl->setString(countstr);

	}

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

void FirstChargeLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* node = (Node*)pSender;
	int tag = node->getTag();
	if (tag >= 1000)
		CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (tag >= 1000)
		{
			if (tag == 1001)
				removeSelf();
			else
				ShopLayer::beginPay(this->getTag());
		}
		else
		{
			MSGAWDSDATA* data = (MSGAWDSDATA*)node->getUserData();
			if (clickres != NULL)
			{
				delete clickres;
				clickres = NULL;
			}
			std::string resid = data->rid;

			int t = -1;
			for (int k = 0; k < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); k++)
			{
				if (resid.compare(0, 1, RES_TYPES_CHAR[k]) == 0)
				{
					t = k;
					break;
				}
			}

			if (t >= T_ARMOR && t <= T_FASHION)
			{
				clickres = new Equip();
			}
			else if (t >= T_WG && t <= T_NG)
			{
				clickres = new GongFa();
			}
			else
			{
				clickres = new ResBase();
			}

			clickres->setId(resid);
			clickres->setType(t);

			DynamicValueInt dvcount;
			dvcount.setValue(data->count);
			clickres->setCount(dvcount);

			DynamicValueInt dv;
			dv.setValue(data->qu);
			clickres->setQU(dv);

			Layer* layer = NULL;
			if (t >= T_ARMOR && t <= T_NG)
			{
				layer = EquipDescLayer::create(clickres, 1);
			}
			else
			{
				layer = ResDescLayer::create(clickres, 2);
			}
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
	}
}

void FirstChargeLayer::removeSelf()
{
	AnimationEffect::closeAniEffect((Layer*)this);
}