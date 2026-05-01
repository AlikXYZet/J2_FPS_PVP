//

using UnrealBuildTool;

public class FPS : ModuleRules
{
    public FPS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {

            // Base:
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",

            // Core:
            "PhysicsCore",

            // Online:
            "OnlineSubsystem",
            "OnlineSubsystemUtils",

            // Niagara
            "Niagara",
            "NiagaraCore",

            // Gameplay Ability System
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
        });

        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[] {

            // Scripting
            "EditorScriptingUtilitiesExpansion",
            });
        }
    }
}
