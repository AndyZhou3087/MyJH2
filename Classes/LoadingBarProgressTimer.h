
#ifndef ___LOADINGBAR_CCPROGRESS_TIMER_H__
#define ___LOADINGBAR_CCPROGRESS_TIMER_H__

#include "2d/CCActionInterval.h"
#include "cocos2d.h"
NS_CC_BEGIN

class LoadingBarProgressTo : public ActionInterval
{
public:

	static LoadingBarProgressTo* create(float duration, float percent);

    //
    // Overrides
    //
	virtual LoadingBarProgressTo* clone() const override;
	virtual LoadingBarProgressTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
	virtual void update(float time);
    
	LoadingBarProgressTo();
	~LoadingBarProgressTo();

    /** 
     * @brief Initializes with a duration and destination percentage. 
     * @param duration Specify the duration of the ProgressTo action. It's a value in seconds.
     * @param percent Specify the destination percentage.
     * @return If the creation success, return true; otherwise, return false.
     */
    bool initWithDuration(float duration, float percent);

protected:
    float _to;
    float _from;

private:
	CC_DISALLOW_COPY_AND_ASSIGN(LoadingBarProgressTo);
};


class LoadingBarProgressFromTo : public ActionInterval
{
public:

	static LoadingBarProgressFromTo* create(float duration, float fromPercentage, float toPercentage);

    //
    // Overrides
    //
	virtual LoadingBarProgressFromTo* clone() const override;
	virtual LoadingBarProgressFromTo* reverse() const override;
    virtual void startWithTarget(Node *target) override;
	virtual void update(float time);
    
	LoadingBarProgressFromTo();
	~LoadingBarProgressFromTo();

    /**
     * @brief Initializes the action with a duration, a "from" percentage and a "to" percentage.
     * @param duration Specify the duration of the ProgressFromTo action. It's a value in seconds.
     * @param fromPercentage Specify the source percentage.
     * @param toPercentage Specify the destination percentage.
     * @return If the creation success, return true; otherwise, return false.
     */
    bool initWithDuration(float duration, float fromPercentage, float toPercentage);

protected:
    float _to;
    float _from;

private:
	CC_DISALLOW_COPY_AND_ASSIGN(LoadingBarProgressFromTo);
};

// end of actions group
/// @}

NS_CC_END

#endif // __MY_ACTION_CCPROGRESS_TIMER_H__
