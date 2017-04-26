#include "Simulation.h"

Simulation::Simulation(): 
maxContacts(0), 
contacts(nullptr), 
resolver(maxContacts * 2), 
calculateIterations(true), 
mRestitution(0.5f)
{
}


Simulation::~Simulation()
{
	this->Shutdown();
}

void Simulation::Initialise(shared_ptr<ConfigClass> InConfig)
{	
	mConfig = InConfig;
	//Initialise Ball Manager
	mBallManager = make_shared<BallManagerClass>();
	mBallManager->Initialise(mConfig);
	//Initialise Gw Manager
	mGwManager = make_shared<GravityWellManager>();
	mGwManager->Initialise(mConfig->GetOwnerID());

	//Initialise Contact Generators
	maxContacts = mConfig->GetNumberOfBalls() * 10;
	mManifold = make_unique<ContactManifold>();

	//Initialise Force generators and Fgen list
	//Gravity Force Generator
	
	//Frictional Forces(Drag) Generator
	
	
}

void Simulation::Shutdown()
{
	mManifold.reset();
	mBallManager.reset();
	mGwManager.reset();
	mConfig.reset();
}


void Simulation::RunPhysics(float dt)
{
	mBallManager->ClearAccumulator();
	mManifold->Clear();

	//1.Apply force Generator

	//2.Integrate Object physics
	mBallManager->Integrate(dt);

	//Generate Contact
	unsigned usedContacts = GenerateContacts();
	//Resolve Contact
	if(usedContacts > 0)
	{
		mManifold->ResolveContact(dt);
	}

}

unsigned Simulation::GenerateContacts()
{
	GroundBallCollision();
	BallBallCollision();
	WallBallCollision();
	return mManifold->GetNumPoints();
}

void Simulation::GroundBallCollision()
{	
	for (auto element : mBallManager->GetBallIndex())
	{
		float y = element->GetPosition().y - element->GetRadius();
		if (y < 0.0f)
		{			
			ManifoldPoint contact;
			contact.contactNormal = Vector3::Up;
			contact.balls[0] = element;
			contact.balls[1] = nullptr;
			contact.penetration = -y;
			contact.restitution = mRestitution;
			mManifold->Add(contact);
		}
	}
}

void Simulation::BallBallCollision()
{
	for (auto b1 : mBallManager->GetBallIndex())
	{		
		for (auto b2 : mBallManager->GetBallIndex())
		{			
			Vector3 midline = b1->GetPosition() - b2->GetPosition();
			float d = midline.LengthSquared();
			float rSum = b1->GetRadius() + b2->GetRadius();
			if (d> rSum* rSum)
			{
				continue;
			}
			else
			{
				float size = midline.Length();
				ManifoldPoint contact;
				Vector3 normal = midline * (1.f / size);
				contact.contactNormal = normal;
				contact.balls[0] = b1;
				contact.balls[1] = b2;
				contact.penetration = (b1->GetRadius() + b2->GetRadius() - size);
				contact.restitution = mRestitution;
				mManifold->Add(contact);
			}
		}
	}
}

void Simulation::WallBallCollision()
{
	for (auto element : mBallManager->GetBallIndex())
	{
		float ballDistance = element->GetPosition().Dot(element->GetPosition()) + element->GetRadius() * element->GetRadius();
		if (ballDistance >= mConfig->GetSurfaceRadius() * mConfig->GetSurfaceRadius())
		{			
			Vector3 normal = element->GetPosition() + Vector3( element->GetRadius(), element->GetRadius(), element->GetRadius());
			normal.Normalize();
			normal = -normal;

			ManifoldPoint contact;
			contact.contactNormal = normal;
			contact.balls[0] = element;
			contact.balls[1] = nullptr;
			contact.penetration = sqrtf(ballDistance) - mConfig->GetSurfaceRadius();
			contact.restitution = mRestitution;
			mManifold->Add(contact);
		}
	}
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

void Simulation::SetRestitution(float restitustion)
{
	mRestitution = restitustion;
}

float Simulation::GetRestitution() const
{
	return mRestitution;
}
