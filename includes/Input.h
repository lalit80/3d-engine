#ifndef INPUT_H
#define INPUT_H

class Input
{
public:
    static bool keys[256];

    static void KeyDown(unsigned int key);
    static void KeyUp(unsigned int key);

    static bool IsKeyPressed(unsigned int key);
};

#endif
