#ifndef __GAME_STATUS_H__
#define __GAME_STATUS_H__

#include <vector>
#include <Ogre/ExampleApplication.h>

#include "Blocks.h"

typedef std::list<BaseCube*> CubeList;

class GameStatus
{
private:
	Ogre::SceneManager*				_sceneMgr;
	CubeList						_cubeList;
	int								_baseId;

	Blocks*							_currentBlock;

	std::vector<int>				_randNumberList;

public:
	GameStatus(Ogre::SceneManager* inMgr, std::vector<int> randList)
		: _sceneMgr(inMgr)
		, _randNumberList(randList)
		, _baseId(0)
	{

	}

	~GameStatus()
	{

	}

	//////////////////////////////////////////////////////////////////////////////
	// Important functions
	//	-- Initialize
	//	-- Update
	//	-- Destroy
	//////////////////////////////////////////////////////////////////////////////

	bool Initialize()
	{
		MyLog::put(__LINE__, __FILE__, "GameStatus Initializing.\n");

		_currentBlock = new Blocks(_sceneMgr, &_baseId, &_cubeList, _randNumberList);
		if (!_currentBlock->Initialize()) {
			MyLog::put(__LINE__, __FILE__, "GameStatus::_currentBlock initialize() failed.\n");
			return false;
		}

		// for test
// 		for (int i = 0; i <= MAX_X; i++) {
// 			for (int j = 0; j <= MAX_Z; j++) {
// 				AddCube(i, 0, j, CubeType::eGeneral);
// 			}
// 		}
// 		KillCube(28);
// 		KillCube(29);
// 		KillCube(34);
// 		KillCube(35);

		return true;
	}

	bool Update()
	{
		if (!_currentBlock->Update()) {
			MyLog::put(__LINE__, __FILE__, "GameStatus::_currentBlock update() failed.\n");
			return false;
		}

		for (CubeList::iterator it_i = _cubeList.begin(); it_i != _cubeList.end(); it_i++) {
			(*it_i)->Update();
		}

		for (CubeList::iterator it = _cubeList.begin(); it != _cubeList.end(); ) {
			CubeList::iterator ittmp = it++;
			if (!(*ittmp)->Alive() && (*ittmp)->ShouldBeDeleted()) {
				(*ittmp)->Destroy();
				(*ittmp)->getCubeNode()->detachAllObjects();
				_sceneMgr->destroyEntity((*ittmp)->getCubeEntity());
				_sceneMgr->destroySceneNode((*ittmp)->getCubeNode());
				_cubeList.erase(ittmp);
				_currentBlock->TurnEffecting(false);
			}
		}

		// Put this in the end of one cycle (when producing new block)
// 		for (CubeList::iterator it_i = _cubeList.begin(); it_i != _cubeList.end(); it_i++) {
// 			(*it_i)->MoveDown(&_cubeList);
// 		}

		return true;
	}

	bool Destroy()
	{
		for (CubeList::iterator it_i = _cubeList.begin(); it_i != _cubeList.end(); it_i++) {
			if (*it_i) {
				delete *it_i;
			}
		}
		_cubeList.clear();

		if (!_currentBlock->Destroy()) {
			MyLog::put(__LINE__, __FILE__, "GameStatus::_currentBlock destroy() failed.\n");
			return false;
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Custom functions
	//////////////////////////////////////////////////////////////////////////////

	Blocks*			getCurrentBlock() const			{ return _currentBlock; }

	bool AddCube(BaseCube* cube)
	{
		_cubeList.push_back(cube);
	}

	bool AddCube(int _x, int _y, int _z, CubeType ctype)
	{
		Ogre::Entity* ent = _sceneMgr->createEntity("cube.mesh");
		Ogre::SceneNode* node 
			= _sceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(_x*2.5, _y*2.5 - 8.0, _z*2.5));
		node->setScale(0.0245, 0.0245, 0.0245);
		node->attachObject(ent);

		BaseCube* bc;
		switch(ctype)
		{
		case CubeType::eGeneral:
			bc = new GeneralCube(node, ent, _sceneMgr, _x, _y, _z, _baseId++);
			bc->Initialize();
			break;
		case CubeType::eExplosive:
			bc = new ExplosiveCube(node, ent, _sceneMgr, _x, _y, _z, _baseId++);
			bc->Initialize();
			break;
		case CubeType::eExtendable:
			bc = new ExtendableCube(node, ent, _sceneMgr, _x, _y, _z, _baseId++);
			bc->Initialize();
			break;
		default:
			break;
		}
		std::cout << bc->getId() << std::endl;
		_cubeList.push_back(bc);

		return true;
	}

	bool KillCube(int id)
	{
		for (CubeList::iterator it = _cubeList.begin(); it != _cubeList.end(); ) {
			CubeList::iterator ittmp = it++;
			if ((*ittmp)->getId() == id) {
// 				(*ittmp)->Destroy();
// 				_sceneMgr->destroyEntity((*ittmp)->getCubeEntity());
// 				_sceneMgr->destroySceneNode((*ittmp)->getCubeNode());
// 				_cubeList.erase(ittmp);
				(*ittmp)->Killed();
				_currentBlock->TurnEffecting(true);
				return true;
			}
		}

		MyLog::put(__LINE__, __FILE__, "Not find the specified cube.\n");
		return false;
	}
};


#endif ///> end of __GAME_STATUS_H__