#include "MovingLabel.h"
#include "Const.h"
#include "GlobalInstance.h"
#include "Resource.h"

std::queue<MovingLabel*> MovingLabel::queue_labels;
MovingLabel::MovingLabel()
{
	ismoving = false;
}


MovingLabel::~MovingLabel()
{

}

void MovingLabel::show(std::string text, Vec2 pos)
{
	MovingLabel* label = MovingLabel::create(text, pos);
	queue_labels.push(label);
	Director::getInstance()->getRunningScene()->addChild(label, 10);
	if (queue_labels.size() == 1)
	{
		queue_labels.front()->showAction();
	}
}

MovingLabel* MovingLabel::create(std::string text, Vec2 pos)
{
	MovingLabel *pRet = new(std::nothrow)MovingLabel();
	if (pRet && pRet->init(text, pos))
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

bool MovingLabel::init(std::string text, Vec2 pos)
{
	Label::initWithTTF(text, FONT_NAME, 25);
	setPosition(pos);
	this->enableOutline(Color4B(0,0,0, 200), 1);
	this->enableShadow();
	this->setScale(0.0f);

	return true;
}

void MovingLabel::showAction()
{
	if (ismoving)
		return;

	ismoving = true;

	changeTextColor();

	FiniteTimeAction* scales = Sequence::create(ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
	FiniteTimeAction* s1 = Sequence::create(Spawn::create(MoveBy::create(0.3f, Vec2(0, -15)), FadeTo::create(0.3f, 200), NULL), CallFunc::create(CC_CALLBACK_0(MovingLabel::showNext, this)), NULL);

	FiniteTimeAction* moveandout = Spawn::create(MoveBy::create(1.0f, Vec2(0, -50)), FadeTo::create(1.0f, 0), NULL);

	this->runAction(Sequence::create(scales, s1, moveandout, CallFunc::create(CC_CALLBACK_0(MovingLabel::removeSelf, this)), NULL));
}

void MovingLabel::changeTextColor()
{
	std::size_t findpos;
	std::string lbltext = this->getString();

	//std::map<std::string, AllResources>::iterator it;
	//for (it = GlobalInstance::map_AllResources.begin(); it != GlobalInstance::map_AllResources.end(); it++)
	//{
	//	std::string resname = GlobalInstance::map_AllResources[it->first].name;

	//	findpos = lbltext.find(resname);
	//	if (findpos != std::string::npos) 
	//	{
	//		for (int i = findpos/3; i < (findpos + resname.length())/3; i++)
	//		{
	//			this->getLetter(i)->setColor(Color3B(204, 4, 4));
	//		}
	//		break;
	//	}
	//}

	if (lbltext.find(ResourceLang::map_lang["makesucc"]) != std::string::npos)
	{
		std::size_t findpos;
		for (int i = 0; i < 5; i++)
		{
			std::string qukey = StringUtils::format("potential_%d", i);
			std::string qustr = ResourceLang::map_lang[qukey];
			findpos = lbltext.find_last_of(qustr);
			if (findpos != std::string::npos)
			{
				for (int m = findpos/3; m < (findpos + qustr.length())/3; m++)
				{
					this->getLetter(m)->setColor(POTENTIALCOLOR[i]);
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