#ifndef EDITOR_H
#define EDITOR_H

#include <windows.h>

class Editor
{
public:
    static void Init(HWND hwnd);
    static void Begin();
    static void Draw(); 
    static void End();
    static void Shutdown();

    static bool IsInitialized();

private:
    static bool initialized;
};

#endif
