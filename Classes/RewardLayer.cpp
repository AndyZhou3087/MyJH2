#include "RewardLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "MyRes.h"
#include "MainScene.h"
#include "TaskLayer.h"
#include "MapBlockScene.h"

USING_NS_CC;

RewardLayer::RewardLayer()
{

}

RewardLayer::~RewardLayer()
{
	if (g_MapBlockScene != NULL)
		g_MapBlockScene->isRoutingBreakOff = false;
}


RewardLayer* RewardLayer::create(std::vector<MSGAWDSDATA> vec_rewards, int forwhere, int type)
{
	RewardLayer *pRet = new(std::nothrow)RewardLayer();
	if (pRet && pRet->init(vec_rewards, forwhere, type))
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
bool RewardLayer::init(std::vector<MSGAWDSDATA> vec_rewards, int forwhere, int type)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	ParticleSystemQuad *particle = ParticleSystemQuad::create(ResourcePath::makePath("effect/slsb/zhandoushengli/sl_xuanzhanguang.plist"));
	particle->setPosition(Vec2(360, 800));
	this->addChild(particle);


	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("rewardLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("reward_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮1
	cocos2d::ui::Widget* okbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("okbtn");
	okbtn->addTouchEventListener(CC_CALLBACK_2(RewardLayer::onBtnClick, this));

	//按钮1文字
	cocos2d::ui::ImageView* okbtntxt = (cocos2d::ui::ImageView*)okbtn->getChildByName("btntext");
	okbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(ResourceLang::map_lang["tboxdesctext"]);

	int startx[] = {360, 270 ,210};
	int offsetx[] = {0, 180, 150};
	int rewardsize = vec_rewards.size();

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
			else if (t == T_EPIECE)
			{
				Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
				pieceicon->setAnchorPoint(Vec2(0, 1));
				pieceicon->setPosition(10, resbox->getContentSize().height - 10);
				resbox->addChild(pieceicon);
			}

			if (type == 2)//地图包裹资源有最大限制
			{
				for (unsigned int n = 0; n < GlobalInstance::vec_resCreators.size(); n++)
				{
					if (resid.compare(GlobalInstance::vec_resCreators[n]->getName()) == 0)
					{
						int max = GlobalInstance::vec_resCreators[n]->getMaxCap(GlobalInstance::vec_resCreators[n]->getLv().getValue()).getValue();

						if (MyRes::getMyResCount(resid) >= max)
						{
							count = 0;
						}
						else
						{
							int left = max - MyRes::getMyResCount(resid);
							if (left < count)
								count = left;
						}
						break;
					}
				}
			}

			CommonFuncs::playResBoxEffect(resbox, t, qu, 0);

			resbox->setPositionX(startx[rewardsize-1] + offsetx[rewardsize-1]*i);
			resbox->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
			std::string resstr = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);//StringUtils::format("ui/%s.png", vec_rewards[i].rid.c_str());
			res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);

			namelbl->setString(GlobalInstance::map_AllResources[vec_rewards[i].rid].name);
			std::string countstr = StringUtils::format("%d", vec_rewards[i].count);
			countlbl->setString(countstr);
			if (resid.compare("r006") == 0)
			{
				DynamicValueInt dvint;
				dvint.setValue(count);
				GlobalInstance::getInstance()->addMySoliverCount(dvint);
			}
			else if (resid.compare("r012") == 0)
			{
				DynamicValueInt dvint;
				dvint.setValue(count);
				GlobalInstance::getInstance()->addMyCoinCount(dvint);
			}
			else
			{
				if (resid.compare("z002") == 0)
					forwhere = MYPACKAGE;
				MyRes::Add(resid, count, forwhere, qu, GlobalInstance::getInstance()->generateStoneCount(qu));
			}
		}
		else
		{
			resbox->setVisible(false);
		}
	}

	if (type == 1)
	{
		HttpDataSwap::init(this)->getRewardMatch();
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

void RewardLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		removeSelf();
	}
}

void RewardLayer::removeSelf()
{
	AnimationEffect::closeAniEffect((Layer*)this);
	if (g_mainScene != NULL)
	{
		TaskLayer* layer = (TaskLayer*)g_mainScene->getChildByName("9assemblyhall");
		if (layer != NULL)
		{
			layer->delayShowNewerGuide(0);
		}
	}
}