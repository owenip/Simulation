#include "pch.h"
#include "Simulation.h"



Simulation::Simulation()
{
}


Simulation::~Simulation()
{
}

void Simulation::Initialise(shared_ptr<ConfigClass> InConfig)
{	
	mConfig = InConfig;
	//Initialise Ball Manager
	mBallManager = make_shared<BallManagerClass>();
	mBallManager->Initialise(mConfig);

	//Initialise Force generators and Fgen list
		//Gravity Force Generator
		//Friction(Drag) Generator
}

void Simulation::Shutdown()
{
	mBallManager.reset();
}

void Simulation::StartFrame()
{
	mBallManager->ClearAccumulator();
}

void Simulation::RunPhysics(float dt)
{
	//1.Apply force Generator

	//2.Integrate Object physics
	mBallManager->Integrate(dt);

	//Generate Contact
	
	//Resolve Contact

}

shared_ptr<BallManagerClass> Simulation::GetBallManager() const
{
	return mBallManager;
}
