#pragma once
#include "ParticleClass.h"

class BallClass: public ParticleClass
{
public:
	BallClass();
	~BallClass();

	void	Initialize(int OwenerID, float Radius, float mass,
						SimpleMath::Vector3 Position, SimpleMath::Vector3 Velocity, SimpleMath::Vector3 Accerleration, 
						float damping);

	void	Update(float dt);
	void	Shutdown();

	void	SetOwenerID(const int owenerID);
	int		GetOwenerID() const;
	
	void	SetRadisu(const float radius);
	float	GetRadius() const;
	
			
protected:
	int		mOwenerID;
	float	mRadius;
	SimpleMath::Color mColour;
	

};

