// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "csgo.hpp"

struct
{
    uintptr_t lPlayer = hazedumper::signatures::dwLocalPlayer;
    uintptr_t jump = hazedumper::signatures::dwForceJump;
    uintptr_t flags = hazedumper::netvars::m_fFlags;
    uintptr_t entList = hazedumper::signatures::dwEntityList;
    uintptr_t spotted = hazedumper::netvars::m_bSpotted;
    uintptr_t flashDuration = hazedumper::netvars::m_flFlashDuration;
    uintptr_t glowObjectManager = hazedumper::signatures::dwGlowObjectManager;
    uintptr_t team = hazedumper::netvars::m_iTeamNum;
    uintptr_t glowIndex = hazedumper::netvars::m_iGlowIndex;
    uintptr_t health = hazedumper::netvars::m_iHealth;
    

}offsets;

struct
{
    uintptr_t gameModule;
    uintptr_t localPlayer;
    uintptr_t ent;
    uintptr_t glowObject;
    BYTE flag;
    int myTeam;
    int entityTeam;
    int glowIndex;
}val;

struct glowStruct
{
    BYTE base[8];//4 to 8
    float red;//4 to 8
    float green;//8 to C
    float blue;//C to 10
    float alpha;//10 to 14
    BYTE buffer[28];//10 + 16 to 14 + x
    bool renderWhenOccluded;//24 to 28
    bool renderWhenUnOccluded;//25 to 29
    bool fullBloon;//26 to 30
    BYTE buffer1[5];
    int glowStyle;
};

glowStruct setGlowColor(int health)
{
    glowStruct tst;
    tst.red = 1.0f;

    return tst;
}

void setTeamGlow(uintptr_t ent, int glowIndex)
{
    glowStruct tGlow;
    tGlow = *(glowStruct*)(val.glowObject + (val.glowIndex * 0x38));

    tGlow.blue = 1.0f;
    tGlow.alpha = 1.0f;
    tGlow.renderWhenOccluded = true;
    tGlow.renderWhenUnOccluded = false;
    *(glowStruct*)(val.glowObject + (val.glowIndex * 0x38))=tGlow;
}

void setEnemyGlow(uintptr_t ent, int glowIndex)
{

}

void handleGlow()
{

    val.glowObject = *(uintptr_t*)(val.gameModule + offsets.glowObjectManager);
    val.myTeam = *(int*)(val.localPlayer + offsets.team);

    for (short int i = 0; i <= 64; i++)
    {

        //radar
        val.ent = *(uintptr_t*)(val.gameModule + offsets.entList + i * 0x10);
        if (val.ent != 0)
        {
            

            val.glowIndex = *(int*)(val.ent + offsets.glowIndex);
            val.entityTeam = *(int*)(val.ent + offsets.team);

            //glow
            if (val.myTeam == val.entityTeam)
            {
                setTeamGlow(val.ent, val.glowIndex);
            }
            else
            {
                setEnemyGlow(val.ent, val.glowIndex);
            }
            
        }

       

       
    }

}

void main()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    val.gameModule = (uintptr_t)GetModuleHandle("client.dll");
    val.localPlayer = *(uintptr_t*)(val.gameModule + offsets.lPlayer);

    if (val.localPlayer == NULL)
        while (val.localPlayer == NULL)
            val.localPlayer = *(uintptr_t*)(val.gameModule + offsets.lPlayer);

    std::cout << std::hex << val.localPlayer << std::endl;

    while (true)
    {
        val.flag = *(BYTE*)(val.localPlayer + offsets.flags);

        val.glowObject = *(uintptr_t*)(val.gameModule + offsets.glowObjectManager);
        val.myTeam = *(int*)(val.localPlayer + offsets.team);

        for (short int i = 0; i <= 64; i++)
        {

            //radar
            val.ent = *(uintptr_t*)(val.gameModule + offsets.entList + i * 0x10);
            if (val.ent != 0)
            {
                *(bool*)(val.ent + offsets.spotted) = true;

                handleGlow();
            }

            if ((*(int*)(val.localPlayer + offsets.flashDuration)) != 0)
            {
                *(int*)(val.localPlayer + offsets.flashDuration) = 0;
            }

            //bHop
            if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0))
            {
                *(uintptr_t*)(val.gameModule + offsets.jump) = 6;

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

