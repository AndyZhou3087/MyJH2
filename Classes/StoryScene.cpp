#include "StoryScene.h"
#include "Resource.h"
#include "CommonFuncs.h"
#include "Const.h"
#include "MapBlockScene.h"
#include "NewGuideLayer.h"

StoryScene* storylayer = NULL;
StoryScene::StoryScene()
{
	isShowOver = false;
	showindex = 0;
	wordcount = 0;
	clickcount = 0;
}

StoryScene::~StoryScene()
{

}

Scene* StoryScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
	storylayer = StoryScene::create();

    // add layer as a child to scene
	scene->addChild(storylayer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool StoryScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	Node* csbnode = CSLoader::createNode(ResourcePath::makePath("storyLayer.csb"));
	this->addChild(csbnode);

	loadingclicktext = csbnode->getChildByName("loadingclicktext");
	loadingclicktext->setVisible(false);

	bg = (cocos2d::ui::ImageView*)csbnode->getChildByName("bg");
	bg->addTouchEventListener(CC_CALLBACK_2(StoryScene::onClick, this));

	std::string storytext = FileUtils::getInstance()->getStringFromFile(ResourcePath::makePath("story.txt"));

	std::string spitstr = "\n";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	spitstr = "\r\n";
#endif
	std::vector<std::string> vec_text;
	CommonFuncs::split(storytext, vec_text, spitstr);

	for (unsigned int i = 0; i < vec_text.size(); i++)
	{
		Label *countlbl = Label::createWithTTF(vec_text[i], FONT_NAME, 23);
		countlbl->setColor(Color3B::WHITE);
		countlbl->setPosition(Vec2(360, 1200-i*45));
		countlbl->setVisible(false);
		this->addChild(countlbl);
		vec_labels.push_back(countlbl);
	}
	
	storylines = vec_labels.size();

	bg->setEnabled(false);
	this->scheduleOnce(schedule_selector(StoryScene::delayShowText), 0.5f);

    return true;
}

void StoryScene::delayShowText(float dt)
{
	curshowlabel = vec_labels[showindex];
	int index = 0;
	while (curshowlabel->getLetter(index) != NULL)
	{
		curshowlabel->getLetter(index)->setScale(0);
		index++;
	}
	curshowlabel->setVisible(true);
	bg->setEnabled(true);
	curshowlabel->schedule([&](float dt) {
		curshowlabel->getLetter(wordcount)->setScale(1.0f);
		wordcount += 1;
		std::u32string utf32String;
		StringUtils::UTF8ToUTF32(curshowlabel->getString(), utf32String);
		int len = utf32String.length();
		if (wordcount >= len)
		{
			showindex++;
			wordcount = 0;
			curshowlabel->unschedule("schedule_typecallback");
			if (showindex < storylines)
			{
				bg->setEnabled(false);
				storylayer->scheduleOnce(schedule_selector(StoryScene::delayShowText), 0.3f);
			}
			else
			{
				textShowOver();
			}
		}
	}, 0.1f, "schedule_typecallback");
}

void StoryScene::onClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	if (type == ui::Widget::TouchEventType::ENDED)
	{
		Node* node = (Node*)pSender;

		if (isShowOver)
		{
			clickcount++;

			if (clickcount < 2)
				return;

			bg->setEnabled(false);
			NewGuideLayer::setNewGuideInfo(FIRSTGUIDESTEP);
			Director::getInstance()->replaceScene(TransitionFade::create(1.0f, MainScene::createScene()));
		}
		else
		{
			curshowlabel->unschedule("schedule_typecallback");
			storylayer->unschedule(schedule_selector(StoryScene::delayShowText));

			int labelsize = vec_labels.size();
			if (showindex < labelsize)
			{
				int index = 0;
				while (vec_labels[showindex]->getLetter(index) != NULL)
				{
					vec_labels[showindex]->getLetter(index)->setScale(1);
					index++;
				}
			}

			for (unsigned int i = showindex + 1; i < vec_labels.size(); i++)
			{
				vec_labels[i]->setVisible(true);
			}
			textShowOver();

			//if (showindex >= storylines)
			//	showindex = storylines - 1;

			//for (int i = 0; i <= showindex; i++)
			//{
			//	int index = 0;
			//	while (vec_labels[i]->getLetter(index) != NULL)
			//	{
			//		vec_labels[i]->getLetter(index)->setScale(1);
			//		index++;
			//	}
			//	vec_labels[i]->setVisible(true);
			//}
			//showindex++;
			//wordcount = 0;
			//if (showindex >= storylines)
			//{
			//	textShowOver();
			//}
			//else
			//{
			//	bg->setEnabled(false);
			//	storylayer->scheduleOnce(schedule_selector(StoryScene::delayShowText), 0.1f);
			//}
		}
	}
}

void StoryScene::textShowOver()
{
	bg->setEnabled(true);
	isShowOver = true;
	loadingclicktext->setVisible(true);
	loadingclicktext->runAction(RepeatForever::create(Sequence::create(FadeOut::create(1), FadeIn::create(1), NULL)));
}

void StoryScene::onExit()
{
	Layer::onExit();
}
