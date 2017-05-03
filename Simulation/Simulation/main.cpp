#include "pch.h"
#include "SystemClass.h"

int WINAPI WinMain(const HINSTANCE, const HINSTANCE, const PSTR, const int)
{	

#ifdef 	_OPENMP
	cout <<  "OpenMP enabled"  << endl;
#endif


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
