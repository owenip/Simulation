#include "pch.h"
#include "ConfigClass.h"


ConfigClass::ConfigClass():
	mFullScreen(false),
	mScreenWidth(0),
	mScreenHeight(0)
{
	
}


ConfigClass::~ConfigClass()
{
}

bool ConfigClass::Initialize()
{
	if (!ReadConfigFile())
	{
		SetDefaultAll();
	}
	return true;
}

void ConfigClass::Shutdown()
{
}

bool ConfigClass::CheckFullScreen() const
{
	return mFullScreen;
}

int ConfigClass::GetScreenWidth() const
{
	return mScreenWidth;
}

int ConfigClass::GetScreenHeight() const
{
	return mScreenHeight;
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
		//Get line
		while (getline(configfile, line))
		{
			//Get key from the line
			istringstream is_line(line);
			string key;
			if (getline(is_line, key, '='))
			{
				//Get Value from line
				string value;
				if (getline(is_line, value))
				{					
					StoreValue(key, value);
				}
			}			
		}
		configfile.close();
		return true;
	}
}

void ConfigClass::StoreValue(string &key, string &value)
{
	if (key == "FullScreen")
	{
		if (value == "True")
		{
			mFullScreen = true;
		}
		else
		{
			SetDefault(key);
		}
	}
	else if (key == "ScreenWidth")
	{
		try {
			int InScreenWidth = stoi(value);
			mScreenWidth = InScreenWidth;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	else if (key == "ScreenHeight")
	{
		try {
			int InScreenHeight = stoi(value);
			mScreenHeight = InScreenHeight;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	return;
}


void ConfigClass::SetDefault(string & key)
{
	if (key == "FullScreen")
	{
		mFullScreen = false;		
	}
	else if (key == "ScreenWidth")
	{
		mScreenWidth = 800;
	}
	else if (key == "ScreenHeight")
	{	
		mScreenHeight = 600;		
	}
}

void ConfigClass::SetDefaultAll()
{
	SetDefault(string("FullScreen"));
	SetDefault(string("ScreenWidth"));
	SetDefault(string("ScreenHeight"));
}


