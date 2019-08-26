#include "NpcLayer.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "MovingLabel.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "Const.h"
#include "DataSave.h"
#include "MapBlockScene.h"
#include "RewardLayer.h"
#include "NpcgiveLayer.h"

USING_NS_CC;

NpcLayer::NpcLayer()
{
	isShowWord = false;
	m_wordindex = -1;
	m_wordcount = 0;
	m_wordlbl = NULL;
	btntag = -1;
}

NpcLayer::~NpcLayer()
{
	if (g_MapBlockScene != NULL)
		g_MapBlockScene->isRoutingBreakOff = false;
}


NpcLayer* NpcLayer::create(std::string npcid, std::vector<Npc*> vec_enemys)
{
	NpcLayer *pRet = new(std::nothrow)NpcLayer();
	if (pRet && pRet->init(npcid, vec_enemys))
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
bool NpcLayer::init(std::string npcid, std::vector<Npc*> vec_enemys)
{
	if (!Layer::init())
	{
		return false;
	}

	m_npcid = npcid;
	m_vec_enemys = vec_enemys;
	for (unsigned int n = 0; n < GlobalInstance::map_npcrelation[npcid].talk.size(); n++)
	{
		m_wordslist.push_back(GlobalInstance::map_npcrelation[npcid].talk[n]);
		m_wordslist.push_back(GlobalInstance::map_npcrelation[npcid].mytalk[n]);
	}
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color, 0, "colorLayer");

	csbnode = CSLoader::createNode(ResourcePath::makePath("npcLayer.csb"));
	this->addChild(csbnode);
	langtype = GlobalInstance::getInstance()->getLang();

	friendper = GlobalInstance::map_npcrelation[npcid].friendneed;
	masterper = GlobalInstance::map_npcrelation[npcid].masterneed;
	marryper = GlobalInstance::map_npcrelation[npcid].marryneed;

	npctalkframe = (Sprite*)csbnode->getChildByName("npctalkframe");

	//npc
	npc = (cocos2d::ui::ImageView*)csbnode->getChildByName("npc");
	std::string str = StringUtils::format("images/%s.png", GlobalInstance::map_Npcs[npcid].icon.c_str());
	npc->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);
	npc->ignoreContentAdaptWithSize(true);
	//hero
	hero = (cocos2d::ui::ImageView*)csbnode->getChildByName("hero");
	str = StringUtils::format("images/hfull_%d_0.png", DataSave::getInstance()->getHeadId());
	hero->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);
	hero->ignoreContentAdaptWithSize(true);
	hero->setVisible(false);

	hero->setOpacity(150);
	npc->setOpacity(255);
	//npcname
	cocos2d::ui::Text* npcname = (cocos2d::ui::Text*)csbnode->getChildByName("npcname");
	npcname->setString(GlobalInstance::map_AllResources[npcid].name);
	//npcdesc
	cocos2d::ui::Text* npcdesc = (cocos2d::ui::Text*)csbnode->getChildByName("npcdesc");
	npcdesc->setString(GlobalInstance::map_npcrelation[npcid].desc);

	//relationship
	relationship = (cocos2d::ui::Text*)csbnode->getChildByName("relationship");

	friendlycount = (cocos2d::ui::Text*)csbnode->getChildByName("friendlynum");

	cocos2d::ui::Text* text0 = (cocos2d::ui::Text*)csbnode->getChildByName("text0");
	text0->setString(ResourceLang::map_lang["npcfriendlytext"]);

	cocos2d::ui::Text* text1 = (cocos2d::ui::Text*)csbnode->getChildByName("text1");
	text1->setString(ResourceLang::map_lang["npcrelationtext"]);

	cocos2d::ui::Text* hinttext = (cocos2d::ui::Text*)csbnode->getChildByName("hinttext");
	hinttext->setString(ResourceLang::map_lang["npcgivehint"]);

	//好感度进度条
	loadFriendlyPro();

	std::string btnstr[6] = { "talkbtn","givebtn","friendbtn","masterbtn","marrybtn","fightbtn" };
	for (int i = 0; i < 6; i++)
	{
		cocos2d::ui::ImageView* npcbtn = (cocos2d::ui::ImageView*)csbnode->getChildByName(btnstr[i]);
		npcbtn->setTag(i);
		npcbtn->addTouchEventListener(CC_CALLBACK_2(NpcLayer::onBtnClick, this));
		btnArr[i] = npcbtn;
		cocos2d::ui::ImageView* npcbtntxt = (cocos2d::ui::ImageView*)npcbtn->getChildByName("imagetext");
		str = StringUtils::format("npctext_%d", i);
		npcbtntxt->loadTexture(ResourcePath::makeTextImgPath(str, langtype), cocos2d::ui::Widget::TextureResType::PLIST);
		if (i == 2)
		{
			bool isFriend = false;
			for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[npcid].relation.size(); i++)
			{
				if (GlobalInstance::map_myfriendly[npcid].relation[i] == NPC_FRIEND)
				{
					isFriend = true;
					break;
				}
			}
			if (isFriend)
			{
				npcbtn->loadTexture("ui/npc_friend1.png", cocos2d::ui::Widget::TextureResType::PLIST);
				npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_2_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
		}
		else if (i == 3)
		{
			bool isFriend = false;
			for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[npcid].relation.size(); i++)
			{
				if (GlobalInstance::map_myfriendly[npcid].relation[i] == NPC_MASTER)
				{
					isFriend = true;
					break;
				}
			}
			if (isFriend)
			{
				npcbtn->loadTexture("ui/npc_master1.png", cocos2d::ui::Widget::TextureResType::PLIST);
				npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_3_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
		}
		else if (i == 4)
		{
			bool isFriend = false;
			for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[npcid].relation.size(); i++)
			{
				if (GlobalInstance::map_myfriendly[npcid].relation[i] == NPC_COUPEL)
				{
					isFriend = true;
					break;
				}
			}
			if (isFriend)
			{
				npcbtn->loadTexture("ui/npc_marry1.png", cocos2d::ui::Widget::TextureResType::PLIST);
				npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_4_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			}
		}
	}

	//关闭
	closebtn = (cocos2d::ui::Button*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(NpcLayer::onBtnClick, this));
	//按钮文字
	cocos2d::ui::ImageView* closebtntext = (cocos2d::ui::ImageView*)closebtn->getChildByName("imagetext");
	closebtntext->loadTexture(ResourcePath::makeTextImgPath("backbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	scrollView = (cocos2d::ui::ScrollView*)csbnode->getChildByName("scrollView");
	scrollView->setScrollBarEnabled(false);
	scrollView->setBounceEnabled(true);

	if (GlobalInstance::eventstartmappos > -1 || GlobalInstance::map_AllResources[npcid].desc.length() <= 0)
	{
		text0->setVisible(false);
		for (int i = 0; i < 5; i++)
		{
			std::string namestr = StringUtils::format("friendly_bg_%d", i);
			csbnode->getChildByName(namestr)->setVisible(false);

			namestr = StringUtils::format("friendlybar_%d", i);
			csbnode->getChildByName(namestr)->setVisible(false);
			friendlycount->setVisible(false);
			text1->setVisible(false);
			relationship->setVisible(false);
			hinttext->setVisible(false);

			std::string btnstr[6] = { "talkbtn","givebtn","friendbtn","masterbtn","marrybtn","fightbtn" };
			for (int i = 0; i < 6; i++)
			{
				cocos2d::ui::ImageView* npcbtn = (cocos2d::ui::ImageView*)csbnode->getChildByName(btnstr[i]);
				if (i == 0)
				{
					npcbtn->setPositionX(250);
				}
				else if (i == 5)
				{
					npcbtn->setPositionX(480);
				}	
				else
				{
					npcbtn->setVisible(false);
				}
			}
		}
	}

	//屏蔽下层点击
	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		/*if (isShowWord)
		{
			if (m_wordslist.size() <= 0)
				return;

			if (m_wordslist[m_wordindex].length() > 0)
			{
				showFastWords();
				return;
			}
		}
		m_wordindex++;

		int size = m_wordslist.size();

		if (m_wordindex >= size)
		{
			return;
		}
		else
		{
			checkWordLblColor(m_wordslist[m_wordindex]);
		}*/
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	return true;
}

void NpcLayer::loadFriendlyPro()
{
	int friendly = 0;
	if (GlobalInstance::map_myfriendly.find(m_npcid) != GlobalInstance::map_myfriendly.end())
	{
		friendly = GlobalInstance::map_myfriendly[m_npcid].friendly;
	}

	int maxfriendly = GlobalInstance::map_npcrelation[m_npcid].friendmax;
	if (friendly > maxfriendly)
		friendly = maxfriendly;
	else if (friendly < -maxfriendly)
		friendly = -maxfriendly;

	int per = maxfriendly / 5;
	int count = abs(friendly / per);
	for (int m = 0; m < 5; m++)
	{
		std::string barstr = StringUtils::format("friendlybar_%d", m);
		cocos2d::ui::LoadingBar* friendbar = (cocos2d::ui::LoadingBar*)csbnode->getChildByName(barstr);
		friendbar->setPercent(0);
		if (friendly < 0)
		{
			friendbar->loadTexture("ui/friendly_bar_1.png", cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			friendbar->loadTexture("ui/friendly_bar_0.png", cocos2d::ui::Widget::TextureResType::PLIST);
		}
		if (m < count)
		{
			friendbar->setPercent(100);
		}
		else if (m == count)
		{
			float percent = fabs(friendly % per * 100.0f / per);
			friendbar->setPercent(percent);
		}
	}

	std::string relationstr;
	for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[m_npcid].relation.size(); i++)
	{
		std::string st = StringUtils::format("npcrelation_%d", GlobalInstance::map_myfriendly[m_npcid].relation[i]);
		relationstr.append(ResourceLang::map_lang[st]);
		relationstr.append(",");
	}
	if (relationstr.length() > 0)
	{
		relationship->setString(relationstr.substr(0, relationstr.length() - 1));
	}
	else
	{
		relationship->setString(ResourceLang::map_lang["npcrelation_0"]);
	}

	if (GlobalInstance::map_myfriendly[m_npcid].friendly < 0)
	{
		friendlycount->setTextColor(Color4B(255, 0, 0, 255));
	}
	else
	{
		friendlycount->setTextColor(Color4B(0, 128, 0, 255));
	}
	std::string friendlycountstr = StringUtils::format("%d", GlobalInstance::map_myfriendly[m_npcid].friendly);
	friendlycount->setString(friendlycountstr);

	GlobalInstance::getInstance()->saveNpcFriendly();
}

void NpcLayer::checkWordLblColor(std::string wordstr)
{
	if (isScheduled(schedule_selector(NpcLayer::showTypeText)))
	{
		unschedule(schedule_selector(NpcLayer::showTypeText));
	}
	if (m_wordlbl == NULL)
	{
		m_wordlbl = Label::createWithTTF(wordstr, FONT_NAME, 20);
		m_wordlbl->setLineBreakWithoutSpace(true);
		m_wordlbl->setAnchorPoint(Vec2(0, 1));
		m_wordlbl->setMaxLineWidth(scrollView->getContentSize().width);
		scrollView->addChild(m_wordlbl, 0, "talklbl");
	}
	else
	{
		m_wordcount = 0;
		m_wordlbl->unschedule("schedule_typecallback");
		m_wordlbl->setString(wordstr);
	}
	int innerheight = m_wordlbl->getStringNumLines() * 20;//contentlbl->getHeight();
	int contentheight = scrollView->getContentSize().height;
	if (innerheight < contentheight)
		innerheight = contentheight;
	scrollView->setInnerContainerSize(Size(scrollView->getContentSize().width, innerheight));
	m_wordlbl->setPosition(Vec2(0, innerheight));

	int index = 0;
	while (m_wordlbl->getLetter(index) != NULL)
	{
		m_wordlbl->getLetter(index)->setColor(Color3B::WHITE);
		m_wordlbl->getLetter(index)->setScale(0);
		index++;
	}

	this->scheduleOnce(schedule_selector(NpcLayer::showTypeText), 0.1f);
}

void NpcLayer::showTypeText(float dt)
{
	if (btntag == 0)
	{
		if (m_wordindex % 2 == 0)
		{
			hero->setOpacity(150);
			npc->setOpacity(255);
			npctalkframe->setFlippedX(false);
		}
		else
		{
			hero->setVisible(true);
			hero->setOpacity(255);
			npc->setOpacity(150);
			npctalkframe->setFlippedX(true);
		}
	}
	else
	{
		hero->setVisible(false);
		hero->setOpacity(150);
		npc->setOpacity(255);
		npctalkframe->setFlippedX(false);
	}
	
	m_wordlbl->schedule([&](float dt) {
		isShowWord = true;
		m_wordlbl->getLetter(m_wordcount)->setScale(1.0f);
		m_wordcount += 1;
		std::u32string utf32String;
		StringUtils::UTF8ToUTF32(m_wordlbl->getString(), utf32String);
		int len = utf32String.length();
		if (m_wordcount % 64 == 0)
		{
			scrollView->jumpToPercentVertical(scrollView->getScrolledPercentVertical() + 100 / ceil(len / 64));
		}
		if (m_wordcount >= len)
		{
			isShowWord = false;
			m_wordcount = 0;
			m_wordlbl->unschedule("schedule_typecallback");
		}

	}, 0.1f, "schedule_typecallback");
}

void NpcLayer::showFastWords()
{
	if (m_wordlbl != NULL)
	{
		isShowWord = false;
		m_wordcount = 0;
		m_wordlbl->unschedule("schedule_typecallback");
		int index = 0;
		while (m_wordlbl->getLetter(index) != NULL)
		{
			m_wordlbl->getLetter(index)->setScale(1);
			index++;
		}
	}

}

void NpcLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* clicknode = (Node*)pSender;
		int tag = clicknode->getTag();
		btntag = tag;

		int myfriendly = GlobalInstance::map_myfriendly[m_npcid].friendly;

		switch (tag)
		{
		case 1000://关闭
			AnimationEffect::closeAniEffect((Layer*)this);
			break;
		case 0://对话
		{
			if (isShowWord)
			{
				if (m_wordslist.size() <= 0)
					return;

				if (m_wordslist[m_wordindex].length() > 0)
				{
					showFastWords();
					return;
				}
			}
			m_wordindex++;

			int size = m_wordslist.size();

			if (m_wordindex >= size)
			{
				return;
			}
			else
			{
				if (m_wordslist[m_wordindex].length() > 0)
				{
					checkWordLblColor(m_wordslist[m_wordindex]);
				}
			}
			break;
		}
		case 1://赠送
		{
			NpcgiveLayer* layer = NpcgiveLayer::create(m_npcid);
			this->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
			break;
		}
		case 2://结交
		{
			bool isFriend = false;
			for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[m_npcid].relation.size(); i++)
			{
				if (GlobalInstance::map_myfriendly[m_npcid].relation[i] == NPC_FRIEND)
				{
					isFriend = true;
					break;
				}
			}
			if (!isFriend)
			{
				if (myfriendly >= friendper)
				{
					if (checkNpcRelation(NPC_MASTER))
					{
						//m_wordlbl->setString(ResourceLang::map_lang["npcrelationfail1"]);
						checkWordLblColor(ResourceLang::map_lang["npcrelationfail1"]); 
					}
					else if (checkMutexNpc())
					{
						mutexNpcBreak();
					}
					else
					{
						CommonFuncs::playCommonLvUpAnim(this, "npctext_jjcg");

						GlobalInstance::map_myfriendly[m_npcid].relation.push_back(NPC_FRIEND);
						checkWordLblColor(GlobalInstance::map_npcrelation[m_npcid].friendword);

						std::vector<MSGAWDSDATA> vec_rewards;
						for (unsigned int i = 0; i < GlobalInstance::map_npcrelation[m_npcid].reward.size(); i++)
						{
							std::vector<std::string> one_res = GlobalInstance::map_npcrelation[m_npcid].reward[i];
							std::string resid = one_res[0];
							int count = atoi(one_res[1].c_str());
							int qu = atoi(one_res[2].c_str());

							MSGAWDSDATA wdata;
							wdata.rid = resid;
							wdata.count = count;
							wdata.qu = qu;
							vec_rewards.push_back(wdata);
						}
						if (vec_rewards.size() > 0)
						{
							RewardLayer* layer = RewardLayer::create(vec_rewards);
							this->addChild(layer);
							AnimationEffect::openAniEffect((Layer*)layer);
						}

						btnArr[2]->loadTexture("ui/npc_friend1.png", cocos2d::ui::Widget::TextureResType::PLIST);
						cocos2d::ui::ImageView* npcbtntxt = (cocos2d::ui::ImageView*)btnArr[2]->getChildByName("imagetext");
						npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_2_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
					}
				}
				else
				{
					//m_wordlbl->setString(ResourceLang::map_lang["npcrelationfail"]);
					checkWordLblColor(ResourceLang::map_lang["npcrelationfail"]);
				}
			}
			else//绝交
			{
				btnArr[2]->loadTexture("ui/npc_friend0.png", cocos2d::ui::Widget::TextureResType::PLIST);
				cocos2d::ui::ImageView* npcbtntxt = (cocos2d::ui::ImageView*)btnArr[2]->getChildByName("imagetext");
				npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_2", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

				std::vector<int>::iterator it;
				for (it = GlobalInstance::map_myfriendly[m_npcid].relation.begin(); it != GlobalInstance::map_myfriendly[m_npcid].relation.end(); it++)
				{
					if (*it == NPC_FRIEND)
					{
						GlobalInstance::map_myfriendly[m_npcid].relation.erase(it);
						break;
					}
				}

				GlobalInstance::map_myfriendly[m_npcid].friendly -= GlobalInstance::map_npcrelation[m_npcid].friendmax*0.1f;
				checkWordLblColor(ResourceLang::map_lang["npcfriendbreak"]);
			}
			break;
		}
		case 3://拜师
		{
			bool isMaster = false;
			for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[m_npcid].relation.size(); i++)
			{
				if (GlobalInstance::map_myfriendly[m_npcid].relation[i] == NPC_MASTER)
				{
					isMaster = true;
					break;
				}
			}
			if (!isMaster)
			{
				if (myfriendly >= masterper)
				{
					if (checkOtherMater().length() > 0)
					{
						std::string npcstr = StringUtils::format(ResourceLang::map_lang["npcmasterfail2"].c_str(), GlobalInstance::map_AllResources[checkOtherMater()].name.c_str());
						checkWordLblColor(npcstr);
					}
					else if (checkNpcRelation(NPC_FRIEND))
					{
						checkWordLblColor(ResourceLang::map_lang["npcmasterfail3"]);
					}
					else if (checkMutexNpc())
					{
						mutexNpcBreak();
					}
					else
					{
						CommonFuncs::playCommonLvUpAnim(this, "npctext_bscg");

						GlobalInstance::map_myfriendly[m_npcid].relation.push_back(NPC_MASTER);
						checkWordLblColor(GlobalInstance::map_npcrelation[m_npcid].masterword);

						btnArr[3]->loadTexture("ui/npc_master1.png", cocos2d::ui::Widget::TextureResType::PLIST);
						cocos2d::ui::ImageView* npcbtntxt = (cocos2d::ui::ImageView*)btnArr[3]->getChildByName("imagetext");
						npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_3_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
					}
				}
				else
				{
					checkWordLblColor(ResourceLang::map_lang["npcmasterfail"]);
				}
			}
			else//出师
			{
				checkWordLblColor(ResourceLang::map_lang["npcmasterfinish"]);

				//不能点
				for (int i = 0; i < 6; i++)
				{
					btnArr[i]->setEnabled(false);
				}
				closebtn->setEnabled(false);
				this->schedule(schedule_selector(NpcLayer::checkEnterFight), 1.0f);

				GlobalInstance::npcmasterfinish = 1;//出师战斗

			}
			break;
		}
		case 4://结亲
		{
			bool isMarry = false;
			for (unsigned int i = 0; i < GlobalInstance::map_myfriendly[m_npcid].relation.size(); i++)
			{
				if (GlobalInstance::map_myfriendly[m_npcid].relation[i] == NPC_COUPEL)
				{
					isMarry = true;
					break;
				}
			}
			if (!isMarry)
			{
				if (myfriendly >= marryper)
				{
					if (checkOtherLover())
					{
						checkWordLblColor(ResourceLang::map_lang["npcotherlovetips"]);
					}
					else if (checkMutexNpc())
					{
						std::string restr;
						std::map<std::string, NpcFriendly>::iterator it;
						for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); it++)
						{
							std::string nid = it->first;
							if (it->second.relation.size() > 0)
							{
								for (unsigned int i = 0; i < GlobalInstance::map_npcrelation[m_npcid].enemynpc.size(); i++)
								{
									std::string pid = GlobalInstance::map_npcrelation[m_npcid].enemynpc[i];
									if (pid.compare(nid) == 0)
									{
										std::string m_str;
										for (unsigned m = 0; m < it->second.relation.size(); m++)
										{
											std::string r = StringUtils::format("npcrelation_%d", it->second.relation[m]);
											m_str.append(ResourceLang::map_lang[r]);
											m_str.append(",");
										}
										std::string s = StringUtils::format(ResourceLang::map_lang["npcmutexrelation2"].c_str(), GlobalInstance::map_AllResources[nid].name.c_str(), m_str.substr(0, m_str.length() - 1).c_str());
										restr.append(s);
										restr.append(",");
										it->second.relation.clear();
										it->second.friendly -= it->second.friendly*0.1f;
									}
								}
							}
						}

						GlobalInstance::map_myfriendly[m_npcid].relation.clear();
						GlobalInstance::map_myfriendly[m_npcid].friendly -= GlobalInstance::GlobalInstance::map_npcrelation[m_npcid].friendmax*0.1f;
						std::string pstr = StringUtils::format(ResourceLang::map_lang["npcmutexrelation"].c_str(), restr.c_str());
						checkWordLblColor(pstr);
					}
					else
					{
						CommonFuncs::playCommonLvUpAnim(this, "npctext_jqcg");

						GlobalInstance::map_myfriendly[m_npcid].relation.push_back(NPC_COUPEL);
						checkWordLblColor(GlobalInstance::map_npcrelation[m_npcid].marryword);

						btnArr[4]->loadTexture("ui/npc_marry1.png", cocos2d::ui::Widget::TextureResType::PLIST);
						cocos2d::ui::ImageView* npcbtntxt = (cocos2d::ui::ImageView*)btnArr[4]->getChildByName("imagetext");
						npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_4_1", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
					}
				}
				else
				{
					checkWordLblColor(ResourceLang::map_lang["npccoupelfail"]);
				}
			}
			else
			{
				btnArr[4]->loadTexture("ui/npc_marry.png", cocos2d::ui::Widget::TextureResType::PLIST);
				cocos2d::ui::ImageView* npcbtntxt = (cocos2d::ui::ImageView*)btnArr[4]->getChildByName("imagetext");
				npcbtntxt->loadTexture(ResourcePath::makeTextImgPath("npctext_4", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

				std::vector<int>::iterator it;
				for (it = GlobalInstance::map_myfriendly[m_npcid].relation.begin(); it != GlobalInstance::map_myfriendly[m_npcid].relation.end(); it++)
				{
					if (*it == NPC_COUPEL)
					{
						GlobalInstance::map_myfriendly[m_npcid].relation.erase(it);
						break;
					}
				}

				GlobalInstance::map_myfriendly[m_npcid].friendly -= GlobalInstance::map_npcrelation[m_npcid].friendmax*0.1f;

				checkWordLblColor(ResourceLang::map_lang["npccoupelbreak"]);
			}
			break;
		}
		case 5:
		{

			if (GlobalInstance::eventstartmappos < 0 && GlobalInstance::map_AllResources[m_npcid].desc.length() > 0)
			{
				if (GlobalInstance::map_myfriendly[m_npcid].friendly <= -GlobalInstance::map_npcrelation[m_npcid].friendmax)
				{
					MovingLabel::show(ResourceLang::map_lang["nofirendlyfight"]);
					return;
				}

				GlobalInstance::map_myfriendly[m_npcid].friendly -= GlobalInstance::map_npcrelation[m_npcid].fightcost;
				if (GlobalInstance::map_myfriendly[m_npcid].relation.size() > 0)
				{
					std::vector<int>::iterator it;
					for (it = GlobalInstance::map_myfriendly[m_npcid].relation.begin(); it != GlobalInstance::map_myfriendly[m_npcid].relation.end();)
					{
						if ((*it == NPC_FRIEND && GlobalInstance::map_myfriendly[m_npcid].friendly < GlobalInstance::map_npcrelation[m_npcid].friendneed)
							|| (*it == NPC_MASTER && GlobalInstance::map_myfriendly[m_npcid].friendly < GlobalInstance::map_npcrelation[m_npcid].masterneed)
							|| (*it == NPC_COUPEL && GlobalInstance::map_myfriendly[m_npcid].friendly < GlobalInstance::map_npcrelation[m_npcid].marryneed))
						{
							it = GlobalInstance::map_myfriendly[m_npcid].relation.erase(it);
						}
						else
							it++;
					}
				}

				GlobalInstance::getInstance()->saveNpcFriendly();
			}
			if (g_MapBlockScene != NULL)
			{
				g_MapBlockScene->showFightingLayer(m_vec_enemys);
			}

			this->removeFromParentAndCleanup(true);
			break;
		}
		default:
			break; 
		}

		if (tag != 5)
		{
			loadFriendlyPro();
		}
	}
}

