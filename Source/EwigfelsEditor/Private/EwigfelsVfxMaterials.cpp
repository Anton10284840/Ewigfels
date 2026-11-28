// Copyright Ewigfels. Alle Rechte vorbehalten.

// Effekt-Materialien per Code (/Game/VFX/Materials). Texturen aus "Paragon: Gideon" (Fab Standard License, CREDITS.md).
// - Leuchten (additiv): Farbe = Partikelfarbe × Helligkeit der Textur – HDR-Farben aus Niagara reichen auch bei Tageslicht.
// - Rauch/Nebel (durchscheinend, deckt auch dunkel): Farbe = Partikelfarbe, Deckkraft = Textur × Partikel-Alpha, weicher Übergang
//   zum Boden (Depth Fade).
// - Flipbooks (Rauch, Flammen): Bild wird aus der Lebenszeit des Partikels gewählt (kein SubUV-Modul in Niagara nötig).

#include "EwigfelsVfxMaterials.h"

#include "AssetToolsModule.h"
#include "EditorAssetLibrary.h"
#include "Factories/MaterialFactoryNew.h"
#include "MaterialEditingLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialExpressionDepthFade.h"
#include "Materials/MaterialExpressionFresnel.h"
#include "Materials/MaterialExpressionDotProduct.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionParticleColor.h"
#include "Materials/MaterialExpressionParticleRelativeTime.h"
#include "Materials/MaterialExpressionPower.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Engine/Texture2D.h"

namespace EwigfelsVfxMaterials
{
	const TCHAR* Folder = TEXT("/Game/VFX/Materials");
	const TCHAR* Tex = TEXT("/Game/ParagonGideon/FX/Textures/");

	struct FSpec
	{
		const TCHAR* Name;
		const TCHAR* Texture;  // relativ zu Tex
		bool bSmoke = false;   // durchscheinend statt additiv
		int32 Cols = 1;        // Flipbook-Raster (1 = Einzelbild)
		int32 Rows = 1;
		bool bUseAlpha = false; // Alpha-Kanal der Textur zusätzlich als Maske
		float HotCore = 0.f;   // > 0: helle Stellen werden zusätzlich weißlich (Flammenkern)
		float Gain = 1.f;      // Ausgleich für dunkle Texturen (weiche Kugel ist nur in der Mitte hell)
	};

	const FSpec Specs[] = {
		{ TEXT("M_EwVfx_Glow"),      TEXT("Flares/T_Glow_Sphere_Soft"), false, 1, 1, false, 0.f, 5.f },
		{ TEXT("M_EwVfx_Flare"),     TEXT("Flares/T_Flare_Spikeball"), false, 1, 1, false, 0.f, 3.f },
		{ TEXT("M_EwVfx_Burst"),     TEXT("Masks/T_Radial_Burst") },
		{ TEXT("M_EwVfx_Spark"),     TEXT("Sparks/T_StretchSpark_01") },
		{ TEXT("M_EwVfx_Bolt"),      TEXT("Energy/T_ElectricalBolt_3") },
		{ TEXT("M_EwVfx_Trail"),     TEXT("trails/T_Smoke_Trail_Soft") },
		{ TEXT("M_EwVfx_Fire"),      TEXT("Fire/T_fx_fire_fireball_subUV_01"), false, 8, 16, false, 2.5f },
		{ TEXT("M_EwVfx_Smoke"),     TEXT("Smoke/T_GideonSmoke_01_A"), true, 8, 8 },
		{ TEXT("M_EwVfx_Wisp"),      TEXT("Smoke/T_FluidSmoke_04"), true, 8, 8, true },
		{ TEXT("M_EwVfx_Cloud"),     TEXT("Smoke/T_dust_cloud_01"), true, 1, 1, true },
		{ TEXT("M_EwVfx_GlowCloud"), TEXT("Smoke/T_GideonSmoke_01_A"), false, 8, 8 },
	};

	template <typename T>
	T* Node(UMaterial* M, int32 X, int32 Y)
	{
		return Cast<T>(UMaterialEditingLibrary::CreateMaterialExpression(M, T::StaticClass(), X, Y));
	}

	UMaterial* LoadOrCreate(const FString& Name, TArray<FString>& Errors)
	{
		const FString Path = FString(Folder) / Name;
		if (UEditorAssetLibrary::DoesAssetExist(Path))
		{
			UMaterial* Existing = Cast<UMaterial>(UEditorAssetLibrary::LoadAsset(Path));
			if (Existing)
			{
				UMaterialEditingLibrary::DeleteAllMaterialExpressions(Existing);
			}
			return Existing;
		}
		IAssetTools& Tools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		UMaterial* M = Cast<UMaterial>(Tools.CreateAsset(Name, Folder, UMaterial::StaticClass(), NewObject<UMaterialFactoryNew>()));
		if (!M)
		{
			Errors.Add(Name + TEXT(": ließ sich nicht anlegen"));
		}
		return M;
	}

