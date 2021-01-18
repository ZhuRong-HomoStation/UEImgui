// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class ImPlot : ModuleRules
{
	public ImPlot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		UnsafeTypeCastWarningLevel = WarningLevel.Off;
		
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
