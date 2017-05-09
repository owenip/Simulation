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

	float	GetGwRadius() const;

	void	SetIsPaused(bool InVal);
	bool	GetIsPaused() const;

	void	SetIsEscaped(bool InVal);
	bool	GetIsEscaped();

	void	SetDisplayAll(bool InVal);
	bool	GetDisplayAll() const;

	void	SetPeerID(int &InID);
	int		GetPeerID() const;

	void	SetElasticForce(float &InForce);
	float	GetElasticForce() const;

	void	SetFriction(float &InFriction);
	float	GetFriction() const;

	void	SetTarPhyFreq(float &InVal);
	float	GetTarPhyFreq() const;

	void	SetTarGraphicFreq(float &InVal);
	float	GetTarGraphicFreq() const;
	 
	void	SetTarNetworkFreq(float &InVal);
	float	GetTarNetworkFreq() const;

	void	SetActualPhyFreq(float &InVal);
	float	GetActualPhyFreq() const;

	void	SetActualGraphicFreq(float &InVal);
	float	GetActualGraphicFreq() const;

	void	SetActualNetworkFreq(float &InVal);
	float	GetActualNetworkFreq() const;

	void	SetTimeScale(float &InTimeScale);
	float	GettimeScale() const;

	void	SetInitServer(bool InVal);
	bool	GetInitServer() const;

	void	SetUDPPort(int InStr);
	int		GetUDPPort() const;

	void	SetTCPPort(int InStr);
	int		GetTCPPort() const;

	void	SetHostIP(string InStr);
	string	GetHostIP() const;

	void	SetMaxClient(int InVal);
	int		GetMaxClient() const;

	void	Reset();

private:
	bool ReadConfigFile();
	void StoreValue(string &key, string &value);

	//Set Default Value
	void SetDefault(string &key);
	void SetDefaultAll();

private:
	const std::string configFileName = ".\\Resources\\config.txt";

	std::mutex mutex_config;

	bool mIsEscaped;
	bool mIsPaused;

	bool mFullScreen;
	int mScreenWidth, mScreenHeight;

	//Surface properties
	float mSurfaceRadius;

	//Ball properties
	int mNumberOfBalls;
	float mBallRadius;
	bool mDisplayAll;
	
	//Gravity Well Properties
	float mGwRadius;

	//Local Peer ID
	int mPeerID;

	//Forces Properties
	float mFriction;
	float mElasticity;

	//Target Frequency
	float mTarPhysicsFreq;		//at least 120 ? asap?
	float mTarGraphicFreq;		//60
	float mTarNetworkFreq;		//4-30?

	float mActualPhysicsFreq;		
	float mActualGraphicFreq;
	float mActualNetworkFreq;
	
	float mTimeScale;

	int mMaxClient;
	bool mInitServer;
	int mUDPPort;
	int mTCPPort;
	string	mHostIP;
};

