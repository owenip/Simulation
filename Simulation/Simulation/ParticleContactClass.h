#pragma once
#include "ParticleClass.h"
/**
* A contact represents two objects in contact (in this case
* ParticleContact representing two particles). Resolving a
* contact removes their interpenetration, and applies sufficient
* impulse to keep them apart. Colliding bodies may also rebound.
*
* The contact has no callable functions, it just holds the
* contact details. To resolve a set of contacts, use the particle
* contact resolver class.
*/
class ParticleContactResolver;

class ParticleContactClass
{
	friend class ParticleContactResolver;

public:
	//Hold the contacting particles
	//2th particle will be NULL for contacts with the scenery
	ParticleClass* particle[2];

	//Hold the normal restitution cofficient at the contact
	float restitution;

	//Hold the direction of the contact in world coordinates
	SimpleMath::Vector3 contactNormal;

	//Hold the depth of penetration at the contract
	float penetration;

	//Hold the amount each particle is moved by during interpenetration resolution
	SimpleMath::Vector3 particleMovement[2];

protected:
	//Resolves this contact for both velocity and interpenetration
	void Resolve(float duration);

	//Calculate the separating velocity at this contact
	float CalculateSeparatingVelocity() const;

private:
	//Handle the impulse calculations for this collision
	void ResolveVelocity(float duration);

	//Handle the interpenetration resolution for this contact
	void ResolveInterpenetration(float duration);
};

class ParticleContactResolver
{
	protected:
		//Hold the number of iterations allowed
		unsigned iterations;

		//Actulat number of iterations used
		unsigned iterationsUsed;

	public:
		ParticleContactResolver(unsigned iterations);

		//Sets the number of iterations that can be used
		void SetIterations(unsigned iterations);

		//Resolve a set of particle for both penetration and velocity
		//Contacts that cannot interact with each other should be
		//passed to separate calls to resolveContacts, as the
		//resolution algorithm takes much longer for lots of contacts
		//than it does for the same number of contacts in small sets.*/

		/*	@param contactArray Pointer to an array of particle contact
			* objects.
			*
			* @param numContacts The number of contacts in the array to
			* resolve.
			*
			* @param numIterations The number of iterations through the
			* resolution algorithm.This should be at least the number of
			* contacts(otherwise some constraints will not be resolved -
				*although sometimes this is not noticable).If the
			* iterations are not needed they will not be used, so adding
			* more iterations may not make any difference.But in some
			* cases you would need millions of iterations.Think about
			* the number of iterations as a bound : if you specify a large
			* number, sometimes the algorithm WILL use it, and you may
			* drop frames.
			*
			* @param duration The duration of the previous integration step.
			* This is used to compensate for forces applied.*/
		void ResolveContacts(ParticleContactClass *contactArray, 
			unsigned numContacts, 
			float duration);
};