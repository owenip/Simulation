#pragma once
#include "RigidBodyClass.h"

class BallClass: public RigidBodyClass
{
public:
	BallClass();
	~BallClass();

	void	Initialize(int BallID, int OwenerID, float Radius, float mass,
						SimpleMath::Vector3 Position, SimpleMath::Vector3 Velocity, SimpleMath::Vector3 Acceleration, 
						float damping);

	void	Update(float dt);
	void	Shutdown();

	void	SetBallID(const int BallID);
	int		GetBallId() const;

	void	SetOwenerID(const int owenerID);
	int		GetOwenerID() const;
	
	void	SetRadisu(const float radius);
	float	GetRadius() const;
	
		
protected:
	int		mBallID;
	int		mOwenerID;
	float	mRadius;
	SimpleMath::Color mColour;
	

};

