#pragma once

#include  "RigidBodyClass.h"
#include  "ParticleForceGenerator.h"

	class ForceGenerator
	{
		public:

			/**
			* Overload this in implementations of the interface to calculate
			* and update the force applied to the given rigid body.
			*/
			virtual void UpdateForce(RigidBodyClass *body, float duration) = 0;
	};		
	/**
	* A force generator that applies a gravitational force. One instance
	* can be used for multiple rigid bodies.
	*/
	class Gravity : public ForceGenerator
	{
		/** Holds the acceleration due to gravity. */
		Vector3 gravity;

	public:

		/** Creates the generator with the given acceleration. */
		Gravity(const Vector3 &gravity);

		/** Applies the gravitational force to the given rigid body. */
		virtual void UpdateForce(RigidBodyClass *body, float duration);
	};

	/**
	* A force generator that applies a Spring force.
	*/
	class Spring : public ForceGenerator
	{
		/**
		* The point of connection of the spring, in local
		* coordinates.
		*/
		Vector3 connectionPoint;

		/**
		* The point of connection of the spring to the other object,
		* in that object's local coordinates.
		*/
		Vector3 otherConnectionPoint;

		/** The particle at the other end of the spring. */
		RigidBodyClass *other;

		/** Holds the sprint constant. */
		float springConstant;

		/** Holds the rest length of the spring. */
		float restLength;

	public:

		/** Creates a new spring with the given parameters. */
		Spring(const Vector3 &localConnectionPt,
			RigidBodyClass *other,
			const Vector3 &otherConnectionPt,
			float springConstant,
			float restLength);

		/** Applies the spring force to the given rigid body. */
		virtual void UpdateForce(RigidBodyClass *body, float duration);
	};

	/**
	* Holds all the force generators and the bodies they apply to.
	*/
	class ForceRegistry
	{
	protected:

		/**
		* Keeps track of one force generator and the body it
		* applies to.
		*/
		struct ForceRegistration
		{
			RigidBodyClass *body;
			ForceGenerator *fg;
		};

		/**
		* Holds the list of registrations.
		*/
		typedef std::vector<ForceRegistration> Registry;
		Registry registrations;

	public:
		/**
		* Registers the given force generator to apply to the
		* given body.
		*/
		void Add(RigidBodyClass* body, ForceGenerator *fg);

		/**
		* Removes the given registered pair from the registry.
		* If the pair is not registered, this method will have
		* no effect.
		*/
		void Remove(RigidBodyClass* body, ForceGenerator *fg);

		/**
		* Clears all registrations from the registry. This will
		* not delete the bodies or the force generators
		* themselves, just the records of their connection.
		*/
		void Clear();

		/**
		* Calls all the force generators to update the forces of
		* their corresponding bodies.
		*/
		void UpdateForces(float duration);
	};



