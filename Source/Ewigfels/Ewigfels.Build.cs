// Copyright Ewigfels. Alle Rechte vorbehalten.

using UnrealBuildTool;

/// Primäres Spielmodul. Systeme liegen vorerst als Unterordner hier
/// (WorldState, Quest, Dialogue, Save) und werden zu eigenen Modulen, sobald sie groß genug sind.
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
			"EnhancedInput",
			"Json",
			"SUDS" // Dialoge (Plugins/ThirdParty/SUDS, Install-Plugins.ps1)
		});
	}
}
