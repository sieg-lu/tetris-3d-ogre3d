#ifndef __EXPLOSIVE_CUBE_H__
#define __EXPLOSIVE_CUBE_H__

#include "BaseCube.h"

class ExplosiveCube : public BaseCube
{
private:


public:
	ExplosiveCube(Ogre::SceneNode* node, Ogre::Entity* ent, Ogre::SceneManager* mgr, int _x, int _y, int _z, int id)
		: BaseCube(node, ent, mgr, _x, _y, _z, id)
	{
		ent->setMaterialName("Examples/OgreSpin");
	}

	virtual ~ExplosiveCube()
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
		MyLog::put(__LINE__, __FILE__, "ExplosiveCube Initializing.\n");

		BaseCube::Initialize();

		return true;
	}

	virtual bool Update()
	{
//		std::cout << "Explosive.\n";

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
		std::cout << "Explosive.\n";

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

			if (exist[xxx][yyy][zzz]._exist) {
				int t_id = exist[xxx][yyy][zzz]._cubeId;
				for (CubeList::iterator it = clist->begin(); it != clist->end(); ) {
					CubeList::iterator ittmp = it++;
					if ((*ittmp)->getId() == t_id) {
						(*ittmp)->Killed();
					}
				}
			}
		}
		this->Killed();

		return true;
	}
};


#endif ///> end of __EXPLOSIVE_CUBE_H__