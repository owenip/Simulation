#include "pch.h"
#include "ContactClass.h"

void ContactClass::setBodyData(RigidBodyClass * one, RigidBodyClass * two, float friction, float restitution)
{
	ContactClass::body[0] = one;
	ContactClass::body[1] = two;
	ContactClass::friction = friction;
	ContactClass::restitution = restitution;
}

void ContactClass::matchAwakeState()
{
	// Collisions with the world never cause a body to wake up.
	if (!body[1]) return;

	bool body0awake = body[0]->GetAwake();
	bool body1awake = body[1]->GetAwake();

	// Wake up only the sleeping one
	if (body0awake ^ body1awake) {
		if (body0awake) body[1]->SetAwake();
		else body[0]->SetAwake();
	}
}

/*
* Swaps the bodies in the current contact, so body 0 is at body 1 and
* vice versa. This also changes the direction of the contact normal,
* but doesn't update any calculated internal data. If you are calling
* this method manually, then call calculateInternals afterwards to
* make sure the internal data is up to date.
*/
void ContactClass::swapBodies()
{
	contactNormal *= -1;

	RigidBodyClass *temp = body[0];
	body[0] = body[1];
	body[1] = temp;
}

/*
* Constructs an arbitrary orthonormal basis for the contact.  This is
* stored as a 3x3 matrix, where each vector is a column (in other
* words the matrix transforms contact space into world space). The x
* direction is generated from the contact normal, and the y and z
* directionss are set so they are at right angles to it.
*/
inline
void ContactClass::calculateContactBasis()
{
	SimpleMath::Vector3 contactTangent[2];

	// Check whether the Z-axis is nearer to the X or Y axis
	if (fabsf(contactNormal.x) > fabsf(contactNormal.y))
	{
		// Scaling factor to ensure the results are normalised
		const float s = 1.0f / sqrtf(contactNormal.z*contactNormal.z +
			contactNormal.x*contactNormal.x);

		// The new X-axis is at right angles to the world Y-axis
		contactTangent[0].x = contactNormal.z*s;
		contactTangent[0].y = 0;
		contactTangent[0].z = -contactNormal.x*s;

		// The new Y-axis is at right angles to the new X- and Z- axes
		contactTangent[1].x = contactNormal.y*contactTangent[0].x;
		contactTangent[1].y = contactNormal.z*contactTangent[0].x -
			contactNormal.x*contactTangent[0].z;
		contactTangent[1].z = -contactNormal.y*contactTangent[0].x;
	}
	else
	{
		// Scaling factor to ensure the results are normalised
		const float s = 1.0f / sqrtf(contactNormal.z*contactNormal.z +
			contactNormal.y*contactNormal.y);

		// The new X-axis is at right angles to the world X-axis
		contactTangent[0].x = 0;
		contactTangent[0].y = -contactNormal.z*s;
		contactTangent[0].z = contactNormal.y*s;

		// The new Y-axis is at right angles to the new X- and Z- axes
		contactTangent[1].x = contactNormal.y*contactTangent[0].z -
			contactNormal.z*contactTangent[0].y;
		contactTangent[1].y = -contactNormal.x*contactTangent[0].z;
		contactTangent[1].z = contactNormal.x*contactTangent[0].y;
	}

	// Make a matrix from the three vectors.
	contactToWorld = SimpleMath::Matrix(contactNormal, 
										contactTangent[0], 
										contactTangent[1]);
}


SimpleMath::Vector3 ContactClass::calculateLocalVelocity(unsigned bodyIndex, float duration)
{
	RigidBodyClass *thisBody = body[bodyIndex];

	// Work out the velocity of the contact point.
	SimpleMath::Vector3 velocity =
		thisBody->GetRotation().Cross(relativeContactPosition[bodyIndex]);
	velocity += thisBody->GetVelocity();

	// Turn the velocity into contact-coordinates.
	SimpleMath::Vector3 contactVelocity = velocity.Transform(velocity, contactToWorld.Transpose());

	// Calculate the ammount of velocity that is due to forces without
	// reactions.
	SimpleMath::Vector3 accVelocity = thisBody->GetLastFrameAcceleration() * duration;

	// Calculate the velocity in contact-coordinates.
	accVelocity = accVelocity.Transform(accVelocity, contactToWorld.Transpose());

	// We ignore any component of acceleration in the contact normal
	// direction, we are only interested in planar acceleration
	accVelocity.x = 0;

	// Add the planar velocities - if there's enough friction they will
	// be removed during velocity resolution
	contactVelocity += accVelocity;

	// And return it
	return contactVelocity;
}



