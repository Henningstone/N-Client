/* (c) MAP94 and Patafix. See www.n-lvl.com/ndc/nclient/ for more information. */
#include "lua.h"
/*
#include "components/flow.h"
#include "components/particles.h"
#include "components/menus.h"
#include <game/generated/client_data.h>

#include <engine/serverbrowser.h>
#include <engine/textrender.h>
#include <engine/sound.h>
#include <engine/graphics.h>
#include <engine/storage.h>
#include <engine/server/server.h>

#include <game/client/lineinput.h>
#include <game/client/components/menus.h>
#include <game/client/components/chat.h>
#include <game/client/components/countryflags.h>
#include <game/client/components/skins.h>*/

#include "commands/character.cpp"
#include "commands/chat.cpp"
#include "commands/collision.cpp"
#include "commands/console.cpp"
#include "commands/config.cpp"
#include "commands/game.cpp"
#include "commands/events.cpp"
#include "commands/message.cpp"
#include "commands/player.cpp"
#include "commands/entities.cpp"
#include "commands/dummy.cpp"

#define NON_HASED_VERSION
#include <game/version.h>
#undef NON_HASED_VERSION

CLuaFile::CLuaFile()
{
    mem_zero(this, sizeof(CLuaFile));
    Close();
}

CLuaFile::~CLuaFile()
{
    #ifndef CONF_PLATFORM_MACOSX
    End();
    if (m_pLua)
        lua_close(m_pLua);
    m_pLua = 0;
    #endif
}
void CLuaFile::Tick()
{
    if (!g_Config.m_SvLua)
        return;

    ErrorFunc(m_pLua);

    FunctionPrepare("Tick");
    PushInteger((int)(time_get() * 10 / time_freq()));
    FunctionExec();

    lua_gc(m_pLua, LUA_GCCOLLECT, 1000);

    ErrorFunc(m_pLua);
}
void CLuaFile::TickDefered()
{
    if (!g_Config.m_SvLua)
        return;

    ErrorFunc(m_pLua);

    FunctionPrepare("TickDefered");
    PushInteger((int)(time_get() * 10 / time_freq()));
    FunctionExec();

    ErrorFunc(m_pLua);
}
void CLuaFile::PostTick()
{
    if (!g_Config.m_SvLua)
        return;

    ErrorFunc(m_pLua);

    FunctionPrepare("PostTick");
    PushInteger((int)(time_get() * 10 / time_freq()));
    FunctionExec();

    ErrorFunc(m_pLua);
}

void CLuaFile::End()
{
    if (m_pLua == 0)
        return;

    //try to call the end function
    //Maybe the lua file need to save data eg. a ConfigFile
    FunctionExec("end");
}

