#include "stdafx.h"

int temp = 0;

extern "C"
{
	__declspec(dllexport) void __cdecl OnFrame()
	{
		auto entity = EntityData1Ptrs[0];
		if (entity != nullptr) {
			if (ControllerPointers[0]->PressedButtons & Buttons_C) {
				RestartCheckpoint(&entity->Position, &entity->Rotation);
				entity->Action = 1;
				temp = 10;
			}
		}

		if (entity == nullptr) return;

		if (temp > 0) {
			temp -= 1;
			Camera_Data1->Position = entity->Position;
			Camera_Data1->Rotation = entity->Rotation;
		}
	}
	
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}
