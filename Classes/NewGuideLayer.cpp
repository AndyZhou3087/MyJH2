#include "NewGuideLayer.h"
#include "GlobalInstance.h"
#include "DataSave.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "Resource.h"
#include "MapBlockScene.h"
#include "Hero.h"
#include "MyRes.h"
#include "Equipable.h"
#include "FightingLayer.h"
#include "MainScene.h"
#include "HomeHillLayer.h"
#include "InnRoomLayer.h"
#include "RandHeroLayer.h"
#include "HeroAttrLayer.h"
#include "TaskDescLayer.h"
#include "TaskLayer.h"
#include "OutTownLayer.h"
#include "Quest.h"
#include "HospitalLayer.h"
#include "MarketLayer.h"

std::string descText[] = { "小师妹：掌门师兄，六大派掌门和魔教应该就在前面了，咱们快去看看。", //0
"", //1
"小师妹：掌门师兄，传言果然是真的，十四天书现世，六大派和魔教为了争夺天书才打起来。", //2
"魔教人物：小子！现在帮我们对付他们，老子给你黄金万两！",//3
"昆仑派：少侠！魔教为非作歹，祸害江湖，我辈武林人士岂能与他们为伍，少侠快来助我们除掉这些武林败类！",//4
"小师妹：掌门师兄，你可以选择一方进行帮助，都是为了十四天书，出手吧师兄！", //进入战斗界面//5
"小师妹：掌门师兄，战斗过程为全自动，掌门只要根据角色的属性去分配好位置就可以了。",//6
"小师妹：前排的密宗，防御力比较高，适合在前面抗伤害。",//7
"小师妹：后面的刺客攻击力很高，但是很容易受伤，适合在后面进行攻击。",//8
"小师妹：武当的属性比较平衡，什么位置都可以站。", //9
"小师妹：道士会用道术进行多重攻击，还可以治疗队友。",//10
//开始战斗--结束
"小师妹：师兄，战斗结束了，天书现世，世道已经乱了，他们一会恢复过来，咱们就走不掉了，还是快撤吧！", //11
"赏善罚恶使张三：天书现世，世道已经乱了，这些东西不是你该拿的，交出来吧！",//12
"小师妹：师兄，咱们还是先回去吧，连赏善罚恶二使都来了，回去好好想想对策！",//跳回主场景//13
//"",//14
"小师妹：这里虽然不比师傅以前传给师兄的地方，但是好在这里人烟稀少，咱们可以好好经营一下，师兄先给门派起个响亮的名字吧！",//设置
//这里是否需要滑动引导
//后山引导（改为条件引导）
"小师妹：经营门派最重要的还是要先把基础设施完善起来，我们先去后山看看有什么资源吧!",//15
"小师妹：这里后山还真是什么都没有啊，师兄，先升级一下后山，解锁更多的资源吧！",
"",
"小师妹：资源有了，我们该招点工人来干活了。",
"",
"小师妹：工人有了，但是师兄你要分配给他们任务，每个工人采集资源都会消耗一定的食材，记得要保证平衡哦！",
"",

//这里是否需要滑动引导
"小师妹：师兄，客栈里会有形形色色的侠客在里面打尖，我们可以招募一些侠客到咱们的门下，先去客栈看看吧！",
"小师妹：路过的侠客都会在客栈歇脚，掌门可以看看有没有合适的人招募到门下。",
"小师妹：这里有三位英雄，现下正是发展门派之时，掌门师兄可依次招募进来。",//重复招募三次角色
"",
"",
"",
"",
"",
"小师妹：英雄已经招募好了，掌门快去看看三位英雄的属性吧!",//第一次招募必须有侠客
"小师妹：点击英雄头像就可以查看详细属性了，掌门快看看属性吧！",//点击第一个侠客英雄
"小师妹：每个英雄都有不同的武功，搭配合理的武功才可以触发技能，快去给英雄们装备武功吧！",//装备内功
"",
"",
"",
"",
"",
"",//返回
"",//客栈关闭
"小师妹：师兄，闯荡江湖的事就交给师兄了，我在议事厅打探消息，师兄可以随时来议事厅找我。",
"小师妹：师兄你来啦，我刚打探到的新消息，师兄可以看看。",
"",//接受任务
"",
"",
//这里是否需要滑动引导
"小师妹：掌门师兄快去青龙帮瞧一瞧吧，只要得到十四天书，师兄就可以独步武林了！",//出门
"小师妹：最后我再交代一下，在这里师兄可以设置参战角色的位置，快去设置一下吧！",
"",
"",
"",
"",
"",
"小师妹：好了，掌门师兄快去青龙帮吧！",//出发
"",
"",
//第一次探索回到主界面//这里是否需要滑动引导
"小师妹：掌门师兄回来啦！现在完成了任务，去议事厅看看可以领取任务奖励哦！",
"",
"",
"",
"小师妹：完成了一个任务，我们可以进行下一个任务了，随着任务的完成会解锁更多的地图，同时也可以获得更多的奖励哦！",
"",
"",
"小师妹：除了主线任务以外，还有支线和每日任务可以做，根据任务不同获得的奖励也有不同，记得每日都完成任务哦！",

//条件引导
//医馆
"小师妹：有角色在战斗中死亡了，不过没关系，我们去医馆找医生给他治疗就可以复活了。",
"",
"小师妹：角色都已经恢复了，可以重新参加战斗了。",

//市场
"小师妹：角色到了十级要进行转职了，转职需要九转金丹，掌门师兄去市场看看吧！",//66
"小师妹：市场每隔一段时间都会刷新物品数量，一次买空后只能等下次刷新了哦，看到九转金丹了，快买来试一试吧！",
"",
"小师妹：现在有九转金丹了，每个职业都可以转成不同的职业，快去客栈给角色进行转职吧!",
"",
"",
"小师妹：掌门师兄快看，这位侠客可以转职成这两个角色，掌门师兄快选一个吧！",

//铁匠铺//73
"小师妹：掌门师兄回来啦！我们现在有了一定的资源，掌门可以去铁匠铺看看能造些什么东西。",
"小师妹：我们打造一把青铜刀试一下，铁匠铺的等级越高，就会有更高的几率造出更好品质的武器哦。",
"",//制造
"",
//这里是否需要滑动引导
"小师妹：既然造出来的青铜刀，就给英雄装备上吧！",
"",
"",
"",
"",

//接上面后山的条件引导
"",//82
"",
"",
"小师妹：每个角色转职后都会有不同的职业特性，转职后也会有属性的提升，记得一定要帮助侠客们转职哦！",

//回城卷轴
"小师妹：师兄，我们要赶快回到帮派里，刚好这里有茅山派的回城卷轴，师兄快用一下吧！",//86
"",
};

