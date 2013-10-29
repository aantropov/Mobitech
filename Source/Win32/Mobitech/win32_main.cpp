#include "stdafx.h"
#include "..\..\Core\Mobitech.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    Engine engine;
    engine.Initialize();
    engine.Run();
    engine.Stop();

    return 0;
}