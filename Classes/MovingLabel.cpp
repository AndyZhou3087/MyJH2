#include "MovingLabel.h"
#include "Const.h"
#include "GlobalInstance.h"
#include "Resource.h"
#include "MainScene.h"

std::queue<MovingLabel*> MovingLabel::queue_labels;
Node* MovingLabel::lastRunningScene = NULL;

MovingLabel::MovingLabel()
{
	ismoving = false;
}

MovingLabel::~MovingLabel()
{

}

void MovingLabel::show(std::string text, Color4B color, Vec2 pos, Node* parent)
{
	if (lastRunningScene != Director::getInstance()->getRunningScene())
	{
		lastRunningScene = Director::getInstance()->getRunningScene();
		while (!queue_labels.empty()) queue_labels.pop();
	}

	if (queue_labels.size() <= 5)
	{
		MovingLabel* label = MovingLabel::create(text, color, pos);
		queue_labels.push(label);
		if (parent == NULL)
		{
			Director::getInstance()->getRunningScene()->addChild(label, 100000);
		}
		else
			parent->addChild(label, 100000);

		if (queue_labels.size() == 1)
		{
			queue_labels.front()->showAction();
		}
	}
}

void MovingLabel::showbyNode(Node* node, std::string text, Color4B color, Vec2 pos)
{
	show(text, color, pos, node);
}

MovingLabel* MovingLabel::create(std::string text, Color4B color, Vec2 pos)
{
	MovingLabel *pRet = new(std::nothrow)MovingLabel();
	if (pRet && pRet->init(text, color, pos))
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

bool MovingLabel::init(std::string text, Color4B color, Vec2 pos)
{
	if (!Sprite::initWithFile(ResourcePath::makeImagePath("blankdot.png")))
		return false;

	mlbl = Label::createWithTTF(text, FONT_NAME, 25);

	mlbl->setTextColor(color);
	//mlbl->enableOutline(Color4B(255,255,255, 200), 1);
	//mlbl->enableShadow();

	mlbl->setAlignment(TextHAlignment::CENTER);
	this->addChild(mlbl, 1);
	cocos2d::ui::ImageView* bg = cocos2d::ui::ImageView::create("ui/movinglblbg.png", cocos2d::ui::Widget::TextureResType::PLIST);
	bg->setScale9Enabled(true);
	bg->setContentSize(Size(mlbl->getContentSize().width + 60, mlbl->getContentSize().height + 12));
	this->addChild(bg, 0);

	this->setPosition(pos);
	this->setScale(0.7f);
	this->setVisible(false);
	return true;
}

void MovingLabel::showAction()
{
	if (ismoving)
		return;

	ismoving = true;

	changeTextColor();

	this->setVisible(true);
	FiniteTimeAction* scales = Spawn::create(ScaleTo::create(0.2f, 1.0f), MoveBy::create(0.2f, Vec2(0, 60)), NULL);
	FiniteTimeAction* s1 = Sequence::create(scales, DelayTime::create(0.7f), Hide::create(), CallFunc::create(CC_CALLBACK_0(MovingLabel::showNext, this)), NULL);

	this->runAction(Sequence::create(s1, CallFunc::create(CC_CALLBACK_0(MovingLabel::removeSelf, this)), NULL));
}

void MovingLabel::changeTextColor()
{
	std::string lbltext = mlbl->getString();

	std::u32string utf32lblString;
	StringUtils::UTF8ToUTF32(lbltext, utf32lblString);

	//std::map<std::string, AllResources>::iterator it;
	//for (it = GlobalInstance::map_AllResources.begin(); it != GlobalInstance::map_AllResources.end(); it++)
	//{
	//	std::string resname = GlobalInstance::map_AllResources[it->first].name;

	//	std::u32string utf32ResString;
	//	StringUtils::UTF8ToUTF32(resname, utf32ResString);

	//	std::size_t findpos = utf32lblString.find(utf32ResString);
	//	if (findpos != std::string::npos) 
	//	{
	//		for (int i = findpos; i < (findpos + utf32ResString.length()); i++)
	//		{
	//			this->getLetter(i)->setColor(Color3B(255, 61, 61));
	//		}
	//		break;
	//	}
	//}

	if (lbltext.find(ResourceLang::map_lang["makesucc"]) != std::string::npos || lbltext.find(ResourceLang::map_lang["useequipcardhint"]) != std::string::npos)
	{
		std::size_t findpos = std::string::npos;
		for (int i = 0; i < 5; i++)
		{
			std::string qukey = StringUtils::format("potential_%d", i);
			std::string qustr = ResourceLang::map_lang[qukey];

			std::u32string utf32QuString;
			StringUtils::UTF8ToUTF32(qustr, utf32QuString);

			findpos = utf32lblString.substr(utf32lblString.length() - 2).find_last_of(utf32QuString);
			if (findpos != std::string::npos)
			{
				findpos = utf32lblString.length() - 2 + findpos;
				for (unsigned int m = findpos; m < (findpos + utf32QuString.length()); m++)
				{
					mlbl->getLetter(m)->setColor(POTENTIALCOLOR[i]);
				}
				break;
			}
		}
	}
}

void MovingLabel::showNext()
{
	queue_labels.pop();
	if (queue_labels.size() > 0)
		queue_labels.front()->showAction();
}

void MovingLabel::removeSelf()
{
	this->removeFromParentAndCleanup(true);
}