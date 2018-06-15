#include "MapBlock.h"
#include "CommonFuncs.h"
#include "GlobalInstance.h"
#include "Const.h"

MapBlock::MapBlock()
{
	m_postype = -1;
}


MapBlock::~MapBlock()
{

}

MapBlock* MapBlock::create(int row, int col, std::string boradName, std::string buildname)
{
	MapBlock *pRet = new(std::nothrow)MapBlock();
	if (pRet && pRet->init(row, col, boradName, buildname))
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

bool MapBlock::init(int row, int col, std::string boardName, std::string buildname)
{
	Row = row;
	Col = col;
	std::string filename = StringUtils::format("mapui/boardblock_%s", boardName.c_str());
	if (Sprite::initWithSpriteFrameName(filename))
	{
		setAnchorPoint(Vec2(0, 0));
		setPosition(Vec2(col*MAPBLOCKWIDTH, row*MAPBLOCKHEIGHT));

		if (buildname.length() > 0)
		{
			std::string filename = StringUtils::format("mapui/buildblock_%s", buildname.c_str());
			Sprite* buildblock = Sprite::createWithSpriteFrameName(filename);
			buildblock->setAnchorPoint(Vec2(0, 1));
			buildblock->setPosition(Vec2(0, MAPBLOCKHEIGHT));
			this->addChild(buildblock);
		}
		return true;
	}
	return false;
}