int voc[6] = { 6,4,11,3,8,1 };
std::string gf[6][2] = { { "x014","w014" },{ "x004","w004" },{ "x035","w035" },{ "x016","w016" },{ "x023","w023" },{ "x006","w006" } };
int stenNodesArr[70] = { 72,17,19,75,34,37,25,27,29,32,35,38,71,79,16,18,20,21,64,23,31,78,39,70,53,15,22,69,77,40,55,45,63,66,73,
0,1,11,13,67,68,46,48,50,52,24,26,28,30,33,36,80,47,49,51,54,74,76,81,42,60,43,58,61,57,41,59,44,56,62 };

NewGuideLayer* g_NewGuideLayer = NULL;
NewGuideLayer::NewGuideLayer()
{
	opacity = 0;
	iscannext = false;
	m_colorlayer = NULL;
	m_wordlbl = NULL;
}
NewGuideLayer::~NewGuideLayer()
{
	g_NewGuideLayer = NULL;
}

NewGuideLayer* NewGuideLayer::create(int type, std::vector<Node*> stencilNodes)
{
	NewGuideLayer *pRet = new NewGuideLayer();
	if (pRet && pRet->init(type, stencilNodes))
	{
		pRet->autorelease();
	}
	else
	{
		delete pRet;
		pRet = NULL;
	}
	return pRet;
}

