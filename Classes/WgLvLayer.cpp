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

USING_NS_CC;

#define M001EXP 1000
#define M002EXP 4000
#define M003EXP 10000
#define M004EXP 20000

WgLvLayer::WgLvLayer()
{

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

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("wgLvLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	//����
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("wglv_text", langtype), cocos2d::ui::Widget::TextureResType::LOCAL);

	//��������
	name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	name->setString(GlobalInstance::map_AllResources[m_res->getId()].name);

	//Ʒ��box
	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	std::string str = StringUtils::format("ui/resbox_qu%d.png", m_res->getQU().getValue());
	resbox->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);

	//iconͼ��
	cocos2d::ui::ImageView* icon = (cocos2d::ui::ImageView*)csbnode->getChildByName("icon");
	str = GlobalInstance::getInstance()->getResUIFrameName(m_res->getId(), m_res->getQU().getValue());
	icon->loadTexture(ResourcePath::makePath(str), cocos2d::ui::Widget::TextureResType::PLIST);


	int curlvexp = 0;
	int nextlvexp = 0;
	int expsize = GlobalInstance::map_GF[m_res->getId()].vec_exp.size();

	if (m_res->getLv().getValue() >= expsize)
		nextlvexp = GlobalInstance::map_GF[m_res->getId()].vec_exp[expsize - 1];
	else
		nextlvexp = GlobalInstance::map_GF[m_res->getId()].vec_exp[m_res->getLv().getValue()];

	for (int i = 0; i < m_res->getLv().getValue(); i++)
	{
		curlvexp += GlobalInstance::map_GF[m_res->getId()].vec_exp[i];
	}

	//������
	expbar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName("wgbar");
	float percent = (m_res->getExp().getValue() - curlvexp)*100.0f / nextlvexp;
	expbar->setPercent(percent);

	explbl = (cocos2d::ui::Text*)csbnode->getChildByName("exptext");
	str = StringUtils::format("%d/%d", m_res->getExp().getValue() - curlvexp, nextlvexp);
	explbl->setString(str);

	//��ǰ�ȼ�
	lvtext = (cocos2d::ui::Text*)csbnode->getChildByName("lvtext");
	str = StringUtils::format("%d", m_res->getLv().getValue());
	lvtext->setString(str);

	cocos2d::ui::Button* closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(WgLvLayer::onBtnClick, this));

	//��ʾ
	cocos2d::ui::Text* tipstext = (cocos2d::ui::Text*)csbnode->getChildByName("tipstext");
	tipstext->setString(ResourceLang::map_lang["wglvtips"]);

	cocos2d::ui::Widget* curnode = (cocos2d::ui::Widget*)csbnode->getChildByName("curnode");
	cocos2d::ui::Widget* nextnode = (cocos2d::ui::Widget*)csbnode->getChildByName("nextnode");

	//��ɫ��������
	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d", i);
		cocos2d::ui::Text* attrtext = (cocos2d::ui::Text*)curnode->getChildByName(str);
		attrtext->setString(ResourceLang::map_lang[str]);

		cocos2d::ui::Text* attrtext2 = (cocos2d::ui::Text*)nextnode->getChildByName(str);
		attrtext2->setString(ResourceLang::map_lang[str]);
	}

	//����ֵ
	hplbl = (cocos2d::ui::Text*)curnode->getChildByName("hp");

	//����ֵ
	atkbl = (cocos2d::ui::Text*)curnode->getChildByName("atk");

	//����ֵ
	dflbl = (cocos2d::ui::Text*)curnode->getChildByName("df");

	//�����ٶ�ֵ
	atkspeedlbl = (cocos2d::ui::Text*)curnode->getChildByName("atkspeed");

	//����ֵ
	critlbl = (cocos2d::ui::Text*)curnode->getChildByName("crit");

	//����ֵ
	dodgelbl = (cocos2d::ui::Text*)curnode->getChildByName("dodge");

	//����ֵ
	hplbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("hp");

	//����ֵ
	atkbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("atk");

	//����ֵ
	dflbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("df");

	//�����ٶ�ֵ
	atkspeedlbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("atkspeed");

	//����ֵ
	critlbl2 = (cocos2d::ui::Text*)nextnode->getChildByName("crit");

	//����ֵ
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
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		if (m_res->getLv().getValue() + 1 == m_res->getMaxLv())
		{
			MovingLabel::show(ResourceLang::map_lang["wgmostlv"]);
			return;
		}

		int tag = clicknode->getTag();
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
			std::string s = StringUtils::format(ResourceLang::map_lang["winexp"].c_str(), count);
			MovingLabel::show(s);
			str = StringUtils::format("%d", MyRes::getMyResCount(str));
			goodarr[tag - 1]->setString(str);
			updataAtrrUI();

			EquipDescLayer* layer = (EquipDescLayer*)this->getParent();
			if (layer != NULL)
			{
				layer->updateAttr();
				StoreHouseLayer* storeHouseLayer = (StoreHouseLayer*)layer->getParent();
				if (storeHouseLayer != NULL)
					storeHouseLayer->updateUI();
			}
		}
		else
		{
			MovingLabel::show(ResourceLang::map_lang["reslack"]);
		}
	}
}

