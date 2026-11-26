// Copyright Ewigfels. Alle Rechte vorbehalten.

using UnrealBuildTool;

/// Spiel-Target (gepacktes Spiel / Standalone).
public class EwigfelsTarget : TargetRules
{
	public EwigfelsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Ewigfels");
	}
}
