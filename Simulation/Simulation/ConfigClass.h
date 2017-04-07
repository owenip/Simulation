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

	//Camera Pos
	
private:
	bool ReadConfigFile();
	void StoreValue(string &key, string &value);

	//Set Default Value
	void SetDefault(string &key);
	void SetDefaultAll();

private:
	const std::string configFileName = ".\\Resources\\config.txt";

	bool mFullScreen;
	int mScreenWidth, mScreenHeight;
	

};

