#include "RebateAwdNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "Const.h"
#include "SoundManager.h"
#include "AnimationEffect.h"

RebateAwdNode::RebateAwdNode()
{
}


RebateAwdNode::~RebateAwdNode()
{

}

RebateAwdNode* RebateAwdNode::create(std::string awdstr)
{
	RebateAwdNode *pRet = new(std::nothrow)RebateAwdNode();
	if (pRet && pRet->init(awdstr))
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

bool RebateAwdNode::init(std::string awdstr)
{
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("rebateawdnode.csb"));
	this->addChild(csbnode, 0, "csbnode");

	Node* select = csbnode->getChildByName("select");
	select->setVisible(false);

	std::vector<std::string> vec_awd;

	CommonFuncs::split(awdstr, vec_awd, ",");

	for (int i = 0; i < 3; i++)
	{
		std::string name = StringUtils::format("resbox%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(name);

		if (i < vec_awd.size())
		{
			cocos2d::ui::ImageView* res = (cocos2d::ui::ImageView*)resbox->getChildByName("res");

			cocos2d::ui::Text* countlbl = (cocos2d::ui::Text*)resbox->getChildByName("countlbl");

			std::vector<std::string> vec_one;

			CommonFuncs::split(vec_awd[i], vec_one, "-");

			std::string resid = vec_one[0];
			int count = atoi(vec_one[1].c_str());
			int qu = 0;

			if (vec_one.size() > 2)
				qu = atoi(vec_one[2].c_str());

			std::string qustr;
			int t = 0;
			for (; t < sizeof(RES_TYPES_CHAR) / sizeof(RES_TYPES_CHAR[0]); t++)
			{
				if (resid.compare(0, 1, RES_TYPES_CHAR[t]) == 0)
					break;
			}
			if (t >= T_ARMOR && t <= T_FASHION)
			{
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			else if (t >= T_WG && t <= T_NG)
			{
				qu = GlobalInstance::map_GF[resid].qu;
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}

			else if (t >= T_RENS && t <= T_BOX)
			{
				qu = atoi(resid.substr(1).c_str()) - 1;
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}

			resbox->loadTexture(ResourcePath::makePath(qustr), cocos2d::ui::Widget::TextureResType::PLIST);

			std::string resstr = StringUtils::format("ui/%s.png", resid.c_str());
			res->loadTexture(resstr, cocos2d::ui::Widget::TextureResType::PLIST);
			
			std::string counstr = StringUtils::format("%d", count);
			countlbl->setString(counstr);

			Node* node = CommonFuncs::playResBoxEffect(resbox, t, qu, 0);
			if (node != NULL)
				node->setScale(0.68f);
		}
		else
		{
			resbox->setVisible(false);
		}
	}

	return true;
}


void RebateAwdNode::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
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

	}
}