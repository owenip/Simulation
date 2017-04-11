#include "pch.h"
#include "ParticleForceGenerator.h"

void ParticleForceRegistry::updateForces(float dt)
{	
	for (Registry::iterator i = registrations.begin(); i != registrations.end(); i++)
	{
		i->fg->updateForce(i->particle, dt);
	}
}

void ParticleForceRegistry::add(ParticleClass * particle, ParticleForceGenerator * fg)
{
	ParticleForceRegistry::ParticleForceRegistration registration;
	registration.particle = particle;
	registration.fg = fg;
	registrations.push_back(registration);
}

void ParticleForceRegistry::remove(ParticleClass * particle, ParticleForceGenerator * fg)
{	
}

void ParticleForceRegistry::clear()
{
	/*for each(ParticleForceRegistry::ParticleForceRegistration registration in registrations)
	{
		delete registration.particle;
		delete registration.fg;
	}*/
	registrations.clear();
}

ParticleGravity::ParticleGravity(const Vector3 & gravity):mGravity(gravity)
{
}

void ParticleGravity::updateForce(ParticleClass * particle, float duration)
{
	// Check that we do not have infinite mass
	if (!particle->HasFiniteMass()) return;

	// Apply the mass-scaled force to the particle
	particle->AddForce(mGravity *  (particle->GetMass()));
}

ParticleDrag::ParticleDrag(float k1, float k2):k1(k1),k2(k2)
{
}

void ParticleDrag::updateForce(ParticleClass * particle, float duration)
{
	SimpleMath::Vector3 force;
	particle->GetVelocity(&force);

	//Calculate the total drag cofficient
	float dragcoeff = force.LengthSquared();
	dragcoeff = k1 * dragcoeff + k2 * dragcoeff * dragcoeff;

	//Calculate the final force and apply it
	force.Normalize();
	force *= -dragcoeff;
	particle->AddForce(force);
}
