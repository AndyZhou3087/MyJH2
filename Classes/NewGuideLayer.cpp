#include "NewGuideLayer.h"
#include "GlobalInstance.h"
#include "DataSave.h"
#include "CommonFuncs.h"
#include "Const.h"

std::map<int, std::vector<std::string>> map_words;

std::string descText[][6] = { "小师妹：掌门师兄，六大派掌门和魔教应该就在前面了，咱们快去看看。","","","小师妹说话：掌门师兄，传言果然是真的，十四天书现世，六大派和魔教为了争夺天书才打起来。",
"魔教人物：小子！现在帮我们对付他们，老子给你黄金万两！","昆仑派：少侠！魔教为非作歹，祸害江湖，我辈武林人士岂能与他们为伍，少侠快来助我们除掉这些武林败类！","小师妹：掌门师兄，你可以选择一方进行帮助，都是为了十四天书，出手吧师兄！",
"小师妹：掌门师兄，战斗过程为全自动，掌门只要根据角色的属性去分配好位置就可以了。","小师妹：前排的武僧，防御力比较高，适合在前面抗伤害","小师妹：后面的刺客攻击力很高，但是很容易受伤，适合在后面进行攻击。",
"小师妹：剑客的属性比较平衡，什么位置都可以站。","小师妹：道士会用道术进行多重攻击，还可以治疗队友。","小师妹：师兄，战斗结束了，天书我们也拿到了，他们一会恢复过来，咱们就走不脱了，还是快撤吧！",
"", "张三：天书现世，世道已经乱了，这些东西不是你该拿的，交出来吧！", "小师妹：师兄，咱们还是先回去吧，连赏善罚恶二使都来了，回去好好想想对策！","",
"小师妹：这里虽然不比师傅以前传给师兄的地方，但是好在这里人烟稀少，咱们可以好好经营一下，师兄先给门派起个响亮的名字吧！", "小师妹：经营门派最重要的还是要先把基础设施完善起来，我们先去后山看看有什么资源吧!",
"小师妹：这里后山还真是什么都没有啊，师兄，先升级一下后山，解锁更多的资源吧!","小师妹：资源有了，我们该招点工人来干活了。","小师妹：工人有了，但是师兄你要分配给他们任务，每个工人采集资源都会消耗一定的食材，记得要保证平衡哦!",

};

NewGuideLayer* g_NewGuideLayer = NULL;

std::vector<std::string> NewGuideLayer::vec_userdata;
bool NewGuideLayer::isShowing = false;
NewGuideLayer::NewGuideLayer()
{
	wordindex = 0;
	iscannext = false;
	isShowing = false;
}

