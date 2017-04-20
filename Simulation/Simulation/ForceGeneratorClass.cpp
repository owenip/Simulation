#include "pch.h"
#include "ForceGeneratorClass.h"

void ForceRegistry::UpdateForces(float duration)
{	
	for (auto i = registrations.begin();i != registrations.end(); i++)
	{
		i->fg->UpdateForce(i->body, duration);
	}
}

void ForceRegistry::Add(RigidBodyClass *body, ForceGenerator *fg)
{
	ForceRegistry::ForceRegistration registration;
	registration.body = body;
	registration.fg = fg;
	registrations.push_back(registration);
}

Gravity::Gravity(const Vector3& gravity)
	: gravity(gravity)
{
}

void Gravity::UpdateForce(RigidBodyClass* body, float duration)
{
	// Check that we do not have infinite mass
	if (!body->HasFiniteMass()) return;

	// Apply the mass-scaled force to the body
	body->AddForce(gravity * body->GetMass());
}

Spring::Spring(const Vector3 &localConnectionPt,
	RigidBodyClass *other,
	const Vector3 &otherConnectionPt,
	float springConstant,
	float restLength)
	: connectionPoint(localConnectionPt),
	otherConnectionPoint(otherConnectionPt),
	other(other),
	springConstant(springConstant),
	restLength(restLength)
{
}
void Spring::UpdateForce(RigidBodyClass* body, float duration)
{
	// Calculate the two ends in world space
	Vector3 lws = body->GetPointInWorldSpace(connectionPoint);
	Vector3 ows = other->GetPointInWorldSpace(otherConnectionPoint);

	// Calculate the vector of the spring
	Vector3 force = lws - ows;

	// Calculate the magnitude of the force
	float magnitude = force.Length();
	magnitude = fabsf(magnitude - restLength);
	magnitude *= springConstant;

	// Calculate the final force and apply it
	force.Normalize(force);
	force *= -magnitude;
	body->AddForceAtPoint(force, lws);
}