bool NewGuideLayer::init(int step, std::vector<Node*> stencilNodes)
{
	if (!Layer::init())
	{
		return false;
	}
	//layer 点击事件，屏蔽下层事件
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

	m_step = step;

	showNode(stencilNodes);
	showWord(descText[m_step]);

	if ((m_step >= 2 && m_step <= 10) || m_step == 12 || m_step == 14 || m_step == 65)
	{
		iscannext = true;
	}

	bool clickRect = false;
	for (int i = 0; i < sizeof(stenNodesArr) / sizeof(stenNodesArr[0]); i++)
	{
		if (stenNodesArr[i] == m_step)
		{
			clickRect = true;
			break;
		}
	}

	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		Vec2 point = Director::getInstance()->convertToGL(touch->getLocationInView());//获得当前触摸的坐标 
		if (stencilNodes.size() > 0)
		{
			Vec2 vec = stencilNodes[stencilNodes.size() - 1]->getParent()->convertToWorldSpace(stencilNodes[stencilNodes.size() - 1]->getPosition());

			auto rect = Rect(vec.x - stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.width / 2, vec.y - stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.height / 2, stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.width, stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.height);
			if (rect.containsPoint(point))//如果触点处于rect中  
			{
				iscannext = true;
				if (m_step != 11)
				{
					listener->setSwallowTouches(false);
				}
			}
			else
			{
				listener->setSwallowTouches(true);
			}
		}
		else
		{
			listener->setSwallowTouches(true);
		}

		return true;
	};
	listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		if (iscannext)
		{
			if (clickRect)
			{
				Vec2 point = Director::getInstance()->convertToGL(touch->getLocationInView());//获得当前触摸的坐标 
				if (stencilNodes.size() > 0)
				{
					Vec2 vec = stencilNodes[stencilNodes.size() - 1]->getParent()->convertToWorldSpace(stencilNodes[stencilNodes.size() - 1]->getPosition());

					auto rect = Rect(vec.x - stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.width / 2, vec.y - stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.height / 2, stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.width, stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.height);
					if (rect.containsPoint(point))//如果触点处于rect中  
					{
						showNextGuide();
					}
				}
			}
			else
			{
				showNextGuide();
			}
		}
		return;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

	return true;
}

