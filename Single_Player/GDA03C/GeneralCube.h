#ifndef __GENERAL_CUBE_H__
#define __GENERAL_CUBE_H__

#include "BaseCube.h"

class GeneralCube : public BaseCube
{
private:


public:
	GeneralCube(Ogre::SceneNode* node, Ogre::Entity* ent, Ogre::SceneManager* mgr, int _x, int _y, int _z, int id)
		: BaseCube(node, ent, mgr, _x, _y, _z, id)
	{
		ent->setMaterialName("Examples/SphereMappedRustySteel");
	}

	virtual ~GeneralCube()
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
		MyLog::put(__LINE__, __FILE__, "GeneralCube Initializing.\n");

		BaseCube::Initialize();

		return true;
	}

	virtual bool Update()
	{
//		std::cout << "General.\n";

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
		std::cout << "General.\n";
		return true;
	}
};


#endif ///> end of __GENERAL_CUBE_H__