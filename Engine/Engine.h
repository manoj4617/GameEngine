#ifndef ENGINE_H
#define ENGINE_H


#define DIRECTINPUT_VERSION 0x0800

//System headers
#include <stdio.h>
#include <tchar.h>
#include <windowsx.h>


//Directx9 headers
#include <d3dx9.h>
#include <dinput.h>
#include <dplay8.h>
#include <dmusicc.h>


#define SAFE_DELETE(p)		 { if(p) {delete (p) ;		(p) = NULL;}}
#define SAFE_DELETE_ARRAY(p) { if(p) {delete [] (p);	(p) = NULL;}}
#define SAFE_RELEASE(p)		 { if(p) { (p)->Release();	(p) = NULL;}}

#include "LinkedList.h"
#include "ResourceManagement.h"
#include "Geometry.h"
#include "State.h"
#include "Input.h"
#include "Scripting.h"



struct EngineSetup {
	HINSTANCE instance;
	char* name;
	void (*StateSetup)();

	EngineSetup() {
		instance = NULL;
		name = (char*)("Application");
		StateSetup = NULL;
	}
};

class Engine {
public:	
	Engine(EngineSetup* setup = NULL);
	virtual ~Engine();

	void Run();
	HWND GetWindow();
	void SetDeactiveFlag(bool deactive);

	void AddState(State* state, bool change = true);
	void RemoveState(State* state);
	void ChangeState(unsigned long id);
	State* GetCurrentState();
	ResourceManager<Script>* GetScriptManager();
	Input* GetInput();

private:
	bool m_loaded;					// Indicates if the engine is loading
	HWND m_window;					// Main window handle
	bool m_deactive;				// Indicates if the application is active or not
		
	EngineSetup* m_setup;			// Copy of the engine setup struct

	LinkedList<State>* m_states;	// LL of states
	State* m_currentState;			// Pointer to current state
	bool m_stateChanged;			// indicates if the state has changed in the current frame

	ResourceManager<Script>* m_scriptManager; // Script Manager

	Input* m_input;					// Input obj
};

extern Engine* g_engine;

#endif



