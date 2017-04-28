#pragma once

#include "ParticleClass.h"

class ParticleForceGenerator
{
public:
	 
	//Overlard this to calculate and update the force applied to the given particle
	virtual void updateForce(ParticleClass *particle, float dt) = 0;
};


/**
* A force generator that applies a gravitational force. One instance
* can be used for multiple particles.
*/
class ParticleGravity : public ParticleForceGenerator
{
	/** Holds the acceleration due to gravity. */
	Vector3 mGravity;
public:
	/** Creates the generator with the given acceleration. */
	ParticleGravity(const Vector3 &gravity);
	/** Applies the gravitational force to the given particle. */
	virtual void updateForce(ParticleClass *particle, float duration);
};

/**
* A force generator that applies a drag force. One instance
* can be used for multiple particles.
*/
class ParticleDrag : public ParticleForceGenerator
{
	/** Holds the velocity drag coeffificent. */
	float k1;

	/** Holds the velocity squared drag coeffificent. */
	float k2;

public:

	/** Creates the generator with the given coefficients. */
	ParticleDrag(float k1, float k2);

	/** Applies the drag force to the given particle. */
	virtual void updateForce(ParticleClass *particle, float duration);
};

class ParticleForceRegistry
{
protected:
	struct ParticleForceRegistration
	{
		ParticleClass *particle;
		ParticleForceGenerator *fg;
	};

	typedef std::vector<ParticleForceRegistration> Registry;
	Registry registrations;

public:
	//Call all the force generators to update the forces of their corresponding particles
	void updateForces(float dt);

	//Registers the given force generator to apply to the give particle.
	void add(ParticleClass *particle, ParticleForceGenerator *fg);

	//Remove the given the registeres pair from the registry
	void remove(ParticleClass *particle, ParticleForceGenerator *fg);

	//Clear all registrations from the registry.
	void clear();


};