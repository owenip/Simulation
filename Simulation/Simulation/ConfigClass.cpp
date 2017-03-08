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

bool ConfigClass::Initialise()
{
	FullScreen = false;
	screenWidth = 1024;
	screenHeight = 768;
	return true;
}

bool ConfigClass::ReadConfigFile()
{
	return false;
}
