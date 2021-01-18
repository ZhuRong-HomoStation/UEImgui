// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class UEImguiEditor : ModuleRules
{
	public UEImguiEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Imgui",
				"ImPlot",
				"ImNodeEditor" ,
			});
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"LevelEditor",
				"UEImgui",
				"EditorSubsystem",
				"UnrealEd",
				"Projects" ,
				"Imgui",
				"ImPlot",
			});
		
		PrivateDefinitions.Add("IMGUI_DEFINE_MATH_OPERATORS");
	}
}
