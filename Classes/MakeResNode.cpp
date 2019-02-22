#include "MakeResNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "MainScene.h"
#include "Const.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "MakeResLayer.h"

MakeResNode::MakeResNode()
{
}


MakeResNode::~MakeResNode()
{

}

MakeResNode* MakeResNode::create(std::string resid)
{
	MakeResNode *pRet = new(std::nothrow)MakeResNode();
	if (pRet && pRet->init(resid))
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

bool MakeResNode::init(std::string resid)
{
	m_resid = resid;
	csbnode = CSLoader::createNode(ResourcePath::makePath("makeResNode.csb"));
	this->addChild(csbnode, 0, "csbnode");

	itembg = (cocos2d::ui::Widget*)csbnode->getChildByName("resitem");
	itembg->addTouchEventListener(CC_CALLBACK_2(MakeResNode::onItemClick, this));
	itembg->setUserData((void*)m_resid.c_str());
	itembg->setSwallowTouches(false);

	cocos2d::ui::ImageView* resimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	std::string resstr = StringUtils::format("ui/%s.png", m_resid.c_str());
	resimg->loadTexture(ResourcePath::makePath(resstr), cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	namelbl->setString(GlobalInstance::map_AllResources[m_resid].name);

	cocos2d::ui::Text* desclbl = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desclbl->setString(GlobalInstance::map_AllResources[m_resid].desc);

	hascount = (cocos2d::ui::Text*)csbnode->getChildByName("hascount");

	updateMyOwnCountUI();

	openlvtext = (cocos2d::ui::Text*)csbnode->getChildByName("openlvtext");
	openlvtext->setVisible(false);
	std::string openlvstr = StringUtils::format(ResourceLang::map_lang["smithyopenlvtext"].c_str(), getResInSmithyLv() + 1);
	openlvtext->setString(openlvstr);

	return true;
}

void MakeResNode::updateMyOwnCountUI()
{
	int count = MyRes::getEquipableCount(m_resid);
	if (count > 0)
	{
		hascount->setVisible(true);
		std::string countstr = StringUtils::format(ResourceLang::map_lang["hasequipcount"].c_str(), count);
		hascount->setString(countstr);
	}
	else
		hascount->setVisible(false);
}

void MakeResNode::onItemClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
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

		SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
		MakeResLayer* layer = MakeResLayer::create(clicknode->getUserData());
		g_mainScene->getChildByName("2smithy")->addChild(layer);
		AnimationEffect::openAniEffect(layer);
	}
}

void MakeResNode::setEnable(bool val)
{
	if (val)
	{
		for (int i = 0; i < csbnode->getChildrenCount(); i++)
		{
			CommonFuncs::removeGray(csbnode->getChildren().at(i));
		}
		updateMyOwnCountUI();
	}
	else
	{
		for (int i = 0; i < csbnode->getChildrenCount(); i++)
		{
			CommonFuncs::changeGray(csbnode->getChildren().at(i));
		}
		hascount->setVisible(val);
	}
	itembg->setEnabled(val);
	openlvtext->setVisible(!val);
}

int MakeResNode::getResInSmithyLv()
{
	Building* buildingdata = Building::map_buildingDatas["2smithy"];
	for (int v = 0; v < buildingdata->maxlevel.getValue(); v++)
	{
		int vsize = buildingdata->vec_exdata.size();

		for (unsigned int i = 0; i < buildingdata->vec_exdata[v].size(); i++)
		{
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(buildingdata->vec_exdata[v][i], vec_tmp, "-");

			if (m_resid.compare(vec_tmp[0]) == 0)
			{
				return v;
			}
		}
	}
	return 0;
}