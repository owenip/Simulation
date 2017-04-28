#include "Simulation.h"

Simulation::Simulation():
	maxContacts(0), 
calculateIterations(false),
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
	mGwManager->Initialise(mConfig->GetOwnerID(), mConfig->GetGwRadius());

	//Initialise Contact Generators
	maxContacts = mConfig->GetNumberOfBalls() * 10;
	mManifold = make_unique<ContactManifold>();

	mRestitution = mConfig->GetElasticForce();

	//Initialise Force generators and Fgen list
	//Gravity Force Generator
	mGravity = SimpleMath::Vector3(0.f, -9.81f, 0.f);
	//Frictional Forces(Drag) Generator
	mGroundFriction = mConfig->GetGroundFriction();	
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

	//1.Apply force
	ApplyGravity();
	//ApplyGroundFriction(dt);
	//2.Integrate Object physics

	mBallManager->Integrate(dt);

	//Generate Contact
	unsigned usedContacts = GenerateContacts();
	//Resolve Contact
	if(usedContacts > 0)
	{
		mManifold->ResolveContact(dt);
	}

	//Up Ball Rotation
	mBallManager->Update(dt);
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
			contact.contactNormal = SimpleMath::Vector3::Up;
			contact.balls[0] = element;
			contact.balls[1] = nullptr;
			contact.penetration = -y;
			contact.restitution = mRestitution;
			mManifold->Add(contact);
		}
	}
}

void Simulation::BallBallCollision() const
{
	for (auto b1 : mBallManager->GetBallIndex())
	{		
		for (auto b2 : mBallManager->GetBallIndex())
		{
			SimpleMath::Vector3 midline = b1->GetPosition() - b2->GetPosition();
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
				SimpleMath::Vector3 normal = midline * (1.f / size);
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

void Simulation::WallBallCollision() const
{
	for (auto element : mBallManager->GetBallIndex())
	{
		SimpleMath::Vector3 d = element->GetPosition();
		d.y = 0;
		//d += Vector3(element->GetRadius(), 0.f, element->GetRadius());
		float ballDistance = d.LengthSquared();
		//float ballDistance = element->GetPosition().LengthSquared() + element->GetRadius() * element->GetRadius();
		if (ballDistance >= (mConfig->GetSurfaceRadius()-element->GetRadius()) * (mConfig->GetSurfaceRadius() - element->GetRadius()))
		{			
			//Vector3 normal = element->GetPosition() + Vector3( element->GetRadius(), element->GetRadius(), element->GetRadius());
			//Vector3 normal = element->GetVelocity();
			//normal.Normalize();
			//normal = -normal;
			d.Normalize();
			ManifoldPoint contact;
			contact.contactNormal = -d;
			contact.balls[0] = element;
			contact.balls[1] = nullptr;
			contact.penetration = sqrtf(ballDistance) - mConfig->GetSurfaceRadius();
			contact.restitution = mRestitution;
			mManifold->Add(contact);
		}
	}
}

void Simulation::ApplyGravity()
{
	for (auto element : mBallManager->GetBallIndex())
	{
		SimpleMath::Vector3 force = element->GetForce();
		//if (!element->HasFiniteMass()) continue;
		if (element->GetPosition().y >= element->GetRadius() || (force.x != 0.f || force.z != 0.f))
		{
			element->AddForce(mGravity);			
		}	
	}
}

void Simulation::ApplyGroundFriction(float dt)
{
	for (auto element : mBallManager->GetBallIndex())
	{
		SimpleMath::Vector3 force = element->GetVelocity();

		if (element->GetPosition().y <= element->GetRadius())
		{
			//element->AddForce(-mGravity);

			if (force.x > 0.f || force.z > 0.f && force.y <= 0.f)
			{
				float dragCoeff = force.Length();
				dragCoeff = mGroundFriction * dragCoeff;

				force.Normalize();
				force *= -dragCoeff;
				element->AddForce(force);
			}
		}
	}
}

void Simulation::ApplyWellForce()
{
	for (auto Gw : mGwManager->GetGwIndex())
	{

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
