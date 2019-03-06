#include "OpenHolesLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "Equip.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "TakeOnLayer.h"
#include "AnimationEffect.h"
#include "BuyResLayer.h"

int openRnd[][5] = { {65,35,25,10,0}, { 80,45,35,25,10 } };

OpenHolesLayer::OpenHolesLayer()
{

}


OpenHolesLayer::~OpenHolesLayer()
{

}


OpenHolesLayer* OpenHolesLayer::create(Equip* res_equip)
{
	OpenHolesLayer *pRet = new(std::nothrow)OpenHolesLayer();

	if (pRet && pRet->init(res_equip))
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

bool OpenHolesLayer::init(Equip* res_equip)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("openHolesLayer.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	m_equip = res_equip;

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("titlename");
	title->setString(ResourceLang::map_lang["openholestitle"]);

	int qu = m_equip->getQU().getValue();
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
	cocos2d::ui::ImageView* resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	CommonFuncs::playResBoxEffect(resbox_qu, qu);

	std::string str = GlobalInstance::getInstance()->getResUIFrameName(m_equip->getId(), qu);

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("equipname");

	std::string namestr = GlobalInstance::map_AllResources[m_equip->getId()].name;
	if (m_equip->getLv().getValue() > 0)
		namestr = StringUtils::format("+%d%s", m_equip->getLv().getValue(), namestr.c_str());
	name->setString(namestr);

	name->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	cocos2d::ui::Text* openholesdesc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	openholesdesc->setString(ResourceLang::map_lang["openholesdesc"]);

	for (int i = 0; i < 2; i++)
	{
		std::string restr;
		std::string resqustr;
		if (i == 0)
		{
			restr = "i002";
			resqustr = "ui/resbox_qu1.png";
		}
		else
		{
			restr = "i004";
			resqustr = "ui/resbox_qu3.png";
		}
		str = StringUtils::format("resbox%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
		resbox->loadTexture(resqustr, cocos2d::ui::Widget::TextureResType::PLIST);
		resbox->addTouchEventListener(CC_CALLBACK_2(OpenHolesLayer::onResClick, this));
		resbox->setTag(i);

		str = StringUtils::format("resname%d", i);
		cocos2d::ui::Text* resname = (cocos2d::ui::Text*)csbnode->getChildByName(str);

		resname->setString(GlobalInstance::map_AllResources[restr].name);

		str = StringUtils::format("rescount%d", i);
		rescount[i] = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		str = StringUtils::format("%d", MyRes::getMyResCount(restr));
		rescount[i]->setString(str);
	}

	tipstext = (cocos2d::ui::Text*)csbnode->getChildByName("tipstext");

	selectindex = 0;

	updateUi();

	this->schedule(schedule_selector(OpenHolesLayer::updateResCount), 1.0f);


	//Ç¿»¯°´Å¥
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(OpenHolesLayer::onBtnClick, this));
	actionbtn->setTag(1000);
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("okbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

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

void OpenHolesLayer::updateUi()
{
	std::string nodename = StringUtils::format("resbox%d", selectindex);
	selectNode = csbnode->getChildByName("select");
	selectNode->setPosition(csbnode->getChildByName(nodename)->getPosition());
	std::string tipstr = StringUtils::format(ResourceLang::map_lang["openholestips"].c_str(), openRnd[selectindex][m_equip->getQU().getValue()]);
	tipstext->setString(tipstr);
}

void OpenHolesLayer::updateResCount(float dt)
{
	for (int i = 0; i < 2; i++)
	{
		std::string restr;
		if (i == 0)
			restr = "i002";
		else
			restr = "i004";

		std::string str = StringUtils::format("%d", MyRes::getMyResCount(restr));
		rescount[i]->setString(str);
	}
}

void OpenHolesLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();

		std::string resid;
		if (selectindex == 0)
			resid = "i002";
		else
			resid = "i004";
		if (checkRes())
		{
			MyRes::Use(resid);

			int r = GlobalInstance::getInstance()->createRandomNum(100) + 1;
			if (r <= openRnd[selectindex][m_equip->getQU().getValue()])
			{
				TakeOnLayer* parent = (TakeOnLayer*)this->getParent();
				parent->openStoneHole();
				MovingLabel::show(ResourceLang::map_lang["openholesucc"]);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["openholefail"]);
			}
			AnimationEffect::closeAniEffect(this);
		}
		else
		{
			std::vector< MSGAWDSDATA> vec_res;
			MSGAWDSDATA rdata;
			rdata.rid = resid;
			rdata.count = 1;
			rdata.qu = 0;
			vec_res.push_back(rdata);
			BuyResLayer* layer = BuyResLayer::create(vec_res);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
	}
}

bool OpenHolesLayer::checkRes()
{
	std::string resid;
	if (selectindex == 0)
		resid = "i002";
	else
		resid = "i004";
	if (MyRes::getMyResCount(resid) <= 0)
	{
		std::string hstr = StringUtils::format(ResourceLang::map_lang["notenouph"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str());
		MovingLabel::show(hstr);
		return false;
	}
	return true;
}

void OpenHolesLayer::onResClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		selectindex = node->getTag();
		updateUi();
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		if (!checkRes())
		{
			std::string resid;
			if (selectindex == 0)
				resid = "i002";
			else
				resid = "i004";
			std::vector< MSGAWDSDATA> vec_res;
			MSGAWDSDATA rdata;
			rdata.rid = resid;
			rdata.count = 1;
			rdata.qu = 0;
			vec_res.push_back(rdata);
			BuyResLayer* layer = BuyResLayer::create(vec_res);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		}
	}
}