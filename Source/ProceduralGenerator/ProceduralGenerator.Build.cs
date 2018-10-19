// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class ProceduralGenerator : ModuleRules
{
	public ProceduralGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "ProceduralMeshComponent","UMG", "Slate", "SlateCore" , "AIModule" });

		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd" });

       // PublicAdditionalPaths.Add("Output19/Source/VictoryBPLibrary");

       // PublicAdditionalLibraries.Add("Output19/Binaries/Win64/UE4Editor-VictoryBPLibrary.dll");

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
