#include "pch.h"
#include "RigidBodyClass.h"

static inline void _calculateTransformMatrix(SimpleMath::Matrix &transformMatrix,
	const SimpleMath::Vector3 &position,
	const SimpleMath::Quaternion &orientation)
{
	transformMatrix._11 = 1 - 2 * orientation.z*orientation.z -
		2 * orientation.w*orientation.w;
	transformMatrix._12 = 2 * orientation.y*orientation.z -
		2 * orientation.x*orientation.w;
	transformMatrix._13 = 2 * orientation.y*orientation.w +
		2 * orientation.x*orientation.z;
	transformMatrix._14 = position.x;

	transformMatrix._21 = 2 * orientation.y*orientation.z +
		2 * orientation.x*orientation.w;
	transformMatrix._22 = 1 - 2 * orientation.y*orientation.y -
		2 * orientation.w*orientation.w;
	transformMatrix._23 = 2 * orientation.z*orientation.w -
		2 * orientation.x*orientation.y;
	transformMatrix._24 = position.y;

	transformMatrix._31 = 2 * orientation.y*orientation.w -
		2 * orientation.x*orientation.z;
	transformMatrix._32 = 2 * orientation.z*orientation.w +
		2 * orientation.x*orientation.y;
	transformMatrix._33 = 1 - 2 * orientation.y*orientation.y -
		2 * orientation.z*orientation.z;
	transformMatrix._34 = position.z;
}


/**
* Internal function to do an intertia tensor transform by a quaternion.
* Note that the implementation of this function was created by an
* automated code-generator and optimizer.
*/
static inline void _transformInertiaTensor(SimpleMath::Matrix &iitWorld,
	const SimpleMath::Quaternion &q,
	const SimpleMath::Matrix &iitBody,
	const SimpleMath::Matrix &rotmat)
{
	float t4 = rotmat._11 * iitBody._11 +
		rotmat._12 * iitBody._21 +
		rotmat._13 * iitBody._31;
	float t9 = rotmat._11 * iitBody._12 +
		rotmat._12 * iitBody._22 +
		rotmat._13 * iitBody._32;
	float t14 = rotmat._11 * iitBody._13 +
		rotmat._12 * iitBody._23 +
		rotmat._13 * iitBody._33;
	float t28 = rotmat._21 * iitBody._11 +
		rotmat._22 * iitBody._21 +
		rotmat._23 * iitBody._31;
	float t33 = rotmat._21 * iitBody._12 +
		rotmat._22 * iitBody._22 +
		rotmat._23 * iitBody._32;
	float t38 = rotmat._21 * iitBody._13 +
		rotmat._22 * iitBody._23 +
		rotmat._23 * iitBody._33;
	float t52 = rotmat._31 * iitBody._11 +
		rotmat._32 * iitBody._21 +
		rotmat._33 * iitBody._31;
	float t57 = rotmat._31 * iitBody._12 +
		rotmat._32 * iitBody._22 +
		rotmat._33 * iitBody._32;
	float t62 = rotmat._31 * iitBody._13 +
		rotmat._32 * iitBody._23 +
		rotmat._33 * iitBody._33;

	iitWorld._11 = t4*rotmat._11 +
		t9*rotmat._12 +
		t14*rotmat._13;
	iitWorld._12 = t4*rotmat._21 +
		t9*rotmat._22 +
		t14*rotmat._23;
	iitWorld._13 = t4*rotmat._31 +
		t9*rotmat._32 +
		t14*rotmat._33;
	iitWorld._21 = t28*rotmat._11 +
		t33*rotmat._12 +
		t38*rotmat._13;
	iitWorld._22 = t28*rotmat._21 +
		t33*rotmat._22 +
		t38*rotmat._23;
	iitWorld._23 = t28*rotmat._31 +
		t33*rotmat._32 +
		t38*rotmat._33;
	iitWorld._31 = t52*rotmat._11 +
		t57*rotmat._12 +
		t62*rotmat._13;
	iitWorld._32 = t52*rotmat._21 +
		t57*rotmat._22 +
		t62*rotmat._23;
	iitWorld._33 = t52*rotmat._31 +
		t57*rotmat._32 +
		t62*rotmat._33;
}

