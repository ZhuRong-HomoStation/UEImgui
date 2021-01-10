#pragma once
#include "CoreMinimal.h"

class UImguiInputAdapter;
class ImGuiContext;
struct ImGuiPlatformIO;
class SImguiWindow;

struct FImguiWindowManager
{
	static void SetupPlatformInterface(ImGuiPlatformIO* PlatformIO);	
	static void SetCurrentContext(ImGuiContext* InContext);
	static void SetCurrentInputAdapter(UImguiInputAdapter* InInputAdapter);
};
