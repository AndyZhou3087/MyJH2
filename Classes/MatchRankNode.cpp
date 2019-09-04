#include "MatchRankNode.h"
#include "CommonFuncs.h"
#include "Resource.h"
#include "MovingLabel.h"
#include "Const.h"
#include "AnimationEffect.h"
#include "SoundManager.h"
#include "DataSave.h"
#include "MatchContentLayer.h"
#include "WaitingProgress.h"
#include "MatchMainLayer.h"

MatchRankNode::MatchRankNode()
{
	clickflag = false;
}


MatchRankNode::~MatchRankNode()
{

}

MatchRankNode* MatchRankNode::create(MyRankData herodata, int index, int type)
{
	MatchRankNode *pRet = new(std::nothrow)MatchRankNode();
	if (pRet && pRet->init(herodata, index, type))
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

bool MatchRankNode::init(MyRankData herodata, int index, int type)
{
	m_herodata = herodata;
	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("matchRankNode.csb"));
	this->addChild(csbnode, 0, "csbnode");

	int langtype = GlobalInstance::getInstance()->getLang();

	cocos2d::ui::ImageView* itembg = (cocos2d::ui::ImageView*)csbnode->getChildByName("itembg");

	cocos2d::ui::ImageView* clickimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("clickimg");
	clickimg->addTouchEventListener(CC_CALLBACK_2(MatchRankNode::onBtnClick, this));
	clickimg->setTag(1000);
	clickimg->setSwallowTouches(false);

	cocos2d::ui::ImageView* herobox = (cocos2d::ui::ImageView*)csbnode->getChildByName("herobox");
	cocos2d::ui::ImageView* heroimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("heroimg");

	//名字
	cocos2d::ui::Text* namelbl = (cocos2d::ui::Text*)csbnode->getChildByName("name");

	//积分
	cocos2d::ui::Text* pointtext = (cocos2d::ui::Text*)csbnode->getChildByName("pointtitle");
	pointtext->setString(ResourceLang::map_lang["matchexptext_1"]);

	cocos2d::ui::Text* point = (cocos2d::ui::Text*)csbnode->getChildByName("point");

	//段位
	cocos2d::ui::Text* divisiontext = (cocos2d::ui::Text*)csbnode->getChildByName("divisiontext");
	divisiontext->setString(ResourceLang::map_lang["matchlvtext_1"]);

	cocos2d::ui::Text* division = (cocos2d::ui::Text*)csbnode->getChildByName("division");
	cocos2d::ui::Text* ranktext = (cocos2d::ui::Text*)csbnode->getChildByName("ranktext");
	cocos2d::ui::ImageView* rankimg = (cocos2d::ui::ImageView*)csbnode->getChildByName("rankimg");

	//我的排名
	cocos2d::ui::Text* myranktext = (cocos2d::ui::Text*)csbnode->getChildByName("myranktext");
	myranktext->setString(ResourceLang::map_lang["mymatchrank"]);
	cocos2d::ui::Text* myrank = (cocos2d::ui::Text*)myranktext->getChildByName("myrank");

	//按钮
	cocos2d::ui::ImageView* actbtn = (cocos2d::ui::ImageView*)csbnode->getChildByName("actionbtn");
	actbtn->addTouchEventListener(CC_CALLBACK_2(MatchRankNode::onBtnClick, this));
	actbtn->setTag(1000);
	actbtn->setSwallowTouches(false);

	//按钮文字
	cocos2d::ui::ImageView* actbtntxt = (cocos2d::ui::ImageView*)actbtn->getChildByName("text");
	actbtntxt->ignoreContentAdaptWithSize(true);
	actbtntxt->loadTexture(ResourcePath::makeTextImgPath("watchline_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);

	if (type == 1)//我的排行
	{
		MyRankData mydata;
		mydata.matchscore = GlobalInstance::myMatchInfo.matchscore;
		mydata.nickname = GlobalInstance::getInstance()->getMyNickName();
		mydata.rank = GlobalInstance::myRankInfo.myrank;
		mydata.map_otherheros = GlobalInstance::myMatchInfo.map_myheros;
		mydata.wincount = GlobalInstance::myMatchInfo.wincount;
		mydata.lostcount = GlobalInstance::myMatchInfo.lostcount;
		m_herodata = mydata;

		ranktext->setVisible(false);
		rankimg->setVisible(false);
		myranktext->setVisible(true);
		itembg->loadTexture("ui/myheroitem.png", cocos2d::ui::Widget::TextureResType::PLIST);
		namelbl->setString(GlobalInstance::getInstance()->getMyNickName());
		std::string str = StringUtils::format("%d", GlobalInstance::myMatchInfo.matchscore);
		point->setString(str);
		str = StringUtils::format("matchlvname_%d", GlobalInstance::getInstance()->getMatchLvByScroe(GlobalInstance::myMatchInfo.matchscore));
		division->setString(ResourceLang::map_lang[str]);
		herobox->loadTexture("ui/main_menu_box.png", cocos2d::ui::Widget::TextureResType::PLIST);
		str = StringUtils::format("ui/h_%d_0.png", DataSave::getInstance()->getHeadId());
		heroimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
		if (GlobalInstance::myRankInfo.myrank < 0)
		{
			myrank->setString(ResourceLang::map_lang["notrank"]);
		}
		else
		{
			str = StringUtils::format("%d", GlobalInstance::myRankInfo.myrank + 1);
			myrank->setString(str);
		}
	}
	else
	{
		myranktext->setVisible(false);
		namelbl->setString(herodata.nickname);
		std::string str = StringUtils::format("%d", herodata.matchscore);
		point->setString(str);
		str = StringUtils::format("matchlvname_%d", GlobalInstance::getInstance()->getMatchLvByScroe(herodata.matchscore));
		division->setString(ResourceLang::map_lang[str]);
		str = getHeroPotentialHeadImg();
		herobox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
		str = getFirstHeroId();
		heroimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
		if (herodata.rank <= 2)
		{
			ranktext->setVisible(false);
			rankimg->setVisible(true);
			str = StringUtils::format("ui/rank_%d.png", herodata.rank + 1);
			rankimg->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);
		}
		else
		{
			ranktext->setVisible(true);
			rankimg->setVisible(false);
			str = StringUtils::format("%d", herodata.rank + 1);
			ranktext->setString(str);
		}
		if (herodata.rank < 10)
		{
			actbtntxt->loadTexture(ResourcePath::makeTextImgPath("matchfight_text", langtype), cocos2d::ui::Widget::TextureResType::PLIST);
			actbtn->setTag(1001);
		}

		if (herodata.playerid.compare(GlobalInstance::getInstance()->UUID()) == 0)
		{
			actbtn->setVisible(false);
		}
	}

	return true;
}

std::string MatchRankNode::getFirstHeroId()
{
	std::string str = "ui/h_0_0.png";
	std::map<std::string, std::string>::iterator it;
	for (it = m_herodata.map_otherheros.begin(); it != m_herodata.map_otherheros.end(); it++)
	{
		if (it->second.length() > 0)
		{
			std::vector<std::string> vec_heros;
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(it->second, vec_heros, ";");
			CommonFuncs::split(vec_heros[0], vec_tmp, "-");

			int vocation = atoi(vec_tmp[2].c_str());
			int sex = atoi(vec_tmp[4].c_str());
			str = StringUtils::format("ui/h_%d_%d.png", vocation, sex);
			break;
		}
	}
	return str;
}

std::string MatchRankNode::getHeroPotentialHeadImg()
{
	std::string str = "ui/main_menu_box.png";
	std::map<std::string, std::string>::iterator it;

	int qu[5] = { 0 };
	for (it = m_herodata.map_otherheros.begin(); it != m_herodata.map_otherheros.end(); it++)
	{
		if (it->second.length() > 0)
		{
			std::vector<std::string> vec_heros;
			std::vector<std::string> vec_tmp;
			CommonFuncs::split(it->second, vec_heros, ";");
			CommonFuncs::split(vec_heros[0], vec_tmp, "-");

			int potential = atoi(vec_tmp[3].c_str());
			qu[potential]++;

		}
	}
	int squ = 0;
	if (qu[4] >= 4)
	{
		squ = 4;
	}
	else if (qu[4] + qu[3] >= 4)
	{
		squ = 3;
	}
	else if (qu[3] + qu[2] >= 4)
	{
		squ = 2;
	}
	else if (qu[2] + qu[1] >= 4)
	{
		squ = 1;
	}
	str = StringUtils::format("ui/herobox_%d.png", squ);
	return str;
}

void MatchRankNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	Node* clicknode = (Node*)pSender;
	
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
			MatchContentLayer* layer = MatchContentLayer::create(m_herodata);
			g_mainScene->addChild(layer);
			AnimationEffect::openAniEffect((Layer*)layer);
		}
		else
		{
			if (GlobalInstance::myRankInfo.myrank <= 10 && GlobalInstance::myRankInfo.myrank >= 0)
			{
				if (g_mainScene != NULL)
				{
					MatchMainLayer* mlayer = (MatchMainLayer*)g_mainScene->getChildByName("8pkground");
					if (mlayer != NULL)
					{
						mlayer->getMatchVsPairData(m_herodata.playerid);

					}
				}
			}
			else
			{
				MovingLabel::show(ResourceLang::map_lang["matchfightlimit"]);
			}
		}
	}
}