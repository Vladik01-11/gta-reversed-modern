#include <StdInc.h>

#include "./Commands.hpp"
#include <CommandParser/Parser.hpp>

#include <TaskTypes/TaskSimplePlayerOnFoot.h>
#include <TaskTypes/TaskSimpleSwim.h>

#include <RunningScript.h>

using namespace notsa::script;
/*!
* Various player commands
*/

/*!
* @brief Create a player at the given world position
* @param playerId Player's id (0 or 1)
* @param pos      World position
*/
int32 CreatePlayer(int32 playerId, CVector pos) {
    if (!CStreaming::IsModelLoaded(MODEL_PLAYER)) {
        CStreaming::RequestSpecialModel(0, "player", STREAMING_GAME_REQUIRED | STREAMING_KEEP_IN_MEMORY | STREAMING_PRIORITY_REQUEST);
        CStreaming::LoadAllRequestedModels(true);
    }

    // Create
    CPlayerPed::SetupPlayerPed(playerId);
    auto player = FindPlayerPed(playerId);
    player->SetCharCreatedBy(PED_MISSION);
    CPlayerPed::DeactivatePlayerPed(playerId);

    // Position in the world
    if (pos.z <= MAP_Z_LOW_LIMIT) {
        pos.z = CWorld::FindGroundZForCoord(pos.x, pos.y);
    }
    pos.z += player->GetDistanceFromCentreOfMassToBaseOfModel();
    player->SetPosn(pos);
    CTheScripts::ClearSpaceForMissionEntity(pos, player);
    CPlayerPed::ReactivatePlayerPed(playerId);

    // Set task
    player->GetTaskManager().SetTask(new CTaskSimplePlayerOnFoot(), TASK_PRIMARY_DEFAULT);

    return playerId;
}

/// Get the position of a player
CVector GetPlayerCoordinates(CPlayerInfo& pinfo) {
    return pinfo.GetPos();
}

bool IsPlayerInArea2D(CRunningScript* S, CPlayerPed& player, CRect area, bool highlightArea) {
    if (highlightArea) {
        CTheScripts::HighlightImportantArea((uint32)S + (uint32)S->m_IP, area, MAP_Z_LOW_LIMIT);
    }

    if (CTheScripts::DbgFlag) {
        CTheScripts::DrawDebugSquare(area);
    }

    return player.bInVehicle
        ? player.m_pVehicle->IsWithinArea(area.left, area.top, area.right, area.bottom)
        : player.IsWithinArea(area.left, area.top, area.right, area.bottom);
}

