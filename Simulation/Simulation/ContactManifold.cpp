#include "pch.h"
#include "ContactManifold.h"
#include "BallClass.h"



ContactManifold::ContactManifold() :
	mNumofPoint(0)
{
	mPoints.reserve(100000);
}


ContactManifold::~ContactManifold()
{
}

void ContactManifold::Add(ManifoldPoint &point)
{
	mPoints.push_back(point);
	++mNumofPoint;
}

void ContactManifold::Add(std::vector<ManifoldPoint> &Inpoints)
{
	this->mPoints.insert(mPoints.end(), Inpoints.begin(), Inpoints.end());
}

void ContactManifold::Clear()
{
	mPoints.clear();
	mNumofPoint = 0;
}

std::vector<ManifoldPoint>* ContactManifold::GetPoints()
{
	return &mPoints;
}

void ContactManifold::GetPoints(std::vector<ManifoldPoint>* pt)
{
	pt = &mPoints;
}

int ContactManifold::GetNumPoints()
{
	mNumofPoint = mPoints.size();
	return mNumofPoint;
}

ManifoldPoint& ContactManifold::GeTPoint(int index)
{
	return mPoints.at(index);
}

void ContactManifold::ResolveContact(float duration)
{
	mDuration = duration;
	unsigned i;
	auto calculatedIterations = 0;
	while (calculatedIterations < mNumofPoint * 2)
	{
		//Find the contact with the largest closing velocity
		auto max = FLT_MAX;
		auto maxIndex = mNumofPoint;
		for (i = 0; i < mNumofPoint; i++)
		{
			float sepVel = CalculateSeparatingVelocity(mPoints[i]);
			if (sepVel < max &&
				(sepVel < 0 || mPoints[i].penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		// Do we have anything worth resolving?
		if (maxIndex == mNumofPoint)
			break;

		// Resolve this contact
		Resolve(mPoints[maxIndex]);

		// Update the interpenetrations for all particles
		SimpleMath::Vector3 *move = mPoints[maxIndex].BallsMovement;
		for (i = 0; i < mNumofPoint; i++)
		{
			if (mPoints[i].balls[0] == mPoints[maxIndex].balls[0])
			{
				mPoints[i].penetration -= move[0].Dot(mPoints[i].contactNormal);
			}
			else if (mPoints[i].balls[0] == mPoints[maxIndex].balls[1])
			{
				mPoints[i].penetration -= move[1].Dot(mPoints[i].contactNormal);
			}
			if (mPoints[i].balls[1])
			{
				if (mPoints[i].balls[1] == mPoints[maxIndex].balls[0])
				{
					mPoints[i].penetration += move[0].Dot(mPoints[i].contactNormal);
				}
				else if (mPoints[i].balls[1] == mPoints[maxIndex].balls[1])
				{
					mPoints[i].penetration += move[1].Dot(mPoints[i].contactNormal);
				}
			}
		}
		calculatedIterations++;
	}

}

void ContactManifold::Resolve(ManifoldPoint &mp)
{
	ResolveVelocity(mp);
	ResolveInterpenetration(mp);
}

void ContactManifold::ResolveVelocity(ManifoldPoint &mp)
{
	// Find the velocity in the direction of the contact
	float separatingVelocity = CalculateSeparatingVelocity(mp);

	// Check if it needs to be resolved
	if (separatingVelocity > 0)
	{
		// The contact is either separating, or stationary - there's
		// no impulse required.
		return;
	}

	// Calculate the new separating velocity
	float newSepVelocity = -separatingVelocity * mp.restitution * mp.friction;

	// Check the velocity build-up due to acceleration only
	SimpleMath::Vector3 accCausedVelocity = mp.balls[0]->GetAccleration();
	if (mp.balls[1])
	{
		accCausedVelocity -= mp.balls[1]->GetAccleration();
	}
	float accCausedSepVelocity = accCausedVelocity.Dot(mp.contactNormal) * mDuration;

	//if got closing velocity due to acceleration build-up,
	//remove it from the new separating velocity
	if (accCausedSepVelocity < 0)
	{
		newSepVelocity += mp.restitution * accCausedSepVelocity;
		if (newSepVelocity < 0.1f)
		{
			newSepVelocity = 0;
		}
	}

	float deltaVelocity = newSepVelocity - separatingVelocity;

	//Apply change in velocity to each object in proportion to their inverse mass
	float totalInverseMass = mp.balls[0]->GetInverseMass();
	if (mp.balls[1])
	{
		totalInverseMass += mp.balls[1]->GetInverseMass();
	}
	// If all particles have infinite mass, then impulses have no effect
	if (totalInverseMass <= 0) return;

	//Calculate te impulse to apply
	auto impulse = deltaVelocity / totalInverseMass;

	//Find the amount of impulse per unit of inverse mass
	SimpleMath::Vector3 impulsePerIMass = mp.contactNormal * impulse;

	//Apply impulse in the direction of contact and are proportional to the inverse mass
	mp.balls[0]->SetVelocity(
		mp.balls[0]->GetVelocity() +
		impulsePerIMass * mp.balls[0]->GetInverseMass()
	);
	if (mp.balls[1])
	{
		//Ball[1] goes in the opposite direction
		mp.balls[1]->SetVelocity(mp.balls[1]->GetVelocity() +
			impulsePerIMass * -mp.balls[1]->GetInverseMass()
		);
	}
}

void ContactManifold::ResolveInterpenetration(ManifoldPoint & mp)
{
	if (mp.penetration <= 0)
		return;

	// The movement of each object is based on their inverse mass, so
	// total that.
	auto totalInverseMass = mp.balls[0]->GetInverseMass();
	if (mp.balls[1])
	{
		totalInverseMass += mp.balls[1]->GetInverseMass();
	}
	// If all particles have infinite mass, then we do nothing
	if (totalInverseMass <= 0) return;

	//Find the amount of penetration resolution per unit of inverse mass
	SimpleMath::Vector3 movePerIMass = mp.contactNormal * (mp.penetration / totalInverseMass);

	//Calculate the movement amount
	mp.BallsMovement[0] = movePerIMass * mp.balls[0]->GetInverseMass();
	if (mp.balls[1])
	{
		mp.BallsMovement[1] = movePerIMass * -mp.balls[1]->GetInverseMass();
	}
	else
	{
		mp.BallsMovement[1] = SimpleMath::Vector3::Zero;
	}

	//Apply the peneration resolution
	mp.balls[0]->SetPosition(mp.balls[0]->GetPosition() + mp.BallsMovement[0]);
	if (mp.balls[1])
	{
		mp.balls[1]->SetPosition(mp.balls[1]->GetPosition() + mp.BallsMovement[1]);
	}
}

float ContactManifold::CalculateSeparatingVelocity(ManifoldPoint & mp) const
{
	SimpleMath::Vector3 relativeVelocity = mp.balls[0]->GetVelocity();
	if (mp.balls[1])
	{
		relativeVelocity -= mp.balls[1]->GetVelocity();
	}
	return relativeVelocity.Dot(mp.contactNormal);
}
