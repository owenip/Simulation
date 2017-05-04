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
	mFriction = mConfig->GetFriction();
	mElasticity = mConfig->GetElasticForce();
	mGravity = SimpleMath::Vector3(0.f, -9.81f, 0.f);
	TarPhyFreq = mConfig->GetTarPhyFreq();
	mPhyTimer.SetFixedTimeStep(false);
	mPeerID = mConfig->GetPeerID();

	//mManifold = make_unique<ContactManifold>();

}

void Simulation::Shutdown()
{
	//mManifold.reset();
	mBallManager.reset();
	mGwManager.reset();
	mConfig.reset();
}

void Simulation::Tick()
{
	while (true)
	{
		mPhyTimer.Tick([&]()
		{
			if (!mConfig->GetIsPaused())
			{
				RunPhysics(mPhyTimer);
			}
		});
		if (mConfig->GetIsEscaped())
		{			
			break;
		}
	}
}

void Simulation::RunPhysics(DX::StepTimer const& timer)
{
	timescale = mConfig->GettimeScale();
	mFriction = mConfig->GetFriction();
	mElasticity = mConfig->GetElasticForce();
	
	
	float dt = float(mPhyTimer.GetElapsedSeconds()) * timescale;
	

	mManifold.Clear();
	mBallManager->ClearAccumulator();
	ApplyGravity();
	ApplyGwForce();
	mBallManager->Integrate(dt);

	//Generate Contact
	unsigned usedContacts = GenerateContacts();
	//Resolve Contact
	if (usedContacts > 0)
	{
		mManifold.ResolveContact(dt);
	}

	mBallManager->Update(dt);
}

void Simulation::ModifyPhyFreq()
{
	mPhyTimer.SetFixedTimeStep(true);
	TarPhyFreq = mConfig->GetTarPhyFreq();	
	mPhyTimer.SetTargetElapsedSeconds(1/TarPhyFreq);
}

void Simulation::SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager)
{
	mBallManager = InBallManager;
}

void Simulation::SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager)
{
	mGwManager = InGwManager;
}

unsigned Simulation::GenerateContacts()
{
	std::vector<ManifoldPoint> temp1;
	GroundBallCollision(temp1);
	mManifold.Add(temp1);
	//GroundBallCollision();
	BallBallCollision();
	WallBallCollision();
	return mManifold.GetNumPoints();
}

void Simulation::GroundBallCollision()
{	
	for (auto ball : mBallManager->GetBallIndex())
	{						
		if (ball->GetOwenerID() != mPeerID)
			continue;
		float y = ball->GetPosition().y - ball->GetRadius();
		if (y < 0.0f)
		{
			ManifoldPoint contact;
			contact.contactNormal = SimpleMath::Vector3::Up;
			contact.balls[0] = ball;
			contact.balls[1] = nullptr;
			contact.penetration = -y;
			contact.restitution = mElasticity;
			contact.friction = mFriction;				
			mManifold.Add(contact);
		}
	}			
}


void  Simulation::GroundBallCollision(std::vector<ManifoldPoint> &vec_private)
{	
	for (int i = 0; i< mBallManager->GetBallIndex().size(); i++)
	{
		if (mBallManager->GetBallIndex()[i]->GetOwenerID() != mPeerID)
		{
			continue;
		}
		float y = mBallManager->GetBallIndex()[i]->GetPosition().y - mBallManager->GetBallIndex()[i]->GetRadius();
		if (y < 0.0f)
		{
			ManifoldPoint contact;
			contact.contactNormal = SimpleMath::Vector3::Up;
			contact.balls[0] = mBallManager->GetBallIndex()[i];
			contact.balls[1] = nullptr;
			contact.penetration = -y;
			contact.restitution = mElasticity;
			contact.friction = mFriction;
			vec_private.push_back(contact);
		}
	}
}

void Simulation::BallBallCollision()
{
	for (auto b1 : mBallManager->GetBallIndex())
	{
		if (b1->GetOwenerID() != mPeerID)
			continue;
		for (auto b2 : mBallManager->GetBallIndex())
		{
			if (b2->GetOwenerID() != mPeerID)
				continue;
			if ((b1->GetBallId() != b2->GetBallId()))
			{
				SimpleMath::Vector3 midline = b1->GetPosition() - b2->GetPosition();
				float d = midline.LengthSquared();
				float rSum = b1->GetRadius() + b2->GetRadius();
				if (d > rSum* rSum)
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
					contact.restitution = mElasticity;
					contact.friction = mFriction;
					mManifold.Add(contact);
				}
			}
		}
	}
}

void Simulation::WallBallCollision()
{
	for (auto element : mBallManager->GetBallIndex())
	{
		if(element->GetOwenerID() != mPeerID)
			continue;
		SimpleMath::Vector3 d = element->GetPosition();
		d.y = 0;
		float ballDistance = d.LengthSquared();
		if (ballDistance >= (mConfig->GetSurfaceRadius() - element->GetRadius()) * (mConfig->GetSurfaceRadius() - element->GetRadius()))
		{
			
			d.Normalize();
			ManifoldPoint contact;
			contact.contactNormal = -d;
			contact.balls[0] = element;
			contact.balls[1] = nullptr;
			contact.penetration = sqrtf(ballDistance) - mConfig->GetSurfaceRadius();
			contact.restitution = mElasticity;
			contact.friction = mFriction;
			mManifold.Add(contact);
		}
	}
}

void Simulation::ApplyGravity()
{
	for (auto ball : mBallManager->GetBallIndex())
	{
		if (ball->GetOwenerID() != mPeerID)
			continue;

		ball->AddForce(SimpleMath::Vector3(-00.f, -9.81f * ball->GetMass(), 0.f));
	}
}

void Simulation::ApplyGwForce()
{
	for (auto ball : mBallManager->GetBallIndex())
	{
		if (ball->GetOwenerID() != mPeerID)
			continue; //Apply force to another peer!!!!
		for (auto Gw : mGwManager->GetGwIndex())
		{			
			SimpleMath::Vector3 midline = ball->GetPosition() - Gw->GetPos();
			float d = midline.LengthSquared();
			float rSum = mConfig->GetGwRadius();
			if (d > rSum* rSum)
			{
				continue;
			}
			float ApplyingForce = Gw->GetForce();
			
			if (ApplyingForce > 0)
			{
				//float size = midline.Length();
				//SimpleMath::Vector3 normal = midline * (1.f / size);
				midline.Normalize();
				ball->AddForce(midline * ApplyingForce);
			}
			else 
			{
				midline = Gw->GetPos() - ball->GetPosition();
				//float size = midline.Length();
				//SimpleMath::Vector3 normal = midline * (1.f / size);
				midline.Normalize();
				ball->AddForce(-midline * ApplyingForce);
			}

		}	
	}
}
