#pragma once
#include "BallManagerClass.h"
#include "GravityWellManager.h"
#include "ContactManifold.h"

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

	void	SetRestitution(float restitustion);
	float	GetRestitution() const;

private:
	unsigned GenerateContacts();
	void GroundBallCollision();
	void BallBallCollision() const;
	void WallBallCollision() const;

	//Force
	void ApplyGravity();
	void ApplyGroundFriction(float dt);
	void ApplyGroundFriction(BallClass* Ball);
	void ApplyWellForce();


protected:
	//Holds the maximum number of contacts allowed (the  size of the contacts array)
	unsigned maxContacts;
	std::unique_ptr<ContactManifold> mManifold;

	
	bool calculateIterations;

private:
	std::shared_ptr<BallManagerClass> mBallManager;
	std::shared_ptr<GravityWellManager> mGwManager;
	std::shared_ptr<ConfigClass> mConfig;

	float mRestitution;
	Vector3 mGravity;

	float mGroundFriction;
	float *mDragCoeffificent[3];

	
	


};

