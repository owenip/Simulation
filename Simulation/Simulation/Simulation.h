#pragma once
#include "BallManagerClass.h"
#include "GravityWellManager.h"
#include "ContactClass.h"

class Simulation
{
public:
	Simulation();
	~Simulation();

	void Initialise(shared_ptr<ConfigClass> InBallManager);
	void Shutdown();

	void StartFrame();
	void RunPhysics(float dt);

	unsigned GenerateContacts();

	vector<ContactGenerator*> GetContactGenIndex();


	void SetBallManager(shared_ptr<BallManagerClass> InBallManager);
	shared_ptr<BallManagerClass> GetBallManagerPtr() const;

	std::shared_ptr<GravityWellManager> GetGwManagerPtr() const;


protected:
	std::vector<ContactGenerator*> ContactGeneratorIndex;
	ContactClass *contacts;

	//Holds the maximum number of contacts allowed (the  size of the contacts array)
	unsigned maxContacts;
	
	bool calculationsIterations;

private:
	std::shared_ptr<BallManagerClass> mBallManager;
	std::shared_ptr<GravityWellManager> mGwManager;
	std::shared_ptr<ConfigClass> mConfig;

};

