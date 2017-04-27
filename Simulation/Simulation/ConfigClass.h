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

	float	GetSurfaceRadius() const;

	int		GetNumberOfBalls() const;
	float	GetBallRadius() const;

	void	SetIsPaused(bool InVal);
	bool	GetIsPaused() const;

	void	SetOwnerID(int InID);
	int		GetOwnerID() const;

	void	SetElasticForce(float InForce);
	float	GetElasticForce() const;

	void	SetGroundFriction(float InFriction);
	float	GetGroundFriction() const;
	
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

	//Surface properties
	float mSurfaceRadius;
	
	//Ball properties
	int mNumberOfBalls;
	float mBallRadius;

	//Local Peer ID
	int mOwnerID;

	//Forces Properties
	float mGroundFriction;

	float mElasticForce;
};

