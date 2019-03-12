#include "SmithyLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "ConsumeResActionLayer.h"
#include "MovingLabel.h"
#include "DataSave.h"
#include "MyRes.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "NewGuideLayer.h"
#include "MainScene.h"
#include "EquipDescLayer.h"
#include "MakeResNode.h"

USING_NS_CC;

SmithyLayer::SmithyLayer()
{
	lastCategoryindex = 1;
}

SmithyLayer::~SmithyLayer()
{
	
}


SmithyLayer* SmithyLayer::create(Building* buidingData)
{
	SmithyLayer *pRet = new(std::nothrow)SmithyLayer();
	if (pRet && pRet->init(buidingData))
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
bool SmithyLayer::init(Building* buidingData)
{
    if ( !Layer::init() )
    {
        return false;
    }

	m_buidingData = buidingData;
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("smithyLayer.csb"));
	this->addChild(csbnode);

	langtype = GlobalInstance::getInstance()->getLang();
	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("titleimg");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("smithytilte", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//等级
	lvUIlbl = (cocos2d::ui::Text*)csbnode->getChildByName("lv");
	std::string str = StringUtils::format("%d%s", buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	//升级按钮
	cocos2d::ui::Widget* lvUpbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("lvupbtn");
	lvUpbtn->setTag(1000);
	lvUpbtn->addTouchEventListener(CC_CALLBACK_2(SmithyLayer::onBtnClick, this));

	//升级按钮文字
	cocos2d::ui::ImageView* lvUpbtntxt = (cocos2d::ui::ImageView*)lvUpbtn->getChildByName("text");
	lvUpbtntxt->loadTexture(ResourcePath::makeTextImgPath("smithylv_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//关闭按钮
	closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1001);
	closebtn->addTouchEventListener(CC_CALLBACK_2(SmithyLayer::onBtnClick, this));

	//滚动控件
	m_contentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("contentscroll");

	m_contentscroll->setScrollBarEnabled(false);
	m_contentscroll->setBounceEnabled(true);

	m_armcontentscroll = (cocos2d::ui::ScrollView*)csbnode->getChildByName("armcontentscroll");

	m_armcontentscroll->setScrollBarEnabled(false);
	m_armcontentscroll->setBounceEnabled(true);

	hintlbl = (cocos2d::ui::Text*)csbnode->getChildByName("hintdesc");
	if (buidingData->level.getValue() >= buidingData->maxlevel.getValue() - 1)
		hintlbl->setString("");
	else
		hintlbl->setString(ResourceLang::map_lang["smithylvdesc"]);

	Node* categoryBtnNode = csbnode->getChildByName("catanode");
	for (int i = 0; i < categoryBtnNode->getChildrenCount(); i++)
	{
		std::string btnstr = StringUtils::format("btn%d", i);
		cocos2d::ui::Button* btn = (cocos2d::ui::Button*)categoryBtnNode->getChildByName(btnstr);
		btn->setTag(i);
		btn->addTouchEventListener(CC_CALLBACK_2(SmithyLayer::onCategory, this));
		vec_categoryBtn.push_back(btn);
	}
	loadData();

	for (int i = 0; i < 8; i++)
	{
		std::string name = StringUtils::format("cataclick%d", i);
		armcataclick[i] = (cocos2d::ui::Widget*)m_armcontentscroll->getChildByName(name);
		armcataclick[i]->setTag(i);
		armcataclick[i]->addTouchEventListener(CC_CALLBACK_2(SmithyLayer::onArmsCategory, this));

		name = StringUtils::format("catac%d", i);
		armcatacontent[i] = m_armcontentscroll->getChildByName(name);

		cocos2d::ui::ImageView* fittext = (cocos2d::ui::ImageView*)armcataclick[i]->getChildByName("fittext");
		name = StringUtils::format("vocfit_text%d", i / 2);
		fittext->loadTexture(ResourcePath::makeTextImgPath(name, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
	loadArmCataUi();

	updateContent(lastCategoryindex);

	this->scheduleOnce(schedule_selector(SmithyLayer::delayShowNewerGuide), newguidetime);

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

void SmithyLayer::delayShowNewerGuide(float dt)
{
	if (!NewGuideLayer::checkifNewerGuide(73))
	{
		if (NewGuideLayer::checkifNewerGuide(74))
		{
			showNewerGuide(74);
		}
		else if (NewGuideLayer::checkifNewerGuide(76))
		{
			showNewerGuide(76);
		}
	}
}

void SmithyLayer::showNewerGuide(int step)
{
	std::vector<Node*> nodes;
	if (step == 74)
	{
		nodes.push_back(armcatacontent[0]->getChildByName(map_catearms[0][0])->getChildByName("csbnode")->getChildByName("resitem"));
	}
	else if (step == 76)
	{
		nodes.push_back(closebtn);
	}
	g_mainScene->showNewerGuideNode(step, nodes);
}

void SmithyLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);

	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		switch (tag)
		{
		case 1000://升级
			if (m_buidingData->level.getValue() < m_buidingData->maxlevel.getValue() - 1)
			{
				ConsumeResActionLayer* layer = ConsumeResActionLayer::create(m_buidingData, CA_BUILDINGLVUP);
				this->addChild(layer);
				AnimationEffect::openAniEffect((Layer*)layer);
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["maxlv"]);
			}
			break;
		case 1001://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		default:
			break;
		}
	}
}

void SmithyLayer::loadData()
{
	map_cateRes.clear();
	int showlv = m_buidingData->maxlevel.getValue();//m_buidingData->level.getValue()
	for (int v = 0; v <= showlv; v++)
	{
		int vsiez = m_buidingData->vec_exdata.size();
		if (v < vsiez)
		{
			for (unsigned int i = 0; i < m_buidingData->vec_exdata[v].size(); i++)
			{
				std::string resid = m_buidingData->vec_exdata[v][i];
				if (resid.compare(0, 1, "a") == 0)
					map_cateRes[SMCATA_1].push_back(resid);
				else if (resid.compare(0, 1, "e") == 0)
					map_cateRes[SMCATA_2].push_back(resid);
				else if (resid.compare(0, 1, "g") == 0)
					map_cateRes[SMCATA_3].push_back(resid);
				else if (resid.compare(0, 1, "f") == 0)
					map_cateRes[SMCATA_4].push_back(resid);
				else
					map_cateRes[SMCATA_0].push_back(resid);
			}
		}
	}

	for (unsigned int i = 0; i < map_cateRes[SMCATA_1].size(); i++)
	{
		std::string armstrid = map_cateRes[SMCATA_1][i];
		int subtype = GlobalInstance::map_Equip[armstrid].type - 1;
		if (subtype < 100)
			map_catearms[subtype].push_back(armstrid);
	}
}

void SmithyLayer::updateContent(int category)
{
	for (unsigned int i = 0; i < vec_categoryBtn.size(); i++)
	{
		cocos2d::ui::ImageView* btntext = (cocos2d::ui::ImageView*)vec_categoryBtn[i]->getChildByName("text");

		if (category != i)
		{
			std::string textstr = StringUtils::format("smcatabtn%d_text_n", i);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			vec_categoryBtn[i]->setBright(true);
		}
		else
		{
			std::string textstr = StringUtils::format("smcatabtn%d_text_s", i);
			vec_categoryBtn[i]->setBright(false);
			btntext->loadTexture(ResourcePath::makeTextImgPath(textstr, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		}
	}

	if (category != 1)
	{
		m_contentscroll->setVisible(true);
		m_armcontentscroll->setVisible(false);

		m_contentscroll->removeAllChildrenWithCleanup(true);
		m_contentscroll->jumpToTop();

		int itemheight = 140;

		int size = map_cateRes[category].size();
		int innerheight = itemheight * size;
		int contentheight = m_contentscroll->getContentSize().height;
		if (innerheight < contentheight)
			innerheight = contentheight;
		m_contentscroll->setInnerContainerSize(Size(650, innerheight));

		if (size > 0)
			hintlbl->setPositionY(155);
		else
			hintlbl->setPositionY(720);

		for (int i = 0; i < size; i++)
		{
			MakeResNode* itemnode = MakeResNode::create(map_cateRes[category][i]);

			itemnode->setPosition(Vec2(m_contentscroll->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
			itemnode->runAction(EaseSineIn::create(MoveBy::create(0.15f + i * 0.07f, Vec2(-m_contentscroll->getContentSize().width / 2 - 600, 0))));

			//itemnode->setPosition(Vec2(m_contentscroll->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));
			m_contentscroll->addChild(itemnode, 0, map_cateRes[category][i]);
			if (itemnode->getResInSmithyLv() > m_buidingData->level.getValue())
				itemnode->setEnable(false);
		}
	}
	else
	{
		m_contentscroll->setVisible(false);
		m_armcontentscroll->setVisible(true);
		m_armcontentscroll->jumpToTop();
		updateArmContent(-1);
	}
}

void SmithyLayer::updateArmContent(int armcategory)
{
	if (armcategory < 0)
	{
		for (int i = 0; i < 8; i++)
		{
			armcatacontent[i]->setVisible(true);
		}
	}
	else
	{
		armcatacontent[armcategory]->setVisible(!armcatacontent[armcategory]->isVisible());
		armcatacontent[armcategory]->setPositionY(armcataclick[armcategory]->getPositionY() - 100);
		armcataclick[armcategory]->getChildByName("armcate_zhe")->setRotation(!armcatacontent[armcategory]->isVisible() * (-90));
	}

	int scrollheight = 100 * 8;
	for (int i = 0; i < 8; i++)
	{
		if (armcatacontent[i]->isVisible())
		{
			scrollheight += map_catearms[i].size() * 120;
		}
	}

	float per = m_armcontentscroll->getScrolledPercentVertical();
	m_armcontentscroll->setInnerContainerSize(Size(m_armcontentscroll->getContentSize().width, scrollheight));
	m_armcontentscroll->jumpToPercentVertical(per);

	if (armcategory < 0)
	{
		if (armcategory == -1)
		{
			armcataclick[0]->setPositionY(m_armcontentscroll->getInnerContainerSize().height - 40);
			armcatacontent[0]->setPositionY(armcataclick[0]->getPositionY() - 100);
			for (int i = 0; i < 8; i++)
			{
				if (i > 0)
				{
					int size = map_catearms[i - 1].size();
					armcataclick[i]->setPositionY(armcataclick[i - 1]->getPositionY() - 100 - 120 * size);
					armcatacontent[i]->setPositionY(armcataclick[i]->getPositionY() - 100);
				}
			}
		}

		std::vector<Node*> vec_item;
		for (int i = 0; i < 8; i++)
		{
			if (armcatacontent[i]->isVisible())
			{
				for (unsigned int n = 0; n < map_catearms[i].size(); n++)
				{
					Node* node = armcatacontent[i]->getChildByName(map_catearms[i][n]);
					vec_item.push_back(node);
				}
			}
		}

		for (unsigned int i = 0;i< vec_item.size();i++)
		{
			vec_item[i]->setPositionX(720);
			vec_item[i]->runAction(EaseSineIn::create(MoveTo::create(0.15f + i * 0.07f, Vec2(50, vec_item[i]->getPositionY()))));
		}
	}
	else
	{
		armcataclick[0]->setPositionY(m_armcontentscroll->getInnerContainerSize().height - 40);
		armcatacontent[0]->setPositionY(armcataclick[0]->getPositionY() - 100);

		//int size = map_catearms[armcategory].size();

		for (int i = 0; i < 8; i++)
		{
			if (i > 0)
			{
				Vec2 pos1;
				if (armcatacontent[i - 1]->isVisible())
				{
					int size = map_catearms[i - 1].size();
					pos1 = Vec2(armcataclick[i]->getPositionX(), armcataclick[i - 1]->getPositionY() - 100 - 120 * size);
					//armcataclick[i]->setPosition(pos1);

					//Vec2 pos2 = Vec2(armcatacontent[i]->getPositionX(), armcataclick[i]->getPositionY() - 100);
					//armcatacontent[i]->setPosition(pos2);
				}
				else
				{
					pos1 = Vec2(armcataclick[i]->getPositionX(), armcataclick[i - 1]->getPositionY() - 100);
					//armcataclick[i]->setPosition(pos1);
					//Vec2 pos2 = Vec2(armcatacontent[i]->getPositionX(), armcataclick[i]->getPositionY() - 100);
					//armcatacontent[i]->setPosition(pos2);
				}
				armcataclick[i]->setPosition(pos1);

				Vec2 pos2 = Vec2(armcatacontent[i]->getPositionX(), armcataclick[i]->getPositionY() - 100);
				armcatacontent[i]->setPosition(pos2);
			}
		}

		std::vector<Node*> vec_item;
		if (armcatacontent[armcategory]->isVisible())
		{
			for (unsigned int n = 0; n < map_catearms[armcategory].size(); n++)
			{
				Node* node = armcatacontent[armcategory]->getChildByName(map_catearms[armcategory][n]);
				vec_item.push_back(node);
			}
		}

		for (unsigned int i = 0; i < vec_item.size(); i++)
		{
			vec_item[i]->setPositionX(720);
			vec_item[i]->runAction(EaseSineIn::create(MoveTo::create(0.15f + i * 0.07f, Vec2(50, vec_item[i]->getPositionY()))));
		}
	}
}

void SmithyLayer::loadArmCataUi()
{
	for (int i = 0; i < 8; i++)
	{
		int size = map_catearms[i].size();
		for (int n = 0; n < size; n++)
		{
			MakeResNode* itemnode = MakeResNode::create(map_catearms[i][n]);
			itemnode->setScale(0.8f);
			itemnode->setPosition(Vec2(50, -n*120));

			armcatacontent[i]->addChild(itemnode, 0, map_catearms[i][n]);
			if (itemnode->getResInSmithyLv() > m_buidingData->level.getValue())
				itemnode->setEnable(false);
			armcatacontent[i]->setVisible(false);
		}
	}
}

void SmithyLayer::onCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		if (lastCategoryindex == node->getTag())
			return;

		lastCategoryindex = node->getTag();

		updateContent(node->getTag());
	}
}

void SmithyLayer::onArmsCategory(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		Node* node = (Node*)pSender;
		updateArmContent(node->getTag());
	}
}

void SmithyLayer::lvup()
{
	std::string str = StringUtils::format("%d%s", m_buidingData->level.getValue() + 1, ResourceLang::map_lang["lvtext"].c_str());
	lvUIlbl->setString(str);

	//loadData();
	//updateContent(lastCategoryindex);
	for (unsigned int i = 0; i < map_cateRes[lastCategoryindex].size(); i++)
	{
		MakeResNode* itemnode;
		
		if (lastCategoryindex == 1)
		{
			for (int n = 0; n < 8; n++)
			{
				for (unsigned int m = 0; m < map_catearms[n].size(); m++)
				{
					itemnode = (MakeResNode*)armcatacontent[n]->getChildByName(map_catearms[n][m]);
					if (itemnode->getResInSmithyLv() <= m_buidingData->level.getValue())
						itemnode->setEnable(true);
				}
			}
		}
		else
		{
			itemnode = (MakeResNode*)m_contentscroll->getChildByName(map_cateRes[lastCategoryindex][i]);
			if (itemnode->getResInSmithyLv() <= m_buidingData->level.getValue())
				itemnode->setEnable(true);
		}

	}
}

void SmithyLayer::makeRes(std::string resid)
{
	//品质概率
	int qu = 0;
	int rnd = GlobalInstance::getInstance()->createRandomNum(100);
	if (m_buidingData->level.getValue() < 4)
	{
		qu = 0;
	}
	else if (m_buidingData->level.getValue() < 9)
	{
		if (rnd < 80)
			qu = 0;
		else
			qu = 1;
	}
	else if (m_buidingData->level.getValue() < 14)
	{
		if (rnd < 60)
			qu = 0;
		else if (rnd < 90)
			qu = 1;
		else
			qu = 2;
	}
	else
	{
		if (rnd < 55)
			qu = 0;
		else if (rnd < 85)
			qu = 1;
		else if (rnd < 95)
			qu = 2;
		else
			qu = 3;
	}
	int stc = GlobalInstance::getInstance()->generateStoneCount(qu);
	ResBase* retres = MyRes::Add(resid, 1, MYSTORAGE, qu, stc);

	std::string qukey = StringUtils::format("potential_%d", qu);
	std::string resstr = StringUtils::format(ResourceLang::map_lang["resmname"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str());
	std::string desc;
	if (resid.compare(0, 1, "a") == 0 || resid.compare(0, 1, "e") == 0 || resid.compare(0, 1, "g") == 0 || resid.compare(0, 1, "f") == 0)
		desc = StringUtils::format("%s%s%s%s", ResourceLang::map_lang["makesucc"].c_str(), resstr.c_str(), ResourceLang::map_lang["potentialtext"].c_str(), ResourceLang::map_lang[qukey].c_str());
	else
		desc = StringUtils::format("%s%s", ResourceLang::map_lang["makesucc"].c_str(), resstr.c_str());
	MovingLabel::show(desc);

	MakeResNode* itemnode;


	if (lastCategoryindex == 1)
	{
		bool isfind = false;
		for (int n = 0; n < 8; n++)
		{
			for (unsigned int m = 0; m < map_catearms[n].size(); m++)
			{
				if (map_catearms[n][m].compare(resid) == 0)
				{
					itemnode = (MakeResNode*)armcatacontent[n]->getChildByName(map_catearms[n][m]);
					isfind = true;
					break;
				}
			}
			if (isfind)
				break;
		}
	}
	else
	{
		itemnode = (MakeResNode*)m_contentscroll->getChildByName(resid);
	}
	itemnode->updateMyOwnCountUI();

	if (!(retres->getType() >= T_ARMOR && retres->getType() <= T_NG))
	{
		return;
	}
	EquipDescLayer* layer = EquipDescLayer::create(retres, 1);
	this->addChild(layer);
	AnimationEffect::openAniEffect(layer);

	if (qu >= 3)
	{
		std::string st = StringUtils::format("potential_%d", qu);
		std::string namestr = StringUtils::format(ResourceLang::map_lang["libraryinfoequipname"].c_str(), GlobalInstance::map_AllResources[resid].name.c_str(), ResourceLang::map_lang[st].c_str());

		std::string contentstr = StringUtils::format(ResourceLang::map_lang["newtemplet0"].c_str(), GlobalInstance::getInstance()->getMyNickName().c_str(), namestr.c_str());
		MainScene::addNews(contentstr, 2);
	}
}