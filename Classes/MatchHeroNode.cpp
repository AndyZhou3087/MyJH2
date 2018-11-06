#include "MatchHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "MovingLabel.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "DataSave.h"
#include "GlobalInstance.h"
#include "Equip.h"
#include "HeroAttrLayer.h"

MatchHeroNode::MatchHeroNode()
{
	hero = NULL;
}


MatchHeroNode::~MatchHeroNode()
{
	delete hero;
}

MatchHeroNode* MatchHeroNode::create(int index, std::string herostr)
{
	MatchHeroNode *pRet = new(std::nothrow)MatchHeroNode();
	if (pRet && pRet->init(index, herostr))
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

bool MatchHeroNode::init(int index, std::string herostr)
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchHeroNode.csb"));
	this->addChild(csbnode, 0, "csbnode");

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* hbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	cocos2d::ui::ImageView* head = (cocos2d::ui::ImageView*)csbnode->getChildByName("head");

	head->addTouchEventListener(CC_CALLBACK_2(MatchHeroNode::onBtnClick, this));
	//名字
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	//等级
	cocos2d::ui::Text* lv = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	//职业
	cocos2d::ui::Text* vocation = (cocos2d::ui::Text*)csbnode->getChildByName("vocation");

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setVisible(false);

	for (int i = 1; i <= 5; i++)
	{
		std::string str = StringUtils::format("star%d", i);
		stars[i - 1] = csbnode->getChildByName(str);
		stars[i - 1]->setVisible(false);
	}

	int startx[] = { 0, -7, -15, -22, -30 };
	if (herostr.length() > 0)
	{
		std::vector<std::string> vec_heros;
		std::vector<std::string> vec_tmp;
		CommonFuncs::split(herostr, vec_heros, ";");
		CommonFuncs::split(vec_heros[0], vec_tmp, "-");

		std::string str = vec_tmp[0];
		hero = new Hero();
		hero->setName(str);
		DynamicValueInt dvint;
		dvint.setValue(atoi(vec_tmp[1].c_str()));
		hero->setExp(dvint);
		hero->setVocation(atoi(vec_tmp[2].c_str()));
		hero->setPotential(atoi(vec_tmp[3].c_str()));
		hero->setSex(atoi(vec_tmp[4].c_str()));
		hero->setRandAttr(atoi(vec_tmp[5].c_str()));
		hero->setChangeCount(atoi(vec_tmp[6].c_str()));
		hero->setState(HS_TRAINING);
		hero->setFTtype(1);

		namelbl->setString(str);
		str = StringUtils::format("vocation_%d", hero->getVocation());
		vocation->setString(ResourceLang::map_lang[str]);

		int vocation = atoi(vec_tmp[2].c_str());
		int sex = atoi(vec_tmp[4].c_str());
		str = StringUtils::format("images/cardh_%d_%d.png", vocation, sex);
		head->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);

		str = StringUtils::format("images/cardherobox_%s.png", vec_tmp[3].c_str());
		hbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);

		str = StringUtils::format("Lv.%d", hero->getLevel() + 1);
		lv->setString(str);

		for (int i = 0; i < hero->getChangeCount() - 1; i++)
		{
			stars[i]->setVisible(true);
			stars[i]->setPositionX(startx[(hero->getChangeCount() - 2)] + 15 * i);
		}

		if (vec_heros[1].length() > 0)
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(vec_heros[1], vec_tmp, "#");
			for (unsigned int i = 0; i < vec_tmp.size(); i++)
			{
				std::vector<std::string> vec_one;
				CommonFuncs::split(vec_tmp[i], vec_one, "-");
				std::string rid = vec_one[0];

				int m = 0;
				for (; m < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); m++)
				{
					if (rid.compare(0, 1, RES_TYPES_CHAR[m]) == 0)
						break;
				}
				if (m >= T_ARMOR && m <= T_FASHION)
				{
					Equip* res = new Equip();
					res->setId(rid);
					res->setType(m);
					DynamicValueInt dv;
					dv.setValue(1);
					res->setCount(dv);
					res->setWhere(MYEQUIP);

					DynamicValueInt dv1;
					dv1.setValue(atoi(vec_one[1].c_str()));
					res->setQU(dv1);

					DynamicValueInt dv2;
					dv2.setValue(atoi(vec_one[2].c_str()));
					res->setLv(dv2);

					CommonFuncs::split(vec_one[3], res->vec_stones, ",");

					res->setWhos(hero->getName());

					hero->setEquipable(res, m);
				}
				else if (m >= T_WG && m <= T_NG)
				{

					GongFa* res = new GongFa();
					res->setId(rid);
					res->setType(m);
					DynamicValueInt dv;
					dv.setValue(1);
					res->setCount(dv);
					res->setWhere(MYEQUIP);

					DynamicValueInt dv1;
					dv1.setValue(atoi(vec_one[1].c_str()));
					res->setQU(dv1);

					DynamicValueInt dv2;
					dv2.setValue(atoi(vec_one[2].c_str()));
					res->setExp(dv2);

					res->setWhos(hero->getName());
					hero->setEquipable(res, m);
				}
			}
		}

		GlobalInstance::matchPairHeros[index] = hero;
	}
	else
	{
		GlobalInstance::matchPairHeros[index] = NULL;
		hbox->loadTexture("images/cardherobox_.png", cocos2d::ui::Widget::TextureResType::LOCAL);
		head->setVisible(false);
		namelbl->setVisible(false);
		lv->setVisible(false);
		vocation->setVisible(false);
		csbnode->getChildByName("cardnamebox")->setVisible(false);
		desclbl->setVisible(true);

		std::string indexstr = StringUtils::format("selheronum%d", index + 1);
		std::string descstr = StringUtils::format(ResourceLang::map_lang["matchnosorttext"].c_str(), ResourceLang::map_lang[indexstr].c_str());
		desclbl->setString(descstr);
	}
	return true;
}

void MatchHeroNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		Layer* layer = HeroAttrLayer::create(hero, 4, 4);
		g_mainScene->addChild(layer);
	}
}
