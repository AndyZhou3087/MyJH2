#ifndef __ASTAR_ROUTING_H__
#define __ASTAR_ROUTING_H__

#include "cocos2d.h"

class AstarRouting
{

public:
	AstarRouting(int cols, int rows);
	~AstarRouting();

	class ShortestPathStep : public cocos2d::Ref
	{
	public:
		ShortestPathStep();
		~ShortestPathStep();
		static ShortestPathStep *creatWithPos(const cocos2d::Vec2 &colrow);
		bool initWithPos(const cocos2d::Vec2 &colrow);
		int getFScore() const;
		bool isEqual(const ShortestPathStep *other) const;
		std::string getDescrpition();
		CC_SYNTHESIZE(cocos2d::Vec2, _pos, Pos);
		CC_SYNTHESIZE(int, _gScore, GScore);
		CC_SYNTHESIZE(int, _hScore, HScore);
		CC_SYNTHESIZE(ShortestPathStep*, _parent, Parent);
	};

	void moveToPosByAStar(const cocos2d::Vec2 &frowcolrow, const cocos2d::Vec2 &tocolrow);
	void insertOpenList(ShortestPathStep* step);
	ssize_t getStepIndex(const ShortestPathStep* step,const cocos2d::Vector<AstarRouting::ShortestPathStep*> &list);
	int moveCostFromCurrentToNextStep(ShortestPathStep* current, ShortestPathStep* next);
	int computeHScoreFromCoordToCoord(const cocos2d::Vec2 &fpos,const cocos2d::Vec2 &tpos);
	void constructPathStartMoveFromStep(AstarRouting::ShortestPathStep* step);
	cocos2d::PointArray* WakabledAdjacentTiledColRow(const cocos2d::Vec2 &colrow);

	void move();
private:
	cocos2d::Vector<ShortestPathStep*> _openStepList;
	cocos2d::Vector<ShortestPathStep*> _closeStepList;
	cocos2d::Vector<ShortestPathStep*> _shortPathList;

	int mapcols;
	int maprows;
};

#endif