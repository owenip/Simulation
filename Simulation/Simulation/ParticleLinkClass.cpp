#include "pch.h"
#include "ParticleLinkClass.h"

float ParticleLinkClass::currentLength() const
{
	auto relativePos = particle[0]->GetPosition() -
						particle[1]->GetPosition();
	return relativePos.Length();
}

unsigned ParticleCable::addContact(ParticleContactClass *contact,
	unsigned limit) const
{
	// Find the length of the cable
	float length = currentLength();

	// Check if we're over-extended
	if (length < maxLength)
	{
		return 0;
	}

	// Otherwise return the contact
	contact->particle[0] = particle[0];
	contact->particle[1] = particle[1];

	// Calculate the normal
	Vector3 normal = particle[1]->GetPosition() - particle[0]->GetPosition();
	normal.Normalize();
	contact->contactNormal = normal;

	contact->penetration = length - maxLength;
	contact->restitution = restitution;

	return 1;
}


unsigned ParticleRod::addContact(ParticleContactClass *contact,
	unsigned limit) const
{
	// Find the length of the rod
	float currentLen = currentLength();

	// Check if we're over-extended
	if (currentLen == length)
	{
		return 0;
	}

	// Otherwise return the contact
	contact->particle[0] = particle[0];
	contact->particle[1] = particle[1];

	// Calculate the normal
	Vector3 normal = particle[1]->GetPosition() - particle[0]->GetPosition();
	normal.Normalize();

	// The contact normal depends on whether we're extending or compressing
	if (currentLen > length) {
		contact->contactNormal = normal;
		contact->penetration = currentLen - length;
	}
	else {
		contact->contactNormal = normal * -1;
		contact->penetration = length - currentLen;
	}

	// Always use zero restitution (no bounciness)
	contact->restitution = 0;

	return 1;
}

float ParticleConstraint::currentLength() const
{
	Vector3 relativePos = particle->GetPosition() - anchor;
	return relativePos.Length();
}

unsigned ParticleCableConstraint::addContact(ParticleContactClass *contact,
	unsigned limit) const
{
	// Find the length of the cable
	float length = currentLength();

	// Check if we're over-extended
	if (length < maxLength)
	{
		return 0;
	}

	// Otherwise return the contact
	contact->particle[0] = particle;
	contact->particle[1] = 0;

	// Calculate the normal
	Vector3 normal = anchor - particle->GetPosition();
	normal.Normalize();
	contact->contactNormal = normal;

	contact->penetration = length - maxLength;
	contact->restitution = restitution;

	return 1;
}

unsigned ParticleRodConstraint::addContact(ParticleContactClass *contact,
	unsigned limit) const
{
	// Find the length of the rod
	float currentLen = currentLength();

	// Check if we're over-extended
	if (currentLen == length)
	{
		return 0;
	}

	// Otherwise return the contact
	contact->particle[0] = particle;
	contact->particle[1] = 0;

	// Calculate the normal
	Vector3 normal = anchor - particle->GetPosition();
	normal.Normalize();

	// The contact normal depends on whether we're extending or compressing
	if (currentLen > length) {
		contact->contactNormal = normal;
		contact->penetration = currentLen - length;
	}
	else {
		contact->contactNormal = normal * -1;
		contact->penetration = length - currentLen;
	}

	// Always use zero restitution (no bounciness)
	contact->restitution = 0;

	return 1;
}