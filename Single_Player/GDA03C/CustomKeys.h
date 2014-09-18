#ifndef __CUSTOM_KEYS_H__
#define __CUSTOM_KEYS_H__

#include <Ogre/ExampleApplication.h>
#include <map>
#include <vector>

#include "TinyXml/tinyxml.h"
#include "Utility.h"

class GameStatus;

typedef std::map<int, std::string> KeysFnTable;
typedef bool (*Func)(GameStatus* gs);
typedef std::map<std::string, Func> FnNameTable;

bool CurrentBlockMoveXPositive(GameStatus* gs)
{
	return gs->getCurrentBlock()->MoveX(Blocks::AxisDirection::ePositive);
}

bool CurrentBlockMoveXNegative(GameStatus* gs)
{
	return gs->getCurrentBlock()->MoveX(Blocks::AxisDirection::eNegative);
}

bool CurrentBlockMoveZPositive(GameStatus* gs)
{
	return gs->getCurrentBlock()->MoveZ(Blocks::AxisDirection::ePositive);
}

bool CurrentBlockMoveZNegative(GameStatus* gs)
{
	return gs->getCurrentBlock()->MoveZ(Blocks::AxisDirection::eNegative);
}

bool CurrentBlockRotateOnY(GameStatus* gs)
{
	return gs->getCurrentBlock()->RotateOnY(90);
}

bool CurrentBlockFall(GameStatus* gs)
{
	gs->getCurrentBlock()->setFallInterval(50);
	return true;
}

class CustomKeys
{
private:
	OIS::Keyboard*					_keyboard;
	GameStatus*						_status;

	KeysFnTable						_keysTable;
	FnNameTable						_fnNameTable;

	bool							_isKeyPressed[256];

public:
	CustomKeys(OIS::Keyboard* kbd, GameStatus* gs)
		: _keyboard(kbd)
		, _status(gs)
	{

	}

	~CustomKeys()
	{
		Destroy();
	}

	//////////////////////////////////////////////////////////////////////////////
	// Important functions
	//	-- Initialize
	//	-- Update
	//	-- Destroy
	//////////////////////////////////////////////////////////////////////////////

	bool Initialize()
	{
		MyLog::put(__LINE__, __FILE__, "CustomKeys Initializing.\n");

		memset(_isKeyPressed, 0, sizeof(_isKeyPressed));
		RegisterKeys();

		return true;
	}

	bool Update()
	{
		_keyboard->capture();
		for (KeysFnTable::iterator it = _keysTable.begin(); it != _keysTable.end(); it++) {
			OIS::KeyCode tmp = (OIS::KeyCode)it->first;
			if (_keyboard->isKeyDown(tmp) && !_isKeyPressed[tmp]) {
				_fnNameTable[it->second](_status);
				_isKeyPressed[tmp] = true;
			} else if (!_keyboard->isKeyDown(tmp)) {
				_isKeyPressed[tmp] = false;
			}
		}

		return true;
	}

	bool Destroy()
	{
		return true;
	}

	//////////////////////////////////////////////////////////////////////////////
	// Custom functions
	//////////////////////////////////////////////////////////////////////////////

	KeysFnTable				getKeysTable() const			{ return _keysTable; }
	FnNameTable				getFnNameTable() const			{ return _fnNameTable; }

	struct KeyFunc
	{
		std::string								_keyName;
		int										_keyCode;
		std::string								_keyFuncName;
		int										_argument;

		void									Output()
		{
			std::cout << _keyName << ' ' << _keyCode << ' '
				<< _keyFuncName << ' ' << _argument << '\n';
		}
	};

	std::vector<KeyFunc> ReadXmlKeys(const char* filepath)
	{
		std::vector<KeyFunc> res;
		
		TiXmlDocument doc;
		if (doc.LoadFile(filepath)) {
			doc.Print();
		} else {
			MyLog::put(__LINE__, __FILE__, "can not parse xml file.\n");
			return std::vector<KeyFunc>();
		}

		TiXmlElement* rootElement = doc.RootElement();
		TiXmlElement* keyElement = rootElement->FirstChildElement();

		while (keyElement) {
			KeyFunc kf;
			TiXmlAttribute* atb = keyElement->FirstAttribute();
			kf._keyName = atb->Value();

			TiXmlElement* elt = keyElement->FirstChildElement();
			while (elt) {
				if (strcmp(elt->FirstAttribute()->Value(), "key_code") == 0) {
					kf._keyCode = strtol(elt->GetText(), NULL, 16);
				} else if (strcmp(elt->FirstAttribute()->Value(), "argument") == 0) {
					kf._argument = atoi(elt->GetText());
				} else {
					kf._keyFuncName = elt->GetText();
				}
				elt = elt->NextSiblingElement();
			}
			keyElement = keyElement->NextSiblingElement();
			res.push_back(kf);
		}
		return res;
	}

	void RegisterKeys()
	{
		std::vector<KeyFunc> vv = ReadXmlKeys("GDA03Keys.xml");

		for (int i = 0; i < vv.size(); i++) {
			vv[i].Output();
			_keysTable[vv[i]._keyCode] = vv[i]._keyFuncName;
		}
	}

	void RegisterFnName(std::string name, bool (*fn)(GameStatus* gs))
	{
		_fnNameTable[name] = fn;
	}
};


#endif ///> end of __CUSTOM_KEYS_H__