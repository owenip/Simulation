#pragma once
#include "BallManagerClass.h"
#include "GravityWellManager.h"
#include "ContactClass.h"
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


	vector<ContactGenerator*> GetContactGenIndex();


	void SetBallManager(shared_ptr<BallManagerClass> InBallManager);
	shared_ptr<BallManagerClass> GetBallManagerPtr() const;

	std::shared_ptr<GravityWellManager> GetGwManagerPtr() const;

	void	SetRestitution(float restitustion);
	float	GetRestitution() const;

private:
	unsigned GenerateContacts();
	void GroundBallCollision();
	void BallBallCollision();
	void WallBallCollision();

protected:
	//Holds the maximum number of contacts allowed (the  size of the contacts array)
	unsigned maxContacts;
	std::unique_ptr<ContactManifold> mManifold;

	std::vector<ContactGenerator*> ContactGeneratorIndex;
	ContactClass *contacts;
	
	//Contact generator
	GroundContacts	groundContactGenerator;
	BallContacts	ballContactGenerator;
	WallContacts	wallContactGenerator;
	//Contact Resolver
	ContactResolver resolver;

	
	
	bool calculateIterations;

private:
	std::shared_ptr<BallManagerClass> mBallManager;
	std::shared_ptr<GravityWellManager> mGwManager;
	std::shared_ptr<ConfigClass> mConfig;

	float mRestitution;

};

