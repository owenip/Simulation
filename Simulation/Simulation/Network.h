#pragma once
#include "pch.h"
#include "BallManagerClass.h"
#include "GravityWellManager.h"

class Network
{
public:
	Network(shared_ptr<ConfigClass> InConfig);
	~Network();



	void SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager);
	void SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager);

private:
	shared_ptr<ConfigClass> mConfig;
	shared_ptr<BallManagerClass> mBallManager;
	shared_ptr<GravityWellManager> mGwManager;

	DX::StepTimer mNetTimer;

	bool mIsEscaped;
};

