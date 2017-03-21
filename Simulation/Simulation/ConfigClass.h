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
	void StoreValue(string &key, string &value);

	//Set Default Value
	void SetDefault(string &key);
	void SetDefaultAll();

private:
	bool fullScreen;
	int screenWidth, screenHeight;

	const std::string configFileName = "config.txt";
};

