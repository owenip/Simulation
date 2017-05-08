#include "pch.h"
#include "BallClass.h"

BallClass::BallClass()
{

}

BallClass::BallClass(int BallID, int OwenerID, float Radius, float mass, SimpleMath::Vector3 Position, SimpleMath::Vector3 Velocity, SimpleMath::Vector3 Accerleration, float damping)
{
	mBallID = BallID;
	mOwenerID = OwenerID;
	mRadius = Radius;
	mContended = false;

	this->SetMass(mass);
	this->SetPosition(Position);
	this->SetVelocity(Velocity);
	this->SetAcceleration(Accerleration);
	this->SetDamping(damping);
}

BallClass::BallClass(int BallID, int OwenerID, float Radius, float mass, SimpleMath::Vector3 Position)
{
	mBallID = BallID;
	mOwenerID = OwenerID;
	mRadius = Radius;
	mContended = false;
	this->SetMass(mass);
	this->SetPosition(Position);	
	mAcceleration = SimpleMath::Vector3::Zero;
	mForceAccum = SimpleMath::Vector3::Zero;
	mVelocity = SimpleMath::Vector3::Zero;
	mDamping = 0.9f;
	mTransferable = false;
}


BallClass::~BallClass()
{
}

void BallClass::Initialize(int BallID, int OwenerID, float Radius, float mass, SimpleMath::Vector3 Position, SimpleMath::Vector3 Velocity, SimpleMath::Vector3 Accerleration, float damping)
{
	mBallID = BallID;
	mOwenerID = OwenerID;
	mRadius = Radius;

	this->SetMass(mass);
	this->SetPosition(Position);
	this->SetVelocity(Velocity);
	this->SetAcceleration(Accerleration);
	this->SetDamping(damping);
}

void BallClass::recalculate()
{
	mVelocity = mForceAccum * mInverseMass;
}



void BallClass::Integrate(float duration)
{
	//assert(duration > 0.0);
	if(duration <= 0 )
	{
		duration = 0.0001f;
	}

	this->mLastPosition = this->mPosition;
	// Update linear position.



	this->mPosition += this->mVelocity * duration;

	// Work out the acceleration from the force.
	SimpleMath::Vector3 resultingAcc = this->mAcceleration;
	resultingAcc += this->mForceAccum *this->mInverseMass;

	//Update linear velocity from the acceleration
	this->mVelocity += resultingAcc * duration;

	//Impose drag
	this->mVelocity *= powf(this->mDamping, duration);
	//mVelocity *= mDamping;


	//if (mPosition.y <= mRadius)
	//{
	//	if (mVelocity.y < 0.5f && mVelocity.y > -0.5f)
	//	{
	//		if ((mVelocity.x < 0.1f && mVelocity.x > -0.1f) || (mVelocity.z < 0.1f && mVelocity.z > -0.1f))
	//		{
	//			//float  resultingVel = mVelocity.Length();
	//			//if (resultingVel < 0.5)
	//				mVelocity = SimpleMath::Vector3::Zero;
	//		}
	//	}
	//}
		
	//Clear the forces
	ClearAccumulator();
}

void BallClass::IntegrateRK(float duration)
{
	Derivative a = evaluate(*this, 0.0f, Derivative());
	Derivative b = evaluate(*this, duration*0.5f, a);
	Derivative c = evaluate(*this, duration*0.5f, b);
	Derivative d = evaluate(*this, duration, c);

	mPosition += 1.0f / 6.0f * duration * (a.velocity + 2.0f*(b.velocity + c.velocity) + d.velocity);
	mVelocity += 1.0f / 6.0f * duration * (a.acceleration + 2.0f*(b.acceleration + c.acceleration) + d.acceleration);
	mVelocity *= mDamping;
	//recalculate();
	//ClearAccumulator();
}




void BallClass::Update(float dt)
{
	//this->Integrate(dt);

}

void BallClass::SetBallID(const int BallID)
{
	this->mBallID = BallID;
}

int BallClass::GetBallId() const
{
	return this->mBallID;
}

void BallClass::SetOwenerID(const int owenerID)
{
	this->mOwenerID = owenerID;
}

int BallClass::GetOwenerID() const
{
	return this->mOwenerID;
}

void BallClass::SetRadius(const float radius)
{
	assert(radius != 0);
	this->mRadius = radius;
}

float BallClass::GetRadius() const
{
	return this->mRadius;
}

void BallClass::SetMass(const float mass)
{
	assert(mass != 0);
	this->mInverseMass = 1.0f / mass;
}

float BallClass::GetMass() const
{
	if (this->mInverseMass == 0)
	{
		return FLT_MAX;
	}
	else
	{
		return 1.0f / this->mInverseMass;
	}
}

void BallClass::SetInverseMass(const float inverseMass)
{
	this->mInverseMass = inverseMass;
}

float BallClass::GetInverseMass() const
{
	return this->mInverseMass;
}

void BallClass::SetDamping(const float damping)
{
	this->mDamping = damping;
}

float BallClass::GetDamping() const
{
	return this->mDamping;
}

void BallClass::SetPosition(SimpleMath::Vector3 & position)
{
	this->mPosition = position;
}

void BallClass::SetPosition(const float x, const float y, const float z)
{
	this->mPosition.x = x;
	this->mPosition.y = y;
	this->mPosition.z = z;
}

void BallClass::GetPosition(SimpleMath::Vector3 &position)
{
	position = this->mPosition;
}

SimpleMath::Vector3 BallClass::GetPosition() const
{
	return this->mPosition;
}

void BallClass::SetVelocity(const SimpleMath::Vector3 &velocity)
{
	this->mVelocity = velocity;
}


void BallClass::SetVelocity(const float x, const float y, const float z)
{
	this->mVelocity.x = x;
	this->mVelocity.y = y;
	this->mVelocity.z = z;
}

void BallClass::GetVelocity(SimpleMath::Vector3 &velocity) const
{
	velocity = this->mVelocity;
}

SimpleMath::Vector3 BallClass::GetVelocity() const
{
	return this->mVelocity;
}

void BallClass::SetAcceleration(const SimpleMath::Vector3 & acceleration)
{
	this->mAcceleration = acceleration;
}

void BallClass::SetAcceleration(const float x, const float y, const float z)
{
	this->mAcceleration.x = x;
	this->mAcceleration.y = y;
	this->mAcceleration.z = z;
}

void BallClass::GetAccleration(SimpleMath::Vector3 & acceleration)
{
	acceleration = this->mAcceleration;
}

SimpleMath::Vector3 BallClass::GetAccleration() const
{
	return this->mAcceleration;
}

void BallClass::AddAcceleration(const SimpleMath::Vector3 & acce)
{
	this->mAcceleration += acce;
}

void BallClass::ClearAccumulator()
{
	this->mForceAccum = SimpleMath::Vector3::Zero;
}

void BallClass::AddForce(const SimpleMath::Vector3 & force)
{
	this->mForceAccum += force;
}

void BallClass::SetForce(const SimpleMath::Vector3 & force)
{
	this->mForceAccum = force;
}

void BallClass::GetForce(SimpleMath::Vector3 & force)
{
	force = this->mForceAccum;
}

SimpleMath::Vector3 BallClass::GetForce()
{
	return this->mForceAccum;
}

void BallClass::SetRotation(SimpleMath::Vector3 &Rotation)
{
	this->mRotation = Rotation;
}

void BallClass::GetRotation(SimpleMath::Vector3 &Rotation)
{
	Rotation = this->mRotation;
}

SimpleMath::Vector3 BallClass::GetRotation() const
{
	return this->mRotation;
}

