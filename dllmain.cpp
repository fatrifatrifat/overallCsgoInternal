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
    DWORD glowObjectManager = 0x535FCB8;
    DWORD team = 0xF4;
    DWORD glowIndex = 0x10488;

}offsets;

struct
{
    DWORD gameModule;
    DWORD localPlayer;
    DWORD ent;
    DWORD glowObject;
    BYTE flag;
    int myTeam;
    int entityTeam;
    int glowIndex;
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

        val.glowObject = *(DWORD*)(val.gameModule + offsets.glowObjectManager);
        val.myTeam = *(int*)(val.localPlayer + offsets.team);

        for (short int i = 0; i <= 64; i++)
        {

            //radar
            val.ent = *(DWORD*)(val.gameModule + offsets.entList + i * 0x10);
            if (val.ent != 0)
            {
                *(bool*)(val.ent + offsets.spotted) = true;

                val.glowIndex = *(int*)(val.ent + offsets.glowIndex);
                val.entityTeam = *(int*)(val.ent + offsets.team);

                //glow
                if (val.myTeam == val.entityTeam)
                {
                    *(float*)(val.glowObject + (val.glowIndex * 0x38) + 0x8) = 0.f;
                    *(float*)(val.glowObject + (val.glowIndex * 0x38) + 0xC) = 0.f;
                    *(float*)(val.glowObject + (val.glowIndex * 0x38) + 0x10) = 2.f;
                    *(float*)(val.glowObject + (val.glowIndex * 0x38) + 0x14) = 1.7f;

                }
                else
                {
                    
                    *(float*)(val.glowObject + (val.glowIndex * 0x38) + 0x8) = 2.f;
                    *(float*)(val.glowObject + (val.glowIndex * 0x38) + 0xC) = 0.f;
                    *(float*)(val.glowObject + (val.glowIndex * 0x38) + 0x10) = 0.f;
                    *(float*)(val.glowObject + (val.glowIndex * 0x38) + 0x14) = 1.7f;

                }
                *(bool*)(val.glowObject + (val.glowIndex * 0x38) + 0x28) = true;
                *(bool*)(val.glowObject + (val.glowIndex * 0x38) + 0x29) = false;
            }

            if ((*(int*)(val.localPlayer + offsets.flashDuration)) != 0)
            {
                *(int*)(val.localPlayer + offsets.flashDuration) = 0;
            }

            //bHop
            if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0))
            {
                *(DWORD*)(val.gameModule + offsets.jump) = 6;

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

