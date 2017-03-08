#pragma once
class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&) = default;
	SystemClass& operator=(const SystemClass&) = default;
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM) const;

private:
	bool Render();
	void InitializeWindow(int& screenWidth, int& screenHeight);
	void ShutdownWindows();
	void CalculateFrameStats();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	std::wstring mMainWndCaption;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;