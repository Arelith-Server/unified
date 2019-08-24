#include "nwnx"

const string NWNX_Administration = "NWNX_Administration";

const int NWNX_ADMINISTRATION_OPTION_ALL_KILLABLE               = 0;  // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_NON_PARTY_KILLABLE         = 1;  // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_REQUIRE_RESURRECTION       = 2;  // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_LOSE_STOLEN_ITEMS          = 3;  // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_LOSE_ITEMS                 = 4;  // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_LOSE_EXP                   = 5;  // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_LOSE_GOLD                  = 6;  // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_LOSE_GOLD_NUM              = 7;
const int NWNX_ADMINISTRATION_OPTION_LOSE_EXP_NUM               = 8;
const int NWNX_ADMINISTRATION_OPTION_LOSE_ITEMS_NUM             = 9;
const int NWNX_ADMINISTRATION_OPTION_PVP_SETTING                = 10; // 0 = No PVP, 1 = Party PVP, 2 = Full PVP
const int NWNX_ADMINISTRATION_OPTION_PAUSE_AND_PLAY             = 11; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_ONE_PARTY_ONLY             = 12; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_ENFORCE_LEGAL_CHARACTERS   = 13; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_ITEM_LEVEL_RESTRICTIONS    = 14; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_CDKEY_BANLIST_ALLOWLIST    = 15; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_DISALLOW_SHOUTING          = 16; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_SHOW_DM_JOIN_MESSAGE       = 17; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_BACKUP_SAVED_CHARACTERS    = 18; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_AUTO_FAIL_SAVE_ON_1        = 19; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_VALIDATE_SPELLS            = 20; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_EXAMINE_EFFECTS            = 21; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_EXAMINE_CHALLENGE_RATING   = 22; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_USE_MAX_HITPOINTS          = 23; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_RESTORE_SPELLS_USES        = 24; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_RESET_ENCOUNTER_SPAWN_POOL = 25; // TRUE/FALSE
const int NWNX_ADMINISTRATION_OPTION_HIDE_HITPOINTS_GAINED      = 26; // TRUE/FALSE

// Gets, sets, and clears the current player password.
// Note that this password can be an empty string.
string NWNX_Administration_GetPlayerPassword();
void NWNX_Administration_SetPlayerPassword(string password);
void NWNX_Administration_ClearPlayerPassword();

// Gets and sets the current DM password.
// Note that this password can be an empty string.
string NWNX_Administration_GetDMPassword();
void NWNX_Administration_SetDMPassword(string password);

// Signals the server to immediately shut down.
void NWNX_Administration_ShutdownServer();

// DEPRECATED: Use BootPC() native function
// Boots the provided player from the server with the provided strref as message.
void NWNX_Administration_BootPCWithMessage(object pc, int strref);

// Deletes the player character from the servervault
//    bPreserveBackup - if true, it will leave the file on server,
//                      only appending ".deleted0" to the bic filename.
// The PC will be immediately booted from the game with a "Delete Character" message
void NWNX_Administration_DeletePlayerCharacter(object pc, int bPreserveBackup = TRUE);

// Ban a given IP - get via GetPCIPAddress()
void NWNX_Administration_AddBannedIP(string ip);
void NWNX_Administration_RemoveBannedIP(string ip);

// Ban a given public cdkey - get via GetPCPublicCDKey
void NWNX_Administration_AddBannedCDKey(string key);
void NWNX_Administration_RemoveBannedCDKey(string key);

// Ban a given player name - get via GetPCPlayerName
// NOTE: Players can easily change their names
void NWNX_Administration_AddBannedPlayerName(string playername);
void NWNX_Administration_RemoveBannedPlayerName(string playername);

// Get a list of all banned IPs/Keys/names as a string
string NWNX_Administration_GetBannedList();

// Set the module's name as shown to the serverlist
void NWNX_Administration_SetModuleName(string name);
// Set the server's name as shown to the serverlist
void NWNX_Administration_SetServerName(string name);

// Get a NWNX_ADMINISTRATION_OPTION_* value
int NWNX_Administration_GetPlayOption(int option);
// Set a NWNX_ADMINISTRATION_OPTION_* to value
void NWNX_Administration_SetPlayOption(int option, int value);

// Delete the TURD of playerName + characterName
void NWNX_Administration_DeleteTURD(string playerName, string characterName);


