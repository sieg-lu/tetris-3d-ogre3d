#ifndef __ENTRY_CLASS_H__
#define __ENTRY_CLASS_H__

#include <Ogre/ExampleApplication.h>
#include "MainClass.h"

#define CAMERA_NAME "SceneCamera"

class EntryClass : public ExampleApplication
{
private:
	MainClass* _mc;

	Ogre::SceneManager *mPrimary, *mSecondary;
	Ogre::Camera *mPrimaryCam, *mSecondaryCam;

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

	void chooseSceneManager(void)
	{
		mPrimary = mRoot->createSceneManager(ST_GENERIC, "primary");
		mSecondary = mRoot->createSceneManager(ST_GENERIC, "secondary");
	}

	virtual void createScene()
	{
		mPrimary->setSkyBox(true, "Examples/SpaceSkyBox");
		mSecondary->setSkyDome(true, "Examples/CloudySky", 5, 8);

		DrawPlayground(100, 100, mPrimary);
		DrawPlayground(100, 100, mSecondary);

		_mc = new MainClass(mWindow, mPrimary, mSecondary);

		if (!_mc->Initialize()) {
			std::cout << "Initialization failed.\n";
			return;
		}
	}

	virtual void createCamera()
	{
// 		mCamera = mSceneMgr->createCamera("MyCamera1");
// 		mCamera->setPosition(-15, 40, MAX_Z*1.25);
// 		mCamera->lookAt(MAX_X*1.25, 0, MAX_Z*1.25);
// 		mCamera->setNearClipDistance(5);
		mPrimary->createCamera(CAMERA_NAME);
		mSecondary->createCamera(CAMERA_NAME);
	}

	virtual void createFrameListener()
	{
		mRoot->addFrameListener(_mc);
	}

	virtual void createViewports()
	{
		mWindow->removeAllViewports();

		Ogre::Viewport *vp = 0;
		mPrimaryCam = mPrimary->getCamera(CAMERA_NAME);
		vp = mWindow->addViewport(mPrimaryCam, 0, 0, 0, 0.5, 1);
		vp->setBackgroundColour(ColourValue(0, 0, 0));
		mPrimaryCam->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
		mPrimaryCam->setPosition(-15, 40, MAX_Z*1.25);
		mPrimaryCam->lookAt(MAX_X*1.25, 0, MAX_Z*1.25);
		mPrimaryCam->setNearClipDistance(5);

		mSecondaryCam = mSecondary->getCamera(CAMERA_NAME);
		vp = mWindow->addViewport(mSecondaryCam, 1, 0.5, 0, 0.5, 1);
		vp->setBackgroundColour(ColourValue(0, 0, 0));
		mSecondaryCam->setAspectRatio(Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
		mSecondaryCam->setPosition(-15, 40, MAX_Z*1.25);
		mSecondaryCam->lookAt(MAX_X*1.25, 0, MAX_Z*1.25);
		mSecondaryCam->setNearClipDistance(5);
	}

	void DrawPlayground(int width, int height, Ogre::SceneManager* mgr)
	{
		Ogre::Plane plane(Vector3::UNIT_Y, -10);
		Ogre::MeshManager::getSingleton().createPlane("plane",
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
			width, height, width, height, true, 1, 5, 5, Vector3::UNIT_X);
		Ogre::Entity* ent = mgr->createEntity("LightPlaneEntity", "plane");
		mgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
		ent->setMaterialName("Examples/GrassFloor");
	}


};



#endif ///> end of __ENTRY_CLASS_H__