void ContactClass::calculateDesiredDeltaVelocity(float duration)
{
	const static float velocityLimit = 0.25f;

	// Calculate the acceleration induced velocity accumulated this frame
	float velocityFromAcc = 0;

	if (body[0]->GetAwake())
	{
		SimpleMath::Vector3 temp = (body[0]->GetLastFrameAcceleration* duration);
		velocityFromAcc += temp.Dot(contactNormal);
	}

	if (body[1] && body[1]->GetAwake())
	{
		SimpleMath::Vector3 temp = body[1]->GetLastFrameAcceleration() * duration ;
		velocityFromAcc -= temp.Dot(contactNormal);
	}

	// If the velocity is very slow, limit the restitution
	float thisRestitution = restitution;
	if (fabsf(contactVelocity.x) < velocityLimit)
	{
		thisRestitution = 0.0f;
	}

	// Combine the bounce velocity with the removed
	// acceleration velocity.
	desiredDeltaVelocity =
		-contactVelocity.x
		- thisRestitution * (contactVelocity.x - velocityFromAcc);
}

void ContactClass::calculateInternals(float duration)
{
	// Check if the first object is NULL, and swap if it is.
	if (!body[0]) swapBodies();
	assert(body[0]);

	// Calculate an set of axis at the contact point.
	calculateContactBasis();

	// Store the relative position of the contact relative to each body
	relativeContactPosition[0] = contactPoint - body[0]->GetPosition();
	if (body[1]) {
		relativeContactPosition[1] = contactPoint - body[1]->GetPosition();
	}

	// Find the relative velocity of the bodies at the contact point.
	contactVelocity = calculateLocalVelocity(0, duration);
	if (body[1]) {
		contactVelocity -= calculateLocalVelocity(1, duration);
	}

	// Calculate the desired change in velocity for resolution
	calculateDesiredDeltaVelocity(duration);
}


void ContactClass::applyVelocityChange(SimpleMath::Vector3 velocityChange[2],
                                  SimpleMath::Vector3 rotationChange[2])
{
	// Get hold of the inverse mass and inverse inertia tensor, both in
	// world coordinates.
	SimpleMath::Matrix inverseInertiaTensor[2];
	body[0]->GetInverseInertiaTensorWorld(&inverseInertiaTensor[0]);
	if (body[1])
		body[1]->GetInverseInertiaTensorWorld(&inverseInertiaTensor[1]);

	// We will calculate the impulse for each contact axis
	SimpleMath::Vector3 impulseContact;

	if (friction == 0.0f)
	{
		// Use the short format for frictionless contacts
		impulseContact = calculateFrictionlessImpulse(inverseInertiaTensor);
	}
	else
	{
		// Otherwise we may have impulses that aren't in the direction of the
		// contact, so we need the more complex version.
		impulseContact = calculateFrictionImpulse(inverseInertiaTensor);
	}

	// Convert impulse to world coordinates
	SimpleMath::Vector3 impulse = impulseContact.Transform(impulseContact,contactToWorld);

	// Split in the impulse into linear and rotational components
	SimpleMath::Vector3 impulsiveTorque = relativeContactPosition[0].Cross(impulse);
	rotationChange[0] = impulsiveTorque.Transform(impulsiveTorque, inverseInertiaTensor[0]);		
	velocityChange[0] = SimpleMath::Vector3::Zero;
	velocityChange[0] += (impulse * body[0]->GetInverseMass());

	// Apply the changes
	body[0]->AddVelocity(velocityChange[0]);
	body[0]->AddRotation(rotationChange[0]);

	if (body[1])
	{
		// Work out body one's linear and angular changes
		SimpleMath::Vector3 impulsiveTorque = impulse.Cross(relativeContactPosition[1]);
		rotationChange[1] = impulsiveTorque.Transform(impulsiveTorque, inverseInertiaTensor[1]);
		velocityChange[1] = SimpleMath::Vector3::Zero;
		velocityChange[1] += (impulse * -body[1]->GetInverseMass()); 
		
		// And apply them.
		body[1]->AddVelocity(velocityChange[1]);
		body[1]->AddRotation(rotationChange[1]);
	}
}

