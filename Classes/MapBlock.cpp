#include "MapBlock.h"
#include "Const.h"

std::vector<FOURProperty> MapBlock::vec_randMonsters;
std::vector<FOURProperty> MapBlock::vec_randMonstersRes;
int MapBlock::randMonstersMinCount= 0;
int MapBlock::randMonstersMaxCount = 0;

MapBlock::MapBlock()
{
	m_postype = -1;
	m_iscansee = false;
}


MapBlock::~MapBlock()
{
}

MapBlock* MapBlock::create(int row, int col, std::string boradName)
{
	MapBlock *pRet = new(std::nothrow)MapBlock();
	if (pRet && pRet->init(row, col, boradName))
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

bool MapBlock::init(int row, int col, std::string boardName)
{
	Row = row;
	Col = col;
	std::string filename = StringUtils::format("mapui/boardblock_%s", boardName.c_str());
	if (Sprite::initWithSpriteFrameName(filename))
	{
		setAnchorPoint(Vec2(0, 0));
		setPosition(Vec2(col*MAPBLOCKWIDTH, row*MAPBLOCKHEIGHT));

		return true;
	}
	return false;
}

void MapBlock::setBuilding(std::string buildname)
{
	m_buildname = buildname;
	if (buildname.length() > 0)
	{
		std::string filename = StringUtils::format("mapui/buildblock_%s", buildname.c_str());
		Sprite* buildblock = Sprite::createWithSpriteFrameName(filename);
		buildblock->setAnchorPoint(Vec2(0, 1));
		buildblock->setPosition(Vec2(Col*MAPBLOCKWIDTH, (Row + 1)*MAPBLOCKHEIGHT));
		this->getParent()->addChild(buildblock, this->getLocalZOrder() + 10000);
	}
}

void MapBlock::setPosIcon()
{
	std::string posiconname = StringUtils::format("mappos/postype%d.csb", m_postype);
	auto posicon = CSLoader::createNode(posiconname);

	if (m_buildname.compare("26") == 0 || m_buildname.compare("27") == 0)//特殊处理草-可以占在草里，不这样，桥上有问题
	{
		posicon->setPosition(Vec2(this->getContentSize().width / 2, this->getContentSize().height / 2 - 10));
		this->addChild(posicon, 0, "posicon");
	}
	else
	{
		posicon->setPosition(Vec2(Col*MAPBLOCKWIDTH + MAPBLOCKWIDTH/2, Row*MAPBLOCKHEIGHT + MAPBLOCKHEIGHT/2));
		std::string posname = StringUtils::format("posicon%d", this->getLocalZOrder());
		this->getParent()->addChild(posicon, this->getLocalZOrder() + 10001, posname);
	}

	auto action = CSLoader::createTimeline(posiconname);
	posicon->runAction(action);
	action->gotoFrameAndPlay(0, true);
}

void MapBlock::removePosIcon()
{
	if (m_buildname.compare("26") == 0 || m_buildname.compare("27") == 0)//特殊处理草-可以占在草里，不这样，桥上有问题
		this->removeChildByName("posicon");
	else
	{
		std::string posname = StringUtils::format("posicon%d", this->getLocalZOrder());
		this->getParent()->removeChildByName(posname);
	}
}