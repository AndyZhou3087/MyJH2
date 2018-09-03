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
	static NewGuideLayer* create(int step, std::vector<Node*> stencilNodes);
	static bool checkifNewerGuide(int index);
private:
	Layer* m_colorlayer;
	ClippingNode* m_clippingNode;
	int m_step;

private:
	void showAnim(Vec2 pos);
	Vec2 starPos;

};
extern NewGuideLayer* g_NewGuideLayer;
#endif
