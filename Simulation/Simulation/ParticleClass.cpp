#include "pch.h"
#include "ParticleClass.h"

ParticleClass::ParticleClass():
mRotation(Vector3::Zero)
{
}


ParticleClass::~ParticleClass()
{
}

void ParticleClass::Integrate(float duration)
{
	assert(duration > 0.0);

	mLastPosition = mPosition;
	// Update linear position.
	mPosition += mVelocity * duration;

	// Work out the acceleration from the force.
	Vector3 resultingAcc = mAcceleration;
	resultingAcc += mForceAccum * mInverseMass;

	//Update linear velocity from the acceleration
	mVelocity += resultingAcc * duration;

	//Impose drag
	mVelocity *= powf(mDamping, duration);

	//Clear the forces
	ClearAccumulator();

}

void ParticleClass::SetMass(const float mass)
{
	assert(mass != 0);
	ParticleClass::mInverseMass = 1.0f / mass;
}

float ParticleClass::GetMass() const
{
	if (mInverseMass == 0)
	{
		return FLT_MAX;
	}
	else
	{
		return 1.0f / mInverseMass;
	}
}

void ParticleClass::SetInverseMass(const float inverseMass)
{
	ParticleClass::mInverseMass = inverseMass;
}

float ParticleClass::GetInverseMass() const
{
	return mInverseMass;
}

bool ParticleClass::HasFiniteMass() const
{
	return (mInverseMass >= 0.0f);
}

void ParticleClass::SetDamping(const float damping)
{
	ParticleClass::mDamping = damping;
}

float ParticleClass::GetDamping() const
{
	return mDamping;
}

void ParticleClass::SetPosition(Vector3 position)
{
	ParticleClass::mPosition = position;
}

//void ParticleClass::SetPosition(Vector3 & position)
//{
//	ParticleClass::mPosition = position;
//}

void ParticleClass::SetPosition(const float x, const float y, const float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void ParticleClass::GetPosition(Vector3 * position)
{
	*position = ParticleClass::mPosition;
}

Vector3 ParticleClass::GetPosition() const
{
	return mPosition;
}

void ParticleClass::SetVelocity(const Vector3 & velocity)
{
	ParticleClass::mVelocity = velocity;
}

void ParticleClass::SetVelocity(const float x, const float y, const float z)
{
	mVelocity.x = x;
	mVelocity.y = y;
	mVelocity.z = z;
}

void ParticleClass::GetVelocity(Vector3 * velocity) const
{
	*velocity = ParticleClass::mVelocity;
}

Vector3 ParticleClass::GetVelocity() const
{
	return mVelocity;
}

void ParticleClass::SetAcceleration(const Vector3 & acceleration)
{
	ParticleClass::mAcceleration = acceleration;
}

void ParticleClass::SetAcceleration(const float x, const float y, const float z)
{
	mAcceleration.x = x;
	mAcceleration.y = y;
	mAcceleration.z = z;
}

void ParticleClass::GetAccleration(Vector3 * acceleration)
{
	*acceleration = ParticleClass::mAcceleration;
}

Vector3 ParticleClass::GetAccleration() const
{
	return mAcceleration;
}

void ParticleClass::ClearAccumulator()
{
	mForceAccum = Vector3::Zero;
}

void ParticleClass::AddForce(const Vector3 & force)
{
	mAcceleration += force;
}

void ParticleClass::Shutdown()
{
}
