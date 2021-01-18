// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class ImNodeEditor : ModuleRules
{
	public ImNodeEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		UnsafeTypeCastWarningLevel = WarningLevel.Off;
		bUseUnity = false;
		
		PublicIncludePaths.AddRange(
			new string[] 
			{
				Path.Combine(ModuleDirectory, "Public")
			});

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core" ,
				"Imgui" ,
			});
	}
}
