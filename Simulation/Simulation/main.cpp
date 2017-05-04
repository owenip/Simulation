#include "pch.h"
#include "SystemClass.h"

int WINAPI WinMain(const HINSTANCE, const HINSTANCE, const PSTR, const int)
{	
	
	//AllocConsole();
	//FILE *stream;
	//freopen_s(&stream, "CONOUT$", "w", stdout);

	AllocConsole();
	FILE* new_stdout;
	freopen_s(&new_stdout, "CONOUT$", "w", stdout);

	SetThreadAffinityMask(GetCurrentThread(),0b1);
	bool result;

	std::unique_ptr<SystemClass> mSystem;
	mSystem = make_unique<SystemClass>();

	result = mSystem->Initialize();
	if(result)
	{
		mSystem->Run();
	}

	mSystem->Shutdown();
	mSystem.reset();

	return 0;

}
