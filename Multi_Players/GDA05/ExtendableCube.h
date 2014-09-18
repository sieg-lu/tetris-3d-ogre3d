#ifndef __EXTENDABLE_CUBE_H__
#define __EXTENDABLE_CUBE_H__

#include <map>

#include "BaseCube.h"
#include "Utility.h"

typedef std::list<BaseCube*> CubeList;

class ExtendableCube : public BaseCube
{
private:


public:
	ExtendableCube(Ogre::SceneNode* node, Ogre::Entity* ent, Ogre::SceneManager* mgr, int _x, int _y, int _z, int id)
		: BaseCube(node, ent, mgr, _x, _y, _z, id)
	{
		ent->setMaterialName("Examples/SceneCubeMap1");
	}

	virtual ~ExtendableCube()
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
		MyLog::put(__LINE__, __FILE__, "ExtendableCube Initializing.\n");

		BaseCube::Initialize();

		return true;
	}

	virtual bool Update()
	{
//		std::cout << "Extendable.\n";

		return true;
	}

	virtual bool Destroy()
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Custom functions
	//////////////////////////////////////////////////////////////////////////////

	virtual bool AfterHittingBottom(CubeList* clist, int *baseId)
	{
		std::cout << "Extendable.\n";

		struct Exist {
			bool _exist;
			int _cubeId;
		};
		Exist exist[MAX_X + 1][MAX_Y + 1][MAX_Z + 1];
		memset(exist, 0, sizeof(exist));

		for (CubeList::iterator it_i = clist->begin(); it_i != clist->end(); it_i++) {
			int xxx = (*it_i)->getXPos();
			int yyy = (*it_i)->getYPos();
			int zzz = (*it_i)->getZPos();
			exist[xxx][yyy][zzz]._exist = true;
			exist[xxx][yyy][zzz]._cubeId = (*it_i)->getId();
		}

		int dir[6][3] = { 
			{ 1, 0, 0 },
			{ -1, 0, 0 },
			{ 0, 1, 0 },
			{ 0, -1, 0 },
			{ 0, 0, 1 },
			{ 0, 0, -1 }
		};

		for (int i = 0; i < 6; i++) {
			int xxx = getXPos() + dir[i][0];
			int yyy = getYPos() + dir[i][1];
			int zzz = getZPos() + dir[i][2];
			if (!Utility::isXPosLegal(xxx) || !Utility::isYPosLegal(yyy) || !Utility::isZPosLegal(zzz)) {
				continue;
			}

			if (!exist[xxx][yyy][zzz]._exist) {
				Ogre::Entity* ent = _sceneMgr->createEntity("cube.mesh");
				Ogre::SceneNode* node 
					= _sceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(xxx*2.5, yyy*2.5 - 8.0, zzz*2.5));
				node->setScale(0.0245, 0.0245, 0.0245);
				node->attachObject(ent);
				BaseCube* bc = new GeneralCube(node, ent, _sceneMgr, xxx, yyy, zzz, *baseId++);
				clist->push_back(bc);
			}
		}

		return true;
	}
};


#endif ///> end of __EXTENDABLE_CUBE_H__