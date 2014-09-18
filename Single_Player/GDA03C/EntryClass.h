#ifndef __ENTRY_CLASS_H__
#define __ENTRY_CLASS_H__

#include <Ogre/ExampleApplication.h>
#include "MainClass.h"

class EntryClass : public ExampleApplication
{
private:
	MainClass* _mc;

public:
	EntryClass() 
	{

	}

	virtual ~EntryClass()
	{
		if (_mc) {
			_mc->Destroy();
			delete _mc;
		}
	}

	virtual void createScene()
	{
		_mc = new MainClass(mWindow, mCamera, mSceneMgr);

		DrawPlayground(100, 100);
		if (!_mc->Initialize()) {
			std::cout << "Initialization failed.\n";
			return;
		}
	}

	virtual void createCamera()
	{
		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(-15, 40, MAX_Z*1.25);
		mCamera->lookAt(MAX_X*1.25, 0, MAX_Z*1.25);
		mCamera->setNearClipDistance(5);
	}

	virtual void createFrameListener()
	{
		mRoot->addFrameListener(_mc);
	}

	void DrawPlayground(int width, int height)
	{
		Ogre::Plane plane(Vector3::UNIT_Y, -10);
		Ogre::MeshManager::getSingleton().createPlane("plane",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			width, height, width, height, true, 1, 5, 5, Vector3::UNIT_X);
		Ogre::Entity* ent = mSceneMgr->createEntity("LightPlaneEntity", "plane");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
		ent->setMaterialName("Examples/GrassFloor");
	}


};



#endif ///> end of __ENTRY_CLASS_H__