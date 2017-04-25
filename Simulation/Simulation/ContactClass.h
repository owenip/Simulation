#pragma once
#include "pch.h"
#include "BallClass.h"

class ContactResolver;

class ContactClass
{
	friend class ContactResolver;

public:
	ContactClass();
	~ContactClass();

public:
	//Hold Balls that are involved in the contact
	BallClass* ball[2];

	//Hold the normal restitution coefficient at the contact
	float restitution;

	//Hold the direction of the contact in world coordinates
	SimpleMath::Vector3 contactNormal;

	//Hold the depth of penetration at the contract
	float penetration;

	//Hold the amount each ball is moved by during interpenetration resolution
	SimpleMath::Vector3 BallsMovement[2];

protected:
	//Resolves this contact for both velocity and interpenetration
	void Resolve(float duration);

	//Calculate the separating velocity at this contact
	float CalculateSeparatingVelocity();

private:
	//Handle the impulse calculations for this collision
	void ResolveVelocity(float duration);

	//Handle the interpenetration resolution for this contact
	void ResolveInterpenetration(float duration);
};

class ContactResolver
{
protected:
	//Hold the number of iterations allowed
	unsigned iterations;

	//Actulat number of iterations used
	unsigned iterationsUsed;

public:
	ContactResolver(unsigned iterations);

	//Sets the number of iterations that can be used
	void SetIterations(unsigned iterations);

	//Resolve a set of Ball/Object fpr bpth pentration and velocity
	//Contacts that cannot interact with each other should be passed to separate tp resolveContacts as the 
	//resolution takes much longer for lots of contacts than it does for the same number of contacts in small sets.
	void ResolveContacts(ContactClass *contactArray,
		unsigned numContacts,
		float duration);
};

class ContactGenerator
{
protected:
	~ContactGenerator() = default;
public:
	/**
	* Fills the given contact structure with the generated
	* contact. The contact pointer should point to the first
	* available contact in a contact array, where limit is the
	* maximum number of contacts in the array that can be written
	* to. The method returns the number of contacts that have
	* been written.
	*/
	virtual unsigned addContact(ContactClass *contact,
		unsigned limit) const = 0;
};

class GroundContacts : public ContactGenerator
{
private:
	std::vector<BallClass*> balls;
public:
	virtual ~GroundContacts() = default;
	void Init(std::vector<BallClass*> InBalls);
	unsigned addContact(ContactClass *contact, unsigned limit) const override;
};

class BallContacts : public ContactGenerator
{
private:
	std::vector<BallClass*> balls;
public:
	virtual ~BallContacts() = default;
	void Init(std::vector<BallClass*> InBalls);
	unsigned addContact(ContactClass *contact, unsigned limit) const override;
};

class WallContacts : public ContactGenerator
{
private:
	std::vector<BallClass*> balls;
	float mSurfaceRadius;
public:
	virtual ~WallContacts() = default;
	void Init(std::vector<BallClass*> InBalls, float InSurfaceRadius);
	unsigned addContact(ContactClass *contact, unsigned limit) const override;
};



