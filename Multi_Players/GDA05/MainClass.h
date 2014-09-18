#ifndef __MAIN_CLASS_H__
#define __MAIN_CLASS_H__

#include "iostream"
#include <stdio.h>
#include <string.h>

#include <Ogre/ExampleApplication.h>
#include <Ogre/SdkTrays.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <RakPeerInterface.h>
#include <RakNetTypes.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "GameStatus.h"
#include "CustomKeys.h"

#define MAX_CLIENTS 1
#define SERVER_PORT 60002
#define CAMERA_NAME "SceneCamera"

char gServerIp[16] = "127.0.0.1";

class MainClass : public ExampleFrameListener, public OgreBites::SdkTrayListener
{
private:
	SceneManager*						_scenePrimaryMgr;
	SceneManager*						_sceneSecondaryMgr;

	GameStatus*							_currentStatus1P;
	GameStatus*							_currentStatus2P;

	CustomKeys*							_customKeys1P;
	CustomKeys*							_customKeys2P;

	bool								_cPressed;

	OgreBites::SdkTrayManager*			_trayMgr;
	OgreBites::Label*					_scoreLbl1P;
	OgreBites::Label*					_scoreLbl2P;

	bool								_isServer;
	RakNet::RakPeerInterface*			_peerServer;
	RakNet::RakPeerInterface*			_peerClient;

	std::vector<int>					_randNumberList1;
	std::vector<int>					_randNumberList2;

public:

	//////////////////////////////////////////////////////////////////////////////
	// Basic functions
	//	-- Constructor / Destructor
	//	-- Inherited function
	//////////////////////////////////////////////////////////////////////////////

	MainClass(RenderWindow* inWin, SceneManager* inManager1, SceneManager* inManager2, bool server)
		: _scenePrimaryMgr(inManager1)
		, _sceneSecondaryMgr(inManager2)
		, _isServer(server)
		, ExampleFrameListener(inWin, inManager1->getCamera(CAMERA_NAME))
		, _trayMgr(new OgreBites::SdkTrayManager("mTrayMgr", mWindow, mMouse, this))
	{
		_randNumberList1.clear();
		_randNumberList2.clear();
	}

	virtual ~MainClass()
	{

	}

	virtual bool frameStarted(const FrameEvent& evt)
	{
		return (Update() && ExampleFrameListener::frameStarted(evt));
	}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
	{
		// override the original function to disable the roaming.
		if( mKeyboard->isKeyDown(OIS::KC_ESCAPE)) {
			return false;
		}
		return true;
	}

