#include "EquipQuUpLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "StoreHouseLayer.h"
#include "SimpleResPopLayer.h"
#include "ResDescLayer.h"

int needequipcount[] = {0, 5, 3, 2 , 1};

int needjcount[] = {0, 5, 10, 20, 40};

EquipQuUpLayer::EquipQuUpLayer()
{
	boxeffectnode = NULL;
}


EquipQuUpLayer::~EquipQuUpLayer()
{
	MyRes::saveData();
}


EquipQuUpLayer* EquipQuUpLayer::create(Equip* res_equip)
{
	EquipQuUpLayer*pRet = new(std::nothrow)EquipQuUpLayer();

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

bool EquipQuUpLayer::init(Equip* res_equip)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("equipquupLayer.csb"));
	this->addChild(csbnode);

	langtype = GlobalInstance::getInstance()->getLang();

	m_equip = res_equip;

	cocos2d::ui::Text* title = (cocos2d::ui::Text*)csbnode->getChildByName("title");
	title->setString(ResourceLang::map_lang["quuptitle"]);

	int qu = m_equip->getQU().getValue();
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", qu + 1);
	resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	if (boxeffectnode == NULL)
		boxeffectnode = CommonFuncs::playResBoxEffect(resbox_qu, m_equip->getType(), qu + 1, m_equip->getLv().getValue());

	std::string str = GlobalInstance::getInstance()->getResUIFrameName(m_equip->getId(), qu + 1);

	cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	std::string namestr = GlobalInstance::map_AllResources[m_equip->getId()].name;
	name->setString(namestr);

	name->setTextColor(Color4B(POTENTIALCOLOR[qu + 1]));

	for (int i = 0; i < 2; i++)
	{
		str = StringUtils::format("resbox_%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

		qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		resbox->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);
		resbox->addTouchEventListener(CC_CALLBACK_2(EquipQuUpLayer::onResClick, this));
		resbox->setTag(i);
		resbox->setScale(0.6f);

		str = StringUtils::format("resname_%d", i);
		cocos2d::ui::Text* resname = (cocos2d::ui::Text*)csbnode->getChildByName(str);
		std::string restr;
		cocos2d::ui::ImageView* nres = (cocos2d::ui::ImageView*)resbox->getChildByName("res");

		std::string countlblkey = StringUtils::format("rescount_%d", i);
		cocos2d::ui::Text* rescount = (cocos2d::ui::Text*)csbnode->getChildByName(countlblkey);

		if (i == 0)
		{
			CommonFuncs::playResBoxEffect(resbox, m_equip->getType(), qu, 0);
	
			std::string rstr = GlobalInstance::getInstance()->getResUIFrameName(m_equip->getId(), qu);
			nres->loadTexture(rstr, cocos2d::ui::Widget::TextureResType::PLIST);

			restr = m_equip->getId();
			
			int qucount = MyRes::getEquipableCountByQU(restr, qu);

			str = StringUtils::format("%d/%d", qucount, needequipcount[qu+1]);
			rescount->setString(str);

			if (qucount < needequipcount[qu + 1])
			{
				rescount->setColor(Color3B(255, 0, 0));
			}
		}
		else
		{
			restr = StringUtils::format("l%s", m_equip->getId().c_str());
			std::string rstr = GlobalInstance::getInstance()->getResUIFrameName(restr, 0);
			nres->loadTexture(rstr, cocos2d::ui::Widget::TextureResType::PLIST);

			Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
			pieceicon->setAnchorPoint(Vec2(0, 1));
			pieceicon->setPosition(10, resbox->getContentSize().height - 10);
			resbox->addChild(pieceicon);

			int jcount = MyRes::getMyResCount(restr);
			str = StringUtils::format("%d/%d", jcount, needjcount[qu + 1]);
			rescount->setString(str);

			needjresid = restr;

			if (jcount < needjcount[qu + 1])
			{
				rescount->setColor(Color3B(255, 0, 0));
			}
		}
		resname->setString(GlobalInstance::map_AllResources[restr].name);
	}


	//强化按钮
	cocos2d::ui::Widget* actionbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(EquipQuUpLayer::onBtnClick, this));
	actionbtn->setTag(1000);
	cocos2d::ui::ImageView* actionbtntxt = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	actionbtntxt->loadTexture(ResourcePath::makeTextImgPath("quupbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);


	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		AnimationEffect::closeAniEffect((Layer*)this);
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void EquipQuUpLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();

		if (tag == 1000)
		{
			bool islaskres = false;
			int qu = m_equip->getQU().getValue();
			std::string equipid = m_equip->getId();
			int qucount = MyRes::getEquipableCountByQU(equipid, m_equip->getQU().getValue());

			if (qucount < needequipcount[qu + 1])
			{
				islaskres = true;
			}

			int jcount = MyRes::getMyResCount(needjresid);
			if (jcount < needjcount[qu + 1])
			{
				islaskres = true;
			}

			if (islaskres)
			{
				MovingLabel::show(ResourceLang::map_lang["reslack"]);
				return;
			}

			DynamicValueInt dvlv;
			int stonescount = 0;
			for (int i = 0; i < needequipcount[qu + 1]; i++)
			{
				ResBase* e = MyRes::getEquipableByQU(equipid, qu);
				Equip* ep = (Equip*)e;

				if (ep->getLv().getValue() > dvlv.getValue())
					dvlv.setValue(ep->getLv().getValue());

				if (ep->vec_stones.size() > stonescount)
					stonescount = ep->vec_stones.size();

				for (unsigned int n = 0; n < ep->vec_stones.size(); n++)
				{
					std::string stoneid = ep->vec_stones[n];
					if (stoneid.length() >= 3)//有镶嵌宝石
					{
						MyRes::Add(ep->vec_stones[n]);
					}
				}


				MyRes::Use(e);
			}

			if (dvlv.getValue() >= 16 && dvlv.getValue() <= 20)
				dvlv.setValue(dvlv.getValue() - 3);
			else if (dvlv.getValue() >= 11 && dvlv.getValue() <= 15)
				dvlv.setValue(dvlv.getValue() - 2);
			else if (dvlv.getValue() >= 6 && dvlv.getValue() <= 10)
				dvlv.setValue(dvlv.getValue() - 1);

			if (stonescount > 0)
			{
				int r = GlobalInstance::getInstance()->createRandomNum(100);
				if (r < 50)
					stonescount -= 1;
			}

			MyRes::Use(needjresid, needjcount[qu + 1]);
			int st = stonescount;//GlobalInstance::getInstance()->generateStoneCount(qu + 1);
			Equip* adde = (Equip*)MyRes::Add(equipid, 1, MYSTORAGE, qu + 1, st);

			adde->setLv(dvlv);

			std::string showstr = StringUtils::format(ResourceLang::map_lang["quupsucc"].c_str(), GlobalInstance::map_AllResources[equipid].name.c_str());
			MovingLabel::show(showstr);
		}

		AnimationEffect::closeAniEffect((Layer*)this->getParent());

		if (g_mainScene != NULL)
		{
			StoreHouseLayer* mlayer = (StoreHouseLayer*)g_mainScene->getChildByName("3storehouse");
			if (mlayer != NULL)
			{
				mlayer->updateUI();
			}
		}
	}
}

void EquipQuUpLayer::onResClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		std::string restr;
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		Layer* layer;
		if (tag == 0)
		{
			layer = ResDescLayer::create(m_equip, 2);
		}
		else
		{
			SimpleResPopLayer* layer = SimpleResPopLayer::create(needjresid, 2);
		}
		this->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}