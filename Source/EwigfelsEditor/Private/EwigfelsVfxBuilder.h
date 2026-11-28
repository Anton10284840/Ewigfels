// Copyright Ewigfels. Alle Rechte vorbehalten.

#pragma once

#include "CoreMinimal.h"

class UNiagaraSystem;
struct FNiagaraTypeDefinition;

/**
 * Baut Niagara-Systeme per Code über die UE-5.8-Schnittstelle UNiagaraExternalEditUtilities.
 * Werte werden als Text gesetzt (gleiches Format, das Ewigfels.VFX.Vorlagen ausgibt).
 * Fehler werden gesammelt und am Ende ins Log geschrieben.
 */
class FEwigfelsVfxBuilder
{
public:
	/** Legt /Game/VFX/Spells/<Name> neu an (altes Asset wird vorher gelöscht). */
	bool Begin(const FString& Name, const FString& Folder = TEXT("/Game/VFX/Spells"));

	/** Hängt eine Engine-Emitter-Vorlage (/Niagara/DefaultAssets/Templates/Emitters/<Template>) an. */
	FName AddEmitter(const FString& Template, const FString& Name);

	/** Setzt einen Modul-Eingang. Script: EmitterUpdateScript, ParticleSpawnScript, ParticleUpdateScript. */
	void Set(FName Emitter, const TCHAR* Script, const TCHAR* Module, const TCHAR* Input, const FString& ValueText);

	/** Wie Set, aber für einen Aufzählungs-Eingang per Anzeigename (z. B. "Once", "Random", "Uniform"). */
	void SetEnum(FName Emitter, const TCHAR* Script, const TCHAR* Module, const TCHAR* Input, const FString& DisplayName);

	/** Fügt ein Modul-Asset (/Niagara/Modules/...) hinten an einen Stapel an, gibt seinen Namen zurück. */
	FName AddModule(FName Emitter, const TCHAR* Script, const FString& ModulePath);

	/** Modul an- oder ausschalten (bleibt im Stapel). */
	void EnableModule(FName Emitter, const TCHAR* Script, const TCHAR* Module, bool bEnabled);

	/** Renderer-Eigenschaften als JSON (Teilmenge reicht, z. B. {"Material":{"refPath":"..."}}). */
	void Renderer(FName Emitter, int32 Index, const FString& Json);

	/** Fügt einen Renderer hinzu (z. B. UNiagaraLightRendererProperties) und setzt seine Eigenschaften. */
	void AddRenderer(FName Emitter, UClass* RendererClass, const FString& Json);

	/** Emitter-Eigenschaften als JSON (z. B. {"bLocalSpace":true}). */
	void EmitterData(FName Emitter, const FString& Json);

	void UserColor(const TCHAR* Name, const FLinearColor& Default);
	void UserVec3(const TCHAR* Name, const FVector3f& Default);

	/** Speichert, schreibt Fehler ins Log. true = ohne Fehler. */
	bool Finish();

	UNiagaraSystem* System = nullptr;
	TArray<FString> Errors;

	// Wert-Texte
	static FString F(float V);
	static FString I(int32 V);
	static FString B(bool V);
	static FString Col(const FLinearColor& C);
	static FString V3(const FVector3f& V);
	static FString V2(const FVector2f& V);
	/** HLSL-Ausdruck, darf Parameter lesen, z. B. TEXT("float4(User.Color.rgb * 8.0, 1.0)"). */
	static FString Expr(const FString& Hlsl);

private:
	void UserVar(const TCHAR* Name, const FNiagaraTypeDefinition& Type, const void* Data, int32 Size);
	void Collect(const TCHAR* What, const TArray<FText>& ContextErrors);
	FString Current;
};

/** Erstellt die Effekte für eine Weise (id) bzw. alle bekannten. Rückgabe: Anzahl gebauter Systeme. */
namespace EwigfelsSpellVfx
{
	int32 Build(const TArray<FString>& Ids);
}
