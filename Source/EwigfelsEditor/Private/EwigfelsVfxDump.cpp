// Copyright Ewigfels. Alle Rechte vorbehalten.

// "Ewigfels.VFX.Vorlagen": legt ein Wegwerf-System an, hängt jede Engine-Emitter-Vorlage an und schreibt Module,
// Eingänge und Werte nach Saved/VFX/Vorlagen.txt. Hilft beim Schreiben von Ewigfels.VFX.Erstellen.

#include "NiagaraExternalSystemEditorUtilities.h"
#include "NiagaraEmitter.h"
#include "NiagaraSystem.h"
#include "NiagaraScript.h"
#include "HAL/IConsoleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "EditorAssetLibrary.h"

namespace EwigfelsVfxDump
{
	template <typename T>
	FString StructText(const T& Value)
	{
		FString Out;
		T::StaticStruct()->ExportText(Out, &Value, nullptr, nullptr, PPF_None, nullptr);
		return Out;
	}

	void DumpScript(FString& Out, const FNiagaraExt_ScriptStackTopology& Script, const TArray<FNiagaraExt_ModuleInputValues>& Values)
	{
		Out += FString::Printf(TEXT("  [%s]\n"), *Script.ScriptName.ToString());
		for (const FNiagaraExt_ModuleTopology& Module : Script.Modules)
		{
			Out += FString::Printf(TEXT("    Modul %s (%s)%s\n"), *Module.ModuleName.ToString(),
				Module.ModuleScript ? *Module.ModuleScript->GetPathName() : TEXT("SetParameters"), Module.Enabled ? TEXT("") : TEXT(" AUS"));
			const FNiagaraExt_ModuleInputValues* ModuleValues = Values.FindByPredicate([&](const FNiagaraExt_ModuleInputValues& V) { return V.ModuleName == Module.ModuleName; });
			for (const FNiagaraExt_StackInputTopology& Input : Module.Inputs)
			{
				if (!Input.bIsVisible)
				{
					continue;
				}
				FString Value;
				if (ModuleValues)
				{
					if (const FNiagaraExt_StackInputValueEntry* Entry = ModuleValues->Inputs.FindByPredicate([&](const FNiagaraExt_StackInputValueEntry& E) { return E.Name == Input.Name; }))
					{
						Entry->Value.ExportTextItem(Value, FInstancedStruct(), nullptr, PPF_None, nullptr);
					}
				}
				Out += FString::Printf(TEXT("      %s : %s%s = %s\n"), *Input.Name.ToString(), *Input.Type.GetName(),
					Input.bIsStaticSwitch ? TEXT(" (Schalter)") : TEXT(""), *Value.Left(300));
			}
		}
	}

