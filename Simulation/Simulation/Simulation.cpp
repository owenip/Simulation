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
	//Initialise Gw Manager
	mGwManager = make_shared<GravityWellManager>();
	mGwManager->Initialise();

	//Initialise Force generators and Fgen list
		//Gravity Force Generator
		//Friction(Drag) Generator
}

void Simulation::Shutdown()
{
	mBallManager.reset();
	mGwManager.reset();
	mConfig.reset();
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

void Simulation::SetBallManager(shared_ptr<BallManagerClass> InBallManager)
{
	mBallManager = InBallManager;
}

shared_ptr<BallManagerClass> Simulation::GetBallManagerPtr() const
{
	return mBallManager;
}

std::shared_ptr<GravityWellManager> Simulation::GetGwManagerPtr() const
{
	return mGwManager;
}
