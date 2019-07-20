#include "SelectSubMapLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MapBlockScene.h"
#include "Resource.h"
#include "MovingLabel.h"
#include "SoundManager.h"
#include "NewGuideLayer.h"
#include "MainMapScene.h"
#include "DataSave.h"
#include "BuyResLayer.h"
#include "MyRes.h"
#include "ResDescLayer.h"
#include "AnimationEffect.h"
#include "StarDescLayer.h"
#include "StarFrist3AwdLayer.h"

USING_NS_CC;

SelectSubMapLayer::SelectSubMapLayer()
{
	isentermap = false;
}

SelectSubMapLayer::~SelectSubMapLayer()
{
	
}


SelectSubMapLayer* SelectSubMapLayer::create(std::string mapid)
{
	SelectSubMapLayer *pRet = new(std::nothrow)SelectSubMapLayer();
	if (pRet && pRet->init(mapid))
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
bool SelectSubMapLayer::init(std::string mainmapid)
{
    if ( !Layer::init() )
    {
        return false;
    }
	m_mainmapid = mainmapid;

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("selectSubMapLayer.csb"));
	this->addChild(csbnode);
	int langtype = GlobalInstance::getInstance()->getLang();

	//标题
	cocos2d::ui::ImageView* titleimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("title");
	titleimg->loadTexture(ResourcePath::makeTextImgPath("submaptitle_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	//按钮
	cocos2d::ui::Widget* backbtn = (cocos2d::ui::Widget*)csbnode->getChildByName("backbtn");
	backbtn->addTouchEventListener(CC_CALLBACK_2(SelectSubMapLayer::onBtnClick, this));

	//按钮文字
	//cocos2d::ui::ImageView* backbtntxt = (cocos2d::ui::ImageView*)backbtn->getChildByName("text");
	//backbtntxt->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ScrollView* scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollview");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	cocos2d::ui::Widget* morearrow = (cocos2d::ui::Widget*)csbnode->getChildByName("morearrow");

	int size = GlobalInstance::map_mapsdata[mainmapid].map_sublist.size();

	//if (size > 5)
	//{
	//	morearrow->setVisible(true);
	//	morearrow->runAction(RepeatForever::create(Blink::create(1, 8)));
	//}
	//else
	//{
	//	morearrow->setVisible(false);
	//}
	morearrow->setVisible(false);

	starAwdNode = csbnode->getChildByName("starnode");

	int itemheight = 170;
	int innerheight = itemheight * size;
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(720, innerheight));

	int i = 0;
	std::map<std::string, S_SubMap>::iterator it;
	for (it = GlobalInstance::map_mapsdata[mainmapid].map_sublist.begin(); it != GlobalInstance::map_mapsdata[mainmapid].map_sublist.end(); it++)
	{
		Node* subnode = CSLoader::createNode(ResourcePath::makePath("selectMapNode.csb"));

		if (i < 8)
		{
			subnode->setPosition(Vec2(scrollView->getContentSize().width + 600, innerheight - i * itemheight - itemheight / 2));
			subnode->runAction(EaseSineIn::create(MoveBy::create(0.15f + i * 0.07f, Vec2(-scrollView->getContentSize().width / 2 - 600, 0))));
		}
		else
			subnode->setPosition(Vec2(scrollView->getContentSize().width / 2, innerheight - i * itemheight - itemheight / 2));

		scrollView->addChild(subnode);

		cocos2d::ui::Text* mapname = (cocos2d::ui::Text*)subnode->getChildByName("namelbl");
		mapname->setString(GlobalInstance::map_AllResources[it->first].name);

		cocos2d::ui::Text* hpdesc = (cocos2d::ui::Text*)subnode->getChildByName("hpdesc");
		std::string strdesc = StringUtils::format(ResourceLang::map_lang["hpdesc"].c_str(), GlobalInstance::map_mapsdata[mainmapid].map_sublist[it->first].ph);
		hpdesc->setString(strdesc);

		cocos2d::ui::Text* awarddesc = (cocos2d::ui::Text*)subnode->getChildByName("awarddesc");
		std::string tmp;
		int awdsize = GlobalInstance::map_mapsdata[mainmapid].map_sublist[it->first].vec_awd.size();
		for (int i= 0; i < awdsize; i++)
		{
			std::string resid = GlobalInstance::map_mapsdata[mainmapid].map_sublist[it->first].vec_awd[i];

			tmp.append(GlobalInstance::map_AllResources[resid].name);
			if (i < awdsize - 1)
				tmp.append(ResourceLang::map_lang["dunhao"]);
		}
		strdesc = StringUtils::format(ResourceLang::map_lang["awarddesc"].c_str(), tmp.c_str());
		awarddesc->setString(strdesc);

		cocos2d::ui::ImageView* item = (cocos2d::ui::ImageView*)subnode->getChildByName("item");
		item->setTag(i+1);
		item->addTouchEventListener(CC_CALLBACK_2(SelectSubMapLayer::onNodeClick, this));
		item->setSwallowTouches(false);

		int finishOrder = DataSave::getInstance()->getMapOrderCount(m_mainmapid);

		if (i >= finishOrder && !checkMapIsPass(it->first))
		{
			for (int k = 0; k < subnode->getChildren().size(); k++)
				CommonFuncs::changeGray(subnode->getChildren().at(k));
		}

		if ((GlobalInstance::myCurMainData.place.compare(it->first) == 0 && GlobalInstance::myCurMainData.isfinish == QUEST_ACC) || (GlobalInstance::myCurBranchData.place.compare(it->first) == 0 && GlobalInstance::myCurBranchData.isfinish == QUEST_ACC))
		{
			cocos2d::ui::ImageView* taskicon = (cocos2d::ui::ImageView*)subnode->getChildByName("taskicon");
			taskicon->setVisible(true);
			taskicon->setPositionX(mapname->getPositionX() + mapname->getContentSize().width / 2 + 30);

			if (!NewGuideLayer::checkifNewerGuide(53))
			{
				if (NewGuideLayer::checkifNewerGuide(54))
				{
					showNewerGuide(54, item);
				}
			}
		}

		i++;

		if (m_mainmapid.compare("m1-5") != 0 && m_mainmapid.compare("m1-6") != 0)
		{

			verifyStar(it->first);

			std::vector<std::string> vec_finishstar;

			CommonFuncs::split(DataSave::getInstance()->getFinishStar(it->first), vec_finishstar, ",");

			for (unsigned int n = 0; n < 3; n++)
			{
				std::string starname = StringUtils::format("star%d", n);
				Node* star = subnode->getChildByName(starname);

				if (n < vec_finishstar.size())
				{
					star->setVisible(true);
				}
				else
				{
					star->setVisible(false);
				}
			}

			showCStarAwdUI();
		}
		else
		{
			Node* starbox = subnode->getChildByName("starbox");
			starbox->setVisible(false);

			for (int i = 0; i < 3; i++)
			{
				std::string keyname = StringUtils::format("starbg_%d", i);
				Node* starbg = subnode->getChildByName(keyname);
				starbg->setVisible(false);

				keyname = StringUtils::format("star%d", i);
				Node* star = subnode->getChildByName(keyname);
				star->setVisible(false);
			}

			starAwdNode->setVisible(false);
		}
	}

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setString(GlobalInstance::map_AllResources[mainmapid].desc);

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

void SelectSubMapLayer::showNewerGuide(int step, Node* node)
{
	m_step = step;
	nodes.clear();
	if (step == 54)
	{
		nodes.push_back(node);
	}
	this->scheduleOnce(schedule_selector(SelectSubMapLayer::delayShowNewerGuide), 0.1f);
}

void SelectSubMapLayer::delayShowNewerGuide(float dt)
{
	MainMapScene* mapscene = (MainMapScene*)this->getParent();
	if (mapscene != NULL)
	{
		mapscene->showNewerGuideNode(m_step, nodes);
	}
}

void SelectSubMapLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		this->removeFromParentAndCleanup(true);
	}
}

