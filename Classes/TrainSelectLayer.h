#pragma once
#ifndef __TrainSelect_LAYER_H__
#define __TrainSelect_LAYER_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "Building.h"
#include "Hero.h"
#include "MyHeroNode.h"
USING_NS_CC;

class TrainSelectLayer : public cocos2d::Layer
{
public:
	TrainSelectLayer();
	~TrainSelectLayer();

	static TrainSelectLayer* create(Hero* herodata, MyHeroNode* myNode);
	virtual void onExit();
	bool init(Hero* herodata, MyHeroNode* myNode);
private:
	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
	void onImgClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);

	//����UI,��ֵ
	void updateUI();

	//��Դ�㹻����
	bool checkResIsEnough(int index);

	//����
	void action();

	void updatelbl(float dt);
private:
	cocos2d::ui::ImageView* selArr[3];//ѡ�б�ǩ
	cocos2d::ui::Text* countlbl[3];//
	cocos2d::ui::Text* coincountlbl;
	cocos2d::ui::Text* myfcoutlbl;
	Hero* m_data;//��Ҫ���ݵ�����
	std::vector<std::map<std::string, int>> vec_res;//��Ҫ����Դ������
	DynamicValueInt coincount;
	int lastSelectIndex;
	MyHeroNode* myheroNode;
};

#endif