int CLuaFile::Panic(lua_State *L)
{
    dbg_break();
    return 0;
}
void CLuaFile::Init(const char *pFile)
{
    //close first
    Close();

    str_copy(m_aFilename, pFile, sizeof(m_aFilename));

    m_pLua = luaL_newstate();
    luaL_openlibs(m_pLua);

    lua_atpanic(m_pLua, &Panic);

    //include
    lua_register(m_pLua, "Include", this->Include);
    luaL_dostring(m_pLua, "package.path = \"./lua/?.lua;./lua/lib/?.lua\"\n");

    //config
    lua_register(m_pLua, "SetScriptUseSettingPage", this->SetScriptUseSettingPage);
    lua_register(m_pLua, "SetScriptTitle", this->SetScriptTitle);
    lua_register(m_pLua, "SetScriptInfo", this->SetScriptInfo);

    //events
		lua_register(m_pLua, "AddEventListener", this->AddEventListener);
		lua_register(m_pLua, "RemoveEventListener", this->RemoveEventListener);
		lua_register(m_pLua, "EventGetCID", this->EventGetCID);
		 //chat
		lua_register(m_pLua, "ChatGetText", this->ChatGetText);
		//lua_register(m_pLua, "ChatGetClientID", this->ChatGetClientID);
		lua_register(m_pLua, "ChatGetTeam", this->ChatGetTeam);
		lua_register(m_pLua, "ChatHide", this->ChatHide);

		//kill
		//lua_register(m_pLua, "KillGetKillerID", this->KillGetKillerID);
		//lua_register(m_pLua, "KillGetVictimID", this->KillGetVictimID);
		//lua_register(m_pLua, "KillGetWeapon", this->KillGetWeapon);
		
		//Die
		lua_register(m_pLua, "DieGetKillerID", this->DieGetKillerID);
		lua_register(m_pLua, "DieGetVictimID", this->DieGetVictimID);
		lua_register(m_pLua, "DieGetWeaponID", this->DieGetWeaponID);
		
		//WeaponFire		
		lua_register(m_pLua, "WeaponFireGetWeaponID", this->WeaponFireGetWeaponID);
		lua_register(m_pLua, "WeaponFireGetDir", this->WeaponFireGetDir);
		lua_register(m_pLua, "WeaponFireSetReloadTime", this->WeaponFireSetReloadTime);
		lua_register(m_pLua, "WeaponFireDisableSound", this->WeaponFireDisableSound);
		lua_register(m_pLua, "WeaponFireAutoFire", this->WeaponFireAutoFire);

		//OnJump
		lua_register(m_pLua, "JumpGetJumpID", this->JumpGetJumpID);

		//OnTile
		//lua_register(m_pLua, "GetOnTileIndex", this->GetOnTileIndex);
		//lua_register(m_pLua, "GetOnTilePosIndex", this->GetOnTilePosIndex);
		//lua_register(m_pLua, "SettOnTileIndex", this->SetOnTileIndex);

		//OnEntity
		lua_register(m_pLua, "OnEntityGetIndex", this->OnEntityGetIndex);
		lua_register(m_pLua, "OnEntityGetPosIndex", this->OnEntityGetPosIndex);
		lua_register(m_pLua, "OnEntitySetIndex", this->OnEntitySetIndex);
		
		//explosion		
		lua_register(m_pLua, "ExplosionGetPos", this->ExplosionGetPos);
		lua_register(m_pLua, "ExplosionGetDamage", this->ExplosionGetDamage);
		lua_register(m_pLua, "ExplosionGetOwner", this->ExplosionGetOwner);
		lua_register(m_pLua, "ExplosionGetDamage", this->ExplosionGetDamage);
		lua_register(m_pLua, "ExplosionAbort", this->ExplosionAbort);		

    //player
    lua_register(m_pLua, "GetPlayerName", this->GetPlayerName);
    lua_register(m_pLua, "GetPlayerClan", this->GetPlayerClan);
    lua_register(m_pLua, "GetPlayerCountry", this->GetPlayerCountry);
    lua_register(m_pLua, "GetPlayerScore", this->GetPlayerScore);
    lua_register(m_pLua, "GetPlayerPing", this->GetPlayerPing);
    lua_register(m_pLua, "GetPlayerTeam", this->GetPlayerTeam);
    lua_register(m_pLua, "GetPlayerSkin", this->GetPlayerSkin);
	lua_register(m_pLua, "GetPlayerColorFeet", this->GetPlayerColorFeet);
	lua_register(m_pLua, "GetPlayerColorBody", this->GetPlayerColorBody);
	lua_register(m_pLua, "SetPlayerScore", this->SetPlayerScore);
	lua_register(m_pLua, "SetPlayerName", this->SetPlayerName);
	lua_register(m_pLua, "SetPlayerTeam", this->SetPlayerTeam);
	lua_register(m_pLua, "SetPlayerClan", this->SetPlayerClan);
	lua_register(m_pLua, "SetPlayerCountry", this->SetPlayerCountry);

	lua_register(m_pLua, "SetPlayerColorBody", this->SetPlayerColorBody);
	lua_register(m_pLua, "SetPlayerColorFeet", this->SetPlayerColorFeet);

    //character
    lua_register(m_pLua, "Emote", this->Emote);
    lua_register(m_pLua, "GetCharacterPos", this->GetCharacterPos);
    lua_register(m_pLua, "GetCharacterVel", this->GetCharacterVel);
	lua_register(m_pLua, "SetCharacterPos", this->SetCharacterPos);
    lua_register(m_pLua, "SetCharacterVel", this->SetCharacterVel);

	//config
    lua_register(m_pLua, "GetConfigValue", this->GetConfigValue);
    lua_register(m_pLua, "SetConfigValue", this->SetConfigValue);

    //console
    lua_register(m_pLua, "Print", this->Print);
    lua_register(m_pLua, "Console", this->Console);

    //game
    lua_register(m_pLua, "GetGameType", this->GetGameType);
    lua_register(m_pLua, "IsTeamplay", this->IsTeamplay);

    //message
	//  lua_register(m_pLua, "GetNetError", this->GetNetError);
	lua_register(m_pLua, "SendPacket", this->SendPacket);
	lua_register(m_pLua, "FetchPacket", this->FetchPacket);	
	lua_register(m_pLua, "AddModFile", this->AddModFile);
	lua_register(m_pLua, "DeleteModFile", this->DeleteModFile);
	lua_register(m_pLua, "SendFile", this->SendFile);


    //collision
    lua_register(m_pLua, "IntersectLine", this->IntersectLine);
    lua_register(m_pLua, "GetTile", this->GetTile);
    lua_register(m_pLua, "SetTile", this->SetTile);
    lua_register(m_pLua, "GetMapWidth", this->GetMapWidth);
    lua_register(m_pLua, "GetMapHeight", this->GetMapHeight);

    //Chat
    lua_register(m_pLua, "SendBroadcast", this->SendBroadcast);
    lua_register(m_pLua, "SendChat", this->SendChat);
    lua_register(m_pLua, "SendChatTarget", this->SendChatTarget);

    //Entities
    lua_register(m_pLua, "EntityFind", this->EntityFind);
    lua_register(m_pLua, "EntityGetCharacterId", this->EntityGetCharacterId);
    lua_register(m_pLua, "EntityGetPos", this->EntityGetPos);
    lua_register(m_pLua, "EntitySetPos", this->EntitySetPos);
    lua_register(m_pLua, "EntityDestroy", this->EntityDestroy);
    lua_register(m_pLua, "ProjectileFind", this->ProjectileFind);
    lua_register(m_pLua, "ProjectileGetWeapon", this->ProjectileGetWeapon);
    lua_register(m_pLua, "ProjectileGetOwner", this->ProjectileGetOwner);
    lua_register(m_pLua, "ProjectileGetPos", this->ProjectileGetPos);
    lua_register(m_pLua, "ProjectileGetDir", this->ProjectileGetDir);
    lua_register(m_pLua, "ProjectileGetLifespan", this->ProjectileGetLifespan);
    lua_register(m_pLua, "ProjectileGetExplosive", this->ProjectileGetExplosive);
    lua_register(m_pLua, "ProjectileGetSoundImpact", this->ProjectileGetSoundImpact);
    lua_register(m_pLua, "ProjectileCreate", this->ProjectileCreate);
	lua_register(m_pLua, "LaserCreate", this->LaserCreate);

    //game
    lua_register(m_pLua, "CreateExplosion", this->CreateExplosion);
    lua_register(m_pLua, "CreateDeath", this->CreateDeath);
    lua_register(m_pLua, "CreateDamageIndicator", this->CreateDamageIndicator);

    


    lua_register(m_pLua, "CharacterTakeDamage", this->CharacterTakeDamage);
    lua_register(m_pLua, "CharacterGetHealth", this->CharacterGetHealth);
    lua_register(m_pLua, "CharacterGetArmor", this->CharacterGetArmor);
    lua_register(m_pLua, "CharacterSetInputDirection", this->CharacterSetInputDirection);
    lua_register(m_pLua, "CharacterSetInputJump", this->CharacterSetInputJump);
    lua_register(m_pLua, "CharacterSetInputWeapon", this->CharacterSetInputWeapon);
    lua_register(m_pLua, "CharacterSetInputTarget", this->CharacterSetInputTarget);
    lua_register(m_pLua, "CharacterSetInputHook", this->CharacterSetInputHook);
    lua_register(m_pLua, "CharacterSetInputFire", this->CharacterSetInputFire);
    lua_register(m_pLua, "CharacterGetCoreJumped", this->CharacterGetCoreJumped);
    lua_register(m_pLua, "CharacterSpawn", this->CharacterSpawn);
    lua_register(m_pLua, "CharacterIsAlive", this->CharacterIsAlive);
    lua_register(m_pLua, "CharacterKill", this->CharacterKill);
    lua_register(m_pLua, "CharacterIsGrounded", this->CharacterIsGrounded);
    lua_register(m_pLua, "CharacterIncreaseHealth", this->CharacterIncreaseHealth);
    lua_register(m_pLua, "CharacterIncreaseArmor", this->CharacterIncreaseArmor);
    lua_register(m_pLua, "CharacterSetAmmo", this->CharacterSetAmmo);
    lua_register(m_pLua, "CharacterGetAmmo", this->CharacterGetAmmo);
    lua_register(m_pLua, "CharacterGetInputTarget", this->CharacterGetInputTarget);
    lua_register(m_pLua, "CharacterGetActiveWeapon", this->CharacterGetActiveWeapon);
    lua_register(m_pLua, "CharacterDirectInput", this->CharacterDirectInput);
    lua_register(m_pLua, "CharacterPredictedInput", this->CharacterPredictedInput);

    lua_register(m_pLua, "DieGetVictimID", this->DieGetVictimID);
    lua_register(m_pLua, "DieGetKillerID", this->DieGetKillerID);
    lua_register(m_pLua, "DieGetWeaponID", this->DieGetWeaponID);

    lua_register(m_pLua, "AbortSpawn", this->AbortSpawn);
    lua_register(m_pLua, "SpawnGetTeam", this->SpawnGetTeam);


    lua_register(m_pLua, "Win", this->Win);
    lua_register(m_pLua, "SetGametype", this->SetGametype);
	
	lua_register(m_pLua, "GetSelectedTeam", this->GetSelectedTeam);
	lua_register(m_pLua, "AbortTeamJoin", this->AbortTeamJoin);

	lua_register(m_pLua, "DummyCreate", this->DummyCreate);
	lua_register(m_pLua, "IsDummy", this->IsDummy);

    //version
    lua_register(m_pLua, "CheckVersion", this->CheckVersion);
    lua_register(m_pLua, "GetVersion", this->GetVersion);


    lua_pushlightuserdata(m_pLua, this);
    lua_setglobal(m_pLua, "pLUA");

    lua_register(m_pLua, "errorfunc", this->ErrorFunc); //TODO: fix me
	lua_getglobal(m_pLua, "errorfunc");


    if (luaL_loadfile(m_pLua, m_aFilename) == 0)
    {
        lua_pcall(m_pLua, 0, LUA_MULTRET, 0);
        ErrorFunc(m_pLua);
    }
    else
    {
        ErrorFunc(m_pLua);
        dbg_msg("lua", "fail to load file: %s", pFile);
        Close();
        return;
    }
    lua_getglobal(m_pLua, "errorfunc");
    ErrorFunc(m_pLua);
}

