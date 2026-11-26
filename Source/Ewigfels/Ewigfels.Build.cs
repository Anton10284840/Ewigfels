// Copyright Ewigfels. Alle Rechte vorbehalten.

using UnrealBuildTool;

/// Primäres Spielmodul. In Phase 2 kommen eigene Module dazu
/// (EwigfelsCore, Magic, Combat, Quest, Dialogue, AI, UI, Save, World).
public class Ewigfels : ModuleRules
{
	public Ewigfels(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput"
		});
	}
}
