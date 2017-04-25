#include "ContactClass.h"



ContactClass::ContactClass()
{
}


ContactClass::~ContactClass()
{
}

void ContactClass::Resolve(float duration)
{
	ResolveVelocity(duration);
	ResolveInterpenetration(duration);
}

float ContactClass::CalculateSeparatingVelocity() const
{
	Vector3 relativeVelocity = ball[0]->GetVelocity();
	if(ball[1])
	{
		relativeVelocity -= ball[1]->GetVelocity();
	}

	return relativeVelocity.Dot(contactNormal);
}

void ContactClass::ResolveVelocity(float duration)
{
	// Find the velocity in the direction of the contact
	float separatingVelocity = CalculateSeparatingVelocity();

	// Check if it needs to be resolved
	if(separatingVelocity > 0)
	{
		// The contact is either separating, or stationary - there's
		// no impulse required.
		return;
	}

	// Calculate the new separating velocity
	float newSepVelocity = -separatingVelocity * restitution;

	// Check the velocity build-up due to acceleration only
	Vector3 accCausedVelocity = ball[0]->GetAccleration();
	if(ball[1])
	{
		accCausedVelocity -= ball[1]->GetAccleration();
	}
	float accCausedSepVelocity = accCausedVelocity.Dot(contactNormal) * duration;

	//if got closing velocity due to acceleration build-up,
	//remove it from the new separating velocity
	if(accCausedSepVelocity < 0)
	{
		newSepVelocity += restitution * accCausedSepVelocity;
		if(newSepVelocity < 0)
		{
			newSepVelocity = 0;
		}
	}

	float deltaVelocity = newSepVelocity - separatingVelocity;

	//Apply change in velocity to each object in proportion to their inverse mass
	float totalInverseMass = ball[0]->GetInverseMass();
	if(ball[1])
	{
		totalInverseMass += ball[1]->GetInverseMass();
	}
	// If all particles have infinite mass, then impulses have no effect
	if (totalInverseMass <= 0) return;

	//Calculate te impulse to apply
	auto impulse = deltaVelocity / totalInverseMass;

	//Find the amount of impulse per unit of inverse mass
	Vector3 impulsePerIMass = contactNormal * impulse;

	//Apply impulse in the direction of contact and are proportional to the inverse mass
	ball[0]->SetVelocity(
		ball[0]->GetVelocity() +
		impulsePerIMass * ball[0]->GetInverseMass()
	);
	if (ball[1])
	{
		//Ball[1] goes in the opposite direction
		ball[1]->SetVelocity(ball[1]->GetVelocity() +
			impulsePerIMass * -ball[1]->GetInverseMass()
		);
	}


}

void ContactClass::ResolveInterpenetration(float duration)
{
	if (penetration <= 0) return;

	// The movement of each object is based on their inverse mass, so
	// total that.
	auto totalInverseMass = ball[0]->GetInverseMass();
	if(ball[1])
	{
		totalInverseMass += ball[1]->GetInverseMass();
	}
	// If all particles have infinite mass, then we do nothing
	if (totalInverseMass <= 0) return;

	//Find the amount of penetration resolution per unit of inverse mass
	Vector3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	//Calculate the movement amount
	BallsMovement[0] = movePerIMass * ball[0]->GetInverseMass();
	if (ball[1])
	{
		BallsMovement[1] = movePerIMass * -ball[1]->GetInverseMass();
	}
	else
	{
		BallsMovement[1] = Vector3::Zero;
	}

	//Apply the peneration resolution
	ball[0]->SetPosition(ball[0]->GetPosition() + BallsMovement[0]);
	if(ball[1])
	{
		ball[1]->SetPosition(ball[1]->GetPosition() + BallsMovement[1]);
	}
}

ContactResolver::ContactResolver(unsigned InIterations):
	iterations(InIterations)
{

}

void ContactResolver::SetIterations(unsigned InIterations)
{
	iterations = InIterations;
}

