#ifndef __BLOCKS_H__
#define __BLOCKS_H__

#include <Ogre/ExampleApplication.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <vector>
#include <map>
#include <string>
// #include <cmath>

#include "Utility.h"
#include "GeneralCube.h"
#include "ExplosiveCube.h"
#include "ExtendableCube.h"

typedef std::list<BaseCube*> CubeList;

class Blocks
{
private:
	struct BlockTypes
	{
		int							_x;
		int							_z;
		std::string					_type;
	};

	CubeList									_controllableCubeList;

	CubeList*									_existCubeList;

	Ogre::SceneManager*							_sceneMgr;
	Ogre::ParticleSystem*						_fireworkEffects;
	Ogre::SceneNode*							_fireworkNode;
	bool										_effectAttached;
	int*										_pBaseId;
	std::vector<std::vector<BlockTypes>>		_blockTemplate;

	CustomTimer									_timer;
	int											_fallIntervalInit;
	int											_fallInterval;

	int											_score;
	bool										_gameOver;

	boost::mutex								_mutex;

public:
	enum AxisDirection
	{
		ePositive = 1,
		eNegative = -1,
	};

	Blocks(Ogre::SceneManager* inMgr, int* id, CubeList* inExistCubeList)
		: _sceneMgr(inMgr)
		, _pBaseId(id)
		, _existCubeList(inExistCubeList)
		, _fallIntervalInit(800)
		, _fallInterval(_fallIntervalInit)
		, _score(0)
		, _gameOver(false)
	{

	}

	~Blocks()
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
		MyLog::put(__LINE__, __FILE__, "Blocks Initializing.\n");

		ParticleSystem::setDefaultNonVisibleUpdateTimeout(0);
		_fireworkEffects = _sceneMgr->createParticleSystem("Fireworks", "Examples/Fireworks");
		_fireworkNode = _sceneMgr->createSceneNode("FireworkNode");
		_sceneMgr->getRootSceneNode()->addChild(_fireworkNode);
		_fireworkNode->setPosition(MAX_X*1.25, 0, MAX_Z*1.25);
		_effectAttached = false;

		// TO DO: import the block data from the xml file.
		_blockTemplate = ImportDataFromXml("GDA03Cubes.xml");

		Reset();

