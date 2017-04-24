#pragma once
#include "BallManagerClass.h"
#include "GravityWellManager.h"

class Simulation
{
public:
	Simulation();
	~Simulation();

	void Initialise(shared_ptr<ConfigClass> InBallManager);
	void Shutdown();
	void StartFrame();
	void RunPhysics(float dt);

	void SetBallManager(shared_ptr<BallManagerClass> InBallManager);
	shared_ptr<BallManagerClass> GetBallManagerPtr() const;

	std::shared_ptr<GravityWellManager> GetGwManagerPtr() const;

private:
	std::shared_ptr<BallManagerClass> mBallManager;
	std::shared_ptr<GravityWellManager> mGwManager;
	std::shared_ptr<ConfigClass> mConfig;
};

