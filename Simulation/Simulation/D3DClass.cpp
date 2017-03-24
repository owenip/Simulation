#include "pch.h"
#include "D3DClass.h"



D3DClass::D3DClass():
	mConfig(nullptr),
	mSwapChain(nullptr),
	mDevice(nullptr),
	mDeviceContext(nullptr)
{
	//Create Device and device context

	
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(const HWND hwnd, ConfigClass * mConfig)
{
	HRESULT result;

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


