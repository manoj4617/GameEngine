#include "Engine.h"

Engine* g_engine = NULL;

LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
	case WM_ACTIVATEAPP:
		g_engine->SetDeactiveFlag(!wparam);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProcA(wnd, msg, wparam, lparam);
	}
}


// Engine class constructor
Engine::Engine(EngineSetup* setup) {
	
	// indicate that the engine is not yet loaded
	m_loaded = false;

	//  if no setup structure was passed in, then create a default one
	m_setup = new EngineSetup;
	if (setup != NULL) {
		memcpy(m_setup, setup, sizeof(EngineSetup));
	}

	g_engine = this;

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_CLASSDC;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_setup->instance;
	wcex.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"WindowClass";
	wcex.hIconSm = LoadIconW(NULL, IDI_APPLICATION);

	RegisterClassExW(&wcex);

	// Initialize the COM using multithreade concurrency
	CoInitializeEx(NULL, COINITBASE_MULTITHREADED);

	const char* narrowStr = m_setup->name;
	int wideStrLen = MultiByteToWideChar(CP_UTF8, 0, narrowStr, -1, NULL, 0);
	WCHAR* wideStr = new WCHAR[wideStrLen];
	MultiByteToWideChar(CP_UTF8, 0, narrowStr, -1, wideStr, wideStrLen);

	// Create a window and retrieve a handle to it
	m_window = CreateWindowW(L"WindowClass", wideStr, WS_OVERLAPPED, 0, 0, 800, 600, NULL, NULL, m_setup->instance, NULL);

	srand(timeGetTime());

	m_loaded = true;
}

Engine::~Engine() {
	if (m_loaded == true) {

	}

	CoUninitialize();

	UnregisterClass(L"WindowClass", m_setup->instance);

	SAFE_DELETE(m_setup);
}

void Engine::Run() {
	if (m_loaded == true) {
		ShowWindow(m_window, SW_NORMAL);

		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));
		
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else if (!m_deactive) {
				unsigned long currentTime = timeGetTime();
				static unsigned long lastTime = currentTime;
				float elapsed = (currentTime - lastTime) / 1000.0f;
				lastTime = currentTime;
			}
		}
	}

	SAFE_DELETE(g_engine);
}

HWND Engine::GetWindow() {
	return m_window;
}

void Engine::SetDeactiveFlag(bool deactive) {
	m_deactive = deactive;
}