#pragma once
#include "BallManagerClass.h"

class Simulation
{
public:
	Simulation();
	~Simulation();

	void StartFrame();
	void RunPhysics(float dt);

	void SetBallManager(shared_ptr<BallManagerClass> InBallManager);

private:
	std::shared_ptr<BallManagerClass> mBallManager;
};

