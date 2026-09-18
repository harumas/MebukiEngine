#pragma once
#include <Toolkit/Actor/ActorHandle.h>

class Actor;
class ActorService;

class ActorRef
{
public:
	ActorRef() : handle{}, service(nullptr) {}

	ActorRef(ActorHandle handle, ActorService* service)
		: handle(handle), service(service)
	{}

	Actor* operator->();
	Actor& operator*();
	explicit operator bool() const;

	ActorHandle GetHandle() const { return handle; }

private:
	ActorHandle handle;
	ActorService* service;
};
