#include "Component.h"

Component::Component(const std::shared_ptr<Actor>& actorRef)
	: gameObject(actorRef)
{
}