void NewGuideLayer::showNextGuide()
{
	if (m_step != 64)
	{
		DataSave::getInstance()->setIsNewerGuide(m_step, 0);
	}

	if (g_mainScene != NULL)
	{
		g_mainScene->setScrollGliding();
	}

	if (m_step == 0)
	{
		this->removeFromParentAndCleanup(true);
		if (g_MapBlockScene != NULL)
		{
			g_MapBlockScene->delayShowNewerGuide(0);
		}
	}
	else if (m_step > 1 && m_step < 5)// || (m_step >= 11 && m_step < 13))
	{
		this->removeFromParentAndCleanup(true);
		if (g_MapBlockScene != NULL)
		{
			g_MapBlockScene->delayShowNewerGuide(0);
		}
	}
	else if (m_step == 1)
	{
		if (g_MapBlockScene != NULL && !g_MapBlockScene->getIsMoving())
		{
			this->removeFromParentAndCleanup(true);
		}
	}
	else if (m_step == 14 || m_step == 15 || m_step == 16 || m_step == 17 || m_step == 18 || m_step == 22 || m_step == 23
		|| m_step == 24 || m_step == 26 || m_step == 28 || m_step == 31 || m_step == 32 || m_step == 33 || m_step == 35 || m_step == 19
		|| m_step == 36 || m_step == 40 || m_step == 41 || m_step == 45 || m_step == 46 || m_step == 48 || m_step == 50
		|| m_step == 52 || m_step == 53 || m_step == 54 || m_step == 55 || m_step == 56 || m_step == 57 || m_step == 59 || m_step == 63
		|| m_step == 65 || m_step == 66 || m_step == 69 || m_step == 70 || m_step == 71 || m_step == 72 || m_step == 73 || m_step == 74
		|| m_step == 75 || m_step == 77 || m_step == 78 || m_step == 79 || m_step == 80 || m_step == 81  || m_step == 86 || m_step == 87)
	{
		this->removeFromParentAndCleanup(true);
	}
	else if (m_step > 5 && m_step <= 9)
	{
		this->removeFromParentAndCleanup(true);
		if (g_MapBlockScene != NULL)
		{
			FightingLayer* fightlayer = (FightingLayer*)g_MapBlockScene->getChildByName("FightingLayer");
			if (fightlayer != NULL)
			{
				fightlayer->checkNewGuide();
			}
		}
	}
	else if (m_step == 10)
	{
		this->removeFromParentAndCleanup(true);
		if (g_MapBlockScene != NULL)
		{
			FightingLayer* fightlayer = (FightingLayer*)g_MapBlockScene->getChildByName("FightingLayer");
			if (fightlayer != NULL)
			{
				fightlayer->resumeAtkSchedule();
			}
		}
	}
	else if (m_step == 11)
	{
		clearNewGuideData();
		this->removeFromParentAndCleanup(true);
		if (g_MapBlockScene != NULL)
		{
			//g_MapBlockScene->delayShowExit(0);
			g_MapBlockScene->showNewerGuideGoBack();
		}
	}
	else if (m_step == 20 || m_step == 21 || m_step == 82 || m_step == 83 || m_step == 84)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			HomeHillLayer* hill = (HomeHillLayer*)g_mainScene->getChildByName("7homehill");
			if (hill != NULL)
			{
				hill->scheduleOnce(schedule_selector(HomeHillLayer::delayShowNewerGuide), 0.1f);
				//hill->delayShowNewerGuide(0);
			}
		}
	}
	else if (m_step == 39 || m_step == 44 || m_step == 68 || m_step == 76 || m_step == 85)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			g_mainScene->delayShowNewerGuide(0);
		}
	}
	else if (m_step == 25 || m_step == 27 || m_step == 29)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			InnRoomLayer* hill = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
			if (hill != NULL)
			{
				RandHeroLayer* randlayer = (RandHeroLayer*)hill->getChildByName("RandHeroLayer");
				if (randlayer != NULL)
				{
					randlayer->delayShowNewerGuide(0);
				}
			}
		}
	}
	else if (m_step == 30 || m_step == 38)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			InnRoomLayer* hill = (InnRoomLayer*)g_mainScene->getChildByName("6innroom");
			if (hill != NULL)
			{
				hill->delayShowNewerGuide(0);
			}
		}
	}
	else if (m_step == 34 || m_step == 37)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			HeroAttrLayer* randlayer = (HeroAttrLayer*)g_mainScene->getChildByTag(0);
			if (randlayer != NULL)
			{
				randlayer->delayShowNewerGuide(0);
			}
		}
	}
	/*else if (m_step == 42 || m_step == 60)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			TaskDescLayer* layer = (TaskDescLayer*)g_mainScene->getChildByName("TaskDescLayer");
			if (layer != NULL)
			{
				layer->delayShowNewerGuide(0);
			}
		}
	}*/
	else if (m_step == 42 || m_step == 58 || m_step == 60)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			TaskLayer* layer = (TaskLayer*)g_mainScene->getChildByName("9assemblyhall");
			if (layer != NULL)
			{
				layer->delayShowNewerGuide(0);
			}
		}
	}
	else if (m_step == 47 || m_step == 49 || m_step == 51)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			OutTownLayer* layer = (OutTownLayer*)g_mainScene->getChildByName("0outtown");
			if (layer != NULL)
			{
				layer->delayShowNewerGuide(0);
			}
		}
	}
	else if (m_step == 64)
	{
		this->removeFromParentAndCleanup(true);
		//if (g_mainScene != NULL)
		//{
		//	HospitalLayer* layer = (HospitalLayer*)g_mainScene->getChildByName("1hospital");
		//	if (layer != NULL)
		//	{
		//		layer->scheduleOnce(schedule_selector(HospitalLayer::delayShowNewerGuide), newguidetime);
		//		//layer->delayShowNewerGuide(0);
		//	}
		//}
	}
	else if (m_step == 67)
	{
		this->removeFromParentAndCleanup(true);
		if (g_mainScene != NULL)
		{
			MarketLayer* layer = (MarketLayer*)g_mainScene->getChildByName("5market");
			if (layer != NULL)
			{
				layer->delayShowNewerGuide(0);
			}
		}
	}
}

