#ifndef __NewGuideLayer__
#define __NewGuideLayer__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

#define FIRSTGUIDESTEP 13
#define SECONDGUIDESTEP 21
#define THRIDGUIDESTEP 39
#define MIDELEGUIDESTEP 44
#define FOURTHGUIDESTEP 52
#define FIFTHGUIDESTEP 62

#define NEWERLAYERZOER 1999
class NewGuideLayer : public Layer
{
public:
	NewGuideLayer();
	~NewGuideLayer();
	bool init(int type, std::vector<Node*> stencilNodes);
	virtual void onExit();
	static NewGuideLayer* create(int step, std::vector<Node*> stencilNodes);
	static bool checkifNewerGuide(int index);
	static void setNewerGuide(int index);
	static void setNewGuideInfo(int step);//新手加成

	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Layer* m_colorlayer;
	int m_step;
	Label* m_wordlbl;
	bool iscannext;
	int wordindex;
	int opacity;

private:
	void showAnim(Vec2 pos);
	void showWord(std::string wordstr);
	void showNode(std::vector<Node*> stencilNodes);
	static void clearNewGuideData();
	void removeSelf(float dt);
	void showNextGuide();

};
extern NewGuideLayer* g_NewGuideLayer;
#endif