void CLuaFile::Close()
{
    //kill lua
    if (m_pLua)
        lua_close(m_pLua);
    m_pLua = 0;

    //clear
    mem_zero(m_aTitle, sizeof(m_aTitle));
    mem_zero(m_aInfo, sizeof(m_aInfo));
    mem_zero(m_aFilename, sizeof(m_aFilename));
    m_HaveSettings = 0;
    m_FunctionVarNum = 0;
}

int CLuaFile::ErrorFunc(lua_State *L)
{
    lua_getglobal(L, "pLUA");
    CLuaFile *pSelf = (CLuaFile *)lua_touserdata(L, -1);

    lua_pop(L,1);

	int depth = 0;
	int frameskip = 1;
	lua_Debug frame;

    if (lua_tostring(L, -1) == 0)
        return 0;

    dbg_msg("Lua", pSelf->m_aFilename);
    dbg_msg("Lua", lua_tostring(L, -1));

    dbg_msg("Lua", "Backtrace:");
    while(lua_getstack(L, depth, &frame) == 1)
    {
        depth++;

        lua_getinfo(L, "nlSf", &frame);

        /* check for functions that just report errors. these frames just confuses more then they help */
        if(frameskip && str_comp(frame.short_src, "[C]") == 0 && frame.currentline == -1)
            continue;
        frameskip = 0;

        /* print stack frame */
        dbg_msg("Lua", "%s(%d): %s %s", frame.short_src, frame.currentline, frame.name, frame.namewhat);
    }
    lua_pop(L, 1); // remove error message
    lua_gc(L, LUA_GCCOLLECT, 0);
    return 0;
}


