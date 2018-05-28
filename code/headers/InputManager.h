#pragma once
#include <vector>
#include <bitset>

using namespace std;

class InputManager
{
public:
	static InputManager * Inst();
	
	bool IsKeyDown		(const unsigned char key)	{	return downKeys[key];		};
	bool IsKeyPressed	(const unsigned char key)	{	return pressedKeys[key];	};
	bool IsKeyReleased	(const unsigned char key)	{	return releasedKeys[key];	};

	bool IsSpecialDown		(const unsigned int key) { return downSpecial[key];		};
	bool IsSpecialPressed	(const unsigned int key) { return pressedSpecial[key];	};
	bool IsSpecialReleased	(const unsigned int key) { return releasedSpecial[key];	};

	bool IsMouseDown	(const unsigned int btn) { return downMouse[btn];		};
	bool IsMousePressed	(const unsigned int btn) { return pressedMouse[btn];	};
	bool IsMouseReleased(const unsigned int btn) { return releasedMouse[btn];	};

    vector<unsigned int> GetMousePosition() { return mousePosition; }
    vector<int> GetMouseDeltaPosition() { return mouseDeltaPosition; }

	void _Press		(const unsigned char key)	{ pressedKeys[key] = true; downKeys[key] = true;	printf("Pressed:%c\n",key);};
	void _Release	(const unsigned char key)	{ releasedKeys[key] = true; downKeys[key] = false;	printf("Released:%c\n",key);};

	void _PressSpecial(const int key)		{ pressedSpecial[key]	= true; downSpecial[key] = true;	printf("PressedSpecial:%d\n", key); };
	void _ReleaseSpecial(const unsigned int key)	{ releasedSpecial[key]	= true; downSpecial[key] = false;	printf("ReleasedSpecial:%d\n", key); };

	void _MousePos(const unsigned int x, const unsigned int y)
	{
        mouseDeltaPosition[0] += x - mousePosition[0];
        mouseDeltaPosition[1] += y - mousePosition[1];

		mousePosition[0] = x;
		mousePosition[1] = y;
	}
	void _Mouse(const unsigned int btn, const int state, const unsigned int x, const unsigned int y)
	{
        mouseDeltaPosition[0] += x - mousePosition[0];
        mouseDeltaPosition[1] += y - mousePosition[1];

		mousePosition[0] = x;
		mousePosition[1] = y;

		switch (state)
		{
		case 0:
			pressedMouse[btn] = true; downMouse[btn] = true;
			break;
		case 1:
			releasedMouse[btn] = true; downMouse[btn] = false;
			break;
		default:
			break;
		}
	};
	
	void _Clear()	
    { 
        pressedKeys.reset(); releasedKeys.reset();
        pressedSpecial.reset(); releasedSpecial.reset();
        pressedMouse.reset(); releasedMouse.reset(); 
        mouseDeltaPosition = { 0,0 };
    };
	void _ClearAll()
    { 
		pressedKeys.reset();  releasedKeys.reset();  downKeys.reset();
        pressedSpecial.reset(); releasedSpecial.reset(); downSpecial.reset();
		pressedMouse.reset(); releasedMouse.reset(); downMouse.reset();
        mouseDeltaPosition = { 0,0 };
	};


private:
	InputManager();
	
	static InputManager* inputManager;

	vector<unsigned int> mousePosition;
    vector<int> mouseDeltaPosition;

    bool mouseLocked;
    vector<unsigned int> mouseLockPosition;

	bitset<512> downKeys;
	bitset<512> pressedKeys;
	bitset<512> releasedKeys;

	bitset<512> downSpecial;
	bitset<512> pressedSpecial;
	bitset<512> releasedSpecial;

	bitset<10> downMouse;
	bitset<10> pressedMouse;
	bitset<10> releasedMouse;

};