string NWNX_Administration_GetPlayerPassword()
{
    string sFunc = "GET_PLAYER_PASSWORD";

    NWNX_CallFunction(NWNX_Administration, sFunc);
    return NWNX_GetReturnValueString(NWNX_Administration, sFunc);
}

void NWNX_Administration_SetPlayerPassword(string password)
{
    string sFunc = "SET_PLAYER_PASSWORD";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, password);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}

void NWNX_Administration_ClearPlayerPassword()
{
    string sFunc = "CLEAR_PLAYER_PASSWORD";

    NWNX_CallFunction(NWNX_Administration, sFunc);
}

string NWNX_Administration_GetDMPassword()
{
    string sFunc = "GET_DM_PASSWORD";

    NWNX_CallFunction(NWNX_Administration, sFunc);
    return NWNX_GetReturnValueString(NWNX_Administration, sFunc);
}

void NWNX_Administration_SetDMPassword(string password)
{
    string sFunc = "SET_DM_PASSWORD";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, password);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}

void NWNX_Administration_ShutdownServer()
{
    string sFunc = "SHUTDOWN_SERVER";

    NWNX_CallFunction(NWNX_Administration, sFunc);
}

void NWNX_Administration_BootPCWithMessage(object pc, int strref)
{
    WriteTimestampedLogEntry("NWNX_Administration: BootPCWithMessage() is deprecated. Use native BootPC() instead");
    BootPC(pc, GetStringByStrRef(strref));
}

void NWNX_Administration_DeletePlayerCharacter(object pc, int bPreserveBackup)
{
    string sFunc = "DELETE_PLAYER_CHARACTER";

    NWNX_PushArgumentInt(NWNX_Administration, sFunc, bPreserveBackup);
    NWNX_PushArgumentObject(NWNX_Administration, sFunc, pc);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}

void NWNX_Administration_AddBannedIP(string ip)
{
    string sFunc = "ADD_BANNED_IP";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, ip);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}
void NWNX_Administration_RemoveBannedIP(string ip)
{
    string sFunc = "REMOVE_BANNED_IP";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, ip);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}
void NWNX_Administration_AddBannedCDKey(string key)
{
    string sFunc = "ADD_BANNED_CDKEY";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, key);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}
void NWNX_Administration_RemoveBannedCDKey(string key)
{
    string sFunc = "REMOVE_BANNED_CDKEY";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, key);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}
void NWNX_Administration_AddBannedPlayerName(string playername)
{
    string sFunc = "ADD_BANNED_PLAYER_NAME";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, playername);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}
void NWNX_Administration_RemoveBannedPlayerName(string playername)
{
    string sFunc = "REMOVE_BANNED_PLAYER_NAME";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, playername);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}
string NWNX_Administration_GetBannedList()
{
    string sFunc = "GET_BANNED_LIST";

    NWNX_CallFunction(NWNX_Administration, sFunc);
    return NWNX_GetReturnValueString(NWNX_Administration, sFunc);
}


void NWNX_Administration_SetModuleName(string name)
{
    string sFunc = "SET_MODULE_NAME";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, name);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}

void NWNX_Administration_SetServerName(string name)
{
    string sFunc = "SET_SERVER_NAME";
    
    NWNX_PushArgumentString(NWNX_Administration, sFunc, name);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}

int NWNX_Administration_GetPlayOption(int option)
{
    string sFunc = "GET_PLAY_OPTION";

    NWNX_PushArgumentInt(NWNX_Administration, sFunc, option);
    NWNX_CallFunction(NWNX_Administration, sFunc);

    return NWNX_GetReturnValueInt(NWNX_Administration, sFunc);
}

void NWNX_Administration_SetPlayOption(int option, int value)
{
    string sFunc = "SET_PLAY_OPTION";

    NWNX_PushArgumentInt(NWNX_Administration, sFunc, value);
    NWNX_PushArgumentInt(NWNX_Administration, sFunc, option);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}

void NWNX_Administration_DeleteTURD(string playerName, string characterName)
{
    string sFunc = "DELETE_TURD";

    NWNX_PushArgumentString(NWNX_Administration, sFunc, characterName);
    NWNX_PushArgumentString(NWNX_Administration, sFunc, playerName);
    NWNX_CallFunction(NWNX_Administration, sFunc);
}
