#include "NewGuideLayer.h"
#include "GlobalInstance.h"
#include "DataSave.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "Resource.h"

std::string descText[] = { "点击这里可查看建造所需要的资源", "点击材料即可查看材料的详细信息", "点击角色头像即可查看角色属性及更换装备等操作", "点击武功即可选择武功进行使用",
"", "点击内功即可选择内功进行使用", "", "点击武器即可选择武器进行装备，武器越好，造成的伤害越多",
"", "点击装备即可选择防具进行装备，防具越好，受到的伤害越少", "", "",
"","点击角色属性图标即可查看属性状态，每个属性都会对角色造成不同的影响，要注意不让各种状态过低", "", "",
"","", "为了保证充足的精力闯荡江湖，首先需要建造睡觉的床！现在出门找点造床的材料", "",
"移动地点时根据距离不同消耗的时间也有不同", "去的地方越远，消耗的饥饿与精神越多，要注意好状态", "", "做床需要铁块，先采两块铁矿",
"获得一块铁矿，点击放到背包中", "每次只能采一块，再采一块", "可以全部拾取哦~~", "",
"还需要点木材，砍两根松树", "获得一根木材，点击放到背包中", "每次只能采一根，再采一根", "可以全部拾取哦~~",
"", "还需要两块兽皮，打猎狼可以获取狼皮。战斗时不但能获得物品，还可获得经验值，当经验值满足条件后角色即可升级，武学也会自动升级，等级越高越厉害", "获得一块狼皮点击放到背包中", "",
"还需要一块狼皮，继续打猎狼", "可以全部拾取哦~~","", "",
"材料收集齐了，可以回家造一张舒适的床了^^","","", "",
"在床上休息可恢复已消耗的生命值", "仓库里存放了所有采集的资源、锻造的工具，获得的武学，还有一些珍贵的药材。可以在这里使用食物恢复生生命值，使用药材恢复外伤、内伤！", "可以在仓库里选择食物，吃烤肉填饱肚子", "可以在仓库里选择药材，使用三七可以治疗内伤，酒可以恢复精神值，需要在酒桌上使用。",
"感叹号提示着在这个地点的角色有任务找你，快去看一下是什么事情吧，以你现在的实力还打不过这里的任何人，千万不要轻易进行挑战，切记！！！", "", "", "掉落没有拾取的物品会放在临时存放点，每个地点都有一个，来了记得带回去~~", "", "", "", /*后面建筑物引导*/"", "", "", "", "", "", "", "", "", ""
};

NewGuideLayer* g_NewGuideLayer = NULL;
NewGuideLayer::NewGuideLayer()
{

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
	if (stencilNodes.size() > 0)
	{
		m_clippingNode = ClippingNode::create();
		m_clippingNode->setInverted(true);//设置底板可见
		m_clippingNode->setAlphaThreshold(0.5f);//设置透明度Alpha值为0
		this->addChild(m_clippingNode, 1);

		m_colorlayer = LayerColor::create(Color4B(0, 0, 0, 160));
		m_clippingNode->addChild(m_colorlayer);

		Node* stencil = Node::create();
		for (unsigned int i = 0; i < stencilNodes.size(); i++)
		{
			Sprite* cnode = Sprite::createWithSpriteFrameName("mapui/fog.png");
			cnode->setPosition(stencilNodes[i]->getParent()->convertToWorldSpace(stencilNodes[i]->getPosition()));
			float scalex = stencilNodes[i]->getContentSize().width / cnode->getContentSize().width;
			float scaley = stencilNodes[i]->getContentSize().height / cnode->getContentSize().height;
			float scle;
			if (scalex > scaley)
			{
				scle = scalex;
			}
			else
			{
				scle = scaley;
			}
			cnode->setScale(scle);

			stencil->addChild(cnode);
		}
		m_clippingNode->setStencil(stencil);

		showAnim(stencilNodes[stencilNodes.size() - 1]->getParent()->convertToWorldSpace(stencilNodes[stencilNodes.size() - 1]->getPosition()));
	}

	if (step == 18)
	{
		m_colorlayer->setOpacity(125);
	}
	if (descText[step].length() > 0)
	{
		Node* m_csbnode = CSLoader::createNode(ResourcePath::makePath("taskMainDescLayer.csb"));

		//if (step == 2 || step == 13 || step == 18 || step == 21 || step == 25 || step == 26 || step == 28 || step == 30 || step == 31 || step == 34 || step == 35 || step == 37 || step == 38 || step == 45 || step == 51)
		//	textbox->setPosition(Vec2(360, 650));
		//else
		//	textbox->setPosition(Vec2(360, 170));
		this->addChild(m_csbnode, 2);

		cocos2d::ui::Text* wordlbl = (cocos2d::ui::Text*)m_csbnode->getChildByName("guidetext");
		wordlbl->setString(CommonFuncs::gbk2utf(descText[step].c_str()));
	}

	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		Vec2 point = Director::getInstance()->convertToGL(touch->getLocationInView());//获得当前触摸的坐标 
		starPos = touch->getLocation();
		if (stencilNodes.size() > 0)
		{
			Vec2 vec = stencilNodes[stencilNodes.size() - 1]->getParent()->convertToWorldSpace(stencilNodes[stencilNodes.size() - 1]->getPosition());

			auto rect = Rect(vec.x - stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.width / 2, vec.y - stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.height / 2, stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.width, stencilNodes[stencilNodes.size() - 1]->getBoundingBox().size.height);
			if (rect.containsPoint(point))//如果触点处于rect中  
			{
				DataSave::getInstance()->setIsNewerGuide(step, 0);

				listener->setSwallowTouches(false);

				if (!(step >= 2 && step <= 10 || (step >= 13 && step <= 17)))
				{
					this->removeFromParentAndCleanup(true);
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

	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

//#ifdef ANALYTICS
//	if (step == 0)
//		AnalyticUtil::onEvent("newerstart");
//	else if (step == 44)
//		AnalyticUtil::onEvent("newerend");
//#endif

	return true;
}

bool NewGuideLayer::checkifNewerGuide(int index)
{
	return DataSave::getInstance()->getIsNewerGuide(index);
}

void NewGuideLayer::showAnim(Vec2 pos)
{
	auto sj = Sprite::create("images/newerguide/guide_finger_0.png");
	sj->setAnchorPoint(Vec2(0, 1));
	sj->setPosition(pos.x + 20, pos.y - 20);
	this->addChild(sj, 1);

	//创建帧动画序列，名词形式
	auto animation = Animation::create();
	for (int i = 0; i < 2; i++)
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
