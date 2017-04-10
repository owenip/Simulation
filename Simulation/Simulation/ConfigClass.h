#pragma once
class ConfigClass
{
public:
	ConfigClass();
	~ConfigClass() = default;

	bool Initialize();

	bool CheckFullScreen() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;

	int GetNumberOfBalls() const;
	float GetBallRadius() const;
	
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
	
	int mNumberOfBalls;
	float mBallRadius;
};