bool NewGuideLayer::checkifNewerGuide(int index)
{
	return DataSave::getInstance()->getIsNewerGuide(index);
}

void NewGuideLayer::setNewerGuide(int index)
{
	DataSave::getInstance()->setIsNewerGuide(index, false);
}

void NewGuideLayer::showNode(std::vector<Node*> stencilNodes)
{
	if (m_colorlayer != NULL)
	{
		m_colorlayer->removeFromParentAndCleanup(true);
		m_colorlayer = NULL;
	}

	if (m_step != FIFTHGUIDESTEP && m_step != 72)
	{
		if (stencilNodes.size() <= 0)
		{
			opacity = 160;
		}
		m_colorlayer = LayerColor::create(Color4B(0, 0, 0, opacity));
		this->addChild(m_colorlayer);

		if (stencilNodes.size() > 0)
		{
			RenderTexture* m_clippingNode = RenderTexture::create(m_colorlayer->getContentSize().width, m_colorlayer->getContentSize().height);
			m_clippingNode->setPosition(m_colorlayer->getContentSize().width / 2, m_colorlayer->getContentSize().height / 2);
			m_colorlayer->addChild(m_clippingNode, 1);
			m_clippingNode->beginWithClear(0, 0, 0, 0.7f, 0, 0);

			Vec2 m_pos;
			for (unsigned int i = 0; i < stencilNodes.size(); i++)
			{
				Sprite* cnode = Sprite::create("images/fog.png");
				cnode->setBlendFunc({ GL_ZERO, GL_ONE_MINUS_SRC_ALPHA });
				cnode->setAnchorPoint(Vec2(0.5, 0.5));
				m_pos = stencilNodes[i]->getParent()->convertToWorldSpace(stencilNodes[i]->getPosition());
				float scalex = stencilNodes[i]->getContentSize().width / cnode->getContentSize().width;
				float scaley = stencilNodes[i]->getContentSize().height / cnode->getContentSize().height;
				if ((m_step >= 7 && m_step <= 10))
				{
					scalex = 3.0f;
					scaley = 3.0f;
				}
				else if (m_step == 30 || m_step == 47)
				{
					m_pos.y = m_pos.y + 20;
				}
				else if (m_step == 49 || m_step == 51)
				{
					m_pos.y = m_pos.y + 5;
				}
				else if (m_step == 41 || m_step == 54 || m_step == 56)
				{
					m_pos.x = 360;
				}
				cnode->setPosition(m_pos);
				cnode->setScale(scalex*1.5f, scaley*1.5f);
				if (m_step == 0 || m_step == 1 || m_step == 11 || m_step == 13 || m_step == 16 || m_step == 17 || m_step == 18 || m_step == 19 || m_step == 23 || m_step == 25 || m_step == 27 || m_step == 29 || m_step == 34 || m_step == 37 || m_step == 39 || m_step == 52 ||
					m_step == 38 || m_step == 47 || m_step == 49 || m_step == 51 || m_step == 71 || m_step == 75 || m_step == 81 || m_step == 87)
				{
					cnode->setScale(scalex*1.5f);
				}
				m_clippingNode->addChild(cnode);
				cnode->visit();
			}
			m_clippingNode->end();
			Director::getInstance()->getRenderer()->render();

			showAnim(m_pos);
		}
	}
	else
	{
		ClippingNode* m_clippingNode = ClippingNode::create();
		m_clippingNode->setInverted(true);//设置底板可见
		m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
		this->addChild(m_clippingNode, 1);

		m_colorlayer = LayerColor::create(Color4B(0, 0, 0, 160));
		m_clippingNode->addChild(m_colorlayer);

		Node* stencil = Node::create();
		for (unsigned int i = 0; i < stencilNodes.size(); i++)
		{
			//stencilNodes[i]->setTouchEnabled(false);
			std::string str = "ui/catabtn_n.png";
			if (m_step == 72)
			{
				str = "ui/taskbtn.png";
			}
			Sprite* cnode = Sprite::createWithSpriteFrameName(str);
			cnode->setPosition(stencilNodes[i]->getParent()->convertToWorldSpace(stencilNodes[i]->getPosition()));
			cnode->runAction(Blink::create(3.0f, 3));
			stencil->addChild(cnode);
		}
		m_clippingNode->setStencil(stencil);

		this->scheduleOnce(schedule_selector(NewGuideLayer::removeSelf), 3.1f);
	}
}

