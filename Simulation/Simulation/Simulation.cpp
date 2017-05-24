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
	mPhyTimer.SetFixedTimeStep(true);
	mPhyTimer.SetTargetElapsedSeconds(1 / TarPhyFreq);
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
		if (mConfig->GetIsEscaped())
		{
			break;
		}
		if (TarPhyFreq != mConfig->GetTarPhyFreq())
		{
			TarPhyFreq = mConfig->GetTarPhyFreq();
			mPhyTimer.SetTargetElapsedSeconds(1 / TarPhyFreq);
		}
		mPhyTimer.Tick([&]()
		{
			if (!mConfig->GetIsPaused())
			{
				RunPhysics(mPhyTimer);
			}			
		});
		
	}
}

void Simulation::RunPhysics(DX::StepTimer const& timer)
{
	timescale = mConfig->GettimeScale();
	mFriction = mConfig->GetFriction();
	mElasticity = mConfig->GetElasticForce();
	
	float ActualPhyFreq = float(mPhyTimer.GetFramesPerSecond());
	mConfig->SetActualPhyFreq(ActualPhyFreq);
	
	float dt = float(mPhyTimer.GetElapsedSeconds()) * timescale;
	
	GenSimBallIndex();
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
	CheckTransferableBall();
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
	std::thread t_groundCollision([this] {GroundBallCollision(); });
	SetThreadAffinityMask(t_groundCollision.native_handle(), 0b00001000);

	std::thread t_BallBallCollision([this] {BallBallCollision(); });
	SetThreadAffinityMask(t_BallBallCollision.native_handle(), 0b00010000);

	std::thread t_WallBallCollision([this] {WallBallCollision(); });
	SetThreadAffinityMask(t_WallBallCollision.native_handle(), 0b01000000);

	//GroundBallCollision();
	//BallBallCollision();
	//WallBallCollision();

	t_groundCollision.join();
	t_BallBallCollision.join();
	t_WallBallCollision.join();
	return mManifold.GetNumPoints();
}

void Simulation::GroundBallCollision()
{	
	//if (mConfig->GetDisplayAll() == false)
	{
		for (auto ball : mBallManager->GetSimIndex())
		{
			//if (ball->GetOwenerID() != mPeerID)
				//continue;
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

				SimpleMath::Vector3 Vel = ball->GetVelocity();
				if (Vel.y < 0.5f && Vel.y > -0.5f)
				{
					Vel.y = 0.f;
					ball->SetVelocity(Vel);
					SimpleMath::Vector3 mg(-0.f, -9.81f * ball->GetMass(), 0.f);
					SimpleMath::Vector3 FrictionForce = mFriction * mg.Length() * -Vel;
					ball->AddForce(FrictionForce);
				}
			}
		}
	}
	//else
	//{
	//	for (auto ball : mBallManager->GetBallIndex())
	//	{
	//		//if (ball->GetOwenerID() != mPeerID)
	//		//continue;
	//		float y = ball->GetPosition().y - ball->GetRadius();
	//		if (y < 0.0f)
	//		{
	//			ManifoldPoint contact;
	//			contact.contactNormal = SimpleMath::Vector3::Up;
	//			contact.balls[0] = ball;
	//			contact.balls[1] = nullptr;
	//			contact.penetration = -y;
	//			contact.restitution = mElasticity;
	//			contact.friction = mFriction;
	//			mManifold.Add(contact);

	//			SimpleMath::Vector3 Vel = ball->GetVelocity();
	//			if (Vel.y < 0.5f && Vel.y > -0.5f)
	//			{
	//				Vel.y = 0.f;
	//				ball->SetVelocity(Vel);
	//				SimpleMath::Vector3 mg(-0.f, -9.81f * ball->GetMass(), 0.f);
	//				SimpleMath::Vector3 FrictionForce = mFriction * mg.Length() * -Vel;
	//				ball->AddForce(FrictionForce);
	//			}
	//		}
	//	}
	//}
}


