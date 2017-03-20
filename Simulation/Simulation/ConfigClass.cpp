#include "pch.h"
#include "ConfigClass.h"


ConfigClass::ConfigClass():
	fullScreen(false),
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
		//fullScreen = false;
		//screenWidth = 1024;
		//screenHeight = 768;
	}
	return true;
}

void ConfigClass::Shutdown()
{
}

bool ConfigClass::CheckFullScreen() const
{
	return fullScreen;
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
	ifstream configfile(configFileName);
	
	if (!configfile.good())
	{
		throw MessageBoxA(NULL, "Unable to read config file.", "Error", MB_OK | MB_ICONERROR);;
		return false;
	}
	else
	{
		string line;
		while (getline(configfile, line))
		{
			istringstream is_line(line);
			string key;
			if (getline(is_line, key, '='))
			{
				string value;
				if (getline(is_line, value))
				{
					StoreValue(key, value);
				}
			}
		}
		return true;
	}
}

void ConfigClass::StoreValue(string &key, string &value)
{
	if (key == "FullScreen")
	{
		if (value == "True")
		{
			fullScreen = true;
		}
		else
		{
			fullScreen = false;
		}
	}
	else if (key == "ScreenWidth")
	{
		try {
			int InScreenWidth = stoi(value);
			screenWidth = InScreenWidth;
		}
		catch (const std::invalid_argument& ia)
		{
			screenWidth = 800;
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	else if (key == "ScreenHeight")
	{
		try {
			int InScreenHeight = stoi(value);
			screenHeight = InScreenHeight;
		}
		catch (const std::invalid_argument& ia)
		{
			screenHeight = 600;
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	return;
}