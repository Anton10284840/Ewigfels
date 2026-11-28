// Copyright Ewigfels. Alle Rechte vorbehalten.

using UnrealBuildTool;

/// Editor-Modul: Werkzeuge, die Assets erzeugen (z. B. Zauber-Effekte per Code, Ewigfels.VFX.Erstellen).
/// Wird nur im Editor geladen, nie im fertigen Spiel.
public class EwigfelsEditor : ModuleRules
{
	public EwigfelsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"UnrealEd",
			"Slate",
			"SlateCore",
			"AssetTools",
			"AssetRegistry",
			"Niagara",
			"NiagaraCore",
			"NiagaraEditor",   // UNiagaraExternalEditUtilities (Systeme per Code bauen, UE 5.8)
			"Json",
			"EditorScriptingUtilities",
			"Ewigfels"
		});
	}
}
