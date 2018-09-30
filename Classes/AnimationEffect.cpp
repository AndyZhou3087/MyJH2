
#include "AnimationEffect.h"
#include "CannotTouchLayer.h"

USING_NS_CC;

void AnimationEffect::openAniEffect(cocos2d::Layer* layer)
{

	CannotTouchLayer* notTouchLayer = CannotTouchLayer::create();
	notTouchLayer->setScale(3);
	layer->getParent()->addChild(notTouchLayer, 10000);

	LayerColor* color = (LayerColor*)layer->getChildByName("colorLayer");
	if (color != NULL)
	{
		color->setScale(3);
	}
	layer->setCascadeOpacityEnabled(true);
	layer->setOpacity(0);
	layer->setScale(0.8f);
	layer->runAction(Spawn::create(FadeIn::create(0.3f), Sequence::create(ScaleTo::create(0.15f, 1.05f), ScaleTo::create(0.15f, 1.0f), NULL), NULL));
	notTouchLayer->runAction(Sequence::create(DelayTime::create(0.3f), RemoveSelf::create(), NULL));
}

void AnimationEffect::closeAniEffect(cocos2d::Layer* layer)
{
	LayerColor* color = (LayerColor*)layer->getChildByName("colorLayer");
	if (color!=NULL)
	{
		color->runAction(FadeOut::create(0.15f));
	}
	layer->runAction(Sequence::create(ScaleTo::create(0.12f, 1.05f), ScaleTo::create(0.07f, 0.6f), Hide::create(), DelayTime::create(0.03f), RemoveSelf::create(), NULL));
}