#ifndef __BASE_CUBE_H__
#define __BASE_CUBE_H__

#include <list>
#include <Ogre/ExampleApplication.h>
#include "Utility.h"

class BaseCube
{
protected:
	Ogre::SceneNode*			_cubeNode;
	Ogre::Entity*				_cubeEntity;
	Ogre::SceneManager*			_sceneMgr;
	
	// this is to record if the Cube is about to be deleted,
	// if it is true, the _postDeadTimes should be starting counting
	// and of course, when this is activated, the particle system
	// should be attached to the _cubeNode;
	bool						_alive;
	// this is to record how much time (in millisecond) after the Cube is "killed",
	// if it reaches _timeLimit, the cube should be deleted.
	CustomTimer					_postDeadTimer;
	int							_timeLimit;

	//////////////////////////////////////////////////////////////////////////////
	// Invariant:
	//		0 <= _xPos <= MAX_X
	//		0 <= _yPos <= MAX_Y
	//		0 <= _zPos <= MAX_Z
	//////////////////////////////////////////////////////////////////////////////
	int							_xPos;
	int							_yPos;
	int							_zPos;

	int							_id;
public:
	BaseCube(Ogre::SceneNode* node, Ogre::Entity* ent, Ogre::SceneManager* mgr, int _x, int _y, int _z, int id)
		: _cubeNode(node)
		, _cubeEntity(ent)
		, _sceneMgr(mgr)
		, _xPos(_x)
		, _yPos(_y)
		, _zPos(_z)
		, _id(id)
		, _alive(true)
		, _timeLimit(5000)
	{

	}

	virtual ~BaseCube()
	{

	}

	//////////////////////////////////////////////////////////////////////////////
	// Important functions
	//	-- Initialize
	//	-- Update
	//	-- Destroy
	//////////////////////////////////////////////////////////////////////////////

	virtual bool Initialize()
	{
		MyLog::put(__LINE__, __FILE__, "BaseCube Initializing.\n");

		return true;
	}

	virtual bool Update()
	{
		return true;
	}

	virtual bool Destroy()
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Custom functions
	//////////////////////////////////////////////////////////////////////////////

	int					getId() const					{ return _id; }

	Ogre::SceneNode*	getCubeNode() const				{ return _cubeNode; }

	Ogre::Entity*		getCubeEntity() const			{ return _cubeEntity; }

	int					getXPos() const					{ return _xPos; }
	void				setXPos(int val)				{ _xPos = val; }

	int					getYPos() const					{ return _yPos; }
	void				setYPos(int val)				{ _yPos = val; }

	int					getZPos() const					{ return _zPos; }
	void				setZPos(int val)				{ _zPos = val; }

	bool				Alive() const					{ return _alive; }

	bool ShouldBeDeleted()
	{
// 		if (!_alive) {
// 			std::cout << _postDeadTimer.CurrentTime() << std::endl;
// 		}
		return (_postDeadTimer.CurrentTime() >= _timeLimit);
	}

	void Killed()
	{
		_cubeNode->detachAllObjects();
		_alive = false;
		_postDeadTimer.Start();
	}

	typedef std::list<BaseCube*> CubeList;

	virtual bool AfterHittingBottom(CubeList* clist, int *baseId)
	{
		return true;
	}

	typedef std::list<BaseCube*> CubeList;

	bool MoveDown(CubeList* clist)
	{
		int yyy = getYPos();
		yyy--;
		bool flag = true;
		if (Utility::isYPosLegal(yyy)) {
			for (CubeList::iterator it_i = clist->begin(); it_i != clist->end(); it_i++) {
				if (this->getId() == (*it_i)->getId()) {
					continue;
				}
				if (this->Intersects((*it_i), eMovingYNegative)) {
					flag = false;
					break;
				}
			}
			if (flag) {
				this->setYPos(yyy);
				this->getCubeNode()->translate(Ogre::Vector3(0, -2.5, 0));
			}
		} else {
			flag = false;
		}
		return flag;
	}

	enum MovingDirection {
		eMovingYNegative,
		eMovingXPositive,
		eMovingXNegative,
		eMovingZPositive,
		eMovingZNegative,
	};

	bool Intersects(BaseCube* other, MovingDirection dir, double offset = 0.1)
	{
		if (!other->Alive()) {
			return false;
		}

		Ogre::Vector3 min1Corner = this->getCubeNode()->_getWorldAABB().getMinimum();
		Ogre::Vector3 max1Corner = this->getCubeNode()->_getWorldAABB().getMaximum();

		Ogre::Vector3 min2Corner = other->getCubeNode()->_getWorldAABB().getMinimum();
		Ogre::Vector3 max2Corner = other->getCubeNode()->_getWorldAABB().getMaximum();

		switch(dir) 
		{
		case eMovingYNegative:
			min1Corner.y -= offset; max1Corner.y -= offset;
			break;
		case eMovingXPositive:
			min1Corner.x += offset; max1Corner.x += offset;
			break;
		case eMovingXNegative:
			min1Corner.x -= offset; max1Corner.x -= offset;
			break;
		case eMovingZPositive:
			min1Corner.z += offset; max1Corner.z += offset;
			break;
		case eMovingZNegative:
			min1Corner.z -= offset; max1Corner.z -= offset;
			break;
		default:
			break;
		}

		if ((max1Corner.x < min2Corner.x) || (min1Corner.x > max2Corner.x)) {
			return false;
		}
		if ((max1Corner.y < min2Corner.y) || (min1Corner.y > max2Corner.y)) {
			return false;
		}
		if ((max1Corner.z < min2Corner.z) || (min1Corner.z > max2Corner.z)) {
			return false;
		}

		return true;
	}
};


#endif