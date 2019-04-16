#include "AstarRouting.h"
#include "MapBlockScene.h"
#include "MyRes.h"
#include "MovingLabel.h"
#include "Resource.h"
#include "BuySelectLayer.h"
USING_NS_CC;

AstarRouting::ShortestPathStep::ShortestPathStep() :
_parent(nullptr)
{

}

AstarRouting::ShortestPathStep::~ShortestPathStep()
{

}

AstarRouting::ShortestPathStep *AstarRouting::ShortestPathStep::creatWithPos(const cocos2d::Vec2 &colrow)
{
	AstarRouting::ShortestPathStep* pRet = new ShortestPathStep();
	if(pRet && pRet->initWithPos(colrow))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool AstarRouting::ShortestPathStep::initWithPos(const cocos2d::Vec2 &colrow)
{
	bool bRet = false;
	do
	{
		this->setPos(colrow);
		bRet = true;
	}while(0);
	
	return bRet;
}

int AstarRouting::ShortestPathStep::getFScore() const
{
	return this->getHScore() + this->getGScore();
}

bool AstarRouting::ShortestPathStep::isEqual(const AstarRouting::ShortestPathStep *other) const
{
	return this->getPos() == other->getPos();
}

AstarRouting::AstarRouting(int cols, int rows)
{
	mapcols = cols;
	maprows = rows;
}
AstarRouting::~AstarRouting()
{

}

void AstarRouting::moveToPosByAStar(const cocos2d::Vec2 &frowcolrow, const cocos2d::Vec2 &tocolrow)
{
	Vec2 fromTiled = frowcolrow;
	Vec2 toTiled = tocolrow;

	if (fromTiled == toTiled)
	{
		return;
	}
	//if (!g_MapBlockScene->isValidAtWallColRow(toTiled))
	//{
	//	return;
	//}

	_openStepList.clear();
	_closeStepList.clear();

	this->insertOpenList(ShortestPathStep::creatWithPos(fromTiled));

	do{
		ShortestPathStep* currentStep = _openStepList.at(0);
		_closeStepList.pushBack(currentStep);
		_openStepList.erase(0);

		if(currentStep->getPos() == toTiled)
		{
			this->constructPathStartMoveFromStep(currentStep);

			_openStepList.clear();
			_closeStepList.clear();

			break;
		}

		PointArray *adjacentTiledCoord = WakabledAdjacentTiledColRow(currentStep->getPos());
		
		for(ssize_t i = 0; i < adjacentTiledCoord->count() ; ++i)
		{
			ShortestPathStep *step = ShortestPathStep::creatWithPos(adjacentTiledCoord->getControlPointAtIndex(i));

			if(this->getStepIndex(step, _closeStepList) != -1)
			{
				continue;
			}
			int moveCost = this->moveCostFromCurrentToNextStep(currentStep, step);
			
			ssize_t index = this->getStepIndex(step, _openStepList);
			if(index == -1)
			{
				step->setParent(currentStep);
				step->setGScore(currentStep->getGScore() + moveCost);
				step->setHScore(this->computeHScoreFromCoordToCoord(step->getPos(), toTiled));
				this->insertOpenList(step);
			}
			else
			{
				step = _openStepList.at(index);
				if((currentStep->getGScore() + moveCost) < step->getGScore())
				{
					step->setGScore(currentStep->getGScore() + moveCost);
					step->retain();
					_openStepList.erase(index);
					this->insertOpenList(step);
					step->release();
				}
			}
		}

	}while(_openStepList.size()>0);
	
}

cocos2d::PointArray *AstarRouting::WakabledAdjacentTiledColRow(const cocos2d::Vec2 &colrow)
{
	PointArray *tmp = PointArray::create(4);
	Vec2 p(colrow.x, colrow.y - 1);

	//上
	if (g_MapBlockScene->isValidAtWallColRow(p)) {
		tmp->addControlPoint(p);
	}
	// 左
	p.setPoint(colrow.x - 1, colrow.y);
	if (g_MapBlockScene->isValidAtWallColRow(p)) {
		tmp->addControlPoint(p);
	}
	// 下
	p.setPoint(colrow.x, colrow.y + 1);
	if (g_MapBlockScene->isValidAtWallColRow(p)) {
		tmp->addControlPoint(p);
	}
	// 右
	p.setPoint(colrow.x + 1, colrow.y);
	if (g_MapBlockScene->isValidAtWallColRow(p)) {
		tmp->addControlPoint(p);
	}
	return tmp;
}

void AstarRouting::insertOpenList(AstarRouting::ShortestPathStep* step)
{
	auto FScore = step->getFScore();
	ssize_t openCount = _openStepList.size();
	ssize_t i = 0;
	for(; i < openCount; i++)
	{
		if(FScore <= _openStepList.at(i)->getFScore())
			break;
	}

	_openStepList.insert(i, step);
}

ssize_t AstarRouting::getStepIndex(const ShortestPathStep* step,const cocos2d::Vector<AstarRouting::ShortestPathStep*> &list)
{
	for(ssize_t i = 0; i < list.size(); i++)
	{
	//	if(step == list.at(i)) 写好以后调试，两个实例的对象相当的判断不能这样表示
		if(list.at(i)->isEqual(step))
		{
			return i;
		}
	}
	return -1;
}

int AstarRouting::moveCostFromCurrentToNextStep(ShortestPathStep* current, ShortestPathStep* next)
{
	auto offent = next->getPos() - current->getPos();
	if(offent.x == 0 || offent.y == 0)
		return 10;
	else 
		return 14;
	//return 0;

//	return ((current->getPosition().x != next->getPosition().x )
//		&&(current->getPosition().y != next->getPosition().y))? 14 : 10;
}

int AstarRouting::computeHScoreFromCoordToCoord(const Vec2 &fpos,const Vec2 &tpos)
{
//	unsigned int a = fpos.x - tpos.x;
//	unsigned int b = fpos.y - tpos.y;
//	return a + b;

	return abs(fpos.x - tpos.x) + abs(fpos.y - tpos.y);
}

void AstarRouting::constructPathStartMoveFromStep(ShortestPathStep* step)
{
	_shortPathList.clear();
	do{
		if (step->getParent())
		{
			_shortPathList.insert(0, step);
		}
		step = step->getParent();
	} while (step);

	std::vector<Vec2> vec_steps;

	for (int i = 0; i < _shortPathList.size(); i++)
	{
		vec_steps.push_back(_shortPathList.at(i)->getPos());
	}

	if (MyRes::getMyResCount("r001", MYPACKAGE) < vec_steps.size())
	{
		MovingLabel::show(ResourceLang::map_lang["routnofood"]);
		g_MapBlockScene->showBuySelectFood();
	}
	else
	{
		g_MapBlockScene->showRouting(vec_steps);

		move();
	}

}

void AstarRouting::move()
{
	if (_shortPathList.size() == 0) {

		g_MapBlockScene->isMovingRouting = false;
		return;
	}
	g_MapBlockScene->isMovingRouting = true;
	ShortestPathStep *tostep = _shortPathList.at(0);
	g_MapBlockScene->goToDest((int)tostep->getPos().y, (int)tostep->getPos().x);
	_shortPathList.erase(0);

	CallFunc *actionCallBack = CallFunc::create(CC_CALLBACK_0(AstarRouting::move, this));
	Sequence *actionSq = Sequence::create(DelayTime::create(0.53f), actionCallBack, nullptr);
	g_MapBlockScene->getRoutingAnimNode()->runAction(actionSq);

}