NewGuideLayer::~NewGuideLayer()
{
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
	m_colorlayer = NULL;

	initDialog();
	m_step = step;
	Node* csbnode = CSLoader::createNode("newGuideLayer.csb");
	this->addChild(csbnode);

	cocos2d::ui::Text* wordtext = (cocos2d::ui::Text*)csbnode->getChildByName("guidetext");
	wordtext->setVisible(false);

	m_wordlbl = Label::createWithTTF("", "fonts/STXINGKA.TTF", 25);
	m_wordlbl->setColor(Color3B(255, 255, 255));
	m_wordlbl->setLineBreakWithoutSpace(true);
	m_wordlbl->enableShadow(Color4B(255, 255, 255, 255), Size(1, -1));
	m_wordlbl->setMaxLineWidth(330);
	m_wordlbl->setAnchorPoint(Vec2(0, 1));
	m_wordlbl->setPosition(Vec2(34, 250));
	csbnode->addChild(m_wordlbl);

	if (m_step == 4 || m_step == 19 || m_step == 45 || m_step == 65)
	{
		
	}
	else
	{
		
	}
	if (m_step == 69)
	{
		
	}
	else if (m_step == 12)
	{
		
	}

	if (map_words[step].size() > 0)
	{
		std::string wordstr = map_words[step][0];
		showWord(wordstr);
	}
	else
	{
		
	}

	if (stencilNodes.size() > 0)
	{
		m_clippingNode = ClippingNode::create();
		m_clippingNode->setInverted(true);//设置底板可见
		m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
		this->addChild(m_clippingNode, -1);

		int al = 200;
		if (map_words[step].size() <= 0)
			al = 0;
		m_colorlayer = LayerColor::create(Color4B(0, 0, 0, al));
		m_clippingNode->addChild(m_colorlayer);

		Node* stencil = Node::create();
		for (unsigned int i = 0; i < stencilNodes.size(); i++)
		{
			Sprite* cnode = Sprite::createWithSpriteFrameName("mapui/fog.png");
			cnode->setPosition(stencilNodes[i]->getParent()->convertToWorldSpace(stencilNodes[i]->getPosition()));
			float scalex = stencilNodes[i]->getContentSize().width / cnode->getContentSize().width;
			float scaley = stencilNodes[i]->getContentSize().height / cnode->getContentSize().height;
			cnode->setScale(scalex + 0.1f, scaley + 0.1f);
			cnode->runAction(RepeatForever::create(Blink::create(4.0f, 5)));
			stencil->addChild(cnode);
		}
		m_clippingNode->setStencil(stencil);

		showAnim(stencilNodes[stencilNodes.size() - 1]->getParent()->convertToWorldSpace(stencilNodes[stencilNodes.size() - 1]->getPosition()));
	}
	else
	{
		m_colorlayer = LayerColor::create(Color4B(0, 0, 0, 200));
		this->addChild(m_colorlayer, -1);
	}
	isallclick = false;
	if (m_step == 0 || m_step == 2 || m_step == 4 || m_step == 9 || m_step == 15 || m_step == 19 || m_step == 41 || m_step == 45 || m_step == 47 || m_step == 58 || m_step == 59 || m_step == 65)
		isallclick = true;

	if (isallclick)
	{
		iscannext = true;
	}

	//layer 点击事件，屏蔽下层事件
	m_listener = EventListenerTouchOneByOne::create();
	m_listener->setSwallowTouches(true);

	m_listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		Vec2 point = Director::getInstance()->convertToGL(touch->getLocationInView());//获得当前触摸的坐标 
		starPos = touch->getLocation();
		if (!isallclick && stencilNodes.size() > 0)
		{
			Vec2 vec = stencilNodes[stencilNodes.size() - 1]->getParent()->convertToWorldSpace(stencilNodes[stencilNodes.size() - 1]->getPosition());

			auto rect = Rect(vec.x - stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.width / 2, vec.y - stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.height / 2, stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.width, stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.height);
			if (rect.containsPoint(point))//如果触点处于rect中  
			{
				iscannext = true;
				m_listener->setSwallowTouches(false);
			}
		}
		return true;
	};
	m_listener->onTouchEnded = [=](Touch *touch, Event *event)
	{
		if (iscannext)
		{
			wordindex++;

			int size = map_words[m_step].size();
			if (wordindex >= size)
			{
				if (m_step == 0)
				{
					removeSelf();
					/*HomeLayer* homelayer = (HomeLayer*)g_gameLayer->getChildByName("homelayer");
					if (homelayer != NULL)
						homelayer->checkNewerGuide();*/
				}
				else if (m_step == 2)
				{
					removeSelf();
					/*BuildingUILayer* buildlayer = (BuildingUILayer*)g_gameLayer->getChildByName("builduilayer");
					if (buildlayer != NULL)
						buildlayer->checkNewerGuide();*/
				}
				else if (m_step == 4)
				{
					removeSelf();
					/*BuildingUILayer* buildlayer = (BuildingUILayer*)g_gameLayer->getChildByName("builduilayer");
					if (buildlayer != NULL)
						buildlayer->checkNewerGuide();*/
				}
				else if (m_step == 15)
				{
					removeSelf();
					/*GoWhereLayer* golayer = (GoWhereLayer*)g_gameLayer->getChildByName("gowherelayer");
					if (golayer != NULL)
						golayer->showNewerGuide(16);*/
				}
				else if (m_step == 19)
				{
					removeSelf();
					/*ActionGetLayer* alayer = (ActionGetLayer*)g_gameLayer->getChildByName("ActionGetLayer");
					if (alayer != NULL)
						alayer->showNewerGuide(20);*/
				}
				else if (m_step == 41)
				{
					removeSelf();
					/*HomeLayer* homelayer = (HomeLayer*)g_gameLayer->getChildByName("homelayer");
					if (homelayer != NULL)
						homelayer->showNewerGuide(45);*/
				}
				else if (m_step == 45)
				{
					removeSelf();
					/*HomeLayer* homelayer = (HomeLayer*)g_gameLayer->getChildByName("homelayer");
					if (homelayer != NULL)
						homelayer->showNewerGuide(46);*/
				}
				else if (m_step == 47)
				{
					removeSelf();
					/*HeroProperNode* heroProperNode = (HeroProperNode*)g_gameLayer->getChildByName("OutDoor")->getChildByName("csbnode")->getChildByName("HeroProperNode");
					if (heroProperNode != NULL)
						heroProperNode->showNewerGuide(48);*/
				}
				else if (m_step == 58)
				{
					removeSelf();
					/*MapLayer* maplayer = (MapLayer*)g_gameLayer->getChildByName("maplayer");
					if (maplayer != NULL)
						maplayer->showNewerGuide(5);*/
				}
				else if (m_step == 59)
				{
					removeSelf();
					/*TopBar* topbar = (TopBar*)g_gameLayer->getChildByName("topbar");
					if (topbar != NULL)
						topbar->showNewerGuide(60);*/
				}
				else if (m_step == 65)
				{
					removeSelf();
					/*HomeLayer* homelayer = (HomeLayer*)g_gameLayer->getChildByName("homelayer");
					if (homelayer != NULL)
						homelayer->checkNewerGuide();*/
				}
				else
				{
					removeSelf();
				}
			}
			else
			{
				if (m_step == 4 || m_step == 19 || m_step == 65)
				{
					if (wordindex % 2 == 0)
					{
						
					}
					else
					{
						
					}
				}
				else
				{
					if (wordindex % 2 == 0)
					{
						
					}
					else
					{
						
					}
				}

				showWord(map_words[m_step][wordindex]);
			}
		}
		return;
	};

	_eventDispatcher->addEventListenerWithSceneGraphPriority(m_listener, this);
	DataSave::getInstance()->setIsNewerGuide(step, 0);