void SelectSubMapLayer::onNodeClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	cocos2d::ui::ImageView* clicknode = (cocos2d::ui::ImageView*)pSender;
	if (type == ui::Widget::TouchEventType::BEGAN)
	{
		clickflag = true;
		beginTouchPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));
	}
	else if (type == ui::Widget::TouchEventType::MOVED)
	{
		Vec2 movedPoint = clicknode->convertToWorldSpace(Vec2(clicknode->getPositionX(), clicknode->getPositionY()));

		if (fabs(movedPoint.x - beginTouchPoint.x) >= CLICKOFFSETP || fabs(movedPoint.y - beginTouchPoint.y) >= CLICKOFFSETP)
			clickflag = false;
	}
	else if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (!clickflag)
			return;

		if (isentermap)
			return;

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);

		std::string mapid = StringUtils::format("%s-%d", m_mainmapid.c_str(), clicknode->getTag());

		int finishOrder = DataSave::getInstance()->getMapOrderCount(m_mainmapid);

		if (clicknode->getTag() - 1 >= finishOrder && !checkMapIsPass(mapid))
		{
			std::string premapid = StringUtils::format("%s-%d", m_mainmapid.c_str(), clicknode->getTag() - 1);
			std::string str = StringUtils::format(ResourceLang::map_lang["maporderhint"].c_str(), GlobalInstance::map_AllResources[premapid].name.c_str());
			MovingLabel::show(str);
			return;
		}

		showCloudAnim(clicknode->getParent()->getParent(), clicknode->getParent()->getPosition());

		GlobalInstance::eventfrommapid = "";
		GlobalInstance::eventstartmappos = -1;
		GlobalInstance::map_randstartpos = -1;
		//if (m_mainmapid.compare("m1-5") == 0)
		//{
		//	int needph = GlobalInstance::map_mapsdata[m_mainmapid].map_sublist[mapid].ph;
		//	bool isphok = true;

		//	std::string nohpherostr;
		//	int nohpcount = 0;
		//	for (int i = 0; i < 6; i++)
		//	{
		//		if (GlobalInstance::myCardHeros[i] != NULL && GlobalInstance::myCardHeros[i]->getPower().getValue() < needph)
		//		{
		//			nohpcount++;
		//			if (nohpherostr.length() > 0)
		//				nohpherostr.append(ResourceLang::map_lang["dunhao"]);
		//			if (nohpcount <= 3)
		//				nohpherostr.append(GlobalInstance::myCardHeros[i]->getName());
		//			else
		//			{
		//				nohpherostr.append("...");
		//				break;
		//			}
		//			isphok = false;
		//		}
		//	}
		//	if (isphok)
		//	{

		//		DynamicValueInt dv;
		//		for (int i = 0; i < 6; i++)
		//		{
		//			if (GlobalInstance::myCardHeros[i] != NULL)
		//			{
		//				dv.setValue(GlobalInstance::myCardHeros[i]->getPower().getValue() - needph);
		//				GlobalInstance::myCardHeros[i]->setPower(dv);
		//				GlobalInstance::getInstance()->saveHero(GlobalInstance::myCardHeros[i]);
		//			}
		//		}

		//		clicknode->setEnabled(false);

		//		isentermap = true;
		//		GlobalInstance::ishasmazeentry = false;
		//		Director::getInstance()->replaceScene(TransitionFade::create(2.0f, MapBlockScene::createScene(mapid, GlobalInstance::map_mapsdata[m_mainmapid].map_sublist[mapid].bgtype)));

		//	}
		//	else
		//	{
		//		nohpherostr = nohpherostr.substr(0, nohpherostr.length());
		//		nohpherostr.append(ResourceLang::map_lang["nomorehp"]);
		//		MovingLabel::show(nohpherostr);

		//		if (MyRes::getMyResCount("p001", MYSTORAGE) > 0)
		//		{
		//			ResDescLayer* layer = ResDescLayer::create(MyRes::getMyRes("p001", MYSTORAGE), 0);
		//			this->addChild(layer, 0, 1111);
		//			AnimationEffect::openAniEffect((Layer*)layer);
		//		}
		//		else
		//		{
		//			std::vector< MSGAWDSDATA> vec_res;
		//			MSGAWDSDATA rdata;
		//			rdata.rid = "p001";
		//			rdata.count = 1;
		//			rdata.qu = 0;
		//			vec_res.push_back(rdata);
		//			BuyResLayer* layer = BuyResLayer::create(vec_res);
		//			this->addChild(layer);
		//		}
		//	}
		//}
		//else
		//{
			StarDescLayer* layer = StarDescLayer::create(mapid);
			this->addChild(layer);
			AnimationEffect::openAniEffect(layer);
		//}
	}
}