void ContactResolver::ResolveContacts(ContactClass * contactArray, unsigned numContacts, float duration)
{
	unsigned i;

	iterationsUsed = 0;
	while(iterationsUsed < iterations)
	{
		//Find the contact with the largest closing velocity
		auto max = FLT_MAX;
		auto maxIndex = numContacts;
		for(i = 0; i< numContacts; i++)
		{
			auto sepVel = contactArray[i].CalculateSeparatingVelocity();
			if(sepVel < max && 
				(sepVel < 0 || contactArray[i].penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}
		//Exit if nothing to resolve
		if (maxIndex == numContacts)
			break;

		//Resolve this contact
		contactArray[maxIndex].Resolve(duration);

		// Update the interpenetrations for all ball
		Vector3 *move = contactArray[maxIndex].BallsMovement;
		for(i = 0; i < numContacts; i++)
		{
			if(contactArray[i].ball[0] == contactArray[maxIndex].ball[0])
			{
				contactArray[i].penetration -= move[0].Dot(contactArray[i].contactNormal);
			}
			else if(contactArray[i].ball[0] == contactArray[maxIndex].ball[1])
			{
				contactArray[i].penetration -= move[1].Dot(contactArray[i].contactNormal);
			}
			if(contactArray[i].ball[1])
			{
				if(contactArray[i].ball[1] == contactArray[maxIndex].ball[0])
				{
					contactArray[i].penetration += move[0].Dot(contactArray[i].contactNormal);
				}
				else if(contactArray[i].ball[1] == contactArray[maxIndex].ball[1])
				{
					contactArray[i].penetration += move[1].Dot(contactArray[i].contactNormal);
				}
			}
		}

		iterationsUsed++;
	}

}


void GroundContacts::Init(std::vector<BallClass*> InBalls)
{
	GroundContacts::balls = InBalls;
}

unsigned GroundContacts::addContact(ContactClass * contact, unsigned limit) const
{
	unsigned count = 0;
	for each(BallClass* element in balls)
	{
		float y = element->GetPosition().y - element->GetRadius();
		if (y < 0.0f)
		{
			contact->contactNormal = Vector3::Up;
			contact->ball[0] = element;
			contact->ball[1] = nullptr;
			contact->penetration = -y;
			contact->restitution = 0.2f;
			contact++;
			count++;
		}
		if (count >= limit)
			return count;
	}
	return count;
}

void BallContacts::Init(std::vector<BallClass*> InBalls)
{
	BallContacts::balls = InBalls;
}

unsigned BallContacts::addContact(ContactClass * contact, unsigned limit) const
{
	unsigned count = 0;
	for each(BallClass* b1 in balls)
	{
		Vector3 pos1 = b1->GetPosition();
		for each(BallClass* b2 in balls)
		{
			Vector3 pos2 = b2->GetPosition();
			Vector3 midline = pos1 - pos2;
			float size = midline.Length();
			if(size<= 0.0f || size >= (b1->GetRadius()+b2->GetRadius()))
			{
				continue;
			}
			else
			{
				Vector3 normal = midline * (1.f / size);
				contact->contactNormal = normal;
				contact->ball[0] = b1;
				contact->ball[1] = b2;
				contact->penetration = (b1->GetRadius() + b2->GetRadius() - size);
				contact->restitution = 0.2f;
				contact++;
				count++;
			}

			if (count >= limit)
				return count;
		}		
	}
	return count;
}

void WallContacts::Init(std::vector<BallClass*> InBalls, float InSurfaceRadius)
{
	WallContacts::balls = InBalls;
	WallContacts::mSurfaceRadius = InSurfaceRadius;
}

unsigned WallContacts::addContact(ContactClass * contact, unsigned limit) const
{
	unsigned count = 0;
	
	for each(BallClass* element in balls)
	{
		Vector3 normal = element->GetVelocity();
		normal.Normalize();
		normal = -normal;
		Vector3 ballPos = element->GetPosition();		
		float ballDistance = ballPos.Length() + element->GetRadius();
		
		if(ballDistance >= mSurfaceRadius)
		{
			contact->contactNormal = normal;
			contact->ball[0] = element;
			contact->ball[1] = nullptr;
			contact->penetration = ballDistance - mSurfaceRadius;
			contact->restitution = 0.2f;
			contact++;
			count++;
		}else
		{
			continue;
		}

		if (count >= limit)
			return count;
	}
	return count;
}