		return true;
	}

	bool Update()
	{
//		MoveZ(ePositive);
		if (_gameOver) {
			return true;
		}

		if (!MoveDown(_fallInterval)) {
			for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
				(*it_i)->AfterHittingBottom(_existCubeList, _pBaseId);
				_existCubeList->push_back(*it_i);
			}

//			_score += Elimination();

			boost::thread eliThread(boost::bind(&Blocks::ThreadElimination, this));
			eliThread.join();

			_fallInterval = _fallIntervalInit;
			if (!Reset()) {
				std::cout << "LOSE.\n";
				_gameOver = true;
				// TO DO: postLose event!!
			}
		}

		for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
			(*it_i)->Update();
		}

		return true;
	}

	bool Destroy()
	{
		for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
			if (*it_i) {
				delete *it_i;
			}
		}
		_controllableCubeList.clear();

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Custom functions
	//////////////////////////////////////////////////////////////////////////////

	int				getFallInterval() const				{ return _fallInterval; }
	void			setFallInterval(int val)			{ _fallInterval = val; }

	int				getScore() const					{ return _score; }
	bool			GameOver() const					{ return _gameOver; }

	void TurnEffecting(bool flag)
	{
		if (flag && !_effectAttached) {
			_fireworkNode->attachObject(_fireworkEffects);
			_effectAttached = true;
		} else if (!flag && _effectAttached) {
			_fireworkNode->detachObject(_fireworkEffects);
			_effectAttached = false;
		}
	}

	std::vector<std::vector<BlockTypes>> ImportDataFromXml(char* filePath)
	{
		std::vector<std::vector<BlockTypes>> resx;
		TiXmlDocument doc;
		if (doc.LoadFile(filePath)) {
			doc.Print();
		} else {
			MyLog::put(__LINE__, __FILE__, "Cannot import the xml data.\n");
			return std::vector<std::vector<BlockTypes>>();
		}

		TiXmlElement* rootElement = doc.RootElement();						// Blocks
		TiXmlElement* keyElement = rootElement->FirstChildElement();		// Set

		while (keyElement) {
			std::vector<BlockTypes> res;
			BlockTypes bts;

			TiXmlElement* elt = keyElement->FirstChildElement();			// Cube
			while (elt) {
				TiXmlElement* eltc = elt->FirstChildElement();				// X
				bts._x = atoi(eltc->GetText());
				eltc = eltc->NextSiblingElement();							// Z
				bts._z = atoi(eltc->GetText());
				eltc = eltc->NextSiblingElement();							// Type
				bts._type = std::string(eltc->GetText());

				res.push_back(bts);

				elt = elt->NextSiblingElement();
			}
			resx.push_back(res);

			keyElement = keyElement->NextSiblingElement();
		}

		for (int i = 0; i < resx.size(); i++) {
			for (int j = 0; j < resx[i].size(); j++) {
				std::cout << resx[i][j]._x << ' ';
				std::cout << resx[i][j]._z << ' ';
				std::cout << resx[i][j]._type << '\n';
			}
			std::cout << '\n';
		}

		return resx;
	}

	bool Reset()
	{
		_controllableCubeList.clear();

		// TO DO: randomize the block
// 		AddCube(1, MAX_Y, 1, CubeType::eGeneral);
// 		AddCube(2, MAX_Y, 1, CubeType::eGeneral);
// 		AddCube(1, MAX_Y, 2, CubeType::eExplosive);
// 		AddCube(1, MAX_Y, 3, CubeType::eGeneral);

//		srand((unsigned)time(0));
		int index = rand() % _blockTemplate.size();
		std::cout << index << std::endl;
		_controllableCubeList.clear();

		for (int i = 0; i < _blockTemplate[i].size(); i++) {
			int xxx = _blockTemplate[index][i]._x;
			int yyy = MAX_Y;
			int zzz = _blockTemplate[index][i]._z;
			CubeType ct = eNone;

			if (_blockTemplate[index][i]._type == "General") {
				ct = CubeType::eGeneral;
			} else if (_blockTemplate[index][i]._type == "Explosive") {
				ct = CubeType::eExplosive;
			} else if (_blockTemplate[index][i]._type == "Extendable") {
				ct = CubeType::eExtendable;
			}

			AddCube(xxx, yyy, zzz, ct);
		}

		bool face[MAX_X + 1][MAX_Z + 1];
		memset(face, 0, sizeof(face));
		for (CubeList::iterator it_i = _existCubeList->begin(); it_i != _existCubeList->end(); it_i++) {
			if ((*it_i)->getYPos() == MAX_Y) {
				face[(*it_i)->getXPos()][(*it_i)->getZPos()] = true;
			}
		}
		for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
			if (face[(*it_i)->getXPos()][(*it_i)->getZPos()]) {
				return false;
			}
		}

		_timer.Start();
		return true;
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
			bc = new GeneralCube(node, ent, _sceneMgr, _x, _y, _z, (*_pBaseId)++);
			bc->Initialize();
			break;
		case CubeType::eExplosive:
			bc = new ExplosiveCube(node, ent, _sceneMgr, _x, _y, _z, (*_pBaseId)++);
			bc->Initialize();
			break;
		case CubeType::eExtendable:
			bc = new ExtendableCube(node, ent, _sceneMgr, _x, _y, _z, (*_pBaseId)++);
			bc->Initialize();
			break;
		default:
			break;
		}
