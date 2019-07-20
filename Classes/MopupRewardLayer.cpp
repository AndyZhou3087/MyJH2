#include "MopupRewardLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MyMenu.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "FightHeroNode.h"

MopupRewardLayer::MopupRewardLayer()
{
}


MopupRewardLayer::~MopupRewardLayer()
{
	std::vector<ResBase*>::iterator it;
	for (it = vec_dropdownres.begin(); it != vec_dropdownres.end();)
	{
		delete (*it);
		it = vec_dropdownres.erase(it);
	}
}

MopupRewardLayer* MopupRewardLayer::create(std::vector<MSGAWDSDATA> reward_res, int perwinexp)
{
	MopupRewardLayer*pRet = new(std::nothrow)MopupRewardLayer();
	if (pRet && pRet->init(reward_res, perwinexp))
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

bool MopupRewardLayer::init(std::vector<MSGAWDSDATA> reward_res, int perwinexp)
{
	if (!Layer::init())
	{
		return false;
	}

	m_rewards = reward_res;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("mopupretLayer.csb"));
	this->addChild(csbnode);

	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("winrewardtitle", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1002);
	closebtn->addTouchEventListener(CC_CALLBACK_2(MopupRewardLayer::onBtnClick, this));

	//按钮
	cocos2d::ui::Button* ationbtn = (cocos2d::ui::Button*)csbnode->getChildByName("actionbtn");
	ationbtn->setTag(1000);
	ationbtn->addTouchEventListener(CC_CALLBACK_2(MopupRewardLayer::onBtnClick, this));

	//按钮文字
	cocos2d::ui::ImageView* ationbtntxt = (cocos2d::ui::ImageView*)ationbtn->getChildByName("text");
	ationbtntxt->ignoreContentAdaptWithSize(true);
	ationbtntxt->loadTexture(ResourcePath::makeTextImgPath("closebtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* droptxt = (cocos2d::ui::ImageView*)csbnode->getChildByName("winsepbox")->getChildByName("text");
	droptxt->loadTexture(ResourcePath::makeTextImgPath("drop_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* exptxt = (cocos2d::ui::ImageView*)csbnode->getChildByName("winsepbox_0")->getChildByName("text");
	exptxt->loadTexture(ResourcePath::makeTextImgPath("mopupexp_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	dropscrollview = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	dropscrollview->setScrollBarEnabled(false);
	dropscrollview->setBounceEnabled(true);

	loadScrollviewData();
	updateScrollviewContent();

	int herocount = 0;
	for (int i = 0; i < 6; i++)
	{
		FightHeroNode* fightHeroNode = FightHeroNode::create();

		Hero* fighthero = GlobalInstance::myCardHeros[i];

		fightHeroNode->setPosition(145 + i % 3 * 215, 580 - i / 3 * 260);
		fightHeroNode->setData(fighthero, F_HERO, FS_SUCC);
		addChild(fightHeroNode, 0, i);
		if (fighthero != NULL)
		{
			fightHeroNode->setFightState(perwinexp);
			herocount++;
		}
	}
	std::string expstr = StringUtils::format(ResourceLang::map_lang["mopupexpdesc"].c_str(), herocount * perwinexp);
	cocos2d::ui::Text* expdesclbl = (cocos2d::ui::Text*)csbnode->getChildByName("winexpdesc");
	expdesclbl->setString(expstr);

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};

	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}

void MopupRewardLayer::updateScrollviewContent()
{
	cocos2d::ui::ScrollView* sv = dropscrollview;

	std::vector<ResBase*> vec_res;

	std::sort(vec_dropdownres.begin(), vec_dropdownres.end(), sortDropResByType);

	std::vector<ResBase*> vec_tmp;;
	std::vector<ResBase*>::iterator it;
	for (it = vec_dropdownres.begin(); it != vec_dropdownres.end();)
	{
		if ((*it)->getId().compare("r006") == 0 || (*it)->getId().compare("r012") == 0)
		{
			vec_tmp.push_back(*it);
			it = vec_dropdownres.erase(it);
		}
		else
			it++;
	}
	for (unsigned int i=0;i<vec_tmp.size();i++)
		vec_dropdownres.insert(vec_dropdownres.begin(), vec_tmp[i]);
			
	vec_res = vec_dropdownres;

	int ressize = vec_res.size();
	int row = ressize % 4 == 0 ? ressize / 4 : (ressize / 4 + 1);

	int itemheight = 160;

	int innerheight = itemheight * row;

	int contentheight = sv->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	sv->setInnerContainerSize(Size(sv->getContentSize().width, innerheight));


	for (unsigned int m = 0; m < vec_res.size(); m++)
	{
		std::string qustr = "ui/resbox.png";
		int qu = 0;
		if (vec_res[m]->getType() >= T_ARMOR && vec_res[m]->getType() <= T_FASHION)
		{
			qu = ((Equipable*)vec_res[m])->getQU().getValue();
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (vec_res[m]->getType() >= T_WG && vec_res[m]->getType() <= T_NG)
		{
			qu = GlobalInstance::map_GF[vec_res[m]->getId()].qu;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}
		else if (vec_res[m]->getType() >= T_RENS && vec_res[m]->getType() <= T_BOX)
		{
			qu = atoi(vec_res[m]->getId().substr(1).c_str()) - 1;
			qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
		}


		cocos2d::ui::ImageView* boxItem = cocos2d::ui::ImageView::create(qustr, cocos2d::ui::Widget::TextureResType::PLIST);
		boxItem->addTouchEventListener(CC_CALLBACK_2(MopupRewardLayer::onclick, this));
		boxItem->setTouchEnabled(true);

		boxItem->setPosition(Vec2(boxItem->getContentSize().width / 2 + 20 + m % 4 * itemheight, innerheight - itemheight / 2 - m / 4 * itemheight + 5));

		int tag = m;
		boxItem->setUserData((void*)vec_res[m]);
		boxItem->setTag(tag);
		std::string itemname = StringUtils::format("boxitem%d", tag);
		boxItem->setVisible(false);
		sv->addChild(boxItem, 0, itemname);
		boxItem->runAction(Sequence::create(DelayTime::create((m+1)*0.16), ScaleTo::create(0.01f, 1.1f), Show::create(), ScaleTo::create(0.15f, 1.0f), NULL));

		std::string resid = vec_res[m]->getId();

		std::string str = GlobalInstance::getInstance()->getResUIFrameName(resid, qu);

		Sprite * res = Sprite::createWithSpriteFrameName(str);
		res->setPosition(Vec2(boxItem->getContentSize().width / 2, boxItem->getContentSize().height / 2));
		boxItem->addChild(res);

		if (vec_res[m]->getType() == T_EPIECE)
		{
			Sprite* pieceicon = Sprite::createWithSpriteFrameName("ui/pieceicon.png");
			pieceicon->setAnchorPoint(Vec2(0, 1));
			pieceicon->setPosition(10, boxItem->getContentSize().height - 10);
			boxItem->addChild(pieceicon);
		}

		Label *namelbl = Label::createWithTTF(GlobalInstance::map_AllResources[resid].name, FONT_NAME, 23);
		namelbl->setColor(Color3B(34, 74, 79));
		namelbl->setPosition(Vec2(boxItem->getContentSize().width / 2, -10));
		boxItem->addChild(namelbl);

		std::string countstr = StringUtils::format("%d", vec_res[m]->getCount().getValue());
		Label *countlbl = Label::createWithTTF(countstr, FONT_NAME, 23);
		countlbl->setAnchorPoint(Vec2(1, 0));
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(boxItem->getContentSize().width - 10, 10));
		boxItem->addChild(countlbl, 0, "lbl");
	}

}

void MopupRewardLayer::loadScrollviewData()
{
	for (unsigned int i = 0; i < m_rewards.size(); i++)
	{
		std::string rid = m_rewards[i].rid;

		int t = 0;
		for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
		{
			if (rid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
				break;
		}
		int qu = m_rewards[i].qu;
		ResBase* res;
		if (t >= T_ARMOR && t <= T_FASHION)
		{
			res = new Equip();
			int stonecount = GlobalInstance::getInstance()->generateStoneCount(qu);

			for (int m = 0; m < stonecount; m++)
			{
				((Equip*)res)->vec_stones.push_back("o");
			}
		}
		else if (t >= T_WG && t <= T_NG)
		{
			res = new GongFa();
		}
		else
			res = new ResBase();
		res->setType(t);
		res->setId(rid);
		DynamicValueInt dv;
		int count = m_rewards[i].count;
		dv.setValue(count);
		res->setCount(dv);
		dv.setValue(qu);
		res->setQU(dv);

		vec_dropdownres.push_back(res);


		if (rid.compare("r006") == 0)
		{
			DynamicValueInt dvint;
			dvint.setValue(count);
			GlobalInstance::getInstance()->addMySoliverCount(dvint);
		}
		else if (rid.compare("r012") == 0)
		{
			DynamicValueInt dvint;
			dvint.setValue(count);
			GlobalInstance::getInstance()->addMyCoinCount(dvint);
		}
		else
		{
			MyRes::Add(rid, count, MYSTORAGE, qu, GlobalInstance::getInstance()->generateStoneCount(qu));
		}
	}
}

void MopupRewardLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::Button* btnnode = (cocos2d::ui::Button*)pSender;
		int tag = btnnode->getTag();
		switch (tag)
		{
		case 1000://
		{
			AnimationEffect::closeAniEffect(this);
			break;
		}
		case 1002://close
		{
			AnimationEffect::closeAniEffect(this);
		}
			break;
		default:
			break;
		}
	}
}

void MopupRewardLayer::onclick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	int tag = clicknode->getTag();

	if (type == ui::Widget::TouchEventType::BEGAN)
	{

	}
	else if (type == ui::Widget::TouchEventType::MOVED)
	{

	}

	else if (type == ui::Widget::TouchEventType::CANCELED)
	{

	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
	
	}
}


bool MopupRewardLayer::sortDropResByType(ResBase* a, ResBase* b)
{
	if (a->getType() > b->getType())
		return true;
	else 
		return false;
}