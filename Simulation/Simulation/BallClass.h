#pragma once
#include "pch.h"

class BallClass
{
public:
	BallClass();
	BallClass(int BallID, int OwenerID, float Radius, float mass, SimpleMath::Vector3 Position, SimpleMath::Vector3 Velocity, SimpleMath::Vector3 Accerleration, float damping);
	BallClass(int BallID, int OwenerID, float Radius, float mass, SimpleMath::Vector3 Position);
	~BallClass();

	void Initialize(int BallID, int OwenerID, float Radius, float mass,
	                SimpleMath::Vector3 Position, SimpleMath::Vector3 Velocity, SimpleMath::Vector3 Acceleration,
	                float damping);

	void Integrate(float duration);
	void IntegrateRK(float duration);

	bool CollisionWithBall(BallClass* b2, float dt);
	bool CollisionWithGround(float dt);
	bool CollisionWithWall(float dt);


	void Update(float dt);

	void SetBallID(const int BallID);
	int GetBallId() const;

	void SetOwenerID(const int owenerID);
	int GetOwenerID() const;

	void SetRadius(const float radius);
	float GetRadius() const;

	void SetMass(const float mass);
	float GetMass() const;

	void SetInverseMass(const float inverseMass);
	float GetInverseMass() const;

	void SetDamping(const float damping);
	float GetDamping() const;

	void SetPosition(SimpleMath::Vector3& position);
	void SetPosition(const float x, const float y, const float z);

	void GetPosition(SimpleMath::Vector3 &position);
	SimpleMath::Vector3 GetPosition() const;

	void SetVelocity(const SimpleMath::Vector3 &velocity);
	void SetVelocity(const float x, const float y, const float z);

	void GetVelocity(SimpleMath::Vector3 &velocity) const;
	SimpleMath::Vector3 GetVelocity() const;

	void SetAcceleration(const SimpleMath::Vector3& acceleration);
	void SetAcceleration(const float x, const float y, const float z);

	void GetAccleration(SimpleMath::Vector3 &acceleration);
	SimpleMath::Vector3 GetAccleration() const;
	void AddAcceleration(const SimpleMath::Vector3 &acce);

	void ClearAccumulator();
	void AddForce(const SimpleMath::Vector3 &force);
	void SetForce(const SimpleMath::Vector3 &force);

	void GetForce(SimpleMath::Vector3 &force);
	SimpleMath::Vector3 GetForce();

	void SetRotation(SimpleMath::Vector3 &Rotation);
	void GetRotation(SimpleMath::Vector3 &Rotation);
	SimpleMath::Vector3 GetRotation() const;

	SimpleMath::Vector3 mLastPosition;
	SimpleMath::Vector3 mLastVelocity;
	bool mTransferable;
	//Ownership State
	bool mContended;

private:
	void recalculate();
	


private:
	//Rotation
	SimpleMath::Vector3 mRotation;
	
	//Primary Physics State
	SimpleMath::Vector3 mPosition;	
	SimpleMath::Vector3 mAcceleration;
	SimpleMath::Vector3 mForceAccum;

	//Secondary State
	SimpleMath::Vector3 mVelocity;


	//Constant State
	int mBallID;
	int mOwenerID;
	float mRadius;
	SimpleMath::Color mColour;
	float mInverseMass; //Inverse Mass = 1/m
	//0.995 as no drag
	float mDamping;

	
	

	struct Derivative
	{
		SimpleMath::Vector3 velocity;                ///< velocity is the derivative of position.
		SimpleMath::Vector3 acceleration;            ///< force in the derivative of momentum.
	};

	static Derivative evaluate(BallClass ball)
	{
		Derivative output;
		output.velocity = ball.mVelocity;
		output.acceleration = ball.mForceAccum * ball.mInverseMass;
		return output;
	}

	static Derivative evaluate(BallClass &initial, float dt, const Derivative &d)
	{
		initial.mPosition += d.velocity * dt;
		initial.mVelocity += d.acceleration*dt;
		initial.recalculate();
		
		Derivative output;
		output.velocity = initial.mVelocity;
		output.acceleration = initial.mForceAccum * initial.mInverseMass;
		return output;
	}

	
};