void WgLvLayer::updataAtrrUI()
{
	int hp = m_res->getHp();
	std::string attrstr = StringUtils::format("%d", hp);
	hplbl->setString(attrstr);

	//����ֵ
	attrstr = StringUtils::format("%.2f", m_res->getAtk());
	atkbl->setString(attrstr);

	//����ֵ
	attrstr = StringUtils::format("%.2f", m_res->getDf());
	dflbl->setString(attrstr);

	//�����ٶ�ֵ
	attrstr = StringUtils::format("%.3f", m_res->getAtkSpeed());
	atkspeedlbl->setString(attrstr);

	//����ֵ
	attrstr = StringUtils::format("%.3f%%", m_res->getCrit());
	critlbl->setString(attrstr);

	//����ֵ
	attrstr = StringUtils::format("%.3f%%", m_res->getDodge());
	dodgelbl->setString(attrstr);

	//�ȼ�
	std::string str = StringUtils::format("%d", m_res->getLv().getValue() + 1);
	lvtext->setString(str);

	std::string namestr = GlobalInstance::map_AllResources[m_res->getId()].name;
	if (m_res->getLv().getValue() > 0)
		namestr = StringUtils::format("+%d%s", m_res->getLv().getValue(), namestr.c_str());
	name->setString(namestr);

	int curlvexp = 0;
	int nextlvexp = 0;
	int expsize = GlobalInstance::map_GF[m_res->getId()].vec_exp.size();

	if (m_res->getLv().getValue() >= expsize)
		nextlvexp = GlobalInstance::map_GF[m_res->getId()].vec_exp[expsize - 1];
	else
		nextlvexp = GlobalInstance::map_GF[m_res->getId()].vec_exp[m_res->getLv().getValue()];

	for (int i = 0; i < m_res->getLv().getValue(); i++)
	{
		curlvexp += GlobalInstance::map_GF[m_res->getId()].vec_exp[i];
	}

	int mycurlv = m_res->getLv().getValue();
	//������
	float percent = (m_res->getExp().getValue() - curlvexp)*100.0f / nextlvexp;
	
	if (mycurlv > myprelv)
		expbar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, 100), LoadingBarProgressFromTo::create(0.2f, 0, percent), NULL));
	else
		expbar->runAction(Sequence::create(LoadingBarProgressTo::create(0.2f, percent), NULL));

	str = StringUtils::format("%d/%d", m_res->getExp().getValue() - curlvexp, nextlvexp);
	explbl->setString(str);

	for (int i = 1; i < 5; i++)
	{
		str = StringUtils::format("m00%d", i);
		str = StringUtils::format("%d", MyRes::getMyResCount(str));
		goodarr[i - 1]->setString(str);
	}

	if (m_res->getLv().getValue() < m_res->getMaxLv())
	{
		attrstr = StringUtils::format("%d", GlobalInstance::map_GF[m_res->getId()].vec_hp[m_res->getLv().getValue() + 1]);
		hplbl2->setString(attrstr);

		//����ֵ
		attrstr = StringUtils::format("%d", GlobalInstance::map_GF[m_res->getId()].vec_atk[m_res->getLv().getValue() + 1]);
		atkbl2->setString(attrstr);

		//����ֵ
		attrstr = StringUtils::format("%d", GlobalInstance::map_GF[m_res->getId()].vec_df[m_res->getLv().getValue() + 1]);
		dflbl2->setString(attrstr);

		//�����ٶ�ֵ
		attrstr = StringUtils::format("%.3f", GlobalInstance::map_GF[m_res->getId()].vec_speed[m_res->getLv().getValue() + 1]);
		atkspeedlbl2->setString(attrstr);

		//����ֵ
		attrstr = StringUtils::format("%.3f%%", GlobalInstance::map_GF[m_res->getId()].vec_crit[m_res->getLv().getValue() + 1]);
		critlbl2->setString(attrstr);

		//����ֵ
		attrstr = StringUtils::format("%.3f%%", GlobalInstance::map_GF[m_res->getId()].vec_avoid[m_res->getLv().getValue() + 1]);
		dodgelbl2->setString(attrstr);
	}

}

void WgLvLayer::onExit()
{
	Layer::onExit();
}
