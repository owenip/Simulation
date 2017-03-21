#include "pch.h"
#include "GraphicClass.h"



GraphicClass::GraphicClass():
	mConfig(nullptr)
{

}


GraphicClass::~GraphicClass()
{
}

bool GraphicClass::Initialize(const HWND hwnd, const ConfigClass * mConfig)
{
	HRESULT hr = 0;

	//DirectX Class initialise

	//AntTweakBar initialse

	return false;
}

void GraphicClass::Shutdown()
{
	if (mConfig)
	{
		delete mConfig;
		mConfig = nullptr;
	}
}
