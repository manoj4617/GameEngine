#include <Windows.h>

#include "Engine.h"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev, LPSTR cmdLine, int cmdShow) {
	char nameBuffer[] = "Framework Test";
	EngineSetup setup;
	setup.instance = instance;
	setup.name = nameBuffer;

	new Engine(&setup);
	g_engine->Run();

	return true;
}