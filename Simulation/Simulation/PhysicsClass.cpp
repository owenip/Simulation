#include "pch.h"
#include "PhysicsClass.h"
#include "BallClass.h"


PhysicsClass::PhysicsClass()
{
}


PhysicsClass::~PhysicsClass()
{

}

void PhysicsClass::Initialize(shared_ptr<BallManagerClass> BallManager)
{
	mBallManager = BallManager;
}

void PhysicsClass::Shutdown()
{
	mBallManager.reset();
}


void PhysicsClass::StepSimulation(float Indt)
{
	std::vector<BallClass*> mBallIndex;

	SimpleMath::Vector3     Ae;
	int        i;
	float      dt = Indt;
	int        check = NOCOLLISION;
	int        c = 0;

	// Calculate all of the forces and moments on the balls:
	CalcObjectForces();
}

void PhysicsClass::CalcObjectForces()
{
	std::vector<BallClass*> BallIndex;
	mBallManager->GetBallIndex(BallIndex);

	SimpleMath::Vector3    Fb, Mb;
	 SimpleMath::Vector3      vDragVector;
	 SimpleMath::Vector3      vAngularDragVector;
	int       i, j;
	SimpleMath::Vector3      ContactForce;
	SimpleMath::Vector3      pt;
	int       check = NOCOLLISION;
	pCollision    pCollisionData;
	SimpleMath::Vector3      FrictionForce;
	SimpleMath::Vector3      fDir;
	double    speed;
	SimpleMath::Vector3      FRn, FRt;

	for each (BallClass *Ball in BallIndex)
	{
		Ball->vForces = SimpleMath::Vector3::Zero;
		Ball->vMoments = SimpleMath::Vector3::Zero;

		Fb = SimpleMath::Vector3::Zero;
		Mb = SimpleMath::Vector3::Zero;

		// Do drag force:
		vDragVector = -Ball->vVelocityBody;
		vDragVector.Normalize();
		speed = Ball->vVelocityBody.Length();
		Fb += vDragVector * ((1.0f / 2.0f)*speed * speed * rho *
			LINEARDRAGCOEFFICIENT * pow(Ball->GetRadius(), 2) *
			Ball->GetRadius()*XM_PI);

		vAngularDragVector = -Ball->vAngularVelocity;
		vAngularDragVector.Normalize();
		Mb += vAngularDragVector * (Ball->vAngularVelocity.Length() *
			Ball->vAngularVelocity.Length() * rho * ANGULARDRAGCOEFFICIENT
			* 4 * pow(Ball->GetRadius(), 2)*XM_PI);

		// Convert forces from model space to earth space:
		SimpleMath::Quaternion temp = SimpleMath::Quaternion::Identity;
		Ball->qOrientation.Conjugate(temp);
		Ball->vForces = (Ball->qOrientation * Fb);
		Ball->vForces = Ball->vForces *  temp;

		// Apply gravity:
		Ball->vForces.z += GRAVITY * Ball->fMass;

		// Save the moments:
		Ball->vMoments += Mb;

		// Handle contacts with ground plane:
		Ball->vAcceleration = Ball->vForces / Ball->fMass;
		Ball->vAngularAcceleration = 
			(Ball->vMoments -
			(Ball->vAngularVelocity.Cross
				( Ball->vAngularVelocity.Transform(Ball->vAngularVelocity, Ball->mInertia))));
		Ball->vAngularAcceleration.Transform(Ball->vAngularAcceleration, Ball->mInertiaInverse);

		// Resolve ground plane contacts:
		FlushCollisionData();		
		NumCollisions = 0;
		check = CheckGroundPlaneContacts(Ball);

	}
	
}


int PhysicsClass::CheckGroundPlaneContacts(BallClass * Ball)
{
	SimpleMath::Vector3    v1[8];
	SimpleMath::Vector3     tmp;
	SimpleMath::Vector3     u, v;
	SimpleMath::Vector3     f[4];
	SimpleMath::Vector3     vel1;
	SimpleMath::Vector3     pt1;
	SimpleMath::Vector3     Vr;
	float     Vrn;
	SimpleMath::Vector3     n;
	int       status = NOCOLLISION;
	SimpleMath::Vector3     Ar;
	float     Arn;

	if (Ball->vPosition.z <= (Ball->GetRadius() + COEFFICIENTOFRESTITUTIONGROUND))
	{
		pt1 = Ball->vPosition;
		pt1.z = COEFFICIENTOFRESTITUTIONGROUND;
		tmp = pt1;
		pt1 = pt1 - Ball->vPosition;
		vel1 = Ball->vVelocity/*Body*/ +
			(Ball->vAngularVelocityGlobal.Cross(pt1));

		n = SimpleMath::Vector3::Zero;

		Vr = vel1;
		Vrn = Vr.Dot(n);

		if (fabs(Vrn) <= VELOCITYTOLERANCE) // at rest
		{
			// Check the relative acceleration:
			Ar = Ball->vAcceleration +
				(Ball->vAngularVelocityGlobal.Cross(
					Ball->vAngularVelocityGlobal.Cross(pt1))) +
					(Ball->vAngularAccelerationGlobal.Cross(pt1));

			Arn = Ar.Dot(n);
			if (Arn <= 0.0f)
			{
				// We have a contact so fill the data structure
				assert(NumCollisions < mBallManager->GetBallIndex().size() * 8);
				if (NumCollisions < mBallManager->GetBallIndex().size() * 8)
				{
					Collision CollisionData;
					CollisionData.body1 = Ball;
					CollisionData.body2 = nullptr;
					CollisionData.vCollisionNormal = n;
					CollisionData.vCollisionPoint = tmp;
					CollisionData.vRelativeVelocity = Vr;
					CollisionData.vRelativeAcceleration = Ar;

					CollisionData.vCollisionTangent = (n.Cross(Vr)).Cross(n);
					CollisionData.vCollisionTangent = CollisionData.vCollisionTangent * -1;

					CollisionData.vCollisionTangent.Normalize();
					mCollisions.push_back(CollisionData);
					NumCollisions++;
					status = CONTACT;
				}
			}
		}
				
	}
	return status;
}

void PhysicsClass::FlushCollisionData()
{	
	mCollisions.clear();
}
