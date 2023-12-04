#ifndef INPUT_H

#define INPUT_H

class Input {
public:
	Input(HWND window);
	virtual ~Input();

	void Update();

	bool GetKeyPress(char key, bool ignorePressStamp = false);

	bool GetButtonPress(char button, bool ignorePressStamp = false);
	long GetPosX();
	long GetPosY();
	long GetDeltaX();
	long GetDeltaY();
	long GetDeltaWheel();

private:
	HWND m_window;				// Handle of the parent window
	IDirectInput8* m_di;		// DirectInput obj
	unsigned long m_pressStamp;	// Current press stamp (increamented every frame)

	IDirectInputDevice8W* m_keyboard;		// DIrectInput keyboad device
	char m_keyState[256];				// stores the state of the keyboard keys
	unsigned long m_keyPressStamp[256]; // Stamps the last frame each key was pressed

	IDirectInputDevice8* m_mouse;		// DirectInput mouse device
	DIMOUSESTATE m_mouseState;			// stores the state of the mouse
	unsigned long m_buttonPressStamp[3];// Stamps the last frame each button was pressed
	POINT m_position;					// Stores the position of the mouse cursor
};

#endif // !INPUT_H