	bool BuildOne(const FSpec& S, TArray<FString>& Errors)
	{
		UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *(FString(Tex) + S.Texture + TEXT(".") + FPackageName::GetShortName(S.Texture)));
		if (!Texture)
		{
			Errors.Add(FString::Printf(TEXT("%s: Textur %s fehlt"), S.Name, S.Texture));
			return false;
		}
		UMaterial* M = LoadOrCreate(S.Name, Errors);
		if (!M)
		{
			return false;
		}
		M->BlendMode = S.bSmoke ? BLEND_Translucent : BLEND_Additive;
		M->SetShadingModel(MSM_Unlit);
		M->TwoSided = true;
		M->bUsedWithNiagaraSprites = true;
		M->bUsedWithNiagaraRibbons = true;
		M->bUsedWithNiagaraMeshParticles = true;

		// Textur, ggf. Flipbook-Koordinaten aus der Partikel-Lebenszeit
		UMaterialExpressionTextureSample* Sample = Node<UMaterialExpressionTextureSample>(M, -700, 0);
		Sample->Texture = Texture;
		Sample->SamplerType = UMaterialExpressionTextureBase::GetSamplerTypeForTexture(Texture);
		if (S.Cols * S.Rows > 1)
		{
			UMaterialExpressionTextureCoordinate* UV = Node<UMaterialExpressionTextureCoordinate>(M, -1200, -100);
			UMaterialExpressionParticleRelativeTime* Age = Node<UMaterialExpressionParticleRelativeTime>(M, -1200, 0);
			UMaterialExpressionCustom* Flip = Node<UMaterialExpressionCustom>(M, -950, 0);
			Flip->Description = TEXT("Flipbook");
			Flip->OutputType = CMOT_Float2;
			Flip->Code = FString::Printf(TEXT("float frames = %d.0 * %d.0;\n")
				TEXT("float f = floor(saturate(T) * 0.999 * frames);\n")
				TEXT("float2 cell = float2(fmod(f, %d.0), floor(f / %d.0));\n")
				TEXT("return (UV + cell) / float2(%d.0, %d.0);"), S.Cols, S.Rows, S.Cols, S.Cols, S.Cols, S.Rows);
			Flip->Inputs.Reset();
			FCustomInput InUV; InUV.InputName = TEXT("UV"); InUV.Input.Connect(0, UV);
			FCustomInput InT; InT.InputName = TEXT("T"); InT.Input.Connect(0, Age);
			Flip->Inputs.Add(InUV);
			Flip->Inputs.Add(InT);
			Sample->Coordinates.Connect(0, Flip);
		}

		// Helligkeit der Textur (Graustufen oder bunt)
		UMaterialExpressionConstant3Vector* LumWeights = Node<UMaterialExpressionConstant3Vector>(M, -700, 250);
		LumWeights->Constant = FLinearColor(0.3f, 0.59f, 0.11f);
		UMaterialExpressionDotProduct* Lum = Node<UMaterialExpressionDotProduct>(M, -450, 100);
		Lum->A.Connect(0, Sample);   // Ausgang 0 = RGB
		Lum->B.Connect(0, LumWeights);
		UMaterialExpression* Mask = Lum;
		if (S.bUseAlpha)
		{
			UMaterialExpressionMultiply* WithAlpha = Node<UMaterialExpressionMultiply>(M, -300, 150);
			WithAlpha->A.Connect(0, Lum);
			WithAlpha->B.Connect(4, Sample); // Ausgang 4 = A
			Mask = WithAlpha;
		}

		UMaterialExpressionParticleColor* PC = Node<UMaterialExpressionParticleColor>(M, -450, -250);
		UMaterialExpressionComponentMask* PCrgb = Node<UMaterialExpressionComponentMask>(M, -250, -250);
		PCrgb->Input.Connect(0, PC);
		PCrgb->R = PCrgb->G = PCrgb->B = true;
		PCrgb->A = false;

		// Maske × Partikel-Alpha
		UMaterialExpressionMultiply* Strength = Node<UMaterialExpressionMultiply>(M, -150, 100);
		Strength->A.Connect(0, Mask);
		Strength->B.Connect(4, PC); // Ausgang 4 = A
		if (S.Gain != 1.f)
		{
			UMaterialExpressionMultiply* Gain = Node<UMaterialExpressionMultiply>(M, -50, 200);
			Gain->A.Connect(0, Strength);
			Gain->ConstB = S.Gain;
			Strength = Gain;
		}

