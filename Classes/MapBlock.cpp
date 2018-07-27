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

void MapBlock::setTextureCoords(const Rect& rectInPoints, V3F_C4B_T2F_Quad* outQuad)
{
	Texture2D *tex = (_renderMode == RenderMode::QUAD_BATCHNODE) ? _textureAtlas->getTexture() : _texture;
	if (tex == nullptr)
	{
		return;
	}

	const auto rectInPixels = CC_RECT_POINTS_TO_PIXELS(rectInPoints);

	const float atlasWidth = (float)tex->getPixelsWide();
	const float atlasHeight = (float)tex->getPixelsHigh();

	float rw = rectInPixels.size.width;
	float rh = rectInPixels.size.height;

	// if the rect is rotated, it means that the frame is rotated 90 degrees (clockwise) and:
	//  - rectInpoints: origin will be the bottom-left of the frame (and not the top-right)
	//  - size: represents the unrotated texture size
	//
	// so what we have to do is:
	//  - swap texture width and height
	//  - take into account the origin
	//  - flip X instead of Y when flipY is enabled
	//  - flip Y instead of X when flipX is enabled

	if (_rectRotated)
		std::swap(rw, rh);

#if 0//CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
	float left = (2 * rectInPixels.origin.x + 1) / (2 * atlasWidth);
	float right = left + (rw * 2 - 2) / (2 * atlasWidth);
	float top = (2 * rectInPixels.origin.y + 1) / (2 * atlasHeight);
	float bottom = top + (rh * 2 - 2) / (2 * atlasHeight);
#else
	float left = rectInPixels.origin.x / atlasWidth;
	float right = (rectInPixels.origin.x + rw) / atlasWidth;
	float top = rectInPixels.origin.y / atlasHeight;
	float bottom = (rectInPixels.origin.y + rh) / atlasHeight;
#endif // CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL

	if ((!_rectRotated && _flippedX) || (_rectRotated && _flippedY))
	{
		std::swap(left, right);
	}

	if ((!_rectRotated && _flippedY) || (_rectRotated && _flippedX))
	{
		std::swap(top, bottom);
	}

	if (_rectRotated)
	{
		outQuad->bl.texCoords.u = left;
		outQuad->bl.texCoords.v = top;
		outQuad->br.texCoords.u = left;
		outQuad->br.texCoords.v = bottom;
		outQuad->tl.texCoords.u = right;
		outQuad->tl.texCoords.v = top;
		outQuad->tr.texCoords.u = right;
		outQuad->tr.texCoords.v = bottom;
	}
	else
	{
		outQuad->bl.texCoords.u = left;
		outQuad->bl.texCoords.v = bottom;
		outQuad->br.texCoords.u = right;
		outQuad->br.texCoords.v = bottom;
		outQuad->tl.texCoords.u = left;
		outQuad->tl.texCoords.v = top;
		outQuad->tr.texCoords.u = right;
		outQuad->tr.texCoords.v = top;
	}
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
	if (buildname.length() > 0)
	{
		std::string filename = StringUtils::format("mapui/buildblock_%s", buildname.c_str());
		Sprite* buildblock = Sprite::createWithSpriteFrameName(filename);
		buildblock->setAnchorPoint(Vec2(0, 1));
		buildblock->setPosition(Vec2(Col*MAPBLOCKWIDTH, (Row + 1)*MAPBLOCKHEIGHT));
		this->getParent()->addChild(buildblock, this->getLocalZOrder() + 10000);
	}
}