void  Simulation::GroundBallCollision(std::vector<ManifoldPoint> &vec_private)
{	
	//if (mConfig->GetDisplayAll() == false)
	{
		for (int i = 0; i < mBallManager->GetSimIndex().size(); i++)
		{
			//if (mBallManager->GetSimIndex()[i]->GetOwenerID() != mPeerID)
			//{
			//	continue;
			//}
			float y = mBallManager->GetSimIndex()[i]->GetPosition().y - mBallManager->GetSimIndex()[i]->GetRadius();
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
	//else
	//{
	//	for (int i = 0; i < mBallManager->GetBallIndex().size(); i++)
	//	{
	//		//if (mBallManager->GetSimIndex()[i]->GetOwenerID() != mPeerID)
	//		//{
	//		//	continue;
	//		//}
	//		float y = mBallManager->GetBallIndex()[i]->GetPosition().y - mBallManager->GetBallIndex()[i]->GetRadius();
	//		if (y < 0.0f)
	//		{
	//			ManifoldPoint contact;
	//			contact.contactNormal = SimpleMath::Vector3::Up;
	//			contact.balls[0] = mBallManager->GetBallIndex()[i];
	//			contact.balls[1] = nullptr;
	//			contact.penetration = -y;
	//			contact.restitution = mElasticity;
	//			contact.friction = mFriction;
	//			vec_private.push_back(contact);


	//		}
	//	}
	//}
}

void Simulation::BallBallCollision()
{
	//if (mConfig->GetDisplayAll() == false)
	{
		for (auto b1 : mBallManager->GetSimIndex())
		{
			//if (b1->GetOwenerID() != mPeerID)
				//continue;
			for (auto b2 : mBallManager->GetSimIndex())
			{
				//if (b2->GetOwenerID() != mPeerID)
					//continue;
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
	//else
	//{
	//	for (auto b1 : mBallManager->GetBallIndex())
	//	{
	//		//if (b1->GetOwenerID() != mPeerID)
	//		//continue;
	//		for (auto b2 : mBallManager->GetSimIndex())
	//		{
	//			//if (b2->GetOwenerID() != mPeerID)
	//			//continue;
	//			if ((b1->GetBallId() != b2->GetBallId()))
	//			{
	//				SimpleMath::Vector3 midline = b1->GetPosition() - b2->GetPosition();
	//				float d = midline.LengthSquared();
	//				float rSum = b1->GetRadius() + b2->GetRadius();
	//				if (d > rSum* rSum)
	//				{
	//					continue;
	//				}
	//				else
	//				{
	//					float size = midline.Length();
	//					ManifoldPoint contact;
	//					SimpleMath::Vector3 normal = midline * (1.f / size);
	//					contact.contactNormal = normal;
	//					contact.balls[0] = b1;
	//					contact.balls[1] = b2;
	//					contact.penetration = (b1->GetRadius() + b2->GetRadius() - size);
	//					contact.restitution = mElasticity;
	//					contact.friction = mFriction;
	//					mManifold.Add(contact);
	//				}
	//			}
	//		}
	//	}
	//}
}

void Simulation::WallBallCollision()
{
	//if (mConfig->GetDisplayAll() == false)
	{
		for (auto element : mBallManager->GetSimIndex())
		{
			//if(element->GetOwenerID() != mPeerID)
				//continue;
			SimpleMath::Vector3 d = element->GetPosition();
			d.y = 0;
			float ballDistance = d.LengthSquared();
			if (ballDistance >= (mConfig->GetSurfaceRadius() - element->GetRadius()) 
				* (mConfig->GetSurfaceRadius() - element->GetRadius()))
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
	//else
	//{
	//	for (auto element : mBallManager->GetBallIndex())
	//	{
	//		//if(element->GetOwenerID() != mPeerID)
	//		//continue;
	//		SimpleMath::Vector3 d = element->GetPosition();
	//		d.y = 0;
	//		float ballDistance = d.LengthSquared();
	//		if (ballDistance >= (mConfig->GetSurfaceRadius() - element->GetRadius()) * (mConfig->GetSurfaceRadius() - element->GetRadius()))
	//		{

	//			d.Normalize();
	//			ManifoldPoint contact;
	//			contact.contactNormal = -d;
	//			contact.balls[0] = element;
	//			contact.balls[1] = nullptr;
	//			contact.penetration = sqrtf(ballDistance) - mConfig->GetSurfaceRadius();
	//			contact.restitution = mElasticity;
	//			contact.friction = mFriction;
	//			mManifold.Add(contact);
	//		}
	//	}
	//}
}

void Simulation::ApplyGravity()
{
	//if (mConfig->GetDisplayAll() == false)
	{
		for (auto ball : mBallManager->GetSimIndex())
		{
			//if (ball->GetOwenerID() != mPeerID)
				//continue;

			ball->AddForce(SimpleMath::Vector3(-00.f, -9.81f * ball->GetMass(), 0.f));
		}
	}
	//else
	//{
	//	for (auto ball : mBallManager->GetBallIndex())
	//	{
	//		//if (ball->GetOwenerID() != mPeerID)
	//		//continue;

	//		ball->AddForce(SimpleMath::Vector3(-00.f, -9.81f * ball->GetMass(), 0.f));
	//	}
	//}
}

void Simulation::ApplyGwForce()
{
	//if (mConfig->GetDisplayAll() == false)
	{
		for (auto ball : mBallManager->GetSimIndex())
		{
			//if (ball->GetOwenerID() != mPeerID)
				//continue; //Apply force to another peer!!!!
			for (auto Gw : mGwManager->GetGwIndex())
			{
				if (Gw->GetIsActive() == false)
				{
					continue;
				}
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
	//else
	//{
	//	for (auto ball : mBallManager->GetBallIndex())
	//	{
	//		//if (ball->GetOwenerID() != mPeerID)
	//		//continue; //Apply force to another peer!!!!
	//		for (auto Gw : mGwManager->GetGwIndex())
	//		{
	//			if(Gw->GetIsActive() == false)
	//			{
	//				continue;
	//			}
	//			SimpleMath::Vector3 midline = ball->GetPosition() - Gw->GetPos();
	//			float d = midline.LengthSquared();
	//			float rSum = mConfig->GetGwRadius();
	//			if (d > rSum* rSum)
	//			{
	//				continue;
	//			}
	//			float ApplyingForce = Gw->GetForce();

	//			if (ApplyingForce > 0)
	//			{
	//				//float size = midline.Length();
	//				//SimpleMath::Vector3 normal = midline * (1.f / size);
	//				midline.Normalize();
	//				ball->AddForce(midline * ApplyingForce);
	//			}
	//			else
	//			{
	//				midline = Gw->GetPos() - ball->GetPosition();
	//				//float size = midline.Length();
	//				//SimpleMath::Vector3 normal = midline * (1.f / size);
	//				midline.Normalize();
	//				ball->AddForce(-midline * ApplyingForce);
	//			}

	//		}
	//	}
	//}
}

void Simulation::GenSimBallIndex()
{
	mBallManager->ClearSimIndex();
	for (auto ball : mBallManager->GetBallIndex())
	{		
		SimpleMath::Vector3 midline = ball->GetPosition() - mGwManager->GwGetPos(mPeerID);
		float d = midline.LengthSquared();
		float rSum = mConfig->GetGwRadius();
		if (d > rSum* rSum && ball->GetOwenerID() != mPeerID)
		{
			continue;
		}
		mBallManager->AddSimBall(ball);
	}
	for (auto ball : mBallManager->GetBallIndex())
	{
		if (ball->GetOwenerID() == mPeerID)
		{
			mBallManager->AddSimBall(ball);
			continue;
		}
	}
}

void Simulation::CheckTransferableBall()
{
	for (auto ball : mBallManager->GetBallIndex())
	{
		if (ball->GetOwenerID() != mPeerID)
			continue;
		for (auto Gw : mGwManager->GetGwIndex())
		{
			if (Gw->GetIsActive() == false)
			{
				continue;
			}
			SimpleMath::Vector3 midline = ball->GetPosition() - Gw->GetPos();
			float d = midline.LengthSquared();
			float rSum = mConfig->GetGwRadius();
			if (d > rSum* rSum)
			{
				continue;
			}
			ball->mTransferable = true;
		}
	}
}