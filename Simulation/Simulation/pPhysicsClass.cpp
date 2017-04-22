#include "pch.h"
#include "pPhysicsClass.h"

pPhysicsClass::pPhysicsClass(unsigned maxContacts, unsigned iterations):
resolver(iterations),
maxContacts(maxContacts)
{
	contacts = new ParticleContactClass[maxContacts];
	calculateIterations = (iterations == 0);
}

pPhysicsClass::~pPhysicsClass()
{
	delete[] contacts;
}

void pPhysicsClass::startFrame()
{
	for (Particles::iterator p = particles.begin();
		p != particles.end();
		p++)
	{
		// Remove all forces from the accumulator
		(*p)->ClearAccumulator();
	}
}

unsigned pPhysicsClass::generateContacts()
{
	unsigned limit = maxContacts;
	ParticleContactClass *nextContact = contacts;

	for (ContactGenerators::iterator g = contactGenerators.begin();
		g != contactGenerators.end();
		g++)
	{
		unsigned used = (*g)->addContact(nextContact, limit);
		limit -= used;
		nextContact += used;

		// We've run out of contacts to fill. This means we're missing
		// contacts.
		if (limit <= 0) break;
	}

	// Return the number of contacts used.
	return maxContacts - limit;
}

void pPhysicsClass::integrate(float duration)
{
	for (Particles::iterator p = particles.begin();
		p != particles.end();
		p++)
	{
		// Remove all forces from the accumulator
		(*p)->Integrate(duration);
	}
}

void pPhysicsClass::runPhysics(float duration)
{
	// First apply the force generators
	registry.updateForces(duration);

	// Then integrate the objects
	integrate(duration);

	// Generate contacts
	unsigned usedContacts = generateContacts();

	// And process them
	if (usedContacts)
	{
		if (calculateIterations) resolver.SetIterations(usedContacts * 2);
		resolver.ResolveContacts(contacts, usedContacts, duration);
	}
}


pPhysicsClass::Particles& pPhysicsClass::getParticles()
{
	return particles;
}

std::vector<ParticleClass*> pPhysicsClass::getParticlesList()
{
	return particles;
}

pPhysicsClass::ContactGenerators& pPhysicsClass::getContactGenerators()
{
	return contactGenerators;
}


ParticleForceRegistry& pPhysicsClass::getForceRegistry()
{
	return registry;
}

void GroundContacts::init(pPhysicsClass::Particles *particles)
{
	GroundContacts::particles = particles;
}


unsigned GroundContacts::addContact(ParticleContactClass *contact,
	unsigned limit) const
{
	unsigned count = 0;
	for (pPhysicsClass::Particles::iterator p = particles->begin();
		p != particles->end();
		p++)
	{
		float y = (*p)->GetPosition().y;
		if (y < 1.0f )
		{
			contact->contactNormal = Vector3::Up;
			contact->particle[0] = *p;
			contact->particle[1] = nullptr;
			contact->penetration = -y;
			contact->restitution = 0.2f;
			contact++;
			count++;
		}

		if (count >= limit) return count;
	}
	return count;
}