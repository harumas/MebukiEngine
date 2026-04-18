#include "GameInput.h"

std::shared_ptr<InputContext> GameInput::context = nullptr;

void GameInput::AttachContext(std::shared_ptr<InputContext> context)
{
	GameInput::context = context;
}

bool GameInput::GetKey(Key key)
{
	return FastGetKeyBit(GameInput::context->keysCurrentBuffer, key);
}

bool GameInput::GetKeyDown(Key key)
{
	return FastGetKeyBit(GameInput::context->keysCurrentBuffer, key) && !FastGetKeyBit(GameInput::context->keysPreviousBuffer, key);
}

bool GameInput::GetKeyUp(Key key)
{
	return !FastGetKeyBit(GameInput::context->keysCurrentBuffer, key) && FastGetKeyBit(GameInput::context->keysPreviousBuffer, key);
}

// --- マウス ---
bool GameInput::GetMouseButton(MouseButton button)
{
	return FastGetMouseBit(GameInput::context->mouseCurrentBuffer, button);
}
bool GameInput::GetMouseButtonDown(MouseButton button)
{
	return FastGetMouseBit(GameInput::context->mouseCurrentBuffer, button) && !FastGetMouseBit(GameInput::context->mousePreviousBuffer, button);
}
bool GameInput::GetMouseButtonUp(MouseButton button)
{
	return !FastGetMouseBit(GameInput::context->mouseCurrentBuffer, button) && FastGetMouseBit(GameInput::context->mousePreviousBuffer, button);
}