void CLuaFile::ConfigClose()
{
    FunctionExec("ConfigClose");
}

void CLuaFile::PushString(const char *pString)
{
    if (m_pLua == 0)
        return;
    lua_pushstring(m_pLua, pString);
    m_FunctionVarNum++;
}

void CLuaFile::PushInteger(int value)
{
    if (m_pLua == 0)
        return;
    lua_pushinteger(m_pLua, value);
    m_FunctionVarNum++;
}

void CLuaFile::PushFloat(float value)
{
    if (m_pLua == 0)
        return;
    lua_pushnumber(m_pLua, value);
    m_FunctionVarNum++;
}

void CLuaFile::PushBoolean(bool value)
{
    if (m_pLua == 0)
        return;
    lua_pushboolean(m_pLua, value);
    m_FunctionVarNum++;
}

void CLuaFile::PushParameter(const char *pString)
{
    if (m_pLua == 0)
        return;
    if (StrIsInteger(pString))
    {
        PushInteger(str_toint(pString));
    }
    else if (StrIsFloat(pString))
    {
        PushInteger(str_tofloat(pString));
    }
    else
    {
        PushString(pString);
    }

}

bool CLuaFile::FunctionExist(const char *pFunctionName)
{
    bool Ret = false;
    if (m_pLua == 0)
        return false;
    lua_getglobal(m_pLua, pFunctionName);
    Ret = lua_isfunction(m_pLua, -1);
    lua_pop(m_pLua, 1);
    return Ret;
}

