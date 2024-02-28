// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include "csgo.hpp"

using namespace hazedumper::signatures;
using namespace hazedumper::netvars;

struct
{
    uintptr_t lPlayer = dwLocalPlayer;
    uintptr_t jump = dwForceJump;
    uintptr_t flags = m_fFlags;
    uintptr_t entList = dwEntityList;
    uintptr_t spotted = m_bSpotted;
    uintptr_t flashDuration = m_flFlashDuration;
    uintptr_t glowObjectManager = dwGlowObjectManager;
    uintptr_t team = m_iTeamNum;
    uintptr_t glowIndex = m_iGlowIndex;
    uintptr_t health = m_iHealth;
    uintptr_t isDefusing = m_bIsDefusing;
    uintptr_t forceLMB = forceLMB;
    uintptr_t crossH = m_iCrosshairId;

}offsets;

struct
{
    uintptr_t gameModule;
    uintptr_t localPlayer;
    uintptr_t ent;
    uintptr_t glowObject;
    uintptr_t engineModule;
    uintptr_t entity; //for tBot
    BYTE flag;
    int myTeam;
    int entityTeam;
    int glowIndex;
    int health;
    int crosshair;
    bool defusing;
}val;

struct ClrRender
{
    BYTE red, green, blue;
};

ClrRender clrEnemy;

ClrRender clrTeam;

struct glowStruct
{
    BYTE base[8];//4 to 8
    float red;//4 to 8
    float green;//8 to C
    float blue;//C to 10
    float alpha;//10 to 14
    BYTE buffer[16];//10 + 16 to 14 + x
    bool renderWhenOccluded;//24 to 28
    bool renderWhenUnOccluded;//25 to 29
    bool fullBloon;//26 to 30
    BYTE buffer1[5];
    int glowStyle;
};

bool isGlowEnabled = false;  // Added state variable

void toggleGlow() {
    isGlowEnabled = !isGlowEnabled;
}

glowStruct setGlowColor(glowStruct glow, uintptr_t ent)
{
    val.defusing = *(bool*)(val.ent + offsets.isDefusing);
    if (val.defusing)
    {
        glow.red = 1.0f;
        glow.green = 1.0f;
        glow.blue = 1.0f;
        glow.alpha = 1.0f;
    }
    else
    {
        glow.red = (*(int*)(val.ent + offsets.health)) * -0.01 + 1;
        glow.green = (*(int*)(val.ent + offsets.health)) * 0.01;
        glow.alpha = 1.0f;
    }
    glow.renderWhenOccluded = true;
    glow.renderWhenUnOccluded = false;
    return glow;
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
    glowStruct eGlow;
    eGlow = *(glowStruct*)(val.glowObject + (val.glowIndex * 0x38));
    eGlow = (glowStruct)(setGlowColor(eGlow, val.ent));
    *(glowStruct*)(val.glowObject + (val.glowIndex * 0x38)) = eGlow;
    
}

//glowhack
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

            

            if (isGlowEnabled) {
                if (val.myTeam == val.entityTeam) {
                    *(ClrRender*)(val.ent + m_clrRender) = clrTeam;
                    setTeamGlow(val.ent, val.glowIndex);
                }
                else {
                    *(ClrRender*)(val.ent + m_clrRender) = clrEnemy;
                    setEnemyGlow(val.ent, val.glowIndex);
                }
            }
        }

    }

}

void setBrightness()
{
    clrEnemy.red = 255;
    clrEnemy.green = 0;
    clrEnemy.blue = 0;

    clrTeam.red = 0;
    clrTeam.green = 0;
    clrTeam.blue = 255;

    float brightness = 5.0f;

    int ptr = *(int*)(val.engineModule + model_ambient_min);
    int xorptr = *(int*)&brightness^ ptr;
    *(int*)(val.engineModule + model_ambient_min) = xorptr;
}

void shoot()
{

}

bool checkTBot()
{
    val.crosshair = *(int*)(val.localPlayer + offsets.crossH);
    val.entity = *(uintptr_t*)(val.gameModule + offsets.entList+((val.crosshair-1)*0x10));
    val.entityTeam = *(int*)(val.entity + offsets.team);
    val.health = *(int*)(val.entity + offsets.health);
    std::cout << val.entity << std::endl;


    return false;
}

void handleTBot()
{
    if (checkTBot())
    {
        shoot();
    }
}

void main()
{
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    bool canTBot = false;

    val.gameModule = (uintptr_t)GetModuleHandle("client.dll");
    val.engineModule = (uintptr_t)GetModuleHandle("engine.dll");
    val.localPlayer = *(uintptr_t*)(val.gameModule + offsets.lPlayer);

    if (val.localPlayer == NULL)
        while (val.localPlayer == NULL)
            val.localPlayer = *(uintptr_t*)(val.gameModule + offsets.lPlayer);

    std::cout << std::hex << val.localPlayer << std::endl;

    setBrightness();
    while (true)
    {
        val.flag = *(BYTE*)(val.localPlayer + offsets.flags);
        
        for (short int i = 0; i <= 64; i++)
        {

            //radar
            val.ent = *(uintptr_t*)(val.gameModule + offsets.entList + i * 0x10);
            if (val.ent != 0)
            {
                *(bool*)(val.ent + offsets.spotted) = true;

            }

            //glow
            if (GetAsyncKeyState(VK_HOME) & 1) {
                toggleGlow();
            }

            if (isGlowEnabled) {
                handleGlow();
            }
 
        }
        //tbot
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            val.myTeam = *(int*)(val.localPlayer + offsets.team);
            canTBot = !canTBot;
        }

        if (canTBot)
        {
            handleTBot();
        }

        //bHop
        if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0))
        {
            *(uintptr_t*)(val.gameModule + offsets.jump) = 6;

        }

        //anti-flash
        if ((*(int*)(val.localPlayer + offsets.flashDuration)) != 0)
        {
            *(int*)(val.localPlayer + offsets.flashDuration) = 0;
        }

        Sleep(10);
 
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

