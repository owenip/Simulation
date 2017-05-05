#include "Network.h"



Network::Network(shared_ptr<ConfigClass> InConfig) :
	mIsEscaped(false)
{
	mConfig = InConfig;
}


Network::~Network()
{
	mConfig.reset();
	mBallManager.reset();
	mGwManager.reset();
}

void Network::SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager)
{
	mBallManager = InBallManager;
	return;
}

void Network::SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager)
{
	mGwManager = InGwManager;
}
