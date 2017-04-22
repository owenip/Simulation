#include "pch.h"
#include "ParticleContactClass.h"

void ParticleContactClass::Resolve(float duration)
{
	ResolveVelocity(duration);
}

float ParticleContactClass::CalculateSeparatingVelocity() const
{
	auto relativeVelocity = particle[0]->GetVelocity();
	if (particle[1])
		relativeVelocity -= particle[1]->GetVelocity();
	return relativeVelocity.Dot(contactNormal);
}

void ParticleContactClass::ResolveVelocity(float duration)
{
	//Find the velocity in the direction of the contact
	auto separatingVelocity = CalculateSeparatingVelocity();

	//Check whether it needs to be resolved
	if(separatingVelocity > 0)
	{
		//The contact is either separating or stationary 
		//no impulse required
		return;
	}

	// Calculate the new separating velocity.
	auto newSepVelocity = -separatingVelocity * restitution;

	//Check the velocity build-up due to acceleration only
	auto accCausedVelocity = particle[0]->GetAccleration();
	if (particle[1])
		accCausedVelocity -= particle[1]->GetAccleration();
	auto accCausedSepVelocity = accCausedVelocity.Dot(contactNormal) * duration;

	//If a closing velocity is obtained due to accerleration build-up
	//Remove it from the new separating velocity
	if(accCausedSepVelocity < 0)
	{
		newSepVelocity += restitution * accCausedSepVelocity;

		//Check the new separate velocity is not removed to less than zero
		if (newSepVelocity < 0)
			newSepVelocity = 0;
	}

	auto deltaVelocity = newSepVelocity - separatingVelocity;

	//Applying the change in velocity to each object in proportion to
	//their inverse mass 
	//(Lower inverse mass[higher actual mass] get less in velocity)
	auto totalInverseMass = particle[0]->GetInverseMass();
	if (particle[1])
		totalInverseMass += particle[1]->GetInverseMass();

	//If all particles have infinite mass, then impulse has no effect
	if(totalInverseMass <=0)
		return;

	//Calculate te impulse to apply
	auto impulse = deltaVelocity / totalInverseMass;

	//Find the amount of impulse per unit of inverse mass
	auto impulsePerInverseMass = contactNormal * impulse;

	//Apply impulse: they are applied in the direction of contact,
	//and are proportional to the inverse normal
	particle[0]->SetVelocity(
		particle[0]->GetVelocity() + impulsePerInverseMass * particle[0]->GetInverseMass());
	if(particle[1])
	{
		//Particle 1 goes in the opposite direction
		particle[1]->SetVelocity(
			particle[1]->GetVelocity() + impulsePerInverseMass * -particle[1]->GetInverseMass());
	}
	
}

void ParticleContactClass::ResolveInterpenetration(float duration)
{
	//Skip resolving if no penetration
	if(penetration <= 0)
		return;

	//Movement of each object is based on their inverse mass
	//Therefore total inverse mass is needed
	auto totalinverseMass = particle[0]->GetInverseMass();
	if (particle[1])
		totalinverseMass += particle[1]->GetInverseMass();

	//if all particles have infinite mass, skip resolving
	if(totalinverseMass <= 0)
		return;

	//Find the amount of penetration resoultion per unit of inverse mass
	auto movePerInverseMass = contactNormal * (penetration / totalinverseMass);

	//Calculate the movement amounts
	particleMovement[0] = movePerInverseMass * particle[0]->GetInverseMass();
	if(particle[1])
	{
		particleMovement[1] = movePerInverseMass * -particle[1]->GetInverseMass();
	}
	else
	{
		particleMovement[1] = SimpleMath::Vector3::Zero;
	}

	//Apply the penetration resolution
	auto newPos = particleMovement[0] + particle[0]->GetPosition();
	particle[0]->SetPosition(newPos);
	if (particle[1])
		newPos = particle[1]->GetPosition() + particleMovement[1];
		particle[1]->SetPosition(newPos);
}

ParticleContactResolver::ParticleContactResolver(unsigned iterations):
iterations(iterations)
{

}

void ParticleContactResolver::SetIterations(unsigned iterations)
{
	ParticleContactResolver::iterations = iterations;
}

void ParticleContactResolver::ResolveContacts(ParticleContactClass * contactArray, unsigned numContacts, float duration)
{
	unsigned i;

	iterationsUsed = 0;
	while(iterationsUsed < iterations)
	{
		auto max = FLT_MAX;
		auto maxIndex = numContacts;
		for(i=0; i < numContacts; i++)
		{
			auto sepVel = contactArray[i].CalculateSeparatingVelocity();
			if(sepVel < max && 
				(sepVel<0 || contactArray[i].penetration > 0))
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

		//Update the interpenetrations for all particles
		SimpleMath::Vector3 *move = contactArray[maxIndex].particleMovement;
		for(i = 0; i<numContacts;i++)
		{
			SimpleMath::Vector3 temp;
			if (contactArray[i].particle[0] == contactArray[maxIndex].particle[0])
			{
				contactArray[i].penetration -=  contactArray[i].contactNormal.Dot(move[0]);
			}
			else if (contactArray[i].particle[0] == contactArray[maxIndex].particle[1])
			{
				contactArray[i].penetration -= contactArray[i].contactNormal.Dot(move[1]);
			}
			if (contactArray[i].particle[1])
			{
				if (contactArray[i].particle[1] == contactArray[maxIndex].particle[0])
				{
					contactArray[i].penetration += contactArray[i].contactNormal.Dot(move[0]);
				}
				else if (contactArray[i].particle[1] == contactArray[maxIndex].particle[1])
				{
					contactArray[i].penetration += contactArray[i].contactNormal.Dot(move[1]);
				}
			}
		}

	}
}
