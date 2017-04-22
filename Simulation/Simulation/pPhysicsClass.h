#pragma once
#include "ParticleForceGenerator.h"
#include "ParticleLinkClass.h"

class pPhysicsClass
{
public:
	typedef std::vector<ParticleClass*> Particles;
	typedef std::vector<ParticleContactGenerator*> ContactGenerators;

protected:
	/**
	* Holds the particles
	*/
	Particles particles;

	/**
	* True if the world should calculate the number of iterations
	* to give the contact resolver at each frame.
	*/
	bool calculateIterations;

	/**
	* Holds the force generators for the particles in this world.
	*/
	ParticleForceRegistry registry;

	/**
	* Holds the resolver for contacts.
	*/
	ParticleContactResolver resolver;

	/**
	* Contact generators.
	*/
	ContactGenerators contactGenerators;

	/**
	* Holds the list of contacts.
	*/
	ParticleContactClass *contacts;

	/**
	* Holds the maximum number of contacts allowed (i.e. the
	* size of the contacts array).
	*/
	unsigned maxContacts;

public:

	/**
	* Creates a new particle simulator that can handle up to the
	* given number of contacts per frame. You can also optionally
	* give a number of contact-resolution iterations to use. If you
	* don't give a number of iterations, then twice the number of
	* contacts will be used.
	*/
	pPhysicsClass(unsigned maxContacts, unsigned iterations = 0);

	/**
	* Deletes the simulator.
	*/
	~pPhysicsClass();

	/**
	* Calls each of the registered contact generators to report
	* their contacts. Returns the number of generated contacts.
	*/
	unsigned generateContacts();

	/**
	* Integrates all the particles in this world forward in time
	* by the given duration.
	*/
	void integrate(float duration);

	/**
	* Processes all the physics for the particle world.
	*/
	void runPhysics(float duration);

	/**
	* Initializes the world for a simulation frame. This clears
	* the force accumulators for particles in the world. After
	* calling this, the particles can have their forces for this
	* frame added.
	*/
	void startFrame();

	/**
	*  Returns the list of particles.
	*/
	Particles& getParticles();
	std::vector<ParticleClass*> getParticlesList();
	/**
	* Returns the list of contact generators.
	*/
	ContactGenerators& getContactGenerators();

	/**
	* Returns the force registry.
	*/
	ParticleForceRegistry& getForceRegistry();
};
/**
* A contact generator that takes an STL vector of particle pointers and
* collides them against the ground.
*/
class GroundContacts : public ParticleContactGenerator
{
	pPhysicsClass::Particles *particles;

public:
	void init(pPhysicsClass::Particles *particles);

	virtual unsigned addContact(ParticleContactClass *contact,
		unsigned limit) const;
};