void CLuaFile::FunctionPrepare(const char *pFunctionName)
{
    if (m_pLua == 0 || m_aFilename[0] == 0)
        return;

    //lua_pushstring (m_pLua, pFunctionName);
    //lua_gettable (m_pLua, LUA_GLOBALSINDEX);
    lua_getglobal(m_pLua, pFunctionName);
    m_FunctionVarNum = 0;
}

void CLuaFile::FunctionExec(const char *pFunctionName)
{
    if (m_pLua == 0)
        return;
    if (m_aFilename[0] == 0)
        return;

    if (pFunctionName)
    {
        if (FunctionExist(pFunctionName) == false)
            return;
        FunctionPrepare(pFunctionName);
    }
    lua_pcall(m_pLua, m_FunctionVarNum, LUA_MULTRET, 0);
    ErrorFunc(m_pLua);
    m_FunctionVarNum = 0;
}


//functions

int CLuaFile::Include(lua_State *L)
{
    lua_getglobal(L, "pLUA");
    CLuaFile *pSelf = (CLuaFile *)lua_touserdata(L, -1);
    lua_Debug Frame;
    lua_getstack(L, 1, &Frame);
    lua_getinfo(L, "nlSf", &Frame);

    if (!lua_isstring(L, 1))
        return 0;
    if (luaL_loadfile(L, lua_tostring(L, 1)) == 0)
    {
        lua_pcall(L, 0, LUA_MULTRET, 0);
    }

    return 0;
}

int CLuaFile::SetScriptUseSettingPage(lua_State *L)
{
    lua_getglobal(L, "pLUA");
    CLuaFile *pSelf = (CLuaFile *)lua_touserdata(L, -1);
    lua_Debug Frame;
    lua_getstack(L, 1, &Frame);
    lua_getinfo(L, "nlSf", &Frame);

    if (!lua_isnumber(L, 1))
        return 0;
    pSelf->m_HaveSettings = lua_tointeger(L, 1);
    return 0;
}

int CLuaFile::SetScriptTitle(lua_State *L)
{
    lua_getglobal(L, "pLUA");
    CLuaFile *pSelf = (CLuaFile *)lua_touserdata(L, -1);
    lua_Debug Frame;
    lua_getstack(L, 1, &Frame);
    lua_getinfo(L, "nlSf", &Frame);

    if (!lua_isstring(L, 1))
        return 0;
    str_copy(pSelf->m_aTitle, lua_tostring(L, 1), sizeof(pSelf->m_aTitle));
    return 0;
}

int CLuaFile::SetScriptInfo(lua_State *L)
{
    lua_getglobal(L, "pLUA");
    CLuaFile *pSelf = (CLuaFile *)lua_touserdata(L, -1);
    lua_Debug Frame;
    lua_getstack(L, 1, &Frame);
    lua_getinfo(L, "nlSf", &Frame);

    if (!lua_isstring(L, 1))
        return 0;
    str_copy(pSelf->m_aInfo, lua_tostring(L, 1), sizeof(pSelf->m_aInfo));
    return 0;
}

int CLuaFile::CheckVersion(lua_State *L)
{
    lua_getglobal(L, "pLUA");
    CLuaFile *pSelf = (CLuaFile *)lua_touserdata(L, -1);
    lua_Debug Frame;
    lua_getstack(L, 1, &Frame);
    lua_getinfo(L, "nlSf", &Frame);

    if (lua_isstring(L, 1))
        lua_pushboolean(L, str_comp(GAME_LUA_VERSION, lua_tostring(L, 1)) == 0);
    else
        lua_pushboolean(L, false);
    return 1;
}

int CLuaFile::GetVersion(lua_State *L)
{
    lua_getglobal(L, "pLUA");
    CLuaFile *pSelf = (CLuaFile *)lua_touserdata(L, -1);
    lua_Debug Frame;
    lua_getstack(L, 1, &Frame);
    lua_getinfo(L, "nlSf", &Frame);

    lua_pushstring(L, GAME_LUA_VERSION);
    return 1;
}
