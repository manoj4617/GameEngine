#ifndef ENGINE_H
#define ENGINE_H

#define DIRECTINPUT_VERSION 0x0800

//System headers
#include <stdio.h>
#include <tchar.h>
#include <windowsx.h>


//Directx9 headers
#include <d3d9.h>
#include <dinput.h>
#include <dplay8.h>
#include <dmusicc.h>


#define SAFE_DELETE(p)		 { if(p) {delete (p) ;		(p) = NULL}}
#define SAFE_DELETE_ARRAY(p) { if(p) {delete [] (p);	(p) = NULL}}
#define SAFE_RELEASE(p)		 { if(p) { (p)->Release();	(p) = NULL}}

#include "LinkedList.h"
#include "ResourceManagement.h"
//#include "Geometry.h"

struct EngineSetup {
	HINSTANCE instance;
	char* name;

	EngineSetup() {
		instance = NULL;
		name = (char*)("Application");
	}
};

class Engine {
public:	
	Engine(EngineSetup* setup = NULL);
	virtual ~Engine();

	void Run();
	HWND GetWindow();
	void SetDeactiveFlag(bool deactive);

private:
	bool m_loaded;		// Indicates if the engine is loading
	HWND m_window;		// Main window handle
	bool m_deactive;	// Indicates if the application is active or not

	EngineSetup* m_setup;
};

extern Engine* g_engine;

#endif



