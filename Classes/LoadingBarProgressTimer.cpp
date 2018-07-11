
#include "LoadingBarProgressTimer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

NS_CC_BEGIN

#define kLoadingBarCast cocos2d::ui::LoadingBar*

// implementation of MyProgressTo

LoadingBarProgressTo::LoadingBarProgressTo()
{

}

LoadingBarProgressTo::~LoadingBarProgressTo()
{

}

LoadingBarProgressTo* LoadingBarProgressTo::create(float duration, float percent)
{
	LoadingBarProgressTo *progressTo = new (std::nothrow) LoadingBarProgressTo();
    progressTo->initWithDuration(duration, percent);
    progressTo->autorelease();

	
	return progressTo;
}

bool LoadingBarProgressTo::initWithDuration(float duration, float percent)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _to = percent;

        return true;
    }

    return false;
}

LoadingBarProgressTo* LoadingBarProgressTo::clone() const
{
    // no copy constructor    
	auto a = new (std::nothrow) LoadingBarProgressTo();
    a->initWithDuration(_duration, _to);
    a->autorelease();
    return a;
}

LoadingBarProgressTo* LoadingBarProgressTo::reverse() const
{
    CCASSERT(false, "reverse() not supported in ProgressTo");
    return nullptr;
}

void LoadingBarProgressTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
	_from = ((kLoadingBarCast)(target))->getPercent();
}

void LoadingBarProgressTo::update(float time)
{
	((kLoadingBarCast)(_target))->setPercent(_from + (_to - _from) * time);
}

// implementation of ProgressFromTo

LoadingBarProgressFromTo::LoadingBarProgressFromTo()
{

}

LoadingBarProgressFromTo::~LoadingBarProgressFromTo()
{

}

LoadingBarProgressFromTo* LoadingBarProgressFromTo::create(float duration, float fromPercentage, float toPercentage)
{
	LoadingBarProgressFromTo *progressFromTo = new (std::nothrow) LoadingBarProgressFromTo();
    progressFromTo->initWithDuration(duration, fromPercentage, toPercentage);
    progressFromTo->autorelease();

    return progressFromTo;
}

bool LoadingBarProgressFromTo::initWithDuration(float duration, float fromPercentage, float toPercentage)
{
    if (ActionInterval::initWithDuration(duration))
    {
        _to = toPercentage;
        _from = fromPercentage;

        return true;
    }

    return false;
}

LoadingBarProgressFromTo* LoadingBarProgressFromTo::clone() const
{
    // no copy constructor    
	auto a = new (std::nothrow) LoadingBarProgressFromTo();
    a->initWithDuration(_duration, _from, _to);
    a->autorelease();
    return a;
}


LoadingBarProgressFromTo* LoadingBarProgressFromTo::reverse() const
{
	return LoadingBarProgressFromTo::create(_duration, _to, _from);
}

void LoadingBarProgressFromTo::startWithTarget(Node *target)
{
    ActionInterval::startWithTarget(target);
}

void LoadingBarProgressFromTo::update(float time)
{
	((kLoadingBarCast)(_target))->setPercent(_from + (_to - _from) * time);
}

NS_CC_END
