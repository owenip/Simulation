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

	void IntegrateRK(float dt);


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

	void ClearAccumulator();
	void AddForce(const SimpleMath::Vector3 &force);
	void SetForce(const SimpleMath::Vector3 &force);

	void GetForce(SimpleMath::Vector3 &force);
	SimpleMath::Vector3 GetForce();

	void SetRotation(SimpleMath::Vector3 &Rotation);
	void GetRotation(SimpleMath::Vector3 &Rotation);
	SimpleMath::Vector3 GetRotation() const;

	SimpleMath::Vector3 mLastPosition;
private:
	void recalculate();


private:
	//Rotation
	SimpleMath::Vector3 mRotation;
	

	//Primary Physics State
	SimpleMath::Vector3 mPosition;
	SimpleMath::Quaternion mOrientation;	///< the orientation of the cube represented by a unit quaternion.
	SimpleMath::Vector3 mMomentum;			///< the momentum of the cube in kilogram meters per second.
	SimpleMath::Vector3 mAngularMomentum;	///< angular momentum vector.

	SimpleMath::Vector3 mAcceleration;
	SimpleMath::Vector3 mForceAccum;

	//Secondary State
	SimpleMath::Vector3 mVelocity;
	SimpleMath::Quaternion mSpin;
	SimpleMath::Vector3 mAngularVelocity;
	SimpleMath::Matrix bodyToWorld;
	SimpleMath::Matrix worldToBody;

	//Constant State
	int mBallID;
	int mOwenerID;
	float mRadius;
	SimpleMath::Color mColour;

	float mInverseMass; //Inverse Mass = 1/m
	float mInertiaTensor; ///< inertia tensor of the cube (i have simplified it to a single value due to the mass properties a cube).
	float mInverseInertiaTensor; ///< inverse inertia tensor used to convert angular momentum to angular velocity.

	//0.995 as no drag
	float mDamping;

	struct Derivative
	{
		SimpleMath::Vector3 velocity;                ///< velocity is the derivative of position.
		SimpleMath::Vector3 force;                   ///< force in the derivative of momentum.
		SimpleMath::Quaternion spin;                ///< spin is the derivative of the orientation quaternion.
		SimpleMath::Vector3 torque;                  ///< torque is the derivative of angular momentum.
	};

	static Derivative evaluate(const BallClass &ball, float t)
	{
		Derivative output;
		output.velocity = ball.mVelocity;
		output.spin = ball.mSpin;
		forces(ball, t, output.force, output.torque);
		return output;
	}

	static void forces(const BallClass &ball, float t, SimpleMath::Vector3 &force, SimpleMath::Vector3 &torque)
	{
		//// attract towards origin

		//force = -10 * ball.mPosition;

		//// sine force to add some randomness to the motion

		//force.x += 10 * sin(t*0.9f + 0.5f);
		//force.y += 11 * sin(t*0.5f + 0.4f);
		//force.z += 12 * sin(t*0.7f + 0.9f);

		//// sine torque to get some spinning action

		//torque.x = 1.0f * sin(t*0.9f + 0.5f);
		//torque.y = 1.1f * sin(t*0.5f + 0.4f);
		//torque.z = 1.2f * sin(t*0.7f + 0.9f);

		// damping torque so we dont spin too fast

		torque -= 0.2f * ball.mAngularVelocity;
	}
};
