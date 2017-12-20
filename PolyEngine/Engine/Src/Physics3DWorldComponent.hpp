#pragma once

class btDiscreteDynamicsWorld;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;

namespace Poly
{
	class Rigidbody3DWorldComponent
	{
	public:
		Rigidbody3DWorldComponent();

	private:
		btDiscreteDynamicsWorld* DynamicsWorld;

		btDefaultCollisionConfiguration* CollisionConfiguration;
		btCollisionDispatcher* Dispatcher;
		btBroadphaseInterface* Broadphase;
		btSequentialImpulseConstraintSolver* Solver;
	};
}