void NewGuideLayer::showWord(std::string wordstr)
{
	if (descText[m_step].length() > 0)
	{
		Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("newGuideLayer.csb"));
		this->addChild(m_csbnode, 1);

		cocos2d::ui::Widget* textbox = (cocos2d::ui::Widget*)m_csbnode->getChildByName("guide_frame");

		cocos2d::ui::ImageView* guide_people = (cocos2d::ui::ImageView*)textbox->getChildByName("guide_people");
		std::string str;
		if (m_step == 3 || m_step == 4)
		{
			str = StringUtils::format("images/NPC%d.png", m_step - 2);
			guide_people->loadTexture(str, cocos2d::ui::Widget::TextureResType::LOCAL);
		}
		else if (m_step == 12)
		{
			guide_people->loadTexture("images/NPC3.png", cocos2d::ui::Widget::TextureResType::LOCAL);
		}

		for (int i = 0; i < 2; i++)
		{
			str = StringUtils::format("btn%d", i);
			cocos2d::ui::Text* btn = (cocos2d::ui::Text*)textbox->getChildByName(str);
			btn->addTouchEventListener(CC_CALLBACK_2(NewGuideLayer::onBtnClick, this));
			str = StringUtils::format("guidebtn%d", i); 
			btn->setString(ResourceLang::map_lang[str]);
			if (m_step == 5)
			{
				btn->setVisible(true);
			}
		}

		if (m_step == 0 || m_step == 1 || m_step == 8 || m_step == 10 || m_step == 11 || m_step == 13 || m_step == 16 || m_step == 18 || m_step == 23
			|| m_step == 25 || m_step == 27 || m_step == 29 || m_step == 45 || m_step == 52 || m_step == 71 || m_step == 86)
			textbox->setPosition(Vec2(360, 430));
		else if (m_step == 67 || m_step == 73)
		{
			textbox->setPosition(Vec2(360, 530));
		}
		else if (m_step == 63 || m_step == 72)
		{
			textbox->setPosition(Vec2(360, 650));
		}
		else
			textbox->setPosition(Vec2(360, 160));

		cocos2d::ui::Text* wordlbl = (cocos2d::ui::Text*)textbox->getChildByName("guidetext");
		wordlbl->setString(CommonFuncs::gbk2utf(descText[m_step].c_str()));
		wordlbl->setVisible(false);

		m_wordlbl = Label::createWithTTF(descText[m_step], FONT_NAME, 25);
		m_wordlbl->setColor(Color3B(255, 255, 255));
		m_wordlbl->setLineBreakWithoutSpace(true);
		m_wordlbl->setMaxLineWidth(330);
		m_wordlbl->setAnchorPoint(Vec2(0, 1));
		m_wordlbl->setPosition(wordlbl->getPosition());
		textbox->addChild(m_wordlbl);

		std::string utf8word = CommonFuncs::gbk2utf(wordstr.c_str());
		m_wordlbl->setString(utf8word);

		std::vector<std::string> vec_resname;
		vec_resname.push_back(CommonFuncs::gbk2utf("小师妹"));
		vec_resname.push_back(CommonFuncs::gbk2utf("魔教人物"));
		vec_resname.push_back(CommonFuncs::gbk2utf("昆仑派"));
		vec_resname.push_back(CommonFuncs::gbk2utf("赏善罚恶使张三"));

		for (unsigned int i = 0; i < vec_resname.size(); i++)
		{
			std::string resname = vec_resname[i];
			std::size_t findpos = utf8word.find(resname);
			if (findpos != std::string::npos)
			{
				int sindex = findpos / 3;
				int len = resname.size() / 3;
				for (int i = sindex; i < sindex + len; i++)
				{
					m_wordlbl->getLetter(i)->setColor(Color3B(255, 214, 71));
				}
			}
		}
	}
}

