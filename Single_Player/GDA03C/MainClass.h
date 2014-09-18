#ifndef __MAIN_CLASS_H__
#define __MAIN_CLASS_H__

#include <Ogre/ExampleApplication.h>
#include <Ogre/SdkTrays.h>
#include "GameStatus.h"
#include "CustomKeys.h"


class MainClass : public ExampleFrameListener, public OgreBites::SdkTrayListener
{
private:
	SceneManager*						_sceneMgr;
	Ogre::Camera*						_camera;

	GameStatus*							_currentStatus;

	CustomKeys*							_customKeys;

	bool								_cPressed;

	OgreBites::SdkTrayManager*			_trayMgr;
	OgreBites::Label*					_scoreLbl;

public:

	//////////////////////////////////////////////////////////////////////////////
	// Basic functions
	//	-- Constructor / Destructor
	//	-- Inherited function
	//////////////////////////////////////////////////////////////////////////////

	MainClass(RenderWindow* inWin, Camera* inCam, SceneManager* inManager)
		: ExampleFrameListener(inWin, inCam)
		, _sceneMgr(inManager)
		, _camera(inCam)
		, _trayMgr(new OgreBites::SdkTrayManager("TrayManager", mWindow, mMouse, this))
	{

	}

	virtual ~MainClass()
	{

	}

	virtual bool frameStarted(const FrameEvent& evt)
	{
		return (Update() && ExampleFrameListener::frameStarted(evt));
	}

// 	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
// 	{
// 		if( mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
// 			return false;
// 		}
// 		return true;
// 	}

	//////////////////////////////////////////////////////////////////////////////
	// Important functions
	//	-- Initialize
	//	-- Update
	//	-- Destroy
	//////////////////////////////////////////////////////////////////////////////

	bool Initialize()
	{
		MyLog::put(__LINE__, __FILE__, "MainClass Initializing.\n");

		_currentStatus = new GameStatus(_sceneMgr);
		if (!_currentStatus->Initialize()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_currentStatus initialize() failed.\n");
			return false;
		}

		_customKeys = new CustomKeys(ExampleFrameListener::mKeyboard, _currentStatus);
		if (!_customKeys->Initialize()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_customKeys initialize() failed.\n");
			return false;
		}

		_customKeys->RegisterFnName("MoveXPositive", CurrentBlockMoveXPositive);
		_customKeys->RegisterFnName("MoveXNegative", CurrentBlockMoveXNegative);
		_customKeys->RegisterFnName("MoveZPositive", CurrentBlockMoveZPositive);
		_customKeys->RegisterFnName("MoveZNegative", CurrentBlockMoveZNegative);
		_customKeys->RegisterFnName("RotateOnY", CurrentBlockRotateOnY);
		_customKeys->RegisterFnName("BlockFall", CurrentBlockFall);

		_scoreLbl = _trayMgr->createLabel(OgreBites::TL_TOPLEFT, "Score:", "", 220);
		_trayMgr->hideCursor();

		return true;
	}

	bool Update()
	{
		if (!_currentStatus->Update()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_currentStatus update() failed.\n");
			return false;
		}

		if (!_customKeys->Update()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_customKeys update() failed.\n");
			return false;
		}

		if (_currentStatus->getCurrentBlock()->GameOver()) {
			_scoreLbl->setCaption("Game Over ...");
		} else {
			char tmp[10];
			itoa(_currentStatus->getCurrentBlock()->getScore() * 100, tmp, 10);
			_scoreLbl->setCaption(tmp);
		}

// 		std::cout << _camera->getPosition().x << ' '
// 				  << _camera->getPosition().y << ' '
// 				  << _camera->getPosition().z << '\n';

// 		for (KeysFnTable::iterator it = _customKeys->getKeysTable().begin();
// 				it != _customKeys->getKeysTable().end();
// 				it++) {
// 			if (mKeyboard->isKeyDown((OIS::KeyCode)it->first)) {
// 				_customKeys->getFnNameTable()[it->second](this->_currentStatus);
// 			}
// 		}

// 		mKeyboard->capture();
// 		if (mKeyboard->isKeyDown(OIS::KC_C) && !_cPressed) {
// 			_currentStatus->getCurrentBlock()->RotateOnY(90);
// 			_cPressed = true;
// 		} else if (!mKeyboard->isKeyDown(OIS::KC_C)) {
// 			_cPressed = false;
// 		}

		return true;
	}

	bool Destroy()
	{
		if (!_currentStatus->Destroy()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_currentStatus destroy() failed.\n");
			return false;
		}

		if (!_customKeys->Destroy()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_customKeys destroy() failed.\n");
			return false;
		}
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Custom functions
	//////////////////////////////////////////////////////////////////////////////

	GameStatus*				getCurrentStatus() const			{ return _currentStatus; }
};

#endif