#include "GfLibraryInfoLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"
#include "MyRes.h"
#include "AnimationEffect.h"

USING_NS_CC;

GfLibraryInfoLayer::GfLibraryInfoLayer()
{

}

GfLibraryInfoLayer::~GfLibraryInfoLayer()
{

}


GfLibraryInfoLayer* GfLibraryInfoLayer::create(std::string gfid)
{
	GfLibraryInfoLayer *pRet = new(std::nothrow)GfLibraryInfoLayer();
	if (pRet && pRet->init(gfid))
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
bool GfLibraryInfoLayer::init(std::string gfid)
{
	if (!Layer::init())
	{
		return false;
	}

	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color,0,"colorLayer");

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("gfLibraryInfoLayer.csb"));
	this->addChild(csbnode);

	//关闭按钮按钮
	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->addTouchEventListener(CC_CALLBACK_2(GfLibraryInfoLayer::onBtnClick, this));
	closebtn->setTag(1000);

	cocos2d::ui::Widget* smallbg = (cocos2d::ui::Widget*)csbnode->getChildByName("smallbg");
	smallbg->setSwallowTouches(true);

	cocos2d::ui::ImageView* resbox_qu = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox_qu");

	int s = GlobalInstance::map_GF[gfid].qu;
	std::string qustr = StringUtils::format("ui/resbox_qu%d.png", s);

	CommonFuncs::playResBoxEffect(resbox_qu, s);
	resbox_qu->loadTexture(qustr, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string str = GlobalInstance::getInstance()->getResUIFrameName(gfid, s);

	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setTextColor(Color4B(POTENTIALCOLOR[s]));

	cocos2d::ui::Text* quatext = (cocos2d::ui::Text*)csbnode->getChildByName("quatext");
	std::string st = StringUtils::format("potential_%d", s);
	str = ResourceLang::map_lang[st];
	quatext->setString(str);
	quatext->setTextColor(Color4B(POTENTIALCOLOR[s]));

	Node* contentnode = csbnode->getChildByName("contentnode");

	for (int i = 0; i <= 5; i++)
	{
		str = StringUtils::format("attrtext_%d",i);
		attrlblArr[i] = (cocos2d::ui::Text*)contentnode->getChildByName(str);
	}

	getGfAttr(gfid);

	cocos2d::ui::Text* jobtext = (cocos2d::ui::Text*)contentnode->getChildByName("attrtext_6");

	std::string jobstr;
	for (unsigned int i = 0; i < GlobalInstance::map_GF[gfid].vec_skillbns.size(); i++)
	{
		int m = GlobalInstance::map_GF[gfid].vec_skillbns[i];
		if (m == 1)
		{
			if (jobstr.length() > 0)
				jobstr.append(ResourceLang::map_lang["comma"]); 
			jobstr.append(GlobalInstance::map_AllResources[GlobalInstance::vec_herosAttr[i].id].name);
		}
	}
	jobtext->setString(jobstr);

	cocos2d::ui::Text* vocationtextlbl = (cocos2d::ui::Text*)contentnode->getChildByName("vocationtext");
	vocationtextlbl->setString(ResourceLang::map_lang["fitjob"]);

	cocos2d::ui::Text* skilltextlbl = (cocos2d::ui::Text*)contentnode->getChildByName("skilltext");
	skilltextlbl->setString(ResourceLang::map_lang["skilltext"]);
		
	int skilltype = GlobalInstance::map_GF[gfid].skill;
	std::string skillids = StringUtils::format("sk%03d", skilltype);

	cocos2d::ui::Text* skillnamelbl = (cocos2d::ui::Text*)contentnode->getChildByName("skillname");
	std::string skillname = StringUtils::format(ResourceLang::map_lang["skillname"].c_str(), GlobalInstance::map_AllResources[skillids].name.c_str());
	skillnamelbl->setString(skillname);

	cocos2d::ui::Text* skilldesclbl = (cocos2d::ui::Text*)contentnode->getChildByName("skilldesc");
	Label* dlbl = (Label*)skilldesclbl->getVirtualRenderer();
	dlbl->setLineSpacing(5);

	std::string allskilldesc = StringUtils::format(ResourceLang::map_lang["gfkilldesc"].c_str(), GlobalInstance::map_GF[gfid].skillrnd);
		
	std::string skilldesc = GlobalInstance::map_AllResources[skillids].desc;
	if (skilltype == SKILL_1 || skilltype == SKILL_13 || skilltype == SKILL_15 || skilltype == SKILL_20)
		skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gfid].skilleff1);
	else if (skilltype == SKILL_2)
		skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gfid].skilleff1/100);
	else if (skilltype == SKILL_3 || skilltype == SKILL_4)
		skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gfid].skilleff2 - 1);
	else if (skilltype == SKILL_5 || skilltype == SKILL_6)
		skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gfid].skilleff2, GlobalInstance::map_GF[gfid].skilleff1);
	else if (skilltype == SKILL_7)
		skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gfid].skilleff1);
	else if (skilltype == SKILL_8)
		skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gfid].skilleff2, GlobalInstance::map_GF[gfid].skilleff1);
	else if (skilltype == SKILL_9 || skilltype == SKILL_10 || skilltype == SKILL_11 || skilltype == SKILL_12 || skilltype == SKILL_14 || skilltype == SKILL_16 || skilltype == SKILL_18)
		skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gfid].skilleff1, GlobalInstance::map_GF[gfid].skilleff2 - 1);
	else if (skilltype == SKILL_17)
		skilldesc = StringUtils::format(GlobalInstance::map_AllResources[skillids].desc.c_str(), GlobalInstance::map_GF[gfid].skilleff1, GlobalInstance::map_GF[gfid].skilleff1, GlobalInstance::map_GF[gfid].skilleff2 - 1);
		
	allskilldesc.append(skilldesc);
	skilldesclbl->setString(allskilldesc);

	cocos2d::ui::Text* desctext = (cocos2d::ui::Text*)contentnode->getChildByName("desctext");
	Label* desclbl = (Label*)desctext->getVirtualRenderer();
	desclbl->setLineSpacing(5);
	desctext->setString(GlobalInstance::map_AllResources[gfid].desc);
	
	cocos2d::ui::Text* statustext = (cocos2d::ui::Text*)contentnode->getChildByName("statustext");
	statustext->setString(ResourceLang::map_lang["libraryinfostatus"]);

	cocos2d::ui::Text* status = (cocos2d::ui::Text*)contentnode->getChildByName("status");
	if (MyRes::getEquipableCount(gfid) <= 0 && MyRes::getEquipableCount(gfid, MYEQUIP) <= 0)
	{
		status->setString(ResourceLang::map_lang["libraryinfostatus0"]);
	}
	else if (MyRes::getEquipableCount(gfid, MYEQUIP) > 0)
	{
		status->setString(ResourceLang::map_lang["libraryinfostatus1"]);
		status->setTextColor(Color4B(0, 255, 0, 255));
	}
	else
	{
		status->setString(ResourceLang::map_lang["libraryinfostatus2"]);
		status->setTextColor(Color4B(0, 255, 0, 255));
	}

	cocos2d::ui::Text* getwheretitle = (cocos2d::ui::Text*)contentnode->getChildByName("getwheretitle");
	getwheretitle->setString(ResourceLang::map_lang["libraryinfogetwhere"]);

	cocos2d::ui::Text* getwheretext = (cocos2d::ui::Text*)contentnode->getChildByName("getwheretext");
	Label* glbl = (Label*)getwheretext->getVirtualRenderer();
	glbl->setLineSpacing(5);

	bool isfind = false;
	std::string wheretext;
	std::string taskname;
	//是否在主线任务中获得
	std::vector<TaskData>::iterator tit;
	
	for (tit = GlobalInstance::vec_TaskMain.begin(); tit != GlobalInstance::vec_TaskMain.end(); tit++)
	{
		isfind = false;
		for (unsigned int i = 0; i < (*tit).reward1.size(); i++)
		{
			std::vector<std::string> one_res = (*tit).reward1[i];
			std::string resid = one_res[0];
			if (resid.compare(gfid) == 0)
			{
				if (taskname.length() > 0)
					taskname.append(ResourceLang::map_lang["ortext"]);
				std::string tname = StringUtils::format(ResourceLang::map_lang["resmname"].c_str(), (*tit).name.c_str());
				taskname.append(tname);
				isfind = true;
				break;
			}
		}
		if (isfind)
		{
			continue;
		}
		else
		{
			for (unsigned int i = 0; i < (*tit).reward2.size(); i++)
			{
				std::vector<std::string> one_res = (*tit).reward2[i];
				std::string resid = one_res[0];
				if (resid.compare(gfid) == 0)
				{
					if (taskname.length() > 0)
						taskname.append(ResourceLang::map_lang["ortext"]);
					std::string tname = StringUtils::format(ResourceLang::map_lang["resmname"].c_str(), (*tit).name.c_str());
					taskname.append(tname);
					isfind = true;
					break;
				}
			}
		}
	}
	//是否在战胜BOSS中获得
	if (taskname.length() > 0)
	{
		wheretext = StringUtils::format(ResourceLang::map_lang["libraryinfogetwhere0"].c_str(), taskname.c_str());
		wheretext.append(ResourceLang::map_lang["dunhao"]);
	}

	std::string mapwherestr;
	std::map<std::string, S_MainMap>::iterator mit;
	std::map<std::string, S_SubMap>::iterator it;
	for (mit = GlobalInstance::map_mapsdata.begin(); mit != GlobalInstance::map_mapsdata.end(); mit++)
	{
		std::string mainmapid = mit->first;
		for (it = GlobalInstance::map_mapsdata[mainmapid].map_sublist.begin(); it != GlobalInstance::map_mapsdata[mainmapid].map_sublist.end(); it++)
		{
			int awdsize = GlobalInstance::map_mapsdata[mainmapid].map_sublist[it->first].vec_awd.size();
			for (int i = 0; i < awdsize; i++)
			{
				std::string resid = GlobalInstance::map_mapsdata[mainmapid].map_sublist[it->first].vec_awd[i];
				if (resid.compare(gfid) == 0)
				{
					if (mapwherestr.length() > 0)
						mapwherestr.append(ResourceLang::map_lang["ortext"]);
					std::string npcname = "NPC";

					std::vector<TaskData>::iterator tit;

					for (tit = GlobalInstance::vec_TaskMain.begin(); tit != GlobalInstance::vec_TaskMain.end(); tit++)
					{
						if ((*tit).place.compare(it->first) == 0)
						{
							npcname = GlobalInstance::map_AllResources[(*tit).npcid].name;
							break;
						}
					}
					std::string mstr = StringUtils::format(ResourceLang::map_lang["libraryinfowhere"].c_str(), GlobalInstance::map_AllResources[it->first].name.c_str(), npcname.c_str());
					mapwherestr.append(mstr);
				}
			}
		}
	}
	if (mapwherestr.length() > 0)
	{
		mapwherestr = StringUtils::format(ResourceLang::map_lang["libraryinfogetwhere1"].c_str(), mapwherestr.c_str());
		wheretext.append(mapwherestr);
		wheretext.append(ResourceLang::map_lang["dunhao"]);
	}
	wheretext.append(ResourceLang::map_lang["libraryinfogetwhere2"]);
	getwheretext->setString(wheretext);
	//屏蔽下层点击
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

void GfLibraryInfoLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

void GfLibraryInfoLayer::getGfAttr(std::string gfid)
{
	std::vector<float> vec_attrval;

	float hp = GlobalInstance::map_GF[gfid].vec_hp[0];
	float atk = GlobalInstance::map_GF[gfid].vec_atk[0];
	float df = GlobalInstance::map_GF[gfid].vec_df[0];
	float atkspeed = GlobalInstance::map_GF[gfid].vec_speed[0];
	float crit = GlobalInstance::map_GF[gfid].vec_crit[0];
	float dodge = GlobalInstance::map_GF[gfid].vec_avoid[0];

	vec_attrval.push_back(hp);
	vec_attrval.push_back(atk);
	vec_attrval.push_back(df);
	vec_attrval.push_back(atkspeed);
	vec_attrval.push_back(crit);
	vec_attrval.push_back(dodge);

	
	for (int i = 0; i <= 5; i++)
	{
		std::string str = StringUtils::format("addattrtext_%d", i);
		str = StringUtils::format(ResourceLang::map_lang[str].c_str(), vec_attrval[i]);
		attrlblArr[i]->setString(str);
	}

	std::string namestr = GlobalInstance::map_AllResources[gfid].name;
	namelbl->setString(namestr);
}

void GfLibraryInfoLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;
		int tag = node->getTag();
		if (tag == 1000)
		{
			AnimationEffect::closeAniEffect(this);
		}
	}
}

void GfLibraryInfoLayer::onExit()
{
	Layer::onExit();
}