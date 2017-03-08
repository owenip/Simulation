#pragma once
class ConfigClass
{
public:
	ConfigClass();
	~ConfigClass();

	bool Initialize();
	void Shutdown();

	bool CheckFullScreen() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;

private:
	bool ReadConfigFile();

private:
	bool FullScreen;
	int screenWidth, screenHeight;
};

