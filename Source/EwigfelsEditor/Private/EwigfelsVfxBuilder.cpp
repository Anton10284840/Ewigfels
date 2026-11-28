// Copyright Ewigfels. Alle Rechte vorbehalten.

#include "EwigfelsVfxBuilder.h"

#include "EditorAssetLibrary.h"
#include "Framework/Application/SlateApplication.h"
#include "NiagaraEmitter.h"
#include "NiagaraExternalSystemEditorUtilities.h"
#include "NiagaraScript.h"
#include "NiagaraSystem.h"
#include "NiagaraTypes.h"
#include "NiagaraVariant.h"

namespace
{
	const TCHAR* VfxLogPrefix = TEXT("[Ewigfels VFX]");
}

FString FEwigfelsVfxBuilder::F(float V) { return FString::Printf(TEXT("/Script/Niagara.NiagaraFloat(Value=%f)"), V); }
FString FEwigfelsVfxBuilder::I(int32 V) { return FString::Printf(TEXT("/Script/Niagara.NiagaraInt32(Value=%d)"), V); }
FString FEwigfelsVfxBuilder::B(bool V) { return FString::Printf(TEXT("/Script/Niagara.NiagaraBool(Value=%d)"), V ? -1 : 0); }
FString FEwigfelsVfxBuilder::Col(const FLinearColor& C)
{
	return FString::Printf(TEXT("/Script/CoreUObject.LinearColor(R=%f,G=%f,B=%f,A=%f)"), C.R, C.G, C.B, C.A);
}
FString FEwigfelsVfxBuilder::V3(const FVector3f& V)
{
	return FString::Printf(TEXT("/Script/CoreUObject.Vector3f(X=%f,Y=%f,Z=%f)"), V.X, V.Y, V.Z);
}
FString FEwigfelsVfxBuilder::V2(const FVector2f& V)
{
	return FString::Printf(TEXT("/Script/CoreUObject.Vector2f(X=%f,Y=%f)"), V.X, V.Y);
}
FString FEwigfelsVfxBuilder::Expr(const FString& Hlsl)
{
	return FString::Printf(TEXT("/Script/NiagaraEditor.NiagaraExt_StackInputData_HlslExpression(HlslExpression=\"%s\")"), *Hlsl.ReplaceCharWithEscapedChar());
}

void FEwigfelsVfxBuilder::Collect(const TCHAR* What, const TArray<FText>& ContextErrors)
{
	for (const FText& E : ContextErrors)
	{
		Errors.Add(FString::Printf(TEXT("%s %s: %s"), *Current, What, *E.ToString()));
	}
}

bool FEwigfelsVfxBuilder::Begin(const FString& Name, const FString& Folder)
{
	Current = Name;
	Errors.Reset();
	const FString Path = Folder / Name;
	if (UEditorAssetLibrary::DoesAssetExist(Path))
	{
		// Vorhandenes System leeren statt löschen: Löschen und gleich wieder Anlegen unter demselben Namen ergab
		// eine Datei, die beim Laden abstürzte.
		System = Cast<UNiagaraSystem>(UEditorAssetLibrary::LoadAsset(Path));
		if (!System)
		{
			Errors.Add(FString::Printf(TEXT("%s: vorhandenes Asset ist kein Niagara-System"), *Name));
			return false;
		}
		FNiagaraExternalEditContext Ctx(System);
		FNiagaraExt_SystemSummary Summary;
		UNiagaraExternalEditUtilities::GetSystemSummary(System, Summary, Ctx);
		for (const FNiagaraExt_EmitterSummary& Emitter : Summary.Emitters)
		{
			UNiagaraExternalEditUtilities::RemoveEmitter(FNiagaraExt_StackItemReference(System, Emitter.EmitterName), Ctx);
		}
		for (const FNiagaraExt_UserVariable& Var : Summary.UserVariables)
		{
			UNiagaraExternalEditUtilities::RemoveUserVariable(System, Var, Ctx);
		}
		Collect(TEXT("leeren"), Ctx.Errors);
		return true;
	}
	FNiagaraExternalEditContext Ctx;
	System = UNiagaraExternalEditUtilities::CreateNiagaraSystem(Name, Folder, nullptr, Ctx);
	Collect(TEXT("anlegen"), Ctx.Errors);
	return System != nullptr;
}

FName FEwigfelsVfxBuilder::AddEmitter(const FString& Template, const FString& Name)
{
	UNiagaraEmitter* Asset = LoadObject<UNiagaraEmitter>(nullptr, *FString::Printf(TEXT("/Niagara/DefaultAssets/Templates/Emitters/%s.%s"), *Template, *Template));
	if (!System || !Asset)
	{
		Errors.Add(FString::Printf(TEXT("%s: Vorlage %s fehlt"), *Current, *Template));
		return NAME_None;
	}
	FNiagaraExternalEditContext Ctx(System);
	FNiagaraExt_EmitterTopology Topo;
	UNiagaraExternalEditUtilities::AddEmitter(Asset, FName(*Name), Topo, Ctx);
	Collect(*(TEXT("Emitter ") + Name), Ctx.Errors);
	return Topo.EmitterName;
}

