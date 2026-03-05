#include "Input.h"

bool Input::keys[256] = { false };

void Input::KeyDown(unsigned int key)
{
    keys[key] = true;
}

void Input::KeyUp(unsigned int key)
{
    keys[key] = false;
}

bool Input::IsKeyPressed(unsigned int key)
{
    return keys[key];
}