void SelectSubMapLayer::showCloudAnim(Node* target, Vec2 pos)
{
	Sprite* cloud1 = Sprite::createWithSpriteFrameName("mapui/submapanimcloud.png");
	cloud1->setPosition(pos);
	target->addChild(cloud1);
	cloud1->runAction(Spawn::create(MoveBy::create(1.0f, Vec2(150, 0)), FadeOut::create(1.5f), NULL));

	Sprite* cloud2 = Sprite::createWithSpriteFrameName("mapui/submapanimcloud.png");
	cloud2->setFlippedX(true);
	cloud2->setPosition(pos);
	target->addChild(cloud2);
	cloud2->runAction(Spawn::create(MoveBy::create(1.0f, Vec2(-150, 0)), FadeOut::create(1.5f), NULL));
}

void SelectSubMapLayer::showCStarAwdUI()
{
	mychapterstar = 0;

	curchapter = atoi(m_mainmapid.substr(1, m_mainmapid.find_first_of("-") - 1).c_str());

	std::string findstr = StringUtils::format("jhstarm%d", curchapter);

	tinyxml2::XMLDocument *pDoc = new tinyxml2::XMLDocument();

	std::string content = GlobalInstance::getInstance()->getUserDefaultXmlString(1);

	int err = pDoc->Parse(content.c_str());
	if (err != 0)
	{
		delete pDoc;
	}
	tinyxml2::XMLElement *rootEle = pDoc->RootElement();

	tinyxml2::XMLElement *element = rootEle->FirstChildElement();
	while (element != NULL)
	{
		std::string key = element->Name();
		
		if (key.find(findstr) != std::string::npos)
		{
			if (element->GetText() != NULL)
			{
				std::string textstr = DataSave::getInstance()->getFinishStar(key.substr(6));
				std::vector<std::string> vec_tmp;
				CommonFuncs::split(textstr, vec_tmp, ",");
				mychapterstar += vec_tmp.size();
			}
		}
		element = element->NextSiblingElement();
	}
	cocos2d::ui::LoadingBar* bar = (cocos2d::ui::LoadingBar*)starAwdNode->getChildByName("bar");
	int sectionstarposx[3] = {0};

	int starinindex = 0;

	int totalstars = GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_starnum[2];
	for (int i = 0; i < 3; i++)
	{
		std::string ukey = StringUtils::format("box%d", i);
		box[i] = (cocos2d::ui::ImageView*)starAwdNode->getChildByName(ukey);


		int starcount = GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_starnum[i];
		int state = GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_getstate[i];
		if (mychapterstar >= starcount)
		{
			starinindex = i + 1;
			std::string boxstr;
			if (state == 0)
			{
				boxstr = StringUtils::format("ui/cstarbox%d_s.png", i + 1);
				box[i]->runAction(RepeatForever::create(Sequence::create(RotateTo::create(0.1f, 7), RotateTo::create(0.1f, 0), RotateTo::create(0.1f, -7), RotateTo::create(0.1f, 0), DelayTime::create(0.5f), NULL)));
			}
			else if (state == 1)
			{
				boxstr = StringUtils::format("ui/cstarbox%d_p.png", i + 1);
			}
			box[i]->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		ukey = StringUtils::format("c%d", i);

		box[i]->setTag(i);
		box[i]->addTouchEventListener(CC_CALLBACK_2(SelectSubMapLayer::onAwdBoxClick, this));

		cocos2d::ui::Text* starcountlbl = (cocos2d::ui::Text*)starAwdNode->getChildByName(ukey);

		std::string countstr = StringUtils::format("%d", starcount);
		starcountlbl->setString(countstr);

		int lastposx = 0;
		if (i == 0)
			lastposx = bar->getPositionX();
		else
			lastposx = box[i - 1]->getPositionX();
		sectionstarposx[i] = (box[i]->getPositionX() - lastposx);
	}

	if (starinindex >= 3)
		starinindex = 2;

	if (starinindex == 0)
	{
		float posx = mychapterstar * sectionstarposx[0] / GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_starnum[0];
		bar->setPercent(posx*100 / bar->getContentSize().width);
	}
	else
	{
		int p0 = GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_starnum[starinindex] - GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_starnum[starinindex - 1];
		int p1 = mychapterstar - GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_starnum[starinindex - 1];

		float p = p1* sectionstarposx[starinindex] / p0;
		for (int t = 0; t < starinindex; t++)
		{
			p += sectionstarposx[t];
		}
		bar->setPercent(p*100 / bar->getContentSize().width);
	}
}

void SelectSubMapLayer::onAwdBoxClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		cocos2d::ui::ImageView* clicknode = (cocos2d::ui::ImageView*)pSender;

		int boxindex = clicknode->getTag();
		int state = GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_getstate[boxindex];
		if (state == 0)
		{
			if (mychapterstar < GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_starnum[boxindex])
			{
				MovingLabel::show(ResourceLang::map_lang["cnostar"], Color4B(Color3B(255, 229, 188)), Vec2(360, 300));
			}
			else
			{
				GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_getstate[boxindex] = 1;
				clicknode->stopAllActions();
				clicknode->setRotation(0);
				std::string boxstr = StringUtils::format("ui/cstarbox%d_p.png", boxindex + 1);
				clicknode->loadTexture(boxstr, cocos2d::ui::Widget::TextureResType::PLIST);
				StarFrist3AwdLayer* layer = StarFrist3AwdLayer::create(GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_adws[boxindex][0]);
				this->addChild(layer);
				AnimationEffect::openAniEffect(layer);
				std::string savestr;

				for (int i = 0; i < 3; i++)
				{
					std::string onestr = StringUtils::format("%d", GlobalInstance::vec_chaperstarawds[curchapter - 1].vec_getstate[i]);
					savestr.append(onestr);
				}
				DataSave::getInstance()->setChapterStarAwd(curchapter, savestr);
			}
		}
		else if (state == 1)
		{
			MovingLabel::show(ResourceLang::map_lang["cstarawdgeted"], Color4B(Color3B(255, 229, 188)), Vec2(360, 300));
		}
	}
}