//		std::cout << bc->getId() << std::endl;
		_controllableCubeList.push_back(bc);

		return true;
	}

	bool KillControllableCube(int id)
	{
		for (CubeList::iterator it = _controllableCubeList.begin(); it != _controllableCubeList.end(); ) {
			CubeList::iterator ittmp = it++;
			if ((*ittmp)->getId() == id) {
				(*ittmp)->Destroy();
// 				_sceneMgr->destroyEntity((*ittmp)->getCubeEntity());
// 				_sceneMgr->destroySceneNode((*ittmp)->getCubeNode());
// 				_controllableCubeList.erase(ittmp);
				(*ittmp)->Killed();
				return true;
			}
		}

		MyLog::put(__LINE__, __FILE__, "Not find the specified cube.\n");
		return false;
	}

	bool KillExistingCube(int id)
	{
		for (CubeList::iterator it = _existCubeList->begin(); it != _existCubeList->end(); ) {
			CubeList::iterator ittmp = it++;
			if ((*ittmp)->getId() == id) {
				(*ittmp)->Destroy();
// 				_sceneMgr->destroyEntity((*ittmp)->getCubeEntity());
// 				_sceneMgr->destroySceneNode((*ittmp)->getCubeNode());
// 				_existCubeList->erase(ittmp);
				(*ittmp)->Killed();
				return true;
			}
		}

		MyLog::put(__LINE__, __FILE__, "Not find the specified cube.\n");
		return false;
	}

	bool ControllableAction(int x)
	{
		std::cout << "x = " << x << std::endl;
		return (x == 1);
	}

	//////////////////////////////////////////////////////////////////////////////
	// Rotate on Y
	// sX = x cos(angle) - z sin(angle)
	// sZ = x sin(angle) + z cos(angle)
	//
	// Rotate on X
	// sY = y cos(angle) - z sin(angle)
	// sZ = y sin(angle) + z cos(angle)
	//
	// Rotate on Z
	// sX = x cos(angle) - y sin(angle)
	// sY = x sin(angle) + y cos(angle)
	//////////////////////////////////////////////////////////////////////////////

	bool RotateOnY(int angle)			// angle could only be 90, 180, or 270
	{
		if (!(angle == 90 || angle == 180 || angle == 270)) {
			return false;
		}
		bool flag = true;
		int centerIndex = _controllableCubeList.size() / 2;
		int cnt = 0;
		BaseCube* center;
		for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++, cnt++) {
			if (cnt == centerIndex) {
				center = (*it_i);
				break;
			}
		}

		cnt = 0;
		float cosAngle;
		float sinAngle;
		switch(angle)
		{
		case 90:
			cosAngle = 0.0f; sinAngle = 1.0f;
			break;
		case 180:
			cosAngle = -1.0f; sinAngle = 0.0f;
			break;
		case 270:
			cosAngle = 0.0f; sinAngle = -1.0f;
			break;
		}
		for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++, cnt++) {
			if (cnt == centerIndex) {
				continue;
			}
			int deltaX = (*it_i)->getXPos() - center->getXPos();
			int deltaZ = (*it_i)->getZPos() - center->getZPos();
			int xxx = center->getXPos() + deltaX*cosAngle - deltaZ*sinAngle;
			int zzz = center->getZPos() + deltaX*sinAngle + deltaZ*cosAngle;

			if (!Utility::isXPosLegal(xxx) || !Utility::isZPosLegal(zzz)) {
				flag = false;
				break;
			}

			for (CubeList::iterator it_j = _existCubeList->begin(); it_j != _existCubeList->end(); it_j++) {
				if ((*it_i)->Intersects((*it_j), (deltaX > 0 ? BaseCube::MovingDirection::eMovingXPositive : BaseCube::MovingDirection::eMovingXNegative))
				 || (*it_i)->Intersects((*it_j), (deltaZ > 0 ? BaseCube::MovingDirection::eMovingZPositive : BaseCube::MovingDirection::eMovingZNegative))) {
					 flag = false;
					 break;
				}
			}
			if (!flag) {
				break;
			}
		}
		if (flag) {
			cnt = 0;
			for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++, cnt++) {
				if (cnt == centerIndex) {
					continue;
				}
				int deltaX = (*it_i)->getXPos() - center->getXPos();
				int deltaZ = (*it_i)->getZPos() - center->getZPos();
				int xxx = center->getXPos() + deltaX*cosAngle - deltaZ*sinAngle;
				int zzz = center->getZPos() + deltaX*sinAngle + deltaZ*cosAngle;

				Ogre::Real deltaNodeX = (*it_i)->getCubeNode()->getPosition().x - center->getCubeNode()->getPosition().x;
				Ogre::Real deltaNodeZ = (*it_i)->getCubeNode()->getPosition().z - center->getCubeNode()->getPosition().z;
				Ogre::Real nodeXxx = center->getCubeNode()->getPosition().x + deltaNodeX*cosAngle - deltaNodeZ*sinAngle;
				Ogre::Real nodeZzz = center->getCubeNode()->getPosition().z + deltaNodeX*sinAngle + deltaNodeZ*cosAngle;

				(*it_i)->setXPos(xxx);
				(*it_i)->setZPos(zzz);
				(*it_i)->getCubeNode()->setPosition(Ogre::Vector3(nodeXxx, (*it_i)->getCubeNode()->getPosition().y, nodeZzz));
			}
		}
		
		return flag;
	}

	bool MoveDown(int interval)
	{
		if (_timer.CurrentTime() < interval) {
			return true;
		}
		bool flag = true;
		for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
			int yyy = (*it_i)->getYPos();
			if (!Utility::isYPosLegal(yyy - 1)) {
				flag = false;
				break;
			}
//			(*it_i)->getCubeNode()->translate(Ogre::Vector3(0, -0.005, 0));
//			(*it_i)->getCubeNode()->setScale(0.24f, 0.275f, 0.24f);
//			Ogre::AxisAlignedBox box1 = (*it_i)->getCubeNode()->_getWorldAABB();
			for (CubeList::iterator it_j = _existCubeList->begin(); it_j != _existCubeList->end(); it_j++) {
//				Ogre::AxisAlignedBox box2 = (*it_j)->getCubeNode()->_getWorldAABB();
				if ((*it_i)->Intersects(*it_j, BaseCube::MovingDirection::eMovingYNegative)) {
//					std::cout << "Intersect.\n";
					flag = false;
					break;
				}
			}
//			(*it_i)->getCubeNode()->translate(Ogre::Vector3(0, 0.005, 0));
//			(*it_i)->getCubeNode()->setScale(0.24f, 0.24f, 0.24f);
//			(*it_i)->getCubeNode()->translate(Ogre::Vector3(0, 2.75, 0));
			if (!flag) {
				break;
			}
		}
		if (flag) {
			for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
				int yyy = (*it_i)->getYPos();
				(*it_i)->setYPos(yyy - 1);
				(*it_i)->getCubeNode()->translate(Ogre::Vector3(0, -2.5, 0));
			}
			_timer.Start();
		}

		return flag;
	}

	bool MoveX(AxisDirection ad)
	{
		bool flag = true;
		for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
			int xxx = (*it_i)->getXPos();
			if (!Utility::isXPosLegal(xxx + ad)) {
				flag = false;
				break;
			}
			for (CubeList::iterator it_j = _existCubeList->begin(); it_j != _existCubeList->end(); it_j++) {
				if (ad == ePositive && (*it_i)->Intersects(*it_j, BaseCube::MovingDirection::eMovingXPositive)) {
					flag = false;
					break;
				}
				if (ad == eNegative && (*it_i)->Intersects(*it_j, BaseCube::MovingDirection::eMovingXNegative)) {
					flag = false;
					break;
				}
			}

			if (!flag) {
				break;
			}
		}
		if (flag) {
			for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
				int xxx = (*it_i)->getXPos();
				(*it_i)->setXPos(xxx + ad);
				(*it_i)->getCubeNode()->translate(Ogre::Vector3(ad * 2.5, 0, 0));
			}
			_timer.Start();
		}

		return flag;
	}

	bool MoveZ(AxisDirection ad)
	{
		bool flag = true;
		for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
			int zzz = (*it_i)->getZPos();
			if (!Utility::isZPosLegal(zzz + ad)) {
				flag = false;
				break;
			}
			for (CubeList::iterator it_j = _existCubeList->begin(); it_j != _existCubeList->end(); it_j++) {
				if (ad == ePositive && (*it_i)->Intersects(*it_j, BaseCube::MovingDirection::eMovingZPositive)) {
					flag = false;
					break;
				}
				if (ad == eNegative && (*it_i)->Intersects(*it_j, BaseCube::MovingDirection::eMovingZNegative)) {
					flag = false;
					break;
				}
			}

			if (!flag) {
				break;
			}
		}
		if (flag) {
			for (CubeList::iterator it_i = _controllableCubeList.begin(); it_i != _controllableCubeList.end(); it_i++) {
				int zzz = (*it_i)->getZPos();
				(*it_i)->setZPos(zzz + ad);
				(*it_i)->getCubeNode()->translate(Ogre::Vector3(0, 0, ad * 2.5));
			}
			_timer.Start();
		}

		return flag;
	}

	int Elimination()
	{
		struct Exist {
			bool _exist;
			int _cubeId;
		};
		Exist exist[MAX_X + 1][MAX_Y + 1][MAX_Z + 1];
		memset(exist, 0, sizeof(exist));

		for (CubeList::iterator it_i = _existCubeList->begin(); it_i != _existCubeList->end(); it_i++) {
			int xxx = (*it_i)->getXPos();
			int yyy = (*it_i)->getYPos();
			int zzz = (*it_i)->getZPos();
			exist[xxx][yyy][zzz]._exist = true;
			exist[xxx][yyy][zzz]._cubeId = (*it_i)->getId();
		}

		int cnt = 0;
		for (int Y = 0; Y <= MAX_Y; Y++) {
			bool flag = true;
			for (int X = 0; X <= MAX_X; X++) {
				for (int Z = 0; Z <= MAX_Z; Z++) {
					if (!exist[X][Y][Z]._exist) {
						flag = false;
						break;
					}
				}
				if (!flag) {
					break;
				}
			}
			if (flag) {
				TurnEffecting(true);
				cnt++;
				for (int X = 0; X <= MAX_X; X++) {
					for (int Z = 0; Z <= MAX_Z; Z++) {
						KillExistingCube(exist[X][Y][Z]._cubeId);
					}
				}
				for (CubeList::iterator it_i = _existCubeList->begin(); it_i != _existCubeList->end(); it_i++) {
					if ((*it_i)->getYPos() > Y) {
						(*it_i)->MoveDown(_existCubeList);
					}
				}
			}
		}
		return cnt;
	}

	void ThreadElimination()
	{
		_mutex.lock();
		_score += Elimination();
		_mutex.unlock();
	}

};


#endif ///> end of __BLOCKS_H__