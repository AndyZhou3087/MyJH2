#include "TakeOnLayer.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "SoundManager.h"
#include "Resource.h"
#include "GlobalInstance.h"
#include "Equip.h"
#include "json.h"
#include "MyRes.h"

TakeOnLayer::TakeOnLayer()
{

}


TakeOnLayer::~TakeOnLayer()
{

}


TakeOnLayer* TakeOnLayer::create(Equip* res_equip)
{
	TakeOnLayer *pRet = new(std::nothrow)TakeOnLayer();
	if (pRet && pRet->init(res_equip))
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

bool TakeOnLayer::init(Equip* res_equip)
{
	LayerColor* color = LayerColor::create(Color4B(11, 32, 22, 200));
	this->addChild(color);

	Node *csbnode = CSLoader::createNode(ResourcePath::makePath("takeOnLayer.csb"));
	this->addChild(csbnode);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	m_equip = res_equip;

	cocos2d::ui::Widget* closebtn = (cocos2d::ui::Widget*)csbnode->getChildByName("closebtn");
	closebtn->setTag(1000);
	closebtn->addTouchEventListener(CC_CALLBACK_2(TakeOnLayer::onBtnClick, this));

	int qu = m_equip->getQU().getValue();

	std::string str;
	cocos2d::ui::Text* name = (cocos2d::ui::Text*)csbnode->getChildByName("name");
	str = StringUtils::format("+%d%s", m_equip->getLv().getValue(), GlobalInstance::map_AllResources[m_equip->getId()].name.c_str());
	name->setString(str);
	name->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	cocos2d::ui::ImageView* resbox = (cocos2d::ui::ImageView*)csbnode->getChildByName("resbox");
	str = StringUtils::format("ui/resbox_qu%d.png", qu);
	resbox->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::ImageView* p_res = (cocos2d::ui::ImageView*)csbnode->getChildByName("res");
	str = StringUtils::format("ui/%s.png", m_equip->getId().c_str());
	if (qu == 3)
	{
		str = StringUtils::format("ui/%s_2.png", m_equip->getId().c_str());
	}
	else if (qu == 4)
	{
		str = StringUtils::format("ui/%s_3.png", m_equip->getId().c_str());
	}
	p_res->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

	cocos2d::ui::Text* qutext = (cocos2d::ui::Text*)csbnode->getChildByName("qutext");
	qutext->setString(ResourceLang::map_lang["potentialtext"]);

	cocos2d::ui::Text* qulbl = (cocos2d::ui::Text*)csbnode->getChildByName("qu");
	std::string st = StringUtils::format("potential_%d", qu);
	qulbl->setString(ResourceLang::map_lang[st]);
	qulbl->setTextColor(Color4B(POTENTIALCOLOR[qu]));

	cocos2d::ui::Text* desc = (cocos2d::ui::Text*)csbnode->getChildByName("desc");
	desc->setString(GlobalInstance::map_AllResources[m_equip->getId()].desc);

	parseSuitJson();

	if (map_suit.find(m_equip->getId()) != map_suit.end())
	{
		for (unsigned int i = 0; i < 3; i++)
		{
			str = StringUtils::format("desc_%d", i);
			cocos2d::ui::Text* suitresdesc = (cocos2d::ui::Text*)csbnode->getChildByName(str);

			str = StringUtils::format("resbox_%d", i);
			cocos2d::ui::ImageView* suitresbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

			str = StringUtils::format("res_%d", i);
			cocos2d::ui::ImageView* suitres = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);

			if (i < map_suit[m_equip->getId()].vec_suit.size())
			{
				std::string eid = map_suit[m_equip->getId()].vec_suit[i];
				str = StringUtils::format("ui/%s.png", eid.c_str());
				suitres->loadTexture(str, cocos2d::ui::Widget::TextureResType::PLIST);

				if (i > 0)
				{

					std::string attrstr;
					if (i == 1)
						attrstr = "attrtext_0";
					else if (i == 2)
						attrstr = "attrtext_2";
					std::string tmpstr = ResourceLang::map_lang[attrstr];
					std::string tmpstr1 = StringUtils::format("suit%d", i);
					//去掉冒号，UTF8中文占3个字符
					std::string s2 = tmpstr.substr(0, tmpstr.length() - 3);
					str = StringUtils::format("%s%s+%.2f%%", ResourceLang::map_lang[tmpstr1].c_str(), s2.c_str(), map_suit[m_equip->getId()].vec_bns[i-1]);
					suitresdesc->setString(str);
					if (MyRes::getMyResCount(eid) <= 0)
					{
						suitresdesc->setOpacity(128);
						suitres->setOpacity(128);
						suitresbox->setOpacity(128);
					}
				}
			}
			else
			{
				suitres->setOpacity(128);
				suitresbox->setOpacity(128);
			}
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			str = StringUtils::format("resbox_%d", i);
			cocos2d::ui::ImageView* suitresbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			suitresbox->setOpacity(128);
			str = StringUtils::format("res_%d", i);
			cocos2d::ui::ImageView* suitres = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
			suitres->setVisible(false);
		}
	}

	for (int i = 0; i < 3; i++)
	{
		str = StringUtils::format("stone%d", i);
		cocos2d::ui::ImageView* suitresbox = (cocos2d::ui::ImageView*)csbnode->getChildByName(str);
		if (i < m_equip->vec_stones.size())
		{

		}
		else
		{

		}
	}

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [=](Touch *touch, Event *event)
	{
		return true;
	};
	listener->setSwallowTouches(true);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


	return true;
}


void TakeOnLayer::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* btnnode = (Node*)pSender;
		int tag = btnnode->getTag();
		switch (tag)
		{
		case 1000:
		{
			this->removeFromParentAndCleanup(true);
			break;
		}
		default:
			break;
		}
	}
}

void TakeOnLayer::onclick(Ref* pSender)
{
	SoundManager::getInstance()->playSound(SoundManager::SOUND_ID_BUTTON);
	Node* node = (Node*)pSender;
}

void TakeOnLayer::parseSuitJson()
{
	rapidjson::Document doc = ReadJsonFile(ResourcePath::makePath("json/suitequip.json"));
	rapidjson::Value& allData = doc["sq"];
	for (unsigned int i = 0; i < allData.Size(); i++)
	{
		rapidjson::Value& jsonvalue = allData[i];
		if (jsonvalue.IsObject())
		{
			EquipSuit data;
			rapidjson::Value& v = jsonvalue["id"];
			data.id = v.GetString();

			v = jsonvalue["suite"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_suit.push_back(v[m].GetString());
			}
			v = jsonvalue["bns"];
			for (unsigned int m = 0; m < v.Size(); m++)
			{
				data.vec_bns.push_back(v[m].GetDouble());
			}
			map_suit[data.id] = data;
		}
	}
}