#pragma once
#include "BallManagerClass.h"
#include "GravityWellManager.h"
#include "ContactManifold.h"


class Simulation
{
public:
	Simulation();
	~Simulation();

	void Initialise(shared_ptr<ConfigClass> InConfig);
	void Shutdown();

	void Tick();
	void RunPhysics(DX::StepTimer const& timer);
	

	void ModifyPhyFreq();

	void SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager);
	void SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager);

private:
	unsigned GenerateContacts();

	void GroundBallCollision();
	void BallBallCollision();
	void WallBallCollision();

	void ApplyGravity();
	void ApplyGwForce();

private:
	std::shared_ptr<BallManagerClass> mBallManager;
	std::shared_ptr<GravityWellManager> mGwManager;
	std::shared_ptr<ConfigClass> mConfig;
	int mPeerID;
	//Timer
	DX::StepTimer mPhyTimer;
	float TarPhyFreq;
	float timescale;

	ContactManifold mManifold;


	//Forces Properties
	float mFriction;
	float mElasticity;
	SimpleMath::Vector3 mGravity;
};

