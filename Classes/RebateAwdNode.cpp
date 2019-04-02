#include "RebateAwdNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "Const.h"
#include "SoundManager.h"
#include "AnimationEffect.h"
#include "EquipDescLayer.h"
#include "SimpleResPopLayer.h"

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
	langtype = GlobalInstance::getInstance()->getLang();
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("rebateawdnode.csb"));
	this->addChild(csbnode, 0, "csbnode");

	actionbtn = (cocos2d::ui::Button*)csbnode->getChildByName("actionbtn");
	actionbtn->addTouchEventListener(CC_CALLBACK_2(RebateAwdNode::onClick, this));
	actionbtn->setTag(1000);
	actionbtn->setVisible(false);

	getbtntext = (cocos2d::ui::ImageView*)actionbtn->getChildByName("text");
	getbtntext->ignoreContentAdaptWithSize(true);
	getbtntext->loadTexture(ResourcePath::makeTextImgPath("rebatgetbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	selectnode = csbnode->getChildByName("select");
	selectnode->setVisible(false);

	std::vector<std::string> vec_awd;

	CommonFuncs::split(awdstr, vec_awd, ",");

	for (unsigned int i = 0; i < 3; i++)
	{
		std::string name = StringUtils::format("resbox%d", i);
		cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(name);
		resbox->addTouchEventListener(CC_CALLBACK_2(RebateAwdNode::onClick, this));

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
			MSGAWDSDATA resdata;
			resdata.rid = resid;
			resdata.count = count;
			resdata.qu = qu;
			vec_resdata.push_back(resdata);
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
			else if (t >= T_HEROCARD && t <= T_ARMCARD)
			{
				qu = atoi(resid.substr(1).c_str()) + 2;
				qustr = StringUtils::format("ui/resbox_qu%d.png", qu);
			}
			resbox->setTag(i*1000+t);
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
	if (clicknode->getTag() == 1000)
		CommonFuncs::BtnAction(pSender, type);
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

		if (clicknode->getTag() == 1000)
		{
			actionbtn->setEnabled(false);
			selectnode->setVisible(false);
			getbtntext->loadTexture(ResourcePath::makeTextImgPath("rebatgetedbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			GlobalInstance::getInstance()->setIsGetRebateAwds(this->getTag(), true);
			for (unsigned int i = 0; i < vec_resdata.size(); i++)
			{
				std::string awdresid = vec_resdata[i].rid;
				int awdcount = vec_resdata[i].count;
				int awdqu = vec_resdata[i].qu;

				int stc = GlobalInstance::getInstance()->generateStoneCount(awdqu);

				if (awdresid.compare("r006") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(awdcount);
					GlobalInstance::getInstance()->addMySoliverCount(dvint);
				}
				else if (awdresid.compare("r012") == 0)
				{
					DynamicValueInt dvint;
					dvint.setValue(awdcount);
					GlobalInstance::getInstance()->addMyCoinCount(dvint);
				}
				else
					MyRes::Add(awdresid, awdcount, MYSTORAGE, awdqu, stc);

			}
		}
		else
		{
			SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
			cocos2d::ui::ImageView* clickres = (cocos2d::ui::ImageView*)pSender;
			int tag = clickres->getTag();
			std::string resid = vec_resdata[tag/1000].rid;

			int t = tag % 1000;

			if (t >= T_ARMOR && t <= T_NG)
			{
				Layer* layer = EquipDescLayer::create(resid, vec_resdata[tag / 1000].qu, 1);
				g_mainScene->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
			else
			{
				SimpleResPopLayer* layer = SimpleResPopLayer::create(resid, 3);
				g_mainScene->addChild(layer);
				AnimationEffect::openAniEffect(layer);
			}
		}

	}
}

void RebateAwdNode::setSelect()
{
	int tag = this->getTag();
	actionbtn->setVisible(true);
	if (!GlobalInstance::getInstance()->getIsGetRebateAwds(tag))
		selectnode->setVisible(true);
	else
	{
		selectnode->setVisible(false);
		actionbtn->setEnabled(false);
		getbtntext->loadTexture(ResourcePath::makeTextImgPath("rebatgetedbtn_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
	}
}