/*
* Definition of the sleep epsilon extern.
*/
static float sleepEpsilon = 0.03f;

static inline void setSleepEpsilon(float value)
{
	sleepEpsilon = value;
}

void RigidBodyClass::CalculateDerivedData()
{	
	// Calculate the transform matrix for the body.
	_calculateTransformMatrix(transformMatrix, position, orientation);
	/*orientation.Normalize();
	SimpleMath::Vector3 RotatedPos = position;
	RotatedPos.Transform(RotatedPos , orientation);
	SimpleMath::Quaternion tempQ(RotatedPos,1);
	transformMatrix = transformMatrix.CreateFromQuaternion(tempQ);*/

	// Calculate the inertiaTensor in world space.
	_transformInertiaTensor(inverseInertiaTensorWorld,
		orientation,
		inverseInertiaTensor,
		transformMatrix);
}

void RigidBodyClass::Integrate(float duration)
{
	if (!isAwake) return;

	// Calculate linear acceleration from force inputs.
    lastFrameAcceleration = acceleration;
    lastFrameAcceleration += forceAccum * inverseMass;
	
    // Calculate angular acceleration from torque inputs.	
	SimpleMath::Vector3 angularAcceleration = torqueAccum.Transform(torqueAccum,inverseInertiaTensorWorld);

    // Adjust velocities
    // Update linear velocity from both acceleration and impulse.
    velocity += lastFrameAcceleration * duration;

    // Update angular velocity from both acceleration and impulse.
    rotation += angularAcceleration *  duration;

    // Impose drag.
    velocity *= pow(linearDamping, duration);
    rotation *= pow(angularDamping, duration);

    // Adjust positions
    // Update linear position.
    position += velocity * duration;

    // Update angular position.
    orientation = orientation + (rotation * duration);

    // Normalise the orientation, and update the matrices with the new
    // position and orientation
    CalculateDerivedData();

    // Clear accumulators.
    ClearAccumulators();

    // Update the kinetic energy store, and possibly put the body to
    // sleep.
    if (canSleep) {
        float currentMotion = velocity.Dot(velocity) +
            rotation.Dot(rotation);

		float bias = pow(0.5, duration);
        motion = bias*motion + (1-bias)*currentMotion;

        if (motion < sleepEpsilon) 
			SetAwake(false);
        else if (motion > 10 * sleepEpsilon) 
			motion = 10 * sleepEpsilon;
    }
}

void RigidBodyClass::SetMass(const float mass)
{
	assert(mass != 0);
	RigidBodyClass::inverseMass = 1.0f / mass;
}

float RigidBodyClass::GetMass() const
{
	if (inverseMass == 0) {
		return FLT_MAX;
	}
	else {
		return 1.0f / inverseMass;
	}
}

void RigidBodyClass::SetInverseMass(const float inverseMass)
{
	RigidBodyClass::inverseMass = inverseMass;
}

float RigidBodyClass::GetInverseMass() const
{
	return inverseMass;
}

bool RigidBodyClass::HasFiniteMass() const
{
	return inverseMass >= 0.0f;
}

void RigidBodyClass::SetInertiaTensor(const SimpleMath::Matrix & inertiaTensor)
{
	inertiaTensor.Invert(inverseInertiaTensor);
}

void RigidBodyClass::GetInertiaTensor(SimpleMath::Matrix &inertiaTensor) const
{
	inverseInertiaTensor.Invert(inertiaTensor);
}

SimpleMath::Matrix RigidBodyClass::GetInertiaTensor() const
{
	SimpleMath::Matrix it;
	GetInertiaTensor(it);
	return it;
}

void RigidBodyClass::GetInertiaTensorWorld(SimpleMath::Matrix & inertiaTensor) const
{
	inverseInertiaTensorWorld.Invert(inertiaTensor);
}