	void Run(const TArray<FString>& Args)
	{
		TArray<FString> Templates = Args;
		if (Templates.Num() == 0)
		{
			Templates = { TEXT("DynamicBeam"), TEXT("StaticBeam"), TEXT("OmnidirectionalBurst"), TEXT("DirectionalBurst"),
				TEXT("SimpleSpriteBurst"), TEXT("SingleLoopingParticle"), TEXT("Fountain"), TEXT("LocationBasedRibbon"),
				TEXT("HangingParticulates"), TEXT("BlowingParticles"), TEXT("Minimal") };
		}
		const FString TempPath = TEXT("/Game/VFX/_Temp");
		FNiagaraExternalEditContext Ctx;
		UNiagaraSystem* System = UNiagaraExternalEditUtilities::CreateNiagaraSystem(TEXT("NS_Vorlagen_Dump"), TempPath, nullptr, Ctx);
		FString Out;
		if (!System)
		{
			Out = TEXT("System konnte nicht angelegt werden\n");
		}
		for (const FString& Name : Templates)
		{
			if (!System)
			{
				break;
			}
			UNiagaraEmitter* Template = LoadObject<UNiagaraEmitter>(nullptr, *FString::Printf(TEXT("/Niagara/DefaultAssets/Templates/Emitters/%s.%s"), *Name, *Name));
			if (!Template)
			{
				Out += FString::Printf(TEXT("== %s fehlt\n"), *Name);
				continue;
			}
			FNiagaraExternalEditContext AddCtx(System);
			FNiagaraExt_EmitterTopology Topo;
			UNiagaraExternalEditUtilities::AddEmitter(Template, FName(*Name), Topo, AddCtx);
			FNiagaraExternalEditContext ReadCtx(System);
			FNiagaraExt_StackItemReference Ref(System, Topo.EmitterName);
			UNiagaraExternalEditUtilities::GetEmitterTopology(Ref, Topo, ReadCtx);
			TArray<FNiagaraExt_ModuleInputValues> Values;
			UNiagaraExternalEditUtilities::GetEmitterInputValues(Ref, Values, ReadCtx);
			FNiagaraExt_EmitterData Data;
			UNiagaraExternalEditUtilities::GetEmitterData(Ref, Data, ReadCtx);
			Out += FString::Printf(TEXT("== %s (Emitter %s, %s)\n"), *Name, *Topo.EmitterName.ToString(),
				Topo.SimTarget == ENiagaraSimTarget::GPUComputeSim ? TEXT("GPU") : TEXT("CPU"));
			Out += TEXT("  Daten: ") + Data.PropertyValues.Left(600) + TEXT("\n");
			DumpScript(Out, Topo.EmitterSpawnScript, Values);
			DumpScript(Out, Topo.EmitterUpdateScript, Values);
			DumpScript(Out, Topo.ParticleSpawnScript, Values);
			DumpScript(Out, Topo.ParticleUpdateScript, Values);
			for (const FNiagaraExt_RendererRef& R : Topo.Renderers)
			{
				FNiagaraExt_StackItemReference RRef(System, Topo.EmitterName);
				RRef.RendererIndex = R.RendererIndex;
				FNiagaraExt_RendererData RData;
				UNiagaraExternalEditUtilities::GetRendererData(RRef, RData, ReadCtx);
				Out += FString::Printf(TEXT("  Renderer %d %s: %s\n"), R.RendererIndex, *GetNameSafe(R.RendererClass.Get()), *RData.PropertyValues.Left(1500));
			}
			for (const FText& E : AddCtx.Errors) { Out += TEXT("  FEHLER ") + E.ToString() + TEXT("\n"); }
			for (const FText& E : ReadCtx.Errors) { Out += TEXT("  FEHLER ") + E.ToString() + TEXT("\n"); }
		}
		const FString File = FPaths::ProjectSavedDir() / TEXT("VFX/Vorlagen.txt");
		FFileHelper::SaveStringToFile(Out, *File, FFileHelper::EEncodingOptions::ForceUTF8);
		if (System)
		{
			UEditorAssetLibrary::DeleteDirectory(TempPath);
		}
		UE_LOG(LogTemp, Display, TEXT("[Ewigfels VFX] Vorlagen nach %s"), *File);
	}

	/** Ein fertiges System auslesen: Ewigfels.VFX.Auslesen /Game/VFX/Spells/NS_funkenstoss -> Saved/VFX/<Name>.txt */
	void RunAsset(const TArray<FString>& Args)
	{
		for (const FString& Path : Args)
		{
			const FString Name = FPackageName::GetShortName(Path);
			UNiagaraSystem* System = LoadObject<UNiagaraSystem>(nullptr, *FString::Printf(TEXT("%s.%s"), *Path, *Name));
			if (!System)
			{
				UE_LOG(LogTemp, Warning, TEXT("[Ewigfels VFX] %s fehlt"), *Path);
				continue;
			}
			FNiagaraExternalEditContext Ctx(System);
			FNiagaraExt_SystemSummary Summary;
			UNiagaraExternalEditUtilities::GetSystemSummary(System, Summary, Ctx);
			FString Out = FString::Printf(TEXT("== %s\n"), *Name);
			for (const FNiagaraExt_UserVariable& Var : Summary.UserVariables)
			{
				Out += FString::Printf(TEXT("  User %s : %s\n"), *Var.Name.ToString(), *Var.Type.GetName());
			}
			for (const FNiagaraExt_EmitterSummary& Em : Summary.Emitters)
			{
				FNiagaraExt_StackItemReference Ref(System, Em.EmitterName);
				FNiagaraExt_EmitterTopology Topo;
				UNiagaraExternalEditUtilities::GetEmitterTopology(Ref, Topo, Ctx);
				TArray<FNiagaraExt_ModuleInputValues> Values;
				UNiagaraExternalEditUtilities::GetEmitterInputValues(Ref, Values, Ctx);
				Out += FString::Printf(TEXT("-- Emitter %s %s\n"), *Em.EmitterName.ToString(), Em.bEnabled ? TEXT("") : TEXT("AUS"));
				DumpScript(Out, Topo.EmitterUpdateScript, Values);
				DumpScript(Out, Topo.ParticleSpawnScript, Values);
				DumpScript(Out, Topo.ParticleUpdateScript, Values);
				for (const FNiagaraExt_RendererRef& R : Topo.Renderers)
				{
					FNiagaraExt_StackItemReference RRef(System, Em.EmitterName);
					RRef.RendererIndex = R.RendererIndex;
					FNiagaraExt_RendererData RData;
					UNiagaraExternalEditUtilities::GetRendererData(RRef, RData, Ctx);
					Out += FString::Printf(TEXT("  Renderer %d %s: %s\n"), R.RendererIndex, *GetNameSafe(R.RendererClass.Get()), *RData.PropertyValues.Left(3000));
				}
			}
			for (const FText& E : Ctx.Errors) { Out += TEXT("  FEHLER ") + E.ToString() + TEXT("\n"); }
			const FString File = FPaths::ProjectSavedDir() / TEXT("VFX") / (Name + TEXT(".txt"));
			FFileHelper::SaveStringToFile(Out, *File, FFileHelper::EEncodingOptions::ForceUTF8);
			UE_LOG(LogTemp, Display, TEXT("[Ewigfels VFX] %s nach %s"), *Name, *File);
		}
	}

