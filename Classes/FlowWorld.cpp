#include "FlowWorld.h"
#include "Const.h"
#include "GlobalInstance.h"
#include "Resource.h"

FlowWorld::FlowWorld()
{
	_holder = NULL;
	_newsBg = NULL;
	_news = NULL;
	_shap = NULL;
	_cliper = NULL;
}

FlowWorld::~FlowWorld() {}

FlowWorld* FlowWorld::fWorld = NULL;

FlowWorld* FlowWorld::getIntance()
{
	if (!fWorld)
	{
		fWorld = new FlowWorld();
	}
	return fWorld;
}

FlowWorld* FlowWorld::createFW(Sprite* sp)
{
	_newsBg = sp;
	Point p = Point(0.0, 0.0f);
	float w = sp->getContentSize().width;
	float h = sp->getContentSize().height;
	auto shap = DrawNode::create();
	shap->clear();

	//标记显示的矩形区域
	Point point[4] = { Point(p.x, p.y), Point(p.x + w, p.y), Point(p.x + w, p.y + h), Point(p.x, h) };//标记显示的矩形区域
	shap->drawPolygon(point, 4, Color4F(255, 255, 255, 255), 2, Color4F(255, 255, 255, 255));
	shap->setColor(Color3B::RED);//剪切用于显示的矩形区域，参数对应；用于标识区域的Point(Vec2)数组指针，数组长度，填充颜色，边框宽度，边框颜色。
		
	auto cliper = ClippingNode::create();
	cliper->setStencil(shap);
	cliper->setAnchorPoint(Point(0.5, 0.5));
	std::string newstr = GlobalInstance::vec_news[0];
	GlobalInstance::vec_news.erase(GlobalInstance::vec_news.begin());
	_news = Label::createWithTTF(newstr, FONT_NAME, 23);
	_news->setColor(Color3B(255, 255, 255));
	_news->setPosition(Point(w, h/2));
	_news->setAnchorPoint(Vec2(0, 0.5));
	_news->enableOutline(Color4B(63, 6, 24, 6), 2);
	cliper->addChild(_news);
	cliper->addChild(shap);
	sp->addChild(cliper);

	changeTextColor(newstr);

	schedule(schedule_selector(FlowWorld::scrollText));//实现公告文字滚动
	return this;
}

void FlowWorld::scrollText(float)
{
	if (_news->getPosition().x < (-1 * _news->getContentSize().width))
	{
		if (GlobalInstance::vec_news.size() > 0)
		{
			std::string newstr = GlobalInstance::vec_news[0];
			GlobalInstance::vec_news.erase(GlobalInstance::vec_news.begin());
			nextWord(newstr);
		}
		else
		{
			_newsBg->setVisible(false);
			_newsBg->removeAllChildrenWithCleanup(true);
			this->removeFromParentAndCleanup(true);
		}
	}
	else
	{
		 _news->setPositionX(_news->getPositionX() - 2);
	}
}

void FlowWorld::nextWord(std::string str)
{
	std::u32string utf32lblString;
	StringUtils::UTF8ToUTF32(_news->getString(), utf32lblString);
	for (unsigned int i = 0; i < utf32lblString.length(); i++)
	{
		_news->getLetter(i)->setColor(Color3B(255, 255, 255));
	}
	_news->setPositionX(720);
	_news->setString(str);
	changeTextColor(str);
}

void FlowWorld::changeTextColor(std::string str)
{
	std::u32string utf32lblString;
	StringUtils::UTF8ToUTF32(str, utf32lblString);
	std::size_t findpos[4] = { 0 };
	for (unsigned int i = 0; i < utf32lblString.length(); i++)
	{
		for (int n = 0; n < 4; n++)
		{
			std::string findkey = StringUtils::format("newfindstr%d", n);
			std::u32string utf32fitString;
			StringUtils::UTF8ToUTF32(ResourceLang::map_lang[findkey], utf32fitString);
			findpos[n] = utf32lblString.find(utf32fitString);
		}
	}

	if (findpos[0] != std::string::npos && findpos[1] != std::string::npos)
	{
		for (unsigned int m = findpos[0]; m <= findpos[1]; m++)
		{
			_news->getLetter(m)->setColor(Color3B(255, 214, 24));
		}
	}


	if (findpos[2] != std::string::npos && findpos[3] != std::string::npos)
	{
		for (unsigned int m = findpos[2]; m <= findpos[3]; m++)
		{
			_news->getLetter(m)->setColor(Color3B(230, 35, 35));
		}
	}
}