bool IsPlayerInArea3D(CRunningScript* S, CPlayerPed& player, CVector p1, CVector p2, bool highlightArea) {
    if (highlightArea) {
        CTheScripts::HighlightImportantArea((uint32)S + (uint32)S->m_IP, p1.x, p1.y, p2.x, p2.y, (p1.z + p2.z) / 2.0f);
    }

    if (CTheScripts::DbgFlag) {
        CTheScripts::DrawDebugCube(p1, p2);
    }

    return player.bInVehicle
        ? player.m_pVehicle->IsWithinArea(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z)
        : player.IsWithinArea(p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
}

auto IsPlayerPlaying(CPlayerInfo& player) -> notsa::script::CompareFlagUpdate {
    return { player.m_nPlayerState == PLAYERSTATE_PLAYING };
}

bool IsPlayerClimbing(CPlayerPed& player) {
    return player.GetIntelligence()->GetTaskClimb();
}

void SetSwimSpeed(CPlayerPed& player, float speed) {
    if (auto swim = player.GetIntelligence()->GetTaskSwim())
        swim->m_fAnimSpeed = speed;
}

void SetPlayerGroupToFollowAlways(CPlayerPed& player, bool enable) {
    player.ForceGroupToAlwaysFollow(enable);
}

void SetPlayerAbleToUseCrouch(uint32 playerIdx, bool enable) {
    CPad::GetPad(playerIdx)->bDisablePlayerDuck = !enable;
}

// Originally unsupported
bool IsPlayerTouchingObject(CPlayerPed& player, CObject& object) {
    return GetPedOrItsVehicle(player).GetHasCollidedWith(&object);
}

// Originally unsupported
bool IsPlayerTouchingObjectOnFoot(CPlayerPed& player, CObject& object) {
    return player.GetHasCollidedWith(&object);
}

// COMMAND_IS_PLAYER_IN_POSITION_FOR_CONVERSATION - 0x474983
//bool IsPlayerInPositionForConversation(CPed& ped) { // Yes, it's CPed
//
//}

void notsa::script::commands::player::RegisterHandlers() {
    REGISTER_COMMAND_HANDLER(COMMAND_CREATE_PLAYER, CreatePlayer);
    REGISTER_COMMAND_HANDLER(COMMAND_GET_PLAYER_COORDINATES, GetPlayerCoordinates);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_IN_AREA_2D, IsPlayerInArea2D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_IN_AREA_3D, IsPlayerInArea3D);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_PLAYING, IsPlayerPlaying);
    REGISTER_COMMAND_HANDLER(COMMAND_IS_PLAYER_CLIMBING, IsPlayerClimbing);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_SWIM_SPEED, SetSwimSpeed);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_GROUP_TO_FOLLOW_ALWAYS, SetPlayerGroupToFollowAlways);
    REGISTER_COMMAND_HANDLER(COMMAND_SET_PLAYER_DUCK_BUTTON, SetPlayerAbleToUseCrouch);


    REGISTER_UNSUPPORTED_COMMAND_HANDLER(COMMAND_IS_PLAYER_TOUCHING_OBJECT, IsPlayerTouchingObject);
    REGISTER_UNSUPPORTED_COMMAND_HANDLER(COMMAND_IS_PLAYER_TOUCHING_OBJECT_ON_FOOT, IsPlayerTouchingObjectOnFoot);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_HEED_THREATS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANGLED_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_ANGLED_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_SHOOTING_IN_AREA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_CURRENT_PLAYER_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_TAXI);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_SHOOTING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_EXPLODE_PLAYER_HEAD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_VISIBLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ADD_ARMOUR_TO_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_OBJECT_BEEN_DAMAGED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_START_KILL_FRENZY_HEADSHOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ACTIVATE_MILITARY_CRANE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_WARP_PLAYER_INTO_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GIVE_PLAYER_DETONATOR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_STORE_CAR_PLAYER_IS_IN_NO_SAVE);

    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_COORDINATES);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_STORE_CAR_PLAYER_IS_IN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_MODEL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_DEAD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_PRESSING_HORN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_WARP_PLAYER_FROM_CAR_TO_COORD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_HEADING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_HEADING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_HEALTH_GREATER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_ON_FOOT_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_IN_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_ON_FOOT_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STOPPED_IN_AREA_IN_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GIVE_WEAPON_TO_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CURRENT_PLAYER_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TURN_PLAYER_TO_FACE_COORD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_AS_LEADER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_POLICE_IGNORE_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ANY_MEANS_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ON_FOOT_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_IN_CAR_CAR_2D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_ANY_MEANS_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LOCATE_PLAYER_IN_CAR_CAR_3D);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TURN_PLAYER_TO_FACE_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_APPLY_BRAKES_TO_PLAYERS_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_HEALTH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_HEALTH);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_STOP_PLAYER_LOOKING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_GANG_PLAYER_ATTITUDE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_REMOTE_MODE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_ANIM_GROUP_FOR_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_NUM_OF_MODELS_KILLED_BY_PLAYER);
    //REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_NEVER_GETS_TIRED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_FAST_RELOAD);
    //REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_EVERYONE_IGNORE_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_CAMERA_IN_FRONT_OF_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_MAKE_PLAYER_SAFE_FOR_CUTSCENE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_MAKE_PLAYER_UNSAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_MAKE_PLAYER_SAFE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_AMMO_IN_PLAYER_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_HOOKER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_SITTING_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_SITTING_IN_ANY_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_LIFTING_A_PHONE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_JAMES_CAR_ON_PATH_TO_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_ENABLE_PLAYER_CONTROL_CAMERA);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_TARGETTING_ANY_CHAR);
    //REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_TARGETTING_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_TARGETTING_OBJECT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GIVE_REMOTE_CONTROLLED_MODEL_TO_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_CURRENT_PLAYER_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_ON_ANY_BIKE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_PLAYER_GOT_WEAPON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_FACING_CHAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_STANDING_ON_A_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_FOOT_DOWN);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANY_BOAT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANY_HELI);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_ANY_PLANE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_RESET_HAVOC_CAUSED_BY_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_HAVOC_CAUSED_BY_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_FLYING_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SHUT_PLAYER_UP);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_MOOD);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_WEARING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_CAN_DO_DRIVE_BY);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_DRUNKENNESS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_DRUG_LEVEL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_DRUG_LEVEL);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_AUTO_AIM);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_TOUCHING_VEHICLE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CHECK_FOR_PED_MODEL_AROUND_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_HAS_MET_DEBBIE_HARRY);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_MAKE_PLAYER_FIRE_PROOF);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_BUS_FARES_COLLECTED_BY_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_GANG_ATTACK_PLAYER_WITH_COPS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TASK_PLAYER_ON_FOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_TASK_PLAYER_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_CLOSEST_BUYABLE_OBJECT_TO_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TWO_PLAYER_CAMERA_MODE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_LIMIT_TWO_PLAYER_DISTANCE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_RELEASE_TWO_PLAYER_DISTANCE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_PLAYER_TARGETTING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_CLEAR_TWO_PLAYER_CAMERA_MODE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_PASSENGER_CAN_SHOOT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYERS_CAN_BE_IN_SEPARATE_CARS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SWITCH_PLAYER_CROSSHAIR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PLANE_ATTACK_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HELI_ATTACK_PLAYER);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GIVE_PLAYER_TATTOO);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TWO_PLAYER_CAM_MODE_SEPARATE_CARS);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TWO_PLAYER_CAM_MODE_SAME_CAR_SHOOTING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TWO_PLAYER_CAM_MODE_SAME_CAR_NO_SHOOTING);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_TWO_PLAYER_CAM_MODE_NOT_BOTH_IN_CAR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_FIRE_BUTTON);
    //REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_IS_PLAYER_IN_POSITION_FOR_CONVERSATION);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PLANE_ATTACK_PLAYER_USING_DOG_FIGHT);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_JUMP_BUTTON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_CAN_BE_DAMAGED);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYERS_GANG_IN_CAR_ACTIVE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYERS_GANG_IN_CAR_ACTIVE);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_HAS_PLAYER_BOUGHT_ITEM);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_GET_PLAYER_MAX_ARMOUR);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_PLAYER_FIRE_WITH_SHOULDER_BUTTON);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_PLAYER_PUT_ON_GOGGLES);
    REGISTER_COMMAND_UNIMPLEMENTED(COMMAND_SET_RENDER_PLAYER_WEAPON);
}