//#ifdef ANALYTICS
//	if (step == 0)
//		AnalyticUtil::onEvent("newerstart");
//	else if (step == 12)
//		AnalyticUtil::onEvent("firstout");
//	else if (step == 46)
//		AnalyticUtil::onEvent("secondout");
//	else if (step == 58)
//		AnalyticUtil::onEvent("newerend");
//#endif

	return true;
}

void NewGuideLayer::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();
}

bool NewGuideLayer::checkifNewerGuide(int index)
{
	bool ret = false;
	ret = DataSave::getInstance()->getIsNewerGuide(index);
	if (!ret)
	{
		vec_userdata.clear();
	}
	return ret;
}

void NewGuideLayer::showAnim(Vec2 pos)
{
	if (m_step == 58)
		return;

	auto sj = Sprite::create("images/newerguide/guide_finger_0.png");
	sj->setAnchorPoint(Vec2(0, 1));
	sj->setPosition(pos.x, pos.y);
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

void NewGuideLayer::pushUserData(std::string strdata)
{
	vec_userdata.push_back(strdata);
}

void NewGuideLayer::showWord(std::string wordstr)
{
	std::string utf8word = CommonFuncs::gbk2utf(wordstr.c_str());
	m_wordlbl->setString(utf8word);

	std::vector<std::string> vec_resname;
	vec_resname.push_back(CommonFuncs::gbk2utf("木材"));
	vec_resname.push_back(CommonFuncs::gbk2utf("石矿"));
	vec_resname.push_back(CommonFuncs::gbk2utf("铁矿"));
	vec_resname.push_back(CommonFuncs::gbk2utf("切记喝酒必须在酒桌上才能喝！"));
	vec_resname.push_back(CommonFuncs::gbk2utf("仓库（锻造台下方）"));
	vec_resname.push_back(CommonFuncs::gbk2utf("狼"));
	vec_resname.push_back(CommonFuncs::gbk2utf("兔子"));
	std::vector<std::string>::iterator it;
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
				m_wordlbl->getLetter(i)->setColor(Color3B(204, 4, 4));
			}
		}
	}
}

void NewGuideLayer::initDialog()
{
	if (map_words.size() <= 0)
	{
		int total = sizeof(descText) / sizeof(descText[0]);
		for (int i = 0; i < total; i++)
		{
			for (int m = 0; m < 6; m++)
			{
				if (descText[i][m].length() > 0)
				{
					map_words[i].push_back(descText[i][m]);
				}
			}
		}
	}
}

void NewGuideLayer::removeSelf()
{
	vec_userdata.clear();
	isShowing = false;
	this->removeFromParentAndCleanup(true);
	g_NewGuideLayer = NULL;
}