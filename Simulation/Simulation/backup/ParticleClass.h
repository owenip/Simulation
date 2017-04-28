#pragma once
#include  "pch.h"
using namespace DirectX::SimpleMath;


class ParticleClass
{
public:
	ParticleClass();
	~ParticleClass();

	/**
	* Integrates the particle forward in time by the given amount.
	* This function uses a Newton-Euler integration method, which is a
	* linear approximation to the correct integral. For this reason it
	* may be inaccurate in some cases.
	*/
	
	void Integrate(float duration);


	void	SetMass(const float mass);
	float	GetMass() const;

	void	SetInverseMass(const float inverseMass);
	float	GetInverseMass() const;

	//Returns true if the mass of the particle is not-infinite.
	bool	HasFiniteMass() const;

	void	SetDamping(const float damping);
	float	GetDamping() const;

	void	SetPosition(Vector3 position);
	//void	SetPosition(Vector3 &position);
	void	SetPosition(const float x, const float y, const float z);

	void	GetPosition(Vector3 *position);
	Vector3 GetPosition() const;

	void	SetVelocity(const Vector3 &velocity);
	void	SetVelocity(const float x, const float y, const float z);

	void	GetVelocity(Vector3 *velocity) const;
	Vector3	GetVelocity() const;

	void	SetAcceleration(const Vector3 &acceleration);
	void	SetAcceleration(const float x, const float y, const float z);

	void	GetAccleration(Vector3 *acceleration);
	Vector3 GetAccleration() const;

	void	ClearAccumulator();
	void	AddForce(const Vector3 &force);

	void	Shutdown();

	

	//Rotation
	Vector3 mRotation;
	Vector3 mLastPosition;

protected:
	Vector3 mPosition;
	Vector3 mVelocity;
	Vector3 mAcceleration;
	
	/**
	* Holds the accumulated force to be applied at the next
	* simulation iteration only. This value is zeroed at each
	* integration step.
	*/
	Vector3 mForceAccum;

	//Inverse Mass = 1/m
	//This can help to create object with infinite mass
	float mInverseMass;

	
	//0.995 as no drag
	float mDamping;



};