		UMaterialEditorOnlyData* Data = M->GetEditorOnlyData();
		if (S.bSmoke)
		{
			Data->EmissiveColor.Connect(0, PCrgb);
			UMaterialExpressionDepthFade* Fade = Node<UMaterialExpressionDepthFade>(M, 50, 150);
			Fade->InOpacity.Connect(0, Strength);
			Fade->FadeDistanceDefault = 40.f;
			Data->Opacity.Connect(0, Fade);
		}
		else
		{
			UMaterialExpressionMultiply* Emissive = Node<UMaterialExpressionMultiply>(M, 50, -150);
			Emissive->A.Connect(0, PCrgb);
			Emissive->B.Connect(0, Strength);
			UMaterialExpression* Out = Emissive;
			if (S.HotCore > 0.f)
			{
				// helle Kernbereiche zusätzlich weißlich: + Partikelfarbe × Maske^3 × HotCore
				UMaterialExpressionPower* Pow = Node<UMaterialExpressionPower>(M, -150, 300);
				Pow->Base.Connect(0, Strength);
				Pow->ConstExponent = 3.f;
				UMaterialExpressionMultiply* Hot = Node<UMaterialExpressionMultiply>(M, 50, 300);
				Hot->A.Connect(0, Pow);
				Hot->ConstB = S.HotCore;
				UMaterialExpressionAdd* Sum = Node<UMaterialExpressionAdd>(M, 250, 0);
				Sum->A.Connect(0, Emissive);
				Sum->B.Connect(0, Hot);
				Out = Sum;
			}
			Data->EmissiveColor.Connect(0, Out);
		}

