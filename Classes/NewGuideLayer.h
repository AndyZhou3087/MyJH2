#ifndef __NewGuideLayer__
#define __NewGuideLayer__

#include "cocos2d.h"
USING_NS_CC;

#define NEWERLAYERZOER 1999
class NewGuideLayer : public Layer
{
public:
	NewGuideLayer();
	~NewGuideLayer();
	bool init(int type, std::vector<Node*> stencilNodes);
	virtual void onEnterTransitionDidFinish();
	static NewGuideLayer* create(int step, std::vector<Node*> stencilNodes);
	static bool checkifNewerGuide(int index);
	static void pushUserData(std::string strdata);
	static bool isShowing;
private:
	ClippingNode* m_clippingNode;
	int m_step;
	Node* m_talknode;
	LayerColor *m_colorlayer;
	Vec2 starPos;
	Label* m_wordlbl;
	int wordindex;
	bool isallclick;
	bool iscannext;
	static std::vector<std::string> vec_userdata;
	cocos2d::EventListenerTouchOneByOne* m_listener;
private:
	void showAnim(Vec2 pos);
	void showWord(std::string wordstr);
	void initDialog();
	void removeSelf();

};
extern NewGuideLayer* g_NewGuideLayer;
#endif
