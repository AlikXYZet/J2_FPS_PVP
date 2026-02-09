// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditorScriptingUtilitiesExpansion : ModuleRules
{
    public EditorScriptingUtilitiesExpansion(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(new string[] { 

        });


        PrivateIncludePaths.AddRange(new string[] { 

            // Scripting
            System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "Source/Editor/Blutility/Private"
        });


        PublicDependencyModuleNames.AddRange(new string[] { 

            // Base:
            "Core",
        });


        PrivateDependencyModuleNames.AddRange(new string[] { 

            // Base:
            "CoreUObject",
            "Engine",
            "Slate",
            "SlateCore",
            "PhysicsCore",

            // Scripting
            "Blutility",
        });


        DynamicallyLoadedModuleNames.AddRange(new string[] { 

        });
    }
}