void NewGuideLayer::showAnim(Vec2 pos)
{
	auto sj = Sprite::create("images/newerguide/guide_finger_0.png");
	sj->setAnchorPoint(Vec2(0, 1));
	sj->setPosition(pos.x + 20, pos.y + 10);
	this->addChild(sj, 1);

	//创建帧动画序列，名词形式
	auto animation = Animation::create();
	for (int i = 0; i< 2; i++)
	{
		char szName[100] = { 0 };
		sprintf(szName, "images/newerguide/guide_finger_%d.png", i);
		animation->addSpriteFrameWithFile(szName);
	}
	//设置帧动画属性
	animation->setDelayPerUnit(2.0f / 6);//每一帧停留的时间
	animation->setRestoreOriginalFrame(true);//播放完后回到第一帧
	auto animate = Animate::create(animation);
	sj->runAction(RepeatForever::create(animate));
}

void NewGuideLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	//CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		if (m_step == 5)
		{
			DataSave::getInstance()->setIsNewerGuide(m_step, 0);
		}
		if (g_MapBlockScene != NULL)
		{
			g_MapBlockScene->showNewerGuideFight();
		}
		this->removeFromParentAndCleanup(true);
	}
}

void NewGuideLayer::removeSelf(float dt)
{
	DataSave::getInstance()->setIsNewerGuide(m_step, 0);
	this->removeFromParentAndCleanup(true);
	if (g_mainScene != NULL)
	{
		g_mainScene->setScrollGliding();
	}
}

void NewGuideLayer::clearNewGuideData()
{
	for (int i = 0; i < 6; i++)
	{
		Hero* hero = GlobalInstance::myCardHeros[i];
		delete hero;
		GlobalInstance::myCardHeros[i] = NULL;
		for (int j = 0; j < 2; j++)
		{
			GongFa* equ = (GongFa*)MyRes::getMyRes(gf[i][j], MYEQUIP);
			if (equ != NULL)
			{
				MyRes::Use(equ, 1, MYEQUIP);
			}
		}
	}
}

