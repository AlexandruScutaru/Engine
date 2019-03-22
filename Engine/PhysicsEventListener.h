#ifndef PHYSICS_EVENT_LISTENER_H
#define PHYSICS_EVENT_LISTENER_H

#include <rp3d/reactphysics3d.h>
#include <functional>

namespace physics{

	class PhysicsEventListener : public  rp3d::EventListener {
	public:
		PhysicsEventListener(std::function<void(const rp3d::ContactPointInfo&)>);
		~PhysicsEventListener();

		virtual void beginContact(const rp3d::ContactPointInfo& contact) override;
		virtual void newContact(const rp3d::ContactPointInfo& contact) override;

	private:
		std::function<void(const rp3d::ContactPointInfo&)> m_beginContactCallback;
	};

}

#endif // !PHYSICS_EVENT_LISTENER_H
