#pragma once
#include "RigidBodyClass.h"
#include "BallManagerClass.h"

#define		RADIANS(d)		(d/180.0*3.14159)
#define		DEGREES(r)		(r*180.0/3.14159)

#define		NOCOLLISION				0
#define		COLLISION				1
#define		PENETRATING				-1
#define		CONTACT					2

#define    GRAVITY                   -9.87f

#define    LINEARDRAGCOEFFICIENT     0.5f
#define    ANGULARDRAGCOEFFICIENT    0.05f

#define    FRICTIONFACTOR            0.5f

#define    COEFFICIENTOFRESTITUTION  0.8f
#define    COEFFICIENTOFRESTITUTIONGROUND  0.1f

#define    FRICTIONCOEFFICIENTBALLS     0.1f
#define    FRICTIONCOEFFICIENTGROUND    0.1f

#define    ROLLINGRESISTANCECOEFFICIENT 0.025f
#define		VELOCITYTOLERANCE		0.05f

typedef struct	Collision {
	BallClass*			body1;
	BallClass*			body2;
	SimpleMath::Vector3	vCollisionNormal; // outward from face of body2
	SimpleMath::Vector3	vCollisionPoint;  // in global coordinates
	SimpleMath::Vector3	vRelativeVelocity;
	SimpleMath::Vector3	vRelativeAcceleration;
	SimpleMath::Vector3	vCollisionTangent;
}	Collision;

class PhysicsClass
{
public:
	PhysicsClass();
	~PhysicsClass();
	
	void Initialize(shared_ptr<BallManagerClass> BallManager);
	void Shutdown();

	void StepSimulation(float dt);
	int CheckGroundPlaneContacts(BallClass* ball);
	void CalcObjectForces();

	void FlushCollisionData();

private:
	//Balls
	std::shared_ptr<BallManagerClass> mBallManager;
	
	//Collision
	std::vector<Collision> mCollisions;
	int NumCollisions = 0;


	// Variables
	double alpha = 0.0;
	double alpha_dot = 0.0;
	double alpha_dotdot = 0.0;
	double beta = RADIANS(120.0);
	double beta_dot = 0.0;
	double beta_dotdot = 0.0;

	double J = 1.15; // kg m^2
	double I = 0.08; // kg m^2
	double Mc = 0.4; // kg
	double R = 0.62; // m
	double L = 1.1; // m
	double S = 0.4*1.1*0.75; // kg m
	double g = 9.8; // m/s^2
	double gamma = RADIANS(135.0);
	double theta = gamma - alpha;
	double SA = 7.3*0.62*0.5; // kg m
	double Qalpha = 100; // N m
	double Qbeta = -10; // N m
	double a = 0.1*g; // m/s^2
	double time = 0; // s
	double Vc = 0;

	float rho = 1.225f; //Density of Air

	
};

