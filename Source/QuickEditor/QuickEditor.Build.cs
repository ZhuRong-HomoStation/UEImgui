// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class QuickEditor : ModuleRules
{
	public QuickEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] 
			{
                Path.Combine(ModuleDirectory, "Public")
			});

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			});
			
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine" ,
				"Slate",
				"SlateCore",
				"LevelEditor",
				"EditorSubsystem",
				"ToolMenus" ,
				"MainFrame" ,
			});
	}
}
