#include "stdafx.h"

FunctionPointer(void, SetPlayerInvulnerable, (uint8_t player), 0x441FD0);
FunctionPointer(void, DeleteSETObjects, (), 0x413F70);
FunctionPointer(signed int, ResetKnucklesStuff, (), 0x475840);
ObjectFunc(LoadSETObjects, 0x46C3D0);

DataArray(ObjectFuncPtr, LevelObjects, 0x90BF38, 44);
RestartData SavedRestartData[8];

void ResetLevel() {
	DeleteSETObjects();
	CutsceneMode = 0;
	DeleteAllObjects();
	ResetGravity();
	DisableTimeThing();
	if (CurrentCharacter == Characters_Knuckles) ResetKnucklesStuff();
	else if (CurrentCharacter == 7) ResetBigStuff();
	InitHeap();
	InitMasterObjectArray();
	LoadObject(LoadObj_Data1, 8, LoadSETObjects);
	LoadSkyboxObject();
	LoadLevelObject();
	Set0Rings();
	LoadLevel();
	GameState = GameMode != GameModes_Trial ? 4 : 23;
}

void ResetSavedRestartData(uint8_t id) {
	SavedRestartData[id].Frame = 0;
}

void ResetTimeAndRings(uint8_t id) {
	if (!SavedRestartData[id].Frame) SetTime2(RestartLevel.Minute, RestartLevel.Second, RestartLevel.Frame);
	else SetTime2(SavedRestartData[id].Minute, SavedRestartData[id].Second, SavedRestartData[id].Frame);
	AddRings(-GetRings());
	SetPlayerInvulnerable(id);
	ResetScoreAndBonuses();
}

void TeleportCharacter(uint8_t id) {
	EntityData1 * entity = EntityData1Ptrs[id];
	if (!entity) return;

	if (!SavedRestartData[id].Frame)
		RestartCheckpoint(&entity->Position, &entity->Rotation);
	else {
		entity->Position = SavedRestartData[id].Position;
		entity->Rotation = SavedRestartData[id].Rotation;
		ResetTimeAndRings(id);
	}
}

void QuickRestart_Handle(uint8_t id) {
	EntityData1 * entity = EntityData1Ptrs[id];

	//Simple teleport to checkpoint/savedpos
	if (ControllerPointers[id]->PressedButtons & Buttons_Z) {
		TeleportCharacter(id);
	}

	//Reset level, objects and then teleport every player
	if (ControllerPointers[id]->PressedButtons & Buttons_R && id == 0) {
		ResetLevel();
		for (uint8_t i = 0; i < 8; ++i) { TeleportCharacter(id); }
	}

	//Save a position
	if (ControllerPointers[id]->PressedButtons & Buttons_L) {
		if (!SavedRestartData[id].Frame) {
			SavedRestartData[id].Second = TimeSeconds;
			SavedRestartData[id].Minute = TimeMinutes;
			SavedRestartData[id].Frame = TimeFrames;
			SavedRestartData[id].Position = entity->Position;
			SavedRestartData[id].Rotation = entity->Rotation;
		}
		else {
			SavedRestartData[id].Frame = 0;
		}
	}
}

extern "C"
{
	__declspec(dllexport) void __cdecl OnFrame()
	{
		if (GameState == 15 || GameState == 9) {
			for (uint8_t i = 0; i < 8; ++i) {
				if (ControllerPointers[i]->HeldButtons & Buttons_C) {
					QuickRestart_Handle(i);
				}

				if (GameState == 9)
					ResetSavedRestartData(i);
			}
		}
	}
	
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}