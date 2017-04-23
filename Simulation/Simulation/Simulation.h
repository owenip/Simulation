#pragma once
#include "BallManagerClass.h"

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
	shared_ptr<BallManagerClass> GetBallManager() const;

private:
	std::shared_ptr<BallManagerClass> mBallManager;
	shared_ptr<ConfigClass> mConfig;
};