	virtual bool processUnbufferedMouseInput(const FrameEvent& evt)
	{
		// override the original function to disable the looking around.
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Important functions
	//	-- Initialize
	//	-- Update
	//	-- Destroy
	//////////////////////////////////////////////////////////////////////////////

	bool Initialize()
	{
		MyLog::put(__LINE__, __FILE__, "MainClass Initializing.\n");

		if (_isServer) {
			ServerInitialize();
		} else {
			ClientInitialize();
		}

		_currentStatus1P = new GameStatus(_scenePrimaryMgr, _randNumberList1);
		_currentStatus2P = new GameStatus(_sceneSecondaryMgr, _randNumberList2);

// 		_currentStatus1P->Initialize();
// 		_currentStatus2P->Initialize();

		boost::thread p1InitThread(boost::bind(&GameStatus::Initialize, _currentStatus1P));
		boost::thread p2InitThread(boost::bind(&GameStatus::Initialize, _currentStatus2P));

		p1InitThread.join();
		p2InitThread.join();

		_customKeys1P = new CustomKeys(ExampleFrameListener::mKeyboard, _currentStatus1P, "GDA03Keys1P.xml", _peerServer);
//		_customKeys2P = new CustomKeys(ExampleFrameListener::mKeyboard, _currentStatus2P, "GDA03Keys2P.xml", _peerServer);

// 		_customKeys1P->Initialize();
// 		_customKeys2P->Initialize();

		boost::thread p1KeysInit(boost::bind(&CustomKeys::Initialize, _customKeys1P));
//		boost::thread p2KeysInit(boost::bind(&CustomKeys::Initialize, _customKeys2P));

		p1KeysInit.join();
//		p2KeysInit.join();

		_scoreLbl1P = _trayMgr->createLabel(OgreBites::TL_TOPLEFT, _scenePrimaryMgr->getName(), "", 220);
		_scoreLbl2P = _trayMgr->createLabel(OgreBites::TL_TOPRIGHT, _sceneSecondaryMgr->getName(), "", 220);
		_trayMgr->hideCursor();

		return true;
	}

	bool Update()
	{
// 		_currentStatus1P->Update();
// 		_currentStatus2P->Update();

		boost::thread p1Update(boost::bind(&GameStatus::Update, _currentStatus1P));
		boost::thread p2Update(boost::bind(&GameStatus::Update, _currentStatus2P));

		p1Update.join();
		p2Update.join();

// 		_customKeys1P->Update();
// 		_customKeys2P->Update();

		boost::thread p1KeyUpdate(boost::bind(&CustomKeys::Update, _customKeys1P));
//		boost::thread p2KeyUpdate(boost::bind(&CustomKeys::Update, _customKeys2P));
		boost::thread p2KeyUpdate(boost::bind(&MainClass::ReceiveKeyMsg, this));
//		ReceiveKeyMsg();

		p1KeyUpdate.join();
		p2KeyUpdate.join();

		if (_currentStatus1P->getCurrentBlock()->GameOver()) {
			_scoreLbl1P->setCaption("Game Over ...");
		} else {
			char tmp[10];
			itoa(_currentStatus1P->getCurrentBlock()->getScore() * 100, tmp, 10);
			_scoreLbl1P->setCaption(tmp);
		}
		if (_currentStatus2P->getCurrentBlock()->GameOver()) {
			_scoreLbl2P->setCaption("Game Over ...");
		} else {
			char tmp[10];
			itoa(_currentStatus2P->getCurrentBlock()->getScore() * 100, tmp, 10);
			_scoreLbl2P->setCaption(tmp);
		}

		return true;
	}

	bool Destroy()
	{
		if (!_currentStatus1P->Destroy()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_currentStatus destroy() failed.\n");
			return false;
		}
		if (!_currentStatus2P->Destroy()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_currentStatus destroy() failed.\n");
			return false;
		}

		if (!_customKeys1P->Destroy()) {
			MyLog::put(__LINE__, __FILE__, "MainClass::_customKeys destroy() failed.\n");
			return false;
		}
// 		if (!_customKeys2P->Destroy()) {
// 			MyLog::put(__LINE__, __FILE__, "MainClass::_customKeys destroy() failed.\n");
// 			return false;
// 		}

		if (_isServer) {
			_peerServer->Shutdown(30);
			RakNet::RakPeerInterface::DestroyInstance(_peerServer);
		} else {
			_peerClient->Shutdown(30);
			RakNet::RakPeerInterface::DestroyInstance(_peerClient);
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Custom functions
	//////////////////////////////////////////////////////////////////////////////

	GameStatus*				getCurrentStatus() const			{ return _currentStatus1P; }
	bool					getIsServer() const					{ return _isServer; }
	void					setIsServer(bool val)				{ _isServer = val; }

	bool					ServerInitialize()
	{
		_peerServer = RakNet::RakPeerInterface::GetInstance();
		RakNet::Packet* packet;
		RakNet::SocketDescriptor sd(SERVER_PORT, 0);

		_peerServer->Startup(MAX_CLIENTS, &sd, 1);
		_peerServer->SetMaximumIncomingConnections(MAX_CLIENTS);

		_isServerCreated = true;

		std::cout << "Waiting for the client...\n";

		while (true) {
			for (packet = _peerServer->Receive(); packet; _peerServer->DeallocatePacket(packet), packet = _peerServer->Receive()) {
				switch (packet->data[0])
				{
				case ID_NEW_INCOMING_CONNECTION:
					{
						RakNet::BitStream bsOut;
						srand((unsigned)time(0));

						for (int i = 0; i < 100; i++) {
							_randNumberList1.push_back(rand());
							_randNumberList2.push_back(rand());
						}

						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_HELLO);
// 						bsOut.Write(_randNumberList1);
// 						bsOut.Write(_randNumberList2);

						for (int i = 0; i < 100; i++) {
							bsOut.Write(_randNumberList1[i]);
							bsOut.Write(_randNumberList2[i]);
						}
						std::cout << "randomizing seed has been sent.\n";
//						bsOut.Write(RakNet::RakString("hello"));
						_peerServer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

//						system("pause");

						return true;
					}
					break;

				default:
					break;
				}
			}
		}

		return false;
	}

	bool					ClientInitialize()
	{
		_peerClient = RakNet::RakPeerInterface::GetInstance();
		RakNet::Packet* packet;

		RakNet::SocketDescriptor sd;
		_peerClient->Startup(1, &sd, 1);

		_peerClient->Connect(gServerIp, SERVER_PORT, 0, 0);

		std::cout << "Waiting for the server...\n";

		while (true) {
			for (packet = _peerClient->Receive(); packet; _peerClient->DeallocatePacket(packet), packet = _peerClient->Receive()) {
				switch (packet->data[0])
				{
				case ID_GAME_MESSAGE_HELLO:
					{
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
// 						_randNumberList1.resize(100);
// 						_randNumberList2.resize(100);
						for (int i = 0, tmp; i < 100; i++) {
							bsIn.Read(tmp);
							_randNumberList1.push_back(tmp);
							bsIn.Read(tmp);
							_randNumberList2.push_back(tmp);
						}
// 						bsIn.Read(_randNumberList1);
// 						bsIn.Read(_randNumberList2);
						std::cout << "randomizing seed has been received.\n";
// 						RakNet::RakString incoming;
// 						bsIn.Read(incoming);
// 						std::cout << "-----------------------------------" << incoming.C_String() << std::endl;

//						system("pause");

						return true;
					}
					break;
				}
			}
		}
		return false;
	}

	void					ReceiveKeyMsg()
	{
		RakNet::Packet* packet;
		for (packet = _peerServer->Receive(); packet; _peerServer->DeallocatePacket(packet), packet = _peerServer->Receive()) {
			switch (packet->data[0])
			{
			case ID_GAME_MOV_X_POSITIVE:
				{
//					std::cout << "Action Message Received.\n";
					_currentStatus2P->getCurrentBlock()->MoveX(Blocks::AxisDirection::ePositive);
				} break;
			case ID_GAME_MOV_X_NEGATIVE:
				{
//					std::cout << "Action Message Received.\n";
					_currentStatus2P->getCurrentBlock()->MoveX(Blocks::AxisDirection::eNegative);
				} break;
			case ID_GAME_MOV_Z_POSITIVE:
				{
//					std::cout << "Action Message Received.\n";
					_currentStatus2P->getCurrentBlock()->MoveZ(Blocks::AxisDirection::ePositive);
				} break;
			case ID_GAME_MOV_Z_NEGATIVE:
				{
//					std::cout << "Action Message Received.\n";
					_currentStatus2P->getCurrentBlock()->MoveZ(Blocks::AxisDirection::eNegative);
				} break;
			case ID_GAME_ROT_Y:
				{
//					std::cout << "Action Message Received.\n";
					_currentStatus2P->getCurrentBlock()->RotateOnY(90);
				} break;
			case ID_GAME_FALL:
				{
//					std::cout << "Action Message Received.\n";
					_currentStatus2P->getCurrentBlock()->setFallInterval(50);
				} break;
			default:
//				std::cout << "A: Action Message Received.\n";
				break;
			}
		}
	}
};

#endif