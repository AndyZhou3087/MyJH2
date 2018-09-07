#ifndef __NewGuideLayer__
#define __NewGuideLayer__

#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
USING_NS_CC;

#define FIRSTGUIDESTEP 13
#define SECONDGUIDESTEP 21
#define THRIDGUIDESTEP 39
#define FOURTHGUIDESTEP 52

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
	static void setNewGuideInfo(int step);//新手加成

	void onBtnClick(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);
private:
	Layer* m_colorlayer;
	RenderTexture* m_clippingNode;
	int m_step;
	Label* m_wordlbl;
	Vec2 starPos;
	bool iscannext;
	int wordindex;
	int opacity;

private:
	void showAnim(Vec2 pos);
	void showWord(std::string wordstr);
	void showNode(std::vector<Node*> stencilNodes);
	void clearNewGuideData();

};
extern NewGuideLayer* g_NewGuideLayer;
#endif
