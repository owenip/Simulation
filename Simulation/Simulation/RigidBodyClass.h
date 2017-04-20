#pragma once
class RigidBodyClass
{
public:
	RigidBodyClass();
	~RigidBodyClass();

	void	SetMass(const float &InMass);
	float	GetMass() const;

	void				SetInertia(const SimpleMath::Matrix &InMatrix);
	SimpleMath::Matrix	GetInertia() const;
	void				GetInertia(SimpleMath::Matrix &InMatrix) const;

	void				SetInertiaInverse(const SimpleMath::Matrix &InMatrix);
	SimpleMath::Matrix	GetInertiaInverse() const;
	void				GetInertiaInverse(SimpleMath::Matrix &InMatrix) const;

	void				SetPosition(const SimpleMath::Vector3 &InV);
	void				SetPosition(const float x, const float y, const float z);
	SimpleMath::Vector3	GetPosition() const;
	void				GetPosition(SimpleMath::Vector3 &InV) const;

	void				SetVelocity(const SimpleMath::Vector3 &InV);
	SimpleMath::Vector3	GetVelocity() const;
	void				GetVelocity(SimpleMath::Vector3 &InV) const;

	void				SetAcceleration(const SimpleMath::Vector3 &InV);
	SimpleMath::Vector3	GetAcceleration() const;
	void				GetAcceleration(SimpleMath::Vector3 &InV) const;
	
	float					fMass;						// Total mass (constant)
	SimpleMath::Matrix		mInertia;					// Mass moment of inertia in body coordinates
	SimpleMath::Matrix		mInertiaInverse;			// Inverse of mass moment of inertia matrix
	SimpleMath::Vector3		vPosition;					// Position in earth coordinates
	SimpleMath::Vector3		vVelocity;					// Velocity in earth coordinates
	SimpleMath::Vector3		vVelocityBody;				// Velocity in body coordinates
	SimpleMath::Vector3		vAcceleration;				// Acceleration of cg in earth space

	SimpleMath::Vector3		vAngularAcceleration;		// Angular acceleration in body coordinates
	SimpleMath::Vector3		vAngularAccelerationGlobal; // Angular acceleration in global coordinates
	SimpleMath::Vector3		vAngularVelocity;			// Angular velocity in body coordinates
	SimpleMath::Vector3		vAngularVelocityGlobal;		// Angular velocity in global coordinates
	SimpleMath::Vector3		vEulerAngles;				// Euler angles in body coordinates

	float					fSpeed;						// Speed (magnitude of the velocity)
	SimpleMath::Quaternion  qOrientation;				// Orientation in earth coordinates
	SimpleMath::Vector3		vForces;					// Total force on body
	SimpleMath::Vector3		vMoments;					// Total moment (torque) on body
	SimpleMath::Matrix		mIeInverse;					// Inverse of moment of inertia in earth coordinates	
};

