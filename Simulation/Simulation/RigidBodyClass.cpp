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