void NpcLayer::mutexNpcBreak()
{
	std::string restr;
	std::map<std::string, NpcFriendly>::iterator it;
	for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); it++)
	{
		std::string nid = it->first;
		if (it->second.relation.size() > 0)
		{
			for (unsigned int i = 0; i < GlobalInstance::map_npcrelation[m_npcid].enemynpc.size(); i++)
			{
				std::string pid = GlobalInstance::map_npcrelation[m_npcid].enemynpc[i];
				if (pid.compare(nid) == 0)
				{
					std::string m_str;
					for (unsigned m = 0; m < it->second.relation.size(); m++)
					{
						std::string r = StringUtils::format("npcrelation_%d", it->second.relation[m]);
						m_str.append(ResourceLang::map_lang[r]);
						m_str.append(",");
					}
					std::string s = StringUtils::format(ResourceLang::map_lang["npcmutexrelation2"].c_str(), GlobalInstance::map_AllResources[nid].name.c_str(), m_str.substr(0, m_str.length() - 1).c_str());
					restr.append(s);
					restr.append(",");
					it->second.relation.clear();
					it->second.friendly -= it->second.friendly*0.1f;
				}
			}
		}
	}

	GlobalInstance::map_myfriendly[m_npcid].relation.clear();
	GlobalInstance::map_myfriendly[m_npcid].friendly -= GlobalInstance::map_npcrelation[m_npcid].friendmax*0.1f;

	std::string pstr = StringUtils::format(ResourceLang::map_lang["npcmutexrelation"].c_str(), restr.c_str());
	checkWordLblColor(pstr);

	//不能点
	for (int i = 0; i < 6; i++)
	{
		btnArr[i]->setEnabled(false);
	}
	closebtn->setEnabled(false);
	this->schedule(schedule_selector(NpcLayer::checkEnterFight), 1.0f);
}

