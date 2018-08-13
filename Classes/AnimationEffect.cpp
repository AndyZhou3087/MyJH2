
#include "AnimationEffect.h"

USING_NS_CC;

void AnimationEffect::openAniEffect(cocos2d::Layer* layer)
{
	layer->setCascadeOpacityEnabled(true);
	layer->setOpacity(0);
	layer->setScale(0.8f);
	layer->runAction(Spawn::create(FadeIn::create(0.3f), Sequence::create(ScaleTo::create(0.15f, 1.05f), ScaleTo::create(0.15f, 1.0f), NULL), NULL));
}