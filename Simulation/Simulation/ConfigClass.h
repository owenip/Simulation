#pragma once
class ConfigClass
{
public:
	ConfigClass();
	ConfigClass(const ConfigClass&) = default;
	~ConfigClass() = default;

	bool	Initialize();

	bool	CheckFullScreen() const;
	int		GetScreenWidth() const;
	int		GetScreenHeight() const;

	int		GetNumberOfBalls() const;
	float	GetBallRadius() const;

	void	SetIsPaused(bool InVal);
	bool	GetIsPaused() const;

	void	SetOwnerID(int InID);
	int		GetOwenerID();
	
private:
	bool ReadConfigFile();
	void StoreValue(string &key, string &value);

	//Set Default Value
	void SetDefault(string &key);
	void SetDefaultAll();

private:
	const std::string configFileName = ".\\Resources\\config.txt";

	bool mIsPaused;

	bool mFullScreen;
	int mScreenWidth, mScreenHeight;
	
	int mNumberOfBalls;
	float mBallRadius;

	int OwnerID;
};