		const TArray<FString> CompileErrors = UMaterialEditingLibrary::RecompileMaterial(M);
		for (const FString& E : CompileErrors)
		{
			Errors.Add(FString::Printf(TEXT("%s: %s"), S.Name, *E));
		}
		UEditorAssetLibrary::SaveLoadedAsset(M, false);
		return CompileErrors.Num() == 0;
	}

	/** Beleuchteter Stein für Brocken-Meshes (Felsfaust, Bassbrecher): Farbe und Normalen aus Gideons Fels-Texturen. */
	bool BuildRock(TArray<FString>& Errors)
	{
		UTexture2D* Diffuse = LoadObject<UTexture2D>(nullptr, TEXT("/Game/ParagonGideon/FX/Textures/Rock/T_RockChunk_D.T_RockChunk_D"));
		UTexture2D* Normal = LoadObject<UTexture2D>(nullptr, TEXT("/Game/ParagonGideon/FX/Textures/Rock/T_RockChunk_N.T_RockChunk_N"));
		UMaterial* M = Diffuse && Normal ? LoadOrCreate(TEXT("M_EwVfx_Rock"), Errors) : nullptr;
		if (!M)
		{
			Errors.Add(TEXT("M_EwVfx_Rock: Fels-Texturen fehlen"));
			return false;
		}
		M->BlendMode = BLEND_Opaque;
		M->SetShadingModel(MSM_DefaultLit);
		M->TwoSided = false;
		M->bUsedWithNiagaraMeshParticles = true;
		M->bUsedWithNiagaraSprites = true;
		UMaterialExpressionTextureSample* D = Node<UMaterialExpressionTextureSample>(M, -500, -100);
		D->Texture = Diffuse;
		D->SamplerType = UMaterialExpressionTextureBase::GetSamplerTypeForTexture(Diffuse);
		UMaterialExpressionTextureSample* N = Node<UMaterialExpressionTextureSample>(M, -500, 200);
		N->Texture = Normal;
		N->SamplerType = UMaterialExpressionTextureBase::GetSamplerTypeForTexture(Normal);
		// Partikelfarbe tönt den Stein leicht (z. B. erdig oder dunkel)
		UMaterialExpressionParticleColor* PC = Node<UMaterialExpressionParticleColor>(M, -500, -300);
		UMaterialExpressionComponentMask* PCrgb = Node<UMaterialExpressionComponentMask>(M, -300, -300);
		PCrgb->Input.Connect(0, PC);
		PCrgb->R = PCrgb->G = PCrgb->B = true;
		PCrgb->A = false;
		UMaterialExpressionMultiply* Tint = Node<UMaterialExpressionMultiply>(M, -150, -150);
		Tint->A.Connect(0, D);
		Tint->B.Connect(0, PCrgb);
		UMaterialEditorOnlyData* Data = M->GetEditorOnlyData();
		Data->BaseColor.Connect(0, Tint);
		Data->Normal.Connect(0, N);
		UMaterialExpressionConstant* Rough = Node<UMaterialExpressionConstant>(M, -150, 100);
		Rough->R = 0.85f;
		Data->Roughness.Connect(0, Rough);
		const TArray<FString> CompileErrors = UMaterialEditingLibrary::RecompileMaterial(M);
		for (const FString& E : CompileErrors)
		{
			Errors.Add(TEXT("M_EwVfx_Rock: ") + E);
		}
		UEditorAssetLibrary::SaveLoadedAsset(M, false);
		return CompileErrors.Num() == 0;
	}

	/** Schild-Glocke: additiv, Rand leuchtet stark (Fresnel), Mitte fast durchsichtig; Farbe = Partikelfarbe (User.Color). */
	bool BuildShieldFresnel(TArray<FString>& Errors)
	{
		UMaterial* M = LoadOrCreate(TEXT("M_EwVfx_ShieldFresnel"), Errors);
		if (!M)
		{
			return false;
		}
		M->BlendMode = BLEND_Additive;
		M->SetShadingModel(MSM_Unlit);
		M->TwoSided = false;
		M->bUsedWithNiagaraMeshParticles = true;
		M->bUsedWithNiagaraSprites = true;
		UMaterialExpressionFresnel* Rim = Node<UMaterialExpressionFresnel>(M, -500, 100);
		Rim->Exponent = 3.f;
		Rim->BaseReflectFraction = 0.06f;
		// leichtes Schimmern: Gideons weiche Wolkentextur über die Kugel (Welt-Koordinaten wären teurer)
		UTexture2D* Noise = LoadObject<UTexture2D>(nullptr, TEXT("/Game/ParagonGideon/FX/Textures/Tile/Noise/T_Soft_Noise.T_Soft_Noise"));
		UMaterialExpression* Shimmer = nullptr;
		if (Noise)
		{
			UMaterialExpressionTextureSample* N = Node<UMaterialExpressionTextureSample>(M, -500, 300);
			N->Texture = Noise;
			N->SamplerType = UMaterialExpressionTextureBase::GetSamplerTypeForTexture(Noise);
			UMaterialExpressionMultiply* NScale = Node<UMaterialExpressionMultiply>(M, -300, 300);
			NScale->A.Connect(1, N); // R
			NScale->ConstB = 0.6f;
			UMaterialExpressionAdd* NAdd = Node<UMaterialExpressionAdd>(M, -150, 300);
			NAdd->A.Connect(0, NScale);
			NAdd->ConstB = 0.4f;
			Shimmer = NAdd;
		}
		UMaterialExpressionParticleColor* PC = Node<UMaterialExpressionParticleColor>(M, -500, -200);
		UMaterialExpressionComponentMask* PCrgb = Node<UMaterialExpressionComponentMask>(M, -300, -200);
		PCrgb->Input.Connect(0, PC);
		PCrgb->R = PCrgb->G = PCrgb->B = true;
		PCrgb->A = false;
		UMaterialExpressionMultiply* RimA = Node<UMaterialExpressionMultiply>(M, -300, 100);
		RimA->A.Connect(0, Rim);
		RimA->B.Connect(4, PC); // Alpha der Partikelfarbe = Stärke
		UMaterialExpression* Mask = RimA;
		if (Shimmer)
		{
			UMaterialExpressionMultiply* WithShimmer = Node<UMaterialExpressionMultiply>(M, -100, 150);
			WithShimmer->A.Connect(0, RimA);
			WithShimmer->B.Connect(0, Shimmer);
			Mask = WithShimmer;
		}
		UMaterialExpressionMultiply* Emissive = Node<UMaterialExpressionMultiply>(M, 100, 0);
		Emissive->A.Connect(0, PCrgb);
		Emissive->B.Connect(0, Mask);
		M->GetEditorOnlyData()->EmissiveColor.Connect(0, Emissive);
		const TArray<FString> CompileErrors = UMaterialEditingLibrary::RecompileMaterial(M);
		for (const FString& E : CompileErrors)
		{
			Errors.Add(TEXT("M_EwVfx_ShieldFresnel: ") + E);
		}
		UEditorAssetLibrary::SaveLoadedAsset(M, false);
		return CompileErrors.Num() == 0;
	}

	int32 BuildAll()
	{
		TArray<FString> Errors;
		int32 Ok = BuildRock(Errors) ? 1 : 0;
		Ok += BuildShieldFresnel(Errors) ? 1 : 0;
		for (const FSpec& S : Specs)
		{
			Ok += BuildOne(S, Errors) ? 1 : 0;
		}
		for (const FString& E : Errors)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Ewigfels VFX] Material %s"), *E);
		}
		UE_LOG(LogTemp, Display, TEXT("[Ewigfels VFX] Materialien: %d von %d ohne Fehler"), Ok, UE_ARRAY_COUNT(Specs) + 2);
		return Ok;
	}

	FString Path(const TCHAR* Name)
	{
		return FString::Printf(TEXT("%s/%s.%s"), Folder, Name, Name);
	}
}