bool NpcLayer::checkOtherLover()
{
	std::map<std::string, NpcFriendly>::iterator it;
	for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); it++)
	{
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			int rel = it->second.relation[i];
			if (rel == NPC_COUPEL)
			{
				return true;
			}
		}
	}
	return false;
}

bool NpcLayer::checkMutexNpc()
{
	std::map<std::string, NpcFriendly>::iterator it;
	for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); it++)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < GlobalInstance::map_npcrelation[m_npcid].enemynpc.size(); i++)
		{
			std::string pid = GlobalInstance::map_npcrelation[m_npcid].enemynpc[i];
			if (pid.compare(nid) == 0 && it->second.relation.size() > 0)
			{
				return true;
			}
		}
	}
	return false;
}

bool NpcLayer::checkNpcRelation(int relation)
{
	if (find(GlobalInstance::map_myfriendly[m_npcid].relation.begin(), GlobalInstance::map_myfriendly[m_npcid].relation.end(), relation) != GlobalInstance::map_myfriendly[m_npcid].relation.end())
	{
		return true;
	}
	return false;
}

void NpcLayer::checkEnterFight(float dt)
{
	if (isShowWord)
	{
		return;
	}
	this->unschedule(schedule_selector(NpcLayer::checkEnterFight));
	if (g_MapBlockScene != NULL)
	{
		g_MapBlockScene->showFightingLayer(m_vec_enemys);
	}
	this->removeFromParentAndCleanup(true);
}

std::string NpcLayer::checkOtherMater()
{
	std::map<std::string, NpcFriendly>::iterator it;
	for (it = GlobalInstance::map_myfriendly.begin(); it != GlobalInstance::map_myfriendly.end(); it++)
	{
		std::string nid = it->first;
		for (unsigned int i = 0; i < it->second.relation.size(); i++)
		{
			int rel = it->second.relation[i];
			if (rel == NPC_MASTER)
			{
				return nid;
			}
		}
	}
	return "";
}

void NpcLayer::onExit()
{
	Layer::onExit();
}