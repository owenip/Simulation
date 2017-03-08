#pragma once
class ConfigClass
{
public:
	ConfigClass();
	~ConfigClass();

	bool Initialise();

private:
	bool ReadConfigFile();

private:
	bool FullScreen;
	int screenWidth, screenHeight;
};