	/** Eingänge von Modul-Assets: Ewigfels.VFX.Modul /Niagara/Modules/Spawn/Velocity/AddVelocityInCone -> Saved/VFX/Module.txt */
	void RunModule(const TArray<FString>& Args)
	{
		FString Out;
		for (const FString& Path : Args)
		{
			const FString Name = FPackageName::GetShortName(Path);
			UNiagaraScript* Script = LoadObject<UNiagaraScript>(nullptr, *FString::Printf(TEXT("%s.%s"), *Path, *Name));
			Out += FString::Printf(TEXT("== %s %s\n"), *Path, Script ? TEXT("") : TEXT("FEHLT"));
			if (!Script)
			{
				continue;
			}
			FNiagaraExternalEditContext Ctx;
			FNiagaraExt_ModuleSchema Schema;
			UNiagaraExternalEditUtilities::GetModuleSchema(Script, Schema, Ctx);
			for (const FNiagaraExt_StackInputSchema& In : Schema.Inputs)
			{
				FString Options;
				if (const UEnum* Enum = In.Type.GetEnum())
				{
					for (int32 I = 0; I < Enum->NumEnums() - 1; ++I)
					{
						Options += Enum->GetDisplayNameTextByIndex(I).ToString() + TEXT(" | ");
					}
				}
				Out += FString::Printf(TEXT("  %s : %s %s\n"), *In.Name.ToString(), *In.Type.GetName(), *Options);
			}
		}
		const FString File = FPaths::ProjectSavedDir() / TEXT("VFX/Module.txt");
		FFileHelper::SaveStringToFile(Out, *File, FFileHelper::EEncodingOptions::ForceUTF8);
		UE_LOG(LogTemp, Display, TEXT("[Ewigfels VFX] Module nach %s"), *File);
	}

	FAutoConsoleCommand GDumpModule(TEXT("Ewigfels.VFX.Modul"),
		TEXT("Eingänge von Niagara-Modul-Assets auslesen -> Saved/VFX/Module.txt"),
		FConsoleCommandWithArgsDelegate::CreateStatic(&RunModule));

	FAutoConsoleCommand GDumpAsset(TEXT("Ewigfels.VFX.Auslesen"),
		TEXT("Fertiges Niagara-System auslesen: Ewigfels.VFX.Auslesen /Game/VFX/Spells/NS_<id> -> Saved/VFX/<Name>.txt"),
		FConsoleCommandWithArgsDelegate::CreateStatic(&RunAsset));

	FAutoConsoleCommand GDump(TEXT("Ewigfels.VFX.Vorlagen"),
		TEXT("Engine-Emitter-Vorlagen auslesen (Module, Eingänge, Werte) -> Saved/VFX/Vorlagen.txt"),
		FConsoleCommandWithArgsDelegate::CreateStatic(&Run));
}
