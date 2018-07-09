#include "MovingLabel.h"
#include "Const.h"

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
	if (queue_labels.size() > 0)
	{
		if (!queue_labels.front()->ismoving)
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
	this->enableOutline(Color4B(0,0,0, 255), 1);
	this->enableShadow();
	this->setScale(0.0f);
	return true;
}

void MovingLabel::showAction()
{
	ismoving = true;
	FiniteTimeAction* scales = Sequence::create(ScaleTo::create(0.2f, 1.2f), ScaleTo::create(0.1f, 1.0f), NULL);
	FiniteTimeAction* moveandout = Spawn::create(MoveBy::create(1.0f, Vec2(0, -50)), FadeOut::create(1.0f), NULL);
	this->runAction(Sequence::create(scales, moveandout, CallFunc::create(CC_CALLBACK_0(MovingLabel::removeSelf, this)), NULL));
}

void MovingLabel::removeSelf()
{
	queue_labels.pop();
	this->removeFromParentAndCleanup(true);
	if (queue_labels.size() > 0)
	{
		queue_labels.front()->showAction();
	}
}