SimpleMath::Matrix RigidBodyClass::GetInertiaTensorWorld() const
{
	SimpleMath::Matrix it;
	GetInertiaTensorWorld(it);
	return it;
}

void RigidBodyClass::SetInverseInertiaTensor(const SimpleMath::Matrix & inverseInertiaTensor)
{
	RigidBodyClass::inverseInertiaTensor = inverseInertiaTensor;
}

void RigidBodyClass::GetInverseInertiaTensor(SimpleMath::Matrix * inverseInertiaTensor) const
{
	*inverseInertiaTensor = RigidBodyClass::inverseInertiaTensor;
}

SimpleMath::Matrix RigidBodyClass::getInverseInertiaTensor() const
{
	return inverseInertiaTensor;
}

void RigidBodyClass::GetInverseInertiaTensorWorld(SimpleMath::Matrix * inverseInertiaTensor) const
{
	*inverseInertiaTensor = inverseInertiaTensorWorld;
}

SimpleMath::Matrix RigidBodyClass::GetInverseInertiaTensorWorld() const
{
	return inverseInertiaTensorWorld;
}

void RigidBodyClass::SetDamping(const float linearDamping, const float angularDamping)
{
	RigidBodyClass::linearDamping = linearDamping;
	RigidBodyClass::angularDamping = angularDamping;
}

void RigidBodyClass::SetLinearDamping(const float linearDamping)
{
	RigidBodyClass::linearDamping = linearDamping;
}

float RigidBodyClass::GetLinearDamping() const
{
	return linearDamping;
}

void RigidBodyClass::SetAngularDamping(const float angularDamping)
{
	RigidBodyClass::angularDamping = angularDamping;
}

float RigidBodyClass::GetAngularDamping() const
{
	return angularDamping;
}

void RigidBodyClass::SetPosition(const SimpleMath::Vector3 & position)
{
	RigidBodyClass::position = position;
}

