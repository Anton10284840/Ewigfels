// Copyright Ewigfels. Alle Rechte vorbehalten.

using UnrealBuildTool;

/// Editor-Target (das, was du in Visual Studio als "Development Editor" baust).
public class EwigfelsEditorTarget : TargetRules
{
	public EwigfelsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("Ewigfels");
	}
}
