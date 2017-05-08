#include "pch.h"
#include "ConfigClass.h"


ConfigClass::ConfigClass() :
	mIsPaused(false),
	mIsEscaped(false),
	mFullScreen(false),
	mScreenWidth(0),
	mScreenHeight(0),
	mSurfaceRadius(40.f),
	mNumberOfBalls(0),
	mBallRadius(0),
	mDisplayAll(false),
	mGwRadius(10.f),
	mPeerID(0),
	mFriction(0.5f),
	mElasticity(0),
	mTarPhysicsFreq(60.f),
	mTarGraphicFreq(60.f),
	mTarNetworkFreq(120.f),
	mTimeScale(1.f),
	mInitServer(true),
	mUDPPort(9171),
	mTCPPort(9172),
	mHostIP("127.0.0.1"),
	mMaxClient(3)
{
	if (!ReadConfigFile())
	{
		SetDefaultAll();
	}
}

bool ConfigClass::Initialize()
{
	if (!ReadConfigFile())
	{
		SetDefaultAll();
	}
	return true;
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

float ConfigClass::GetSurfaceRadius() const
{
	return mSurfaceRadius;
}

int ConfigClass::GetNumberOfBalls() const
{
	return mNumberOfBalls;
}

float ConfigClass::GetBallRadius() const
{
	return mBallRadius;
}

float ConfigClass::GetGwRadius() const
{
	return mGwRadius;
}

void ConfigClass::SetIsPaused(bool InVal)
{
	mIsPaused = InVal;
}

bool ConfigClass::GetIsPaused() const
{
	return mIsPaused;
}

void ConfigClass::SetIsEscaped(bool InVal)
{
	mIsEscaped = InVal;
}

bool ConfigClass::GetIsEscaped()
{
	return mIsEscaped;
}

void ConfigClass::SetDisplayAll(bool InVal)
{
	mDisplayAll = InVal;
}

bool ConfigClass::GetDisplayAll() const
{
	return mDisplayAll;
}

void ConfigClass::SetPeerID(int &InID)
{
	mPeerID = InID;
}

int ConfigClass::GetPeerID() const
{
	return mPeerID;
}

void ConfigClass::SetElasticForce(float &InForce)
{
	std::lock_guard<std::mutex> config_guard(mutex_config);
	mElasticity = InForce;
}

float ConfigClass::GetElasticForce() const
{
	return mElasticity;
}

void ConfigClass::SetFriction(float &InFriction)
{
	std::lock_guard<std::mutex> config_guard(mutex_config);
	mFriction = InFriction;
}

float ConfigClass::GetFriction() const
{
	return mFriction;
}

void ConfigClass::SetTarPhyFreq(float &InVal)
{
	std::lock_guard<std::mutex> config_guard(mutex_config);
	if(InVal > 0)
		mTarPhysicsFreq = InVal;
}

float ConfigClass::GetTarPhyFreq() const
{
	return mTarPhysicsFreq;
}

void ConfigClass::SetTarGraphicFreq(float &InVal)
{
	std::lock_guard<std::mutex> config_guard(mutex_config);
	if (InVal > 0)
		mTarGraphicFreq = InVal;
}

float ConfigClass::GetTarGraphicFreq() const
{
	return mTarGraphicFreq;
}

void ConfigClass::SetTarNetworkFreq(float &InVal)
{
	std::lock_guard<std::mutex> config_guard(mutex_config);
	if (InVal > 0)
		mTarNetworkFreq = InVal;
}

float ConfigClass::GetTarNetworkFreq() const
{
	return mTarNetworkFreq;
}

void ConfigClass::SetTimeScale(float & InTimeScale)
{
	std::lock_guard<std::mutex> config_guard(mutex_config);
	mTimeScale = InTimeScale;
}

float ConfigClass::GettimeScale() const
{	
	return mTimeScale;
}

void ConfigClass::SetInitServer(bool InVal)
{
	std::lock_guard<std::mutex> config_guard(mutex_config);
	mInitServer = InVal;
}

bool ConfigClass::GetInitServer() const
{
	return mInitServer;
}

void ConfigClass::SetUDPPort(int InStr)
{
	std::lock_guard<std::mutex> config_guard(mutex_config);
	mUDPPort = InStr;
}

int ConfigClass::GetUDPPort() const
{
	return mUDPPort;
}

void ConfigClass::SetTCPPort(int InStr)
{
	mTCPPort = InStr;
}

int ConfigClass::GetTCPPort() const
{
	return mTCPPort;
}

void ConfigClass::SetHostIP(string InStr)
{
	mHostIP = InStr;
}

string ConfigClass::GetHostIP() const
{
	return mHostIP;
}

void ConfigClass::SetMaxClient(int InVal)
{
	mMaxClient = InVal;
}

int ConfigClass::GetMaxClient() const
{
	return mMaxClient;
}

void ConfigClass::Reset()
{
	mIsPaused = false;
	mDisplayAll = false;
	mFriction = 0.995f;
	mElasticity = 0.9;
	mTimeScale = 1.f;
	mTarPhysicsFreq = 2000.f;
	mTarGraphicFreq = 120.f;
	mTarNetworkFreq = 2000.f;
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
	else if (key == "NumOfBalls")
	{
		try {
			int InNumberOfBalls = stoi(value);
			mNumberOfBalls = InNumberOfBalls;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	else if (key == "BallRadius")
	{
		try {
			float InBallRadius = stof(value);
			mBallRadius = InBallRadius;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	else if (key == "Elasticity")
	{
		try {
			float InElasticForce = stof(value);
			mElasticity = InElasticForce;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	else if (key == "Friction")
	{
		try {
			float InDragForce = stof(value);
			mFriction = InDragForce;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	else if (key == "PeerID")
	{
		try {
			int InPeerID = stoi(value);
			mPeerID = InPeerID;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	else if(key == "IsServer")
	{
		if (value == "True"|| value == "T")
		{
			mInitServer = true;
		}
		else if(value == "False" || value == "F")
		{
			mInitServer = false;
		}
		else
		{
			SetDefault(key);
		}
	}
	else if (key == "UDPPort")
	{
		try {
			int InUDPPort = stoi(value);
			mUDPPort = InUDPPort;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}		
	}
	else if (key == "TCPPort")
	{
		try {
			int InTCPPort = stoi(value);
			mTCPPort = InTCPPort;
		}
		catch (const std::invalid_argument& ia)
		{
			SetDefault(key);
			std::cerr << "Invalid argument: " << ia.what() << '\n';
		}
	}
	else if(key == "HostIP")
	{
		mHostIP = value;
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
	else if (key == "NumOfBalls")
	{
		mNumberOfBalls = 50;
	}
	else if (key == "BallRadius")
	{
		mBallRadius = 1.f;
	}
	else if (key == "Elasticity")
	{
		mElasticity = 0.99f;
	}
	else if (key == "Friction")
	{
		mFriction = 0.5f;
	}
	else if (key == "PeerID")
	{
		mPeerID = 0;
	}
	else if(key == "IsServer")
	{
		mInitServer = false;
	}
	else if (key == "UDPPort")
	{
		mUDPPort = 9171;
	}
	else if (key == "TCPPort")
	{
		mUDPPort = 9172;
	}
	else if(key == "HostIP")
	{
		mHostIP = "127.0.0.1";
	}
}

void ConfigClass::SetDefaultAll()
{
	SetDefault(string("FullScreen"));
	SetDefault(string("ScreenWidth"));
	SetDefault(string("ScreenHeight"));
	SetDefault(string("NumOfBalls"));
	SetDefault(string("BallRadius"));
	SetDefault(string("Elasticity"));
	SetDefault(string("Friction"));
	SetDefault(string("PeerID"));
	SetDefault(string("IsServer"));
}