void FEwigfelsVfxBuilder::Set(FName Emitter, const TCHAR* Script, const TCHAR* Module, const TCHAR* Input, const FString& ValueText)
{
	FNiagaraExt_StackItemReference Ref(System, Emitter, FName(Script), FName(Module));
	// "A|B" = Eingang B der dynamischen Eingabe an A
	TArray<FString> Parts;
	FString(Input).ParseIntoArray(Parts, TEXT("|"));
	for (const FString& Part : Parts)
	{
		Ref.InputNameStack.Add(FName(*Part));
	}
	FNiagaraExt_StackInputValue Value;
	const TCHAR* Buffer = *ValueText;
	if (!Value.ImportTextItem(Buffer, PPF_None, nullptr, GLog))
	{
		Errors.Add(FString::Printf(TEXT("%s %s.%s.%s: Wert unlesbar: %s"), *Current, *Emitter.ToString(), Module, Input, *ValueText));
		return;
	}
	FNiagaraExternalEditContext Ctx(Ref);
	UNiagaraExternalEditUtilities::SetStackInputData(Ref, Value, Ctx);
	Collect(*FString::Printf(TEXT("%s.%s.%s"), *Emitter.ToString(), Module, Input), Ctx.Errors);
}

void FEwigfelsVfxBuilder::SetEnum(FName Emitter, const TCHAR* Script, const TCHAR* Module, const TCHAR* Input, const FString& DisplayName)
{
	FNiagaraExt_StackItemReference Ref(System, Emitter, FName(Script), FName(Module));
	Ref.InputNameStack.Add(FName(Input));
	FNiagaraExternalEditContext Ctx(Ref);
	FNiagaraExt_StackInputTopology Topo;
	UNiagaraExternalEditUtilities::GetStackInputTopology(Ref, Topo, Ctx);
	UEnum* Enum = Topo.Type.GetEnum();
	if (!Enum)
	{
		Errors.Add(FString::Printf(TEXT("%s %s.%s.%s: kein Aufzählungs-Eingang"), *Current, *Emitter.ToString(), Module, Input));
		Collect(TEXT("SetEnum"), Ctx.Errors);
		return;
	}
	TArray<FString> Options;
	for (int32 Index = 0; Index < Enum->NumEnums() - 1; ++Index)
	{
		const FString Display = Enum->GetDisplayNameTextByIndex(Index).ToString();
		Options.Add(Display);
		if (Display.Equals(DisplayName, ESearchCase::IgnoreCase))
		{
			Set(Emitter, Script, Module, Input, FString::Printf(
				TEXT("/Script/NiagaraEditor.NiagaraExt_StackInputData_Enum(Enum=\"%s'%s'\",EnumName=\"%s\")"),
				*Enum->GetClass()->GetPathName(), *Enum->GetPathName(), *Enum->GetNameByIndex(Index).ToString()));
			return;
		}
	}
	Errors.Add(FString::Printf(TEXT("%s %s.%s.%s: Option \"%s\" fehlt (möglich: %s)"), *Current, *Emitter.ToString(), Module, Input,
		*DisplayName, *FString::Join(Options, TEXT(", "))));
}

FName FEwigfelsVfxBuilder::AddModule(FName Emitter, const TCHAR* Script, const FString& ModulePath)
{
	const FString Name = FPackageName::GetShortName(ModulePath);
	UNiagaraScript* Asset = LoadObject<UNiagaraScript>(nullptr, *FString::Printf(TEXT("%s.%s"), *ModulePath, *Name));
	if (!Asset)
	{
		Errors.Add(FString::Printf(TEXT("%s: Modul %s fehlt"), *Current, *ModulePath));
		return NAME_None;
	}
	FNiagaraExt_StackItemReference Ref(System, Emitter, FName(Script));
	FNiagaraExternalEditContext Ctx(Ref);
	FNiagaraExt_ModuleTopology Topo;
	UNiagaraExternalEditUtilities::AddModule(Ref, Asset, Topo, Ctx);
	Collect(*(TEXT("Modul ") + Name), Ctx.Errors);
	return Topo.ModuleName;
}

void FEwigfelsVfxBuilder::EnableModule(FName Emitter, const TCHAR* Script, const TCHAR* Module, bool bEnabled)
{
	FNiagaraExt_StackItemReference Ref(System, Emitter, FName(Script), FName(Module));
	FNiagaraExternalEditContext Ctx(Ref);
	UNiagaraExternalEditUtilities::SetModuleEnabled(Ref, bEnabled, Ctx);
	Collect(*FString::Printf(TEXT("%s.%s an/aus"), *Emitter.ToString(), Module), Ctx.Errors);
}