bool SelectSubMapLayer::checkMapIsPass(std::string mapid)
{
	int maintaskf = -1;
	std::vector<std::string> vec;
	CommonFuncs::split(GlobalInstance::myCurMainData.place, vec, "-");
	if (vec.size() >= 3)
	{
		int d1 = atoi(vec[0].substr(1).c_str());
		int d2 = atoi(vec[1].c_str());
		int d3 = atoi(vec[2].c_str());
		maintaskf = d1 * 10000 + d2 * 100 + d3;
	}
	
	int curf = 0;
	std::vector<std::string> vec1;
	CommonFuncs::split(mapid, vec1, "-");
	if (vec1.size() >= 3)
	{
		int d1 = atoi(vec1[0].substr(1).c_str());
		int d2 = atoi(vec1[1].c_str());
		int d3 = atoi(vec1[2].c_str());
		curf = d1 * 10000 + d2 * 100 + d3;
	}

	if (curf <= maintaskf)
		return true;
	return false;
}

void SelectSubMapLayer::verifyStar(std::string mapid)
{

	std::vector<int> vec_starids;
	std::vector<std::string> vec_starc = GlobalInstance::map_mapsdata[m_mainmapid].map_sublist[mapid].vec_starc;

	for (unsigned int i = 0; i < vec_starc.size(); i++)
	{
		std::vector<std::string> vec_one;
		CommonFuncs::split(vec_starc[i], vec_one, "-");

		int cid = atoi(vec_one[0].c_str());

		vec_starids.push_back(cid);
	}

	std::string savestr;
	std::vector<std::string> vec_finishstar;
	std::string finishstarstr = DataSave::getInstance()->getFinishStar(mapid);
	CommonFuncs::split(finishstarstr, vec_finishstar, ",");

	for (unsigned int i = 0; i < vec_finishstar.size(); i++)
	{
		bool isfind = false;
		int ctype = atoi(vec_finishstar[i].c_str());
		for (unsigned int m = 0; m < vec_starids.size(); m++)
		{
			if (ctype == vec_starids[m])
			{
				isfind = true;
				break;
			}
		}
		if (isfind)
		{
			if (savestr.length() > 0)
				savestr.append(",");
			std::string str = StringUtils::format("%d", ctype);
			savestr.append(str);
		}
	}

	if (savestr.compare(finishstarstr) != 0)
	{
		DataSave::getInstance()->setFinishStar(mapid, savestr);
	}
}