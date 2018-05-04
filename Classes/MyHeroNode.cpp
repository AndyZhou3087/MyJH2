#include "MyHeroNode.h"
#include "CommonFuncs.h"

MyHeroNode::MyHeroNode()
{

}


MyHeroNode::~MyHeroNode()
{

}

bool MyHeroNode::init()
{
	Node* csbnode = CSLoader::createNode("topBarNode.csb");
	this->addChild(csbnode);


	return true;
}

void MyHeroNode::onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
	CommonFuncs::BtnAction(pSender, type);
	if (type == ui::Widget::TouchEventType::ENDED)
	{

	}
}