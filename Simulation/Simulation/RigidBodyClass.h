#pragma once

class RigidBodyClass
{
public:

	void CalculateDerivedData();

	//Intergrate the rigid body forward in time by the given amount.
	void Integrate(float duration);

	//Set mass of the rigid body
	void SetMass(const float mass);

	//Mass Getter
	float GetMass() const;

	//Inverse mass Setter
	//Could be zero for unmovable object
	void SetInverseMass(const float inverseMass);
	float GetInverseMass() const;

	bool HasFiniteMass() const;

	void SetInertiaTensor(const SimpleMath::Matrix &inertiaTensor);

	void GetInertiaTensor(SimpleMath::Matrix &inertiaTensor) const;

	SimpleMath::Matrix GetInertiaTensor() const;

	//Copies the current inertia tensor of the rigid body into the given matrix.
	void GetInertiaTensorWorld(SimpleMath::Matrix &inertiaTensor) const;

	//Gets a copy of the current inertia tensor of the rigid body.
	SimpleMath::Matrix GetInertiaTensorWorld() const;

	// Sets the inverse intertia tensor for the rigid body.
	void SetInverseInertiaTensor(const SimpleMath::Matrix &inverseInertiaTensor);

	//Copies the current inverse inertia tensor of the rigid body into the given matrix.
	void GetInverseInertiaTensor(SimpleMath::Matrix *inverseInertiaTensor) const;

	//Gets a copy of the current inverse inertia tensor of the rigid body.
	SimpleMath::Matrix getInverseInertiaTensor() const;

	//Copies the current inverse inertia tensor of the rigid body into the given matrix.
	void GetInverseInertiaTensorWorld(SimpleMath::Matrix *inverseInertiaTensor) const;

	//Gets a copy of the current inverse inertia tensor of the rigid body.
	SimpleMath::Matrix GetInverseInertiaTensorWorld() const;

	//Sets both linear and angular damping in one function call.
	void SetDamping(const float linearDamping, const float angularDamping);

	//Sets the linear damping for the rigid body.
	void SetLinearDamping(const float linearDamping);

	//Gets the current linear damping value.
	float GetLinearDamping() const;

	//Sets the angular damping for the rigid body.
	void SetAngularDamping(const float angularDamping);

	//Gets the current angular damping value.
	float GetAngularDamping() const;

	//Sets the position of the rigid body.
	void SetPosition(const SimpleMath::Vector3 &position);

	/**
	* Sets the position of the rigid body by component.
	*
	* @param x The x coordinate of the new position of the rigid
	* body.
	*
	* @param y The y coordinate of the new position of the rigid
	* body.
	*
	* @param z The z coordinate of the new position of the rigid
	* body.
	*/
	void SetPosition(const float x, const float y, const float z);

	//Fills the given vector with the position of the rigid body.
	void GetPosition(SimpleMath::Vector3 *position) const;

	//Gets the position of the rigid body.
	SimpleMath::Vector3 GetPosition() const;

	//Sets the orientation of the rigid body.
	void SetOrientation(const SimpleMath::Quaternion &orientation);

	/**
	* Sets the orientation of the rigid body by component.
	*
	* @param r The real component of the rigid body's orientation
	* quaternion.
	*
	* @param i The first complex component of the rigid body's
	* orientation quaternion.
	*
	* @param j The second complex component of the rigid body's
	* orientation quaternion.
	*
	* @param k The third complex component of the rigid body's
	* orientation quaternion.
	*
	* @note The given orientation does not need to be normalised,
	* and can be zero. This function automatically constructs a
	* valid rotation quaternion with (0,0,0,0) mapping to
	* (1,0,0,0).
	*/
	void SetOrientation(const float x, const float y,
		const float z, const float w);

	//Fills the given quaternion with the current value of the rigid body's orientation.
	void GetOrientation(SimpleMath::Quaternion *orientation) const;

	//Gets the orientation of the rigid body.
	SimpleMath::Quaternion GetOrientation() const;

	//Fills the given matrix with a transformation representing the rigid body's orientation.
	void GetOrientation(SimpleMath::Matrix & matrix) const;
	
	//Fills the given matrix with a transformation representing the rigid body's position and orientation.
	void GetTransform(SimpleMath::Matrix &transform) const;
	
	//Gets a transformation representing the rigid body's position and orientation.
	SimpleMath::Matrix GetTransform() const;

	//Converts the given point from world space into the body's local space.
	SimpleMath::Vector3 GetPointInLocalSpace(SimpleMath::Vector3 point) const;

	//Converts the given point from world space into the body's local space.
	SimpleMath::Vector3 GetPointInWorldSpace(const SimpleMath::Vector3 &point) const;

	//Converts the given direction from world space into the body's local space.
	SimpleMath::Vector3 GetDirectionInLocalSpace(const SimpleMath::Vector3 &direction) const;

	//Converts the given direction from world space into the body's local space.
	SimpleMath::Vector3 GetDirectionInWorldSpace(const SimpleMath::Vector3 &direction) const;

	//Sets the velocity of the rigid body.
	void SetVelocity(const SimpleMath::Vector3 &velocity);

	/*Sets the velocity of the rigid body by component.The
	* velocity is given in world space.*/
	void SetVelocity(const float x, const float y, const float z);

