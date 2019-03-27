#include "WgLvLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MainScene.h"
#include "MovingLabel.h"
#include "MyRes.h"
#include "DynamicValue.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "EquipDescLayer.h"
#include "StoreHouseLayer.h"
#include "LoadingBarProgressTimer.h"
#include "MainScene.h"

USING_NS_CC;

#define M001EXP 1000
#define M002EXP 4000
#define M003EXP 10000
#define M004EXP 20000

WgLvLayer::WgLvLayer()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	boxeffectnode = NULL;
}

WgLvLayer::~WgLvLayer()
{

}


WgLvLayer* WgLvLayer::create(ResBase* res)
{
	WgLvLayer *pRet = new(std::nothrow)WgLvLayer();
	if (pRet && pRet->init(res))
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
bool WgLvLayer::init(ResBase* res)
{
	if (!Layer::init())
	{
		return false;
	}

	m_res = (GongFa*)res;

	myprelv = m_res->getLv().getValue();

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 250));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("wgLvLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("wglv_text", langtype), cocos2d::ui::Widget::TextureResType::LOCAL);

	//武器名称
	name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	name->setString(GlobalInstance::map_AllResources[m_res->getId()].name);

	int qu = GlobalInstance::map_GF[m_res->getId()].qu;
	//品质box
	resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	std::string str = StringUtils::format("ui/resbox_qu%d.png", qu);
	resbox->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	if (boxeffectnode == NULL)
		boxeffectnode = CommonFuncs::playResBoxEffect(resbox, m_res->getType(), qu, m_res->getLv().getValue());

	//icon图标
	cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)csbnode->getChildByName("icon");
	str = GlobalInstance::getInstance()->getResUIFrameName(m_res->getId(), qu);
	icon->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	//进度条
	expbar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("wgbar");
	expbar->setPercent(0);

	explbl = (cocos2d::ui::Text*)csbnode->getChildByName("exptext");
	//当前等级
	lvtext = (cocos2d::ui::Text*)csbnode->getChildByName("lvtext");

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(WgLvLayer::onBtnClick, this));

	//提示
	cocos2d::ui::Text* tipstext = (cocos2d::ui::Text*)csbnode->getChildByName("tipstext");
	tipstext->setString(ResourceLang::map_lang["wglvtips"]);

	cocos2d::ui::Widget* curnode = (cocos2d::ui::Widget*)csbnode->getChildByName("curnode");
	cocos2d::ui::Widget* nextnode = (cocos2d::ui::Widget*)csbnode->getChildByName("nextnode");

	//角色属性文字
	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d", i);
		cocos2d::ui::Text* attrtext = (cocos2d::ui::Text*)curnode->getChildByName(str);
		attrtext->setString(ResourceLang::map_lang[str]);

		cocos2d::ui::Text* attrtext2 = (cocos2d::ui::Text*)nextnode->getChildByName(str);
		attrtext2->setString(ResourceLang::map_lang[str]);
	}

	//生命值
	hplbl = (cocos2d::ui::Text*)curnode->getChildByName("hp");

	//攻击值
	atkbl = (cocos2d::ui::Text*)curnode->getChildByName("atk");

	//防御值
	dflbl = (cocos2d::ui::Text*)curnode->getChildByName("df");

	//攻击速度值
	atkspeedlbl = (cocos2d::ui::Text*)curnode->getChildByName("atkspeed");

	//暴击值
	critlbl = (cocos2d::ui::Text*)curnode->getChildByName("crit");

	//闪避值
	dodgelbl = (cocos2d::ui::Text*)curnode->getChildByName("dodge");

	//生命值
	hplbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("hp");

	//攻击值
	atkbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("atk");

	//防御值
	dflbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("df");

	//攻击速度值
	atkspeedlbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("atkspeed");

	//暴击值
	critlbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("crit");

	//闪避值
	dodgelbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("dodge");


	for (int i = 1; i < 5; i++)
	{
		std::string str = StringUtils::format("m00%dimg", i);
		cocos2d::ui::ImageView* goodimg = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
		goodimg->addTouchEventListener(CC_CALLBACK_2(WgLvLayer::onGoodsClick, this));
		goodimg->setTag(i);

		str = StringUtils::format("m00%dtext", i);
		goodarr[i - 1] = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		std::string nstr = StringUtils::format("m00%d", i);
		str = StringUtils::format("%d", MyRes::getMyResCount(nstr));
		goodarr[i - 1]->setString(str);

		str = StringUtils::format("m00%dname", i);
		cocos2d::ui::Text* mname = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		mname->setString(GlobalInstance::map_AllResources[nstr].name);
	}

	cocos2d::ui::ImageView* wgquatext = (cocos2d::ui::ImageView*)csbnode->getChildByName("wgquatext");
	wgquatext->loadTexture(ResourcePath::makeTextImgPath("wxquatext",langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* curtext = (cocos2d::ui::ImageView*)csbnode->getChildByName("curtext");
	curtext->loadTexture(ResourcePath::makeTextImgPath("wgcurtext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* nexttext = (cocos2d::ui::ImageView*)csbnode->getChildByName("nexttext");
	nexttext->loadTexture(ResourcePath::makeTextImgPath("wgnexttext", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	updataAtrrUI();

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

void WgLvLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	}
}

void WgLvLayer::onGoodsClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::Button* clicknode = (cocos2d::ui::Button*)pSender;

	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		m_longTouchNode = clicknode;
		if (!isScheduled(schedule_selector(WgLvLayer::longTouchUpdate)))
			schedule(schedule_selector(WgLvLayer::longTouchUpdate), 0.1f);
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		cancelLongTouch();
		addGfExp(clicknode->getTag());
	}
	else if (type == ui::Widget::TouchEventType::CANCELED)
	{
		cancelLongTouch();
	}
}

void WgLvLayer::addGfExp(int gftag)
{
	int maxlv = GlobalInstance::map_GF[m_res->getId()].vec_exp.size();

	//if (m_res->getLv().getValue() + 1 == m_res->getMaxLv())
	if (m_res->getExp().getValue() >= GlobalInstance::map_GF[m_res->getId()].vec_exp[maxlv - 1])
	{
		MovingLabel::show(ResourceLang::map_lang["wgmostlv"]);
		return;
	}

	int tag = gftag;
	int count;
	switch (tag)
	{
	case 1:
		count = M001EXP;
		break;
	case 2:
		count = M002EXP;
		break;
	case 3:
		count = M003EXP;
		break;
	case 4:
		count = M004EXP;
		break;
	default:
		break;
	}

	std::string str = StringUtils::format("m00%d", tag);
	if (MyRes::getMyResCount(str) >= 1)
	{
		MyRes::Use(str);
		DynamicValueInt dal;
		dal.setValue(m_res->getExp().getValue() + count);
		m_res->setExp(dal);
		if (m_res->getExp().getValue() >= GlobalInstance::map_GF[m_res->getId()].vec_exp[maxlv - 1])
		{
			DynamicValueInt dvint;
			dvint.setValue(GlobalInstance::map_GF[m_res->getId()].vec_exp[maxlv - 1]);
			m_res->setExp(dvint);
		}

		std::string s = StringUtils::format(ResourceLang::map_lang["winexp"].c_str(), count);
		MovingLabel::show(s);
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[tag - 1]->setString(str);
		updataAtrrUI();
		MyRes::saveData();

		EquipDescLayer* layer = (EquipDescLayer*)this->getParent();
		if (layer != NULL)
		{
			layer->updateAttr();
			StoreHouseLayer* storeHouseLayer = (StoreHouseLayer*)g_mainScene->getChildByName("3storehouse");
			if (storeHouseLayer != NULL)
				storeHouseLayer->updateUI();
		}
	}
	else
	{
		MovingLabel::show(ResourceLang::map_lang["reslack"]);
	}
}

void WgLvLayer::updataAtrrUI()
{
	int hp = m_res->getHp();
	std::string attrstr = StringUtils::format("%d", hp);
	hplbl->setString(attrstr);

	//攻击值
	attrstr = StringUtils::format("%.2f", m_res->getAtk());
	atkbl->setString(attrstr);

	//防御值
	attrstr = StringUtils::format("%.2f", m_res->getDf());
	dflbl->setString(attrstr);

	//攻击速度值
	attrstr = StringUtils::format("%.3f", m_res->getAtkSpeed());
	atkspeedlbl->setString(attrstr);

	//暴击值
	attrstr = StringUtils::format("%.3f%%", m_res->getCrit());
	critlbl->setString(attrstr);

	//闪避值
	attrstr = StringUtils::format("%.3f%%", m_res->getDodge());
	dodgelbl->setString(attrstr);

	//等级
	std::string str = StringUtils::format("%d", m_res->getLv().getValue() + 1);
	lvtext->setString(str);

	std::string namestr = GlobalInstance::map_AllResources[m_res->getId()].name;
	if (m_res->getLv().getValue() > 0)
		namestr = StringUtils::format("+%d%s", m_res->getLv().getValue() + 1, namestr.c_str());
	name->setString(namestr);

	int curlvexp = 0;
	int nextlvexp = GlobalInstance::map_GF[m_res->getId()].vec_exp[m_res->getLv().getValue()];

	if (m_res->getLv().getValue() > 0)
		curlvexp = GlobalInstance::map_GF[m_res->getId()].vec_exp[m_res->getLv().getValue() - 1];

	int mycurlv = m_res->getLv().getValue();

	//if (mycurlv >= m_res->getMaxLv() - 1)


	//进度条
	float percent = (m_res->getExp().getValue() - curlvexp)*100.0f / (nextlvexp - curlvexp);

	if (mycurlv > myprelv)
	{
		expbar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, 100), DelayTime::create(0.05f), LoadingBarProgressFromTo::create(0.2f, 0, percent), NULL));
		if (boxeffectnode == NULL)
			boxeffectnode = CommonFuncs::playResBoxEffect(resbox, m_res->getType(), m_res->getQU().getValue(), m_res->getLv().getValue());
	}
	else
		expbar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), NULL));

	str = StringUtils::format("%d/%d", m_res->getExp().getValue() - curlvexp, nextlvexp - curlvexp);
	explbl->setString(str);

	myprelv = mycurlv;

	for (int i = 1; i < 5; i++)
	{
		str = StringUtils::format("m00%d", i);
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[i - 1]->setString(str);
	}

	int nextlv = 0;

	if (m_res->getLv().getValue() < m_res->getMaxLv() - 1)
	{
		nextlv = m_res->getLv().getValue() + 1;
	}
	else
	{
		nextlv = m_res->getMaxLv() - 1;
	}
		
	attrstr = StringUtils::format("%d", GlobalInstance::map_GF[m_res->getId()].vec_hp[nextlv]);
	hplbl2->setString(attrstr);

	//攻击值
	attrstr = StringUtils::format("%d", GlobalInstance::map_GF[m_res->getId()].vec_atk[nextlv]);
	atkbl2->setString(attrstr);

	//防御值
	attrstr = StringUtils::format("%d", GlobalInstance::map_GF[m_res->getId()].vec_df[nextlv]);
	dflbl2->setString(attrstr);

	//攻击速度值
	attrstr = StringUtils::format("%.3f", GlobalInstance::map_GF[m_res->getId()].vec_speed[nextlv]);
	atkspeedlbl2->setString(attrstr);

	//暴击值
	attrstr = StringUtils::format("%.3f%%", GlobalInstance::map_GF[m_res->getId()].vec_crit[nextlv]);
	critlbl2->setString(attrstr);

	//闪避值
	attrstr = StringUtils::format("%.3f%%", GlobalInstance::map_GF[m_res->getId()].vec_avoid[nextlv]);
	dodgelbl2->setString(attrstr);

}

void WgLvLayer::longTouchUpdate(float delay)
{
	m_isLongPress = true;
	if (m_longTouchNode != NULL) {
		addGfExp(m_longTouchNode->getTag());
	}
}

void WgLvLayer::cancelLongTouch()
{
	m_isLongPress = false;
	m_longTouchNode = NULL;
	unschedule(schedule_selector(WgLvLayer::longTouchUpdate));
}

void WgLvLayer::onExit()
{
	Layer::onExit();
}