inline
SimpleMath::Vector3 ContactClass::calculateFrictionlessImpulse(SimpleMath::Matrix * inverseInertiaTensor)
{
	SimpleMath::Vector3 impulseContact;

	// Build a vector that shows the change in velocity in
	// world space for a unit impulse in the direction of the contact
	// normal.
	SimpleMath::Vector3 deltaVelWorld = relativeContactPosition[0].Cross(contactNormal);
	deltaVelWorld = deltaVelWorld.Transform(deltaVelWorld, inverseInertiaTensor[0]);
	deltaVelWorld = deltaVelWorld.Cross(relativeContactPosition[0]);

	// Work out the change in velocity in contact coordiantes.
	float deltaVelocity = deltaVelWorld.Dot(contactNormal);

	// Add the linear component of velocity change
	deltaVelocity += body[0]->GetInverseMass();

	// Check if we need to the second body's data
	if (body[1])
	{
		// Go through the same transformation sequence again
		SimpleMath::Vector3 deltaVelWorld = relativeContactPosition[1].Cross(contactNormal);
		deltaVelWorld = deltaVelWorld.Transform(deltaVelWorld, inverseInertiaTensor[1]);
		deltaVelWorld = deltaVelWorld .Cross(relativeContactPosition[1]);

		// Add the change in velocity due to rotation
		deltaVelocity += deltaVelWorld.Dot(contactNormal);

		// Add the change in velocity due to linear motion
		deltaVelocity += body[1]->GetInverseMass();
	}

	// Calculate the required size of the impulse
	impulseContact.x = desiredDeltaVelocity / deltaVelocity;
	impulseContact.y = 0;
	impulseContact.z = 0;
	return impulseContact;
}


inline
SimpleMath::Vector3 ContactClass::calculateFrictionImpulse(SimpleMath::Matrix * inverseInertiaTensor)
{
	SimpleMath::Vector3 impulseContact;
	float inverseMass = body[0]->GetInverseMass();

	// The equivalent of a cross product in matrices is multiplication
	// by a skew symmetric matrix - we build the matrix for converting
	// between linear and angular quantities.
	SimpleMath::Matrix impulseToTorque;
	//impulseToTorque.SetSkewSymmetric(relativeContactPosition[0]);

	// Build the matrix to convert contact impulse to change in velocity
	// in world coordinates.
	SimpleMath::Matrix deltaVelWorld = impulseToTorque;
	deltaVelWorld *= inverseInertiaTensor[0];
	deltaVelWorld *= impulseToTorque;
	deltaVelWorld *= -1;

	// Check if we need to add body two's data
	if (body[1])
	{
		// Set the cross product matrix
		//impulseToTorque.SetSkewSymmetric(relativeContactPosition[1]);

		// Calculate the velocity change matrix
		SimpleMath::Matrix deltaVelWorld2 = impulseToTorque;
		deltaVelWorld2 *= inverseInertiaTensor[1];
		deltaVelWorld2 *= impulseToTorque;
		deltaVelWorld2 *= -1;

		// Add to the total delta velocity.
		deltaVelWorld += deltaVelWorld2;

		// Add to the inverse mass
		inverseMass += body[1]->GetInverseMass();
	}

	// Do a change of basis to convert into contact coordinates.
	SimpleMath::Matrix deltaVelocity = contactToWorld.Transpose();
	deltaVelocity *= deltaVelWorld;
	deltaVelocity *= contactToWorld;

	// Add in the linear velocity change
	deltaVelocity._11 += inverseMass;
	deltaVelocity._22 += inverseMass;
	deltaVelocity._33 += inverseMass;

	// Invert to get the impulse needed per unit velocity
	SimpleMath::Matrix impulseMatrix = deltaVelocity.Invert();

	// Find the target velocities to kill
	SimpleMath::Vector3 velKill(desiredDeltaVelocity,
		-contactVelocity.y,
		-contactVelocity.z);

	// Find the impulse to kill target velocities
	impulseContact = velKill.Transform(velKill,impulseMatrix);

	// Check for exceeding friction
	float planarImpulse = sqrtf(
		impulseContact.y*impulseContact.y +
		impulseContact.z*impulseContact.z
	);
	if (planarImpulse > impulseContact.x * friction)
	{
		// We need to use dynamic friction
		impulseContact.y /= planarImpulse;
		impulseContact.z /= planarImpulse;

		impulseContact.x = deltaVelocity._11 +
			deltaVelocity._12 * friction*impulseContact.y +
			deltaVelocity._13 * friction*impulseContact.z;
		impulseContact.x = desiredDeltaVelocity / impulseContact.x;
		impulseContact.y *= friction * impulseContact.x;
		impulseContact.z *= friction * impulseContact.x;
	}
	return impulseContact;
}