void NewGuideLayer::setNewGuideInfo(int step)
{
	if (step == FIRSTGUIDESTEP)
	{
		if (checkifNewerGuide(step))
		{
			clearNewGuideData();
			MyRes::Add("r001", 10, MYPACKAGE);
			for (int i = 0; i < 6; i++)
			{
				Hero* hero = new Hero();
				hero->generate();
				std::string strname = StringUtils::format("newguideheroname_%d", i);
				hero->setName(ResourceLang::map_lang[strname]);
				hero->setPotential(4);
				hero->setVocation(voc[i]);
				hero->setState(HS_TAKEON);
				hero->setPos(i + 1);
				DynamicValueInt dal;
				dal.setValue(GlobalInstance::vec_herosAttr[4].vec_exp[49]);
				hero->setExp(dal);
				hero->setHp(hero->getMaxHp());
				GlobalInstance::myCardHeros[i] = hero;
				for (int j = 0; j < 2; j++)
				{
					GongFa* equ = (GongFa*)MyRes::getMyRes(gf[i][j], MYEQUIP);
					if (equ == NULL)
					{
						MyRes::Add(gf[i][j], 1, MYEQUIP);
						equ = (GongFa*)MyRes::getMyRes(gf[i][j], MYEQUIP);
						equ->setWhos(hero->getName());
						DynamicValueInt qul;
						qul.setValue(4);
						equ->setQU(qul);
					}
				}
			}

			for (int i = 0; i < step; i++)
			{
				DataSave::getInstance()->setIsNewerGuide(i, 1);
			}
		}
	}
	else if (step == THRIDGUIDESTEP)
	{
		for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
		{
			Hero* hero = GlobalInstance::vec_myHeros[i];
			delete hero;
		}
		for (unsigned int i = 0; i < GlobalInstance::vec_rand3Heros.size(); i++)
		{
			Hero* hero = GlobalInstance::vec_rand3Heros[i];
			delete hero;
		}
		GlobalInstance::vec_myHeros.clear();
		GlobalInstance::vec_rand3Heros.clear();
		for (unsigned int i = 0; i < MyRes::vec_MyResources.size(); i++)
		{
			ResBase* res = MyRes::vec_MyResources[i];
			if (res->getType() >= T_WG && res->getType() <= T_NG)
			{
				GongFa* gongfa = (GongFa*)res;
				gongfa->setWhos("");
				gongfa->setWhere(MYSTORAGE);
			}
		}

		/*DataSave::getInstance()->setRand3HeroData("");
		for (int i = 0; i < 3; i++)
		{
			std::string herokey = StringUtils::format("hero%d", i);
			DataSave::getInstance()->setHeroData(herokey, "");
		}
		DataSave::getInstance()->setMyRes(defaultres);*/

		if (checkifNewerGuide(step))
		{
			for (int i = 22; i < step; i++)
			{
				DataSave::getInstance()->setIsNewerGuide(i, 1);
			}
		}
	}
	else if (step == MIDELEGUIDESTEP)
	{
		GlobalInstance::myCurMainData.isfinish = QUEST_TASK;
		for (unsigned int i = 0; i < GlobalInstance::vec_TaskMain.size(); i++)
		{
			TaskData* data = &GlobalInstance::vec_TaskMain[i];
			if (data->id == 1)
			{
				data->isfinish = QUEST_TASK;
				break;
			}
		}

		if (checkifNewerGuide(step))
		{
			for (int i = THRIDGUIDESTEP + 1; i < step; i++)
			{
				DataSave::getInstance()->setIsNewerGuide(i, 1);
			}
		}
	}
	else if (step == FOURTHGUIDESTEP)
	{
		for (unsigned int i = 0; i < GlobalInstance::vec_myHeros.size(); i++)
		{
			Hero* hero = GlobalInstance::vec_myHeros[i];
			hero->setState(HS_OWNED);
		}
		for (int i = 0; i < 6; i++)
		{
			GlobalInstance::myCardHeros[i] = NULL;
		}

		if (checkifNewerGuide(step))
		{
			for (int i = MIDELEGUIDESTEP + 1; i < step; i++)
			{
				DataSave::getInstance()->setIsNewerGuide(i, 1);
			}
		}
	}
	/*else if (step == FIFTHGUIDESTEP)
	{
		for (unsigned int i = 0; i < Quest::myFinishMainQuest.size(); i++)
		{
			TaskData* data = Quest::myFinishMainQuest[i];
			if (data->id == 1)
			{
				data->isfinish = QUEST_FINISH;
			}
		}

		if (checkifNewerGuide(step))
		{
			for (int i = FOURTHGUIDESTEP + 1; i < step; i++)
			{
				DataSave::getInstance()->setIsNewerGuide(i, 1);
			}
		}
	}*/
}

void NewGuideLayer::onExit()
{
	Layer::onExit();
}