void RigidBodyClass::SetPosition(const float x, const float y, const float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

void RigidBodyClass::GetPosition(SimpleMath::Vector3 * position) const
{
	*position = RigidBodyClass::position;
}

SimpleMath::Vector3 RigidBodyClass::GetPosition() const
{
	return position;
}

void RigidBodyClass::SetOrientation(const SimpleMath::Quaternion & orientation)
{
	RigidBodyClass::orientation = orientation;
	RigidBodyClass::orientation.Normalize();
}

void RigidBodyClass::SetOrientation(const float x, const float y, const float z, const float w)
{
	orientation.x = x;
	orientation.y = y;
	orientation.z = z;
	orientation.w = w;
	orientation.Normalize();
}

void RigidBodyClass::GetOrientation(SimpleMath::Quaternion * orientation) const
{
	*orientation = RigidBodyClass::orientation;
}

SimpleMath::Quaternion RigidBodyClass::GetOrientation() const
{
	return orientation;
}

void RigidBodyClass::GetOrientation(SimpleMath::Matrix &matrix) const
{
	matrix = RigidBodyClass::transformMatrix;
}

void RigidBodyClass::GetTransform(SimpleMath::Matrix & transform) const
{
	transform = RigidBodyClass::transformMatrix;
}

SimpleMath::Matrix RigidBodyClass::GetTransform() const
{
	return transformMatrix;
}

SimpleMath::Vector3 RigidBodyClass::GetPointInLocalSpace(SimpleMath::Vector3 point) const
{
	point = SimpleMath::Vector3::Transform(point, transformMatrix.Invert());
	return point;
}

SimpleMath::Vector3 RigidBodyClass::GetPointInWorldSpace(const SimpleMath::Vector3 & point) const
{
	return SimpleMath::Vector3::Transform(point,transformMatrix);
}

SimpleMath::Vector3 RigidBodyClass::GetDirectionInLocalSpace(const SimpleMath::Vector3 & direction) const
{
	return SimpleMath::Vector3::Transform(direction, transformMatrix.Invert());
}

SimpleMath::Vector3 RigidBodyClass::GetDirectionInWorldSpace(const SimpleMath::Vector3 & direction) const
{
	return SimpleMath::Vector3::Transform(direction,transformMatrix);
}

void RigidBodyClass::SetVelocity(const SimpleMath::Vector3 & velocity)
{
	RigidBodyClass::velocity = velocity;
}

void RigidBodyClass::SetVelocity(const float x, const float y, const float z)
{
	velocity.x = x;
	velocity.y = y;
	velocity.z = z;
}

void RigidBodyClass::GetVelocity(SimpleMath::Vector3 * velocity) const
{
	*velocity = RigidBodyClass::velocity;
}

SimpleMath::Vector3 RigidBodyClass::GetVelocity() const
{
	return velocity;
}

void RigidBodyClass::AddVelocity(const SimpleMath::Vector3 & deltaVelocity)
{
	velocity += deltaVelocity;
}

void RigidBodyClass::SetRotation(const SimpleMath::Vector3 & rotation)
{
	RigidBodyClass::rotation = rotation;
}

void RigidBodyClass::SetRotation(const float x, const float y, const float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;
}

void RigidBodyClass::GetRotation(SimpleMath::Vector3 * rotation) const
{
	*rotation = RigidBodyClass::rotation;
}

SimpleMath::Vector3 RigidBodyClass::GetRotation() const
{
	return rotation;
}

void RigidBodyClass::AddRotation(const SimpleMath::Vector3 & deltaRotation)
{
	rotation += deltaRotation;
}

void RigidBodyClass::SetAwake(const bool awake)
{
	if (awake) {
		isAwake = true;

		// Add a bit of motion to avoid it falling asleep immediately.
		motion = sleepEpsilon*2.0f;
	}
	else {
		isAwake = false;
		velocity = SimpleMath::Vector3::Zero;
		rotation = SimpleMath::Vector3::Zero;
	}
}

void RigidBodyClass::SetCanSleep(const bool canSleep)
{
	RigidBodyClass::canSleep = canSleep;

	if (!canSleep && !isAwake) 
		SetAwake();
}

void RigidBodyClass::GetLastFrameAcceleration(SimpleMath::Vector3 * linearAcceleration) const
{
	*linearAcceleration = lastFrameAcceleration;
}

SimpleMath::Vector3 RigidBodyClass::GetLastFrameAcceleration() const
{
	return lastFrameAcceleration;
}

void RigidBodyClass::ClearAccumulators()
{
	forceAccum = SimpleMath::Vector3::Zero;
	torqueAccum = SimpleMath::Vector3::Zero;
}

void RigidBodyClass::AddForce(const SimpleMath::Vector3 & force)
{
	forceAccum += force;
	isAwake = true;
}

void RigidBodyClass::AddForceAtPoint(const SimpleMath::Vector3 & force, const SimpleMath::Vector3 & point)
{
	// Convert to coordinates relative to center of mass.
	SimpleMath::Vector3 pt = point;
	pt -= position;

	forceAccum += force;
	torqueAccum += pt.Cross(force);

	isAwake = true;
}

void RigidBodyClass::AddForceAtBodyPoint(const SimpleMath::Vector3 & force, const SimpleMath::Vector3 & point)
{
	// Convert to coordinates relative to center of mass.
	SimpleMath::Vector3 pt = GetPointInWorldSpace(point);
	AddForceAtPoint(force, pt);
}

void RigidBodyClass::AddTorque(const SimpleMath::Vector3 & torque)
{
	torqueAccum += torque;
	isAwake = true;
}

void RigidBodyClass::SetAcceleration(const SimpleMath::Vector3 & acceleration)
{
	RigidBodyClass::acceleration = acceleration;
}

void RigidBodyClass::SetAcceleration(const float x, const float y, const float z)
{
	acceleration.x = x;
	acceleration.y = y;
	acceleration.z = z;
}

void RigidBodyClass::GetAcceleration(SimpleMath::Vector3 * acceleration) const
{
	*acceleration = RigidBodyClass::acceleration;
}

SimpleMath::Vector3 RigidBodyClass::getAcceleration() const
{
	return acceleration;
}




