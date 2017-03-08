#include "ConfigClass.h"



ConfigClass::ConfigClass():
	FullScreen(false),
	screenWidth(0),
	screenHeight(0)
{
}


ConfigClass::~ConfigClass()
{
}

bool ConfigClass::Initialize()
{
	if (!ReadConfigFile())
	{
		FullScreen = false;
		screenWidth = 1024;
		screenHeight = 768;
	}
	return true;
}

void ConfigClass::Shutdown()
{
}

bool ConfigClass::CheckFullScreen() const
{
	return FullScreen;
}

int ConfigClass::GetScreenWidth() const
{
	return screenWidth;
}

int ConfigClass::GetScreenHeight() const
{
	return screenHeight;
}

bool ConfigClass::ReadConfigFile()
{
	return false;
}
