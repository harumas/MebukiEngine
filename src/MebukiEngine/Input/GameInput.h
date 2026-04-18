#pragma once
#include <memory>
#include "InputContext.h"
#include "Key.h"

class GameInput final
{
public:
	explicit GameInput() = delete;
	~GameInput() = delete;

	static void AttachContext(std::shared_ptr<InputContext> context);
	static bool GetKey(Key key);
	static bool GetKeyDown(Key key);
	static bool GetKeyUp(Key key);

	static bool GetMouseButton(MouseButton button);
	static bool GetMouseButtonDown(MouseButton button);
	static bool GetMouseButtonUp(MouseButton button);

private:
	static std::shared_ptr<InputContext> context;

	static inline bool FastGetKeyBit(std::bitset<256>* buffer, Key key)
	{
		int k = static_cast<int>(key);
		const uint64_t* rawArray = reinterpret_cast<const uint64_t*>(buffer);

		return (rawArray[k >> 6] >> (k & 63)) & 1;
	}

	static inline bool FastGetMouseBit(std::bitset<5>* buffer, MouseButton button)
	{
		int b = static_cast<int>(button);

		const unsigned long* raw = reinterpret_cast<const unsigned long*>(buffer);

		return (*raw >> b) & 1;
	}
};

