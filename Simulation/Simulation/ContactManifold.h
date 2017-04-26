#pragma once
#include "pch.h"

class BallClass;

struct  ManifoldPoint
{
	BallClass* balls[2];
	SimpleMath::Vector3 contactNormal;
	
	//Hold the amount each ball is moved by during interpenetration resolution
	SimpleMath::Vector3 BallsMovement[2];

	//Hold the normal restitution coefficient at the contact
	float restitution;
	//Hold the depth of penetration at the contract
	float penetration;
};

class ContactManifold
{
public:
	ContactManifold();
	~ContactManifold();
	void Add(ManifoldPoint point);
	void Clear();

	int GetNumPoints() const;
	ManifoldPoint& GeTPoint(int index);

	void ResolveContact(float duration);
	
	
	float CalculateSeparatingVelocity(ManifoldPoint &mp) const;
private:
	void Resolve(ManifoldPoint &mp);

	//Handles the impulse calculations for this collision.
	void ResolveVelocity( ManifoldPoint &mp);
	
	//Handles the interpenetration resolution for this contact.
	void ResolveInterpenetration(ManifoldPoint &mp);

private:
	std::vector<ManifoldPoint> mPoints;
	int mNumofPoint;

	float mDuration;
};

