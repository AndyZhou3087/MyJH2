#include "MatchHeroNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "MovingLabel.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "DataSave.h"
#include "GlobalInstance.h"

MatchHeroNode::MatchHeroNode()
{
	hero = NULL;
}


MatchHeroNode::~MatchHeroNode()
{
	delete hero;
}

MatchHeroNode* MatchHeroNode::create(std::string herostr)
{
	MatchHeroNode *pRet = new(std::nothrow)MatchHeroNode();
	if (pRet && pRet->init(herostr))
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

bool MatchHeroNode::init(std::string herostr)
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchHeroNode.csb"));
	this->addChild(csbnode, 0, "csbnode");

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* hbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("hbox");
	cocos2d::ui::ImageView* head = (cocos2d::ui::ImageView*)csbnode->getChildByName("head");
	//名字
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	//等级
	cocos2d::ui::Text* lv = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	//职业
	cocos2d::ui::Text* vocation = (cocos2d::ui::Text*)csbnode->getChildByName("vocation");

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

		Hero* hero = new Hero();
		DynamicValueInt dvint;
		dvint.setValue(atoi(vec_tmp[1].c_str()));
		hero->setExp(dvint);
		hero->setChangeCount(atoi(vec_tmp[6].c_str()));
		hero->setVocation(atoi(vec_tmp[2].c_str()));

		std::string str = vec_tmp[0];
		namelbl->setString(str);
		str = StringUtils::format("vocation_%d", hero->getVocation());
		vocation->setString(ResourceLang::map_lang[str]);

		int vocation = atoi(vec_tmp[2].c_str());
		int sex = atoi(vec_tmp[4].c_str());
		str = StringUtils::format("images/cardh_%d_%d.png", vocation, sex);
		head->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);

		str = StringUtils::format("images/cardherobox_%s.png", vec_tmp[3].c_str());
		hbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);

		str = StringUtils::format("Lv.%d", hero->getLevel());
		lv->setString(str);

		for (int i = 0; i < hero->getChangeCount() - 1; i++)
		{
			stars[i]->setVisible(true);
			stars[i]->setPositionX(startx[(hero->getChangeCount() - 2)] + 15 * i);
		}

	}


	return true;
}