	//Fills the given vector with the velocity of the rigid body.
	void GetVelocity(SimpleMath::Vector3 *velocity) const;

	//Gets the velocity of the rigid body.
	SimpleMath::Vector3 GetVelocity() const;

	/**
	* Applies the given change in velocity.
	*/
	void AddVelocity(const SimpleMath::Vector3 &deltaVelocity);

	//Sets the rotation of the rigid body.
	void SetRotation(const SimpleMath::Vector3 &rotation);
	void SetRotation(const float x, const float y, const float z);

	//Fills the given vector with the rotation of the rigid body.
	void GetRotation(SimpleMath::Vector3 *rotation) const;

	//Gets the rotation of the rigid body.
	SimpleMath::Vector3 GetRotation() const;

	/**
	* Applies the given change in rotation.
	*/
	void AddRotation(const SimpleMath::Vector3 &deltaRotation);

	/**
	* Returns true if the body is awake and responding to
	* integration.
	*
	* @return The awake state of the body.
	*/
	bool GetAwake() const
	{
		return isAwake;
	}
	void SetAwake(const bool awake = true);
	/**
	* Returns true if the body is allowed to go to sleep at
	* any time.
	*/
	bool GetCanSleep() const
	{
		return canSleep;
	}
	void SetCanSleep(const bool canSleep = true);

	//Gets the current accumulated value for linear acceleration.
	void GetLastFrameAcceleration(SimpleMath::Vector3 *linearAcceleration) const;
	SimpleMath::Vector3 GetLastFrameAcceleration() const;

	//Clears the forces and torques in the accumulators
	void ClearAccumulators();

	/*Adds the given force to centre of mass of the rigid body.
	* The force is expressed in world - coordinates.*/
	void AddForce(const SimpleMath::Vector3 &force);

	/**
	* Adds the given force to the given point on the rigid body.
	* Both the force and the
	* application point are given in world space. Because the
	* force is not applied at the centre of mass, it may be split
	* into both a force and torque.
	*
	* @param force The force to apply.
	*
	* @param point The location at which to apply the force, in
	* world-coordinates.
	*/
	void AddForceAtPoint(const SimpleMath::Vector3 &force, const SimpleMath::Vector3 &point);
	
	/**
	* Adds the given force to the given point on the rigid body.
	* The direction of the force is given in world coordinates,
	* but the application point is given in body space. This is
	* useful for spring forces, or other forces fixed to the
	* body.
	*
	* @param force The force to apply.
	*
	* @param point The location at which to apply the force, in
	* body-coordinates.
	*/
	void AddForceAtBodyPoint(const SimpleMath::Vector3 &force, const SimpleMath::Vector3 &point);
	
	/**
	* Adds the given torque to the rigid body.
	* The force is expressed in world-coordinates.
	*
	* @param torque The torque to apply.
	*/
	void AddTorque(const SimpleMath::Vector3 &torque);
	
	//Sets the constant acceleration of the rigid body.	
	void SetAcceleration(const SimpleMath::Vector3 &acceleration);
	void SetAcceleration(const float x, const float y, const float z);
	//Gets the acceleration of the rigid body.
	void GetAcceleration(SimpleMath::Vector3 *acceleration) const;
	SimpleMath::Vector3 getAcceleration() const;

protected:
	float inverseMass;

	//Hold the inverse of the body's inertia tensor
	SimpleMath::Matrix inverseInertiaTensor;

	//Hold th amount of damping applied to linear motion
	float linearDamping;

	//Hold the amount of damping applied to angular motion
	float angularDamping;

	//Hold the linear position of the rigid body in world space
	SimpleMath::Vector3 position;

	//Hold the angular orientation of the rigid body in world space
	SimpleMath::Quaternion orientation;

	//Hold the linear velocity of the rigid body in world space
	SimpleMath::Vector3 velocity;

	//Hold the angular velocity, or rotation , or the rigid body in world space
	SimpleMath::Vector3 rotation;

	//Hold the inverse tensor of the body in world space
	//The inverse inertia tensor member is specified in the body's local space.
	SimpleMath::Matrix inverseInertiaTensorWorld;

	//Holds the amount of motion of the body
	float motion;

	/**
	* A body can be put to sleep to avoid it being updated
	* by the integration functions or affected by collisions
	* with the world.
	*/
	bool isAwake;

	/**
	* Some bodies may never be allowed to fall asleep.
	* User controlled bodies, for example, should be
	* always awake.
	*/
	bool canSleep;

	//Hold a transform matrix for converting body space into world space and vice versa.
	SimpleMath::Matrix transformMatrix;

	/**
	* Holds the accumulated force to be applied at the next
	* integration step.
	*/
	SimpleMath::Vector3 forceAccum;

	/**
	* Holds the accumulated torque to be applied at the next
	* integration step.
	*/
	SimpleMath::Vector3 torqueAccum;

	/**
	* Holds the acceleration of the rigid body.  This value
	* can be used to set acceleration due to gravity (its primary
	* use), or any other constant acceleration.
	*/
	SimpleMath::Vector3 acceleration;

	/**
	* Holds the linear acceleration of the rigid body, for the
	* previous frame.
	*/
	SimpleMath::Vector3 lastFrameAcceleration;

};

