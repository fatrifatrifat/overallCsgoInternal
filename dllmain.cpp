// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>

struct
{
    DWORD lPlayer = 0xDEF97C;
    DWORD jump = 0x52C0F50;
    DWORD flags = 0x104;
    DWORD entList = 0x4E051DC;
    DWORD spotted = 0x93D;
    DWORD flashDuration = 0x10470;

}offsets;

struct
{
    DWORD gameModule;
    DWORD localPlayer;
    DWORD ent;
    BYTE flag;
}val;

void main()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    val.gameModule = (DWORD)GetModuleHandle("client.dll");
    val.localPlayer = *(DWORD*)(val.gameModule + offsets.lPlayer);

    if (val.localPlayer == NULL)
        while (val.localPlayer == NULL)
            val.localPlayer = *(DWORD*)(val.gameModule + offsets.lPlayer);

    std::cout << std::hex << val.localPlayer << std::endl;

    while (true)
    {
        val.flag = *(BYTE*)(val.localPlayer + offsets.flags);

        for (short int i = 0; i < 64; i++)
        {

            if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0))
            {
                *(DWORD*)(val.gameModule + offsets.jump) = 6;

            }

            val.ent = *(DWORD*)(val.gameModule + offsets.entList + i * 0x10);
            if (val.ent != 0)
            {
                *(bool*)(val.ent + offsets.spotted) = true;
            }

            if ((*(int*)(val.localPlayer + offsets.flashDuration)) != 0)
            {
                *(int*)(val.localPlayer + offsets.flashDuration) = 0;
            }
        }

        
    }

}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
    }
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

