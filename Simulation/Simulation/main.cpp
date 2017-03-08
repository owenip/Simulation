#include "pch.h"
#include "SystemClass.h"


int WINAPI WinMain(const HINSTANCE, const HINSTANCE, const PSTR, const int)
{
	SystemClass* System;
	bool result;


	// Create the system object.
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	// Initialize and run the system object.
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	// Shutdown and release the system object.
	System->Shutdown();
	delete System;
	System = 0; // parasoft-suppress  CODSTA-MCPP-04 "Not require in spec."

	return 0;
}