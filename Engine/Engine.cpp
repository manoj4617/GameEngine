#include "Engine.h"

Engine* g_engine = NULL;

//-----------------------------------------------------------------------------
// Handles Windows messages.
//-----------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------
// The engine class constructor.
//-----------------------------------------------------------------------------
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

	m_states = new LinkedList<State>;
	m_currentState = NULL;

	m_input = new Input(m_window);

	srand(timeGetTime());

	if (m_setup->StateSetup != NULL)
		m_setup->StateSetup();

	m_loaded = true;
}

//-----------------------------------------------------------------------------
// The engine class destructor.
//-----------------------------------------------------------------------------
Engine::~Engine() {
	if (m_loaded == true) {
		if (m_currentState != NULL)
			m_currentState->Close();

		SAFE_DELETE(m_states);

		SAFE_DELETE(m_input);
	}

	CoUninitialize();

	UnregisterClass(L"WindowClass", m_setup->instance);

	SAFE_DELETE(m_setup);
}

//-----------------------------------------------------------------------------
// Enters the engine into the main processing loop.
//-----------------------------------------------------------------------------
void Engine::Run() {
	if (m_loaded == true) {
		ShowWindow(m_window, SW_NORMAL);

		ViewerSetup viewer;


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

				m_input->Update();

				if (m_input->GetKeyPress(DIK_F)) {
					PostQuitMessage(0);
				}

				// Request the viewer from the current state, if there is one
				if (m_currentState != NULL)
					m_currentState->RequestViewer(&viewer);

				// Update the current state, taking state change into account
				m_stateChanged = false;
				if (m_currentState != NULL)
					m_currentState->Update(elapsed);
				if (m_stateChanged == true)
					continue;
			}
		}
	}

	SAFE_DELETE(g_engine);
}

//-----------------------------------------------------------------------------
// Returns the window handle.
//-----------------------------------------------------------------------------
HWND Engine::GetWindow() {
	return m_window;
}

//-----------------------------------------------------------------------------
// Sets the deactive flag.
//-----------------------------------------------------------------------------
void Engine::SetDeactiveFlag(bool deactive) {
	m_deactive = deactive;
}

//-----------------------------------------------------------------------------
// Adds a state to the engine.
//-----------------------------------------------------------------------------
void Engine::AddState(State *state, bool change) {
	m_states->Add(state);

	if (change == false)
		return;

	if (m_currentState != NULL)
		m_currentState->Close();

	m_currentState = m_states->GetLast();
	m_currentState->Load();
}

//-----------------------------------------------------------------------------
// Removes a state from the engine
//-----------------------------------------------------------------------------
void Engine::RemoveState(State *state) {
	m_states->Remove(&state);
}

//-----------------------------------------------------------------------------
// Changes processing to the state with the specified ID.
//-----------------------------------------------------------------------------
void Engine::ChangeState(unsigned long id) {
	m_states->Iterate(true);

	while (m_states->Iterate() != NULL) {
		if (m_states->GetCurrent()->GetId() == id) {
			if (m_currentState != NULL)
				m_currentState->Close();

			m_currentState = m_states->GetCurrent();
			m_currentState->Load();

			m_stateChanged = true;
			
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Returns a pointer to the current state.
//-----------------------------------------------------------------------------
State* Engine::GetCurrentState() {
	return m_currentState;
}


//-----------------------------------------------------------------------------
// Returns a pointer to the input object.
//-----------------------------------------------------------------------------
Input* Engine::GetInput() {
	return m_input;
}