#include "pch.h"
#include "D3DClass.h"



D3DClass::D3DClass():
	mConfig(nullptr)
{
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(const HWND hwnd, ConfigClass * mConfig)
{
	return false;
}

void D3DClass::Shutdown()
{
	if (mConfig)
	{
		delete mConfig;
		mConfig = nullptr;
	}
}


