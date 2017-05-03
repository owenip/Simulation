#include "pch.h"
#include "ConfigClass.h"


ConfigClass::ConfigClass() :
	mIsPaused(false),
	mIsEscaped(false),
	mFullScreen(false),
	mScreenWidth(0),
	mScreenHeight(0),
	mSurfaceRadius(30.f),
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
	mTimeScale(1.f)
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
	mElasticity = InForce;
}

float ConfigClass::GetElasticForce() const
{
	return mElasticity;
}

void ConfigClass::SetFriction(float &InFriction)
{
	mFriction = InFriction;
}

float ConfigClass::GetFriction() const
{
	return mFriction;
}

void ConfigClass::SetTarPhyFreq(float &InVal)
{
	if(InVal > 0)
		mTarPhysicsFreq = InVal;
}

float ConfigClass::GetTarPhyFreq() const
{
	return mTarPhysicsFreq;
}

void ConfigClass::SetTarGraphicFreq(float &InVal)
{
	if (InVal > 0)
		mTarGraphicFreq = InVal;
}

float ConfigClass::GetTarGraphicFreq() const
{
	return mTarGraphicFreq;
}

void ConfigClass::SetTarNetworkFreq(float &InVal)
{
	if (InVal > 0)
		mTarNetworkFreq = InVal;
}

float ConfigClass::GetTarNetworkFreq() const
{
	return mTarNetworkFreq;
}

void ConfigClass::SetTimeScale(float & InTimeScale)
{
	mTimeScale = InTimeScale;
}

float ConfigClass::GettimeScale() const
{
	return mTimeScale;
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
}


