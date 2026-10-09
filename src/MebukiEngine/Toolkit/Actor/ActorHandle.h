#pragma once
#include <cstdint>
struct ActorHandle
{
	uint32_t index = UINT32_MAX;
	uint32_t generation = 0;

	bool IsValid() const
	{
		return index != UINT32_MAX;
	}

	bool operator==(const ActorHandle& h) const
	{
		return index == h.index && generation == h.generation;
	};
};