void FEwigfelsVfxBuilder::Renderer(FName Emitter, int32 Index, const FString& Json)
{
	FNiagaraExt_StackItemReference Ref(System, Emitter);
	Ref.RendererIndex = Index;
	FNiagaraExt_RendererData Data;
	Data.PropertyValues = Json;
	FNiagaraExternalEditContext Ctx(Ref);
	UNiagaraExternalEditUtilities::SetRendererData(Ref, Data, Ctx);
	Collect(*FString::Printf(TEXT("%s Renderer %d"), *Emitter.ToString(), Index), Ctx.Errors);
}

void FEwigfelsVfxBuilder::AddRenderer(FName Emitter, UClass* RendererClass, const FString& Json)
{
	FNiagaraExt_StackItemReference Ref(System, Emitter);
	FNiagaraExternalEditContext Ctx(Ref);
	FNiagaraExt_RendererRef New;
	UNiagaraExternalEditUtilities::AddRenderer(Ref, RendererClass, New, Ctx);
	Collect(*FString::Printf(TEXT("%s neuer Renderer"), *Emitter.ToString()), Ctx.Errors);
	if (New.RendererIndex != INDEX_NONE && !Json.IsEmpty())
	{
		Renderer(Emitter, New.RendererIndex, Json);
	}
}

void FEwigfelsVfxBuilder::EmitterData(FName Emitter, const FString& Json)
{
	FNiagaraExt_StackItemReference Ref(System, Emitter);
	FNiagaraExt_EmitterData Data;
	Data.PropertyValues = Json;
	FNiagaraExternalEditContext Ctx(Ref);
	UNiagaraExternalEditUtilities::SetEmitterData(Ref, Data, Ctx);
	Collect(*FString::Printf(TEXT("%s Emitter-Daten"), *Emitter.ToString()), Ctx.Errors);
}

void FEwigfelsVfxBuilder::UserVar(const TCHAR* Name, const FNiagaraTypeDefinition& Type, const void* Data, int32 Size)
{
	FNiagaraExt_UserVariable Var;
	Var.Name = FName(Name);
	Var.Type = Type;
	Var.DefaultValue.Set(Type, FNiagaraVariant(Data, Size));
	FNiagaraExternalEditContext Ctx(System);
	UNiagaraExternalEditUtilities::AddUserVariable(System, Var, Ctx);
	Collect(Name, Ctx.Errors);
}

void FEwigfelsVfxBuilder::UserColor(const TCHAR* Name, const FLinearColor& Default)
{
	UserVar(Name, FNiagaraTypeDefinition::GetColorDef(), &Default, sizeof(FLinearColor));
}

void FEwigfelsVfxBuilder::UserVec3(const TCHAR* Name, const FVector3f& Default)
{
	UserVar(Name, FNiagaraTypeDefinition::GetVec3Def(), &Default, sizeof(FVector3f));
}

bool FEwigfelsVfxBuilder::Finish()
{
	if (System)
	{
		System->RequestCompile(false);
		System->WaitForCompilationComplete();
		{
			// Kompilier-Meldungen lesen; der Kontext (mit eigenem System-ViewModel) muss vor dem Speichern weg sein.
			// Die Stapel-Prüfung (GetStackIssues) wird bewusst nicht benutzt: sie braucht die Editor-Oberfläche und
			// ein danach gespeichertes System ließ sich nicht mehr laden.
			FNiagaraExternalEditContext Ctx(System);
			FNiagaraExt_SystemCompileState State;
			UNiagaraExternalEditUtilities::GetSystemCompileState(System, State, Ctx);
			for (const FNiagaraExt_ScriptCompileInfo& Script : State.Scripts)
			{
				for (const FNiagaraExt_CompileEvent& Event : Script.CompileEvents)
				{
					if (Event.Severity == ENiagaraExt_CompileEventSeverity::Warning || Event.Severity == ENiagaraExt_CompileEventSeverity::Error)
					{
						Errors.Add(FString::Printf(TEXT("%s Kompilieren %s/%s: %s"), *Current, *Script.EmitterName.ToString(),
							*Script.ScriptName.ToString(), *Event.Message.Left(400)));
					}
				}
			}
			Collect(TEXT("Prüfung"), Ctx.Errors);
		}
		UEditorAssetLibrary::SaveLoadedAsset(System, false);
	}
	for (const FString& E : Errors)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s %s"), VfxLogPrefix, *E);
	}
	UE_LOG(LogTemp, Display, TEXT("%s %s gebaut, %d Fehler"), VfxLogPrefix, *Current, Errors.Num());
	return Errors.Num() == 0;
}
