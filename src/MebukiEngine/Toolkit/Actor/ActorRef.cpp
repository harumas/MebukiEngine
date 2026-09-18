#include "pch.h"
#include "ActorRef.h"
#include "ActorService.h"

Actor* ActorRef::operator->()
{
	return &service->Get(handle);
}

Actor& ActorRef::operator*()
{
	return service->Get(handle);
}

ActorRef::operator bool() const
{
	return service->IsAlive(handle);
}
