// Copyright Ewigfels. Alle Rechte vorbehalten.

// Zauber-Effekte per Code (docs/VFX_PLAN.md). "Ewigfels.VFX.Erstellen [id ...]" baut /Game/VFX/Spells/NS_<id> (+_Impact).
// Farben kommen zur Laufzeit über User.Color / User.GlowColor / User.SparkColor, Strahlen enden bei User.BeamEnd.
// Content/ ist nicht im Git: dieser Code IST die Quelle der Effekte, die Assets sind reproduzierbar.

#include "EwigfelsVfxBuilder.h"
#include "EwigfelsVfxMaterials.h"
#include "EditorAssetLibrary.h"

#include "HAL/IConsoleManager.h"

namespace EwigfelsSpellVfx
{
	using B = FEwigfelsVfxBuilder;

	const TCHAR* ES = TEXT("EmitterUpdateScript");
	const TCHAR* PS = TEXT("ParticleSpawnScript");
	const TCHAR* PU = TEXT("ParticleUpdateScript");

	void StandardUser(B& V, const FLinearColor& Color, const FLinearColor& Glow, const FLinearColor& Spark)
	{
		V.UserColor(TEXT("User.Color"), Color);
		V.UserColor(TEXT("User.GlowColor"), Glow);
		V.UserColor(TEXT("User.SparkColor"), Spark);
	}

	/** HDR-Farbe aus einem User-Parameter: Helligkeit Mul, Deckkraft Alpha. */
	FString Hdr(const TCHAR* Param, float Mul, float Alpha = 1.f)
	{
		return B::Expr(FString::Printf(TEXT("float4(%s.rgb * %.2f, %.2f)"), Param, Mul, Alpha));
	}

	/** Einmal-Emitter: läuft genau einmal, Dauer Seconds. */
	void Once(B& V, FName E, float Seconds)
	{
		V.SetEnum(E, ES, TEXT("EmitterState"), TEXT("Loop Behavior"), TEXT("Once"));
		V.Set(E, ES, TEXT("EmitterState"), TEXT("Loop Duration"), B::F(Seconds));
	}

	/** Zuckender Strahl vom Systemursprung zu User.BeamEnd. */
	FName Beam(B& V, const TCHAR* Name, int32 Points, float Width, float Life, const FString& Color, float Jitter = 0.f, float JitterDelay = 0.03f)
	{
		const FName E = V.AddEmitter(TEXT("DynamicBeam"), Name);
		Once(V, E, Life + 0.05f);
		// Start relativ zum System (= Hand), Ende absolut in Weltkoordinaten (User.BeamEnd)
		V.Set(E, ES, TEXT("BeamEmitterSetup"), TEXT("Absolute Beam Start"), B::B(false));
		V.Set(E, ES, TEXT("BeamEmitterSetup"), TEXT("Beam Start"), B::V3(FVector3f::ZeroVector));
		V.Set(E, ES, TEXT("BeamEmitterSetup"), TEXT("Beam End"), B::Expr(TEXT("User.BeamEnd")));
		V.Set(E, ES, TEXT("BeamEmitterSetup"), TEXT("Absolute Beam End"), B::B(true));
		V.Set(E, ES, TEXT("SpawnBurst_Instantaneous"), TEXT("Spawn Count"), B::I(Points));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime"), B::F(Life));
		V.Set(E, PS, TEXT("BeamWidth"), TEXT("Beam Width"), B::F(Width));
		V.Set(E, PU, TEXT("Color"), TEXT("Color"), Color);
		// Scharfe Knicke statt weicher Kurven
		V.Renderer(E, 0, TEXT("{\"TessellationMode\":\"Disabled\"}"));
		if (Jitter > 0.f)
		{
			// Zickzack: jeder Punkt springt alle JitterDelay Sekunden an eine neue Zufallsstelle (Blitz-Flackern).
			// UpdateBeam würde die Punkte jedes Bild zurück auf die gerade Linie legen, darum aus.
			V.EnableModule(E, PU, TEXT("UpdateBeam"), false);
			const FName J = V.AddModule(E, PU, TEXT("/Niagara/Modules/Update/Position/JitterPosition"));
			V.Set(E, PU, *J.ToString(), TEXT("Jitter Amount"), B::F(Jitter));
			V.Set(E, PU, *J.ToString(), TEXT("Jitter Delay"), B::F(JitterDelay));
		}
		return E;
	}

	/** Einzelner kurzer Leuchtfleck (Mündungs- oder Einschlagsblitz). */
	FName Flash(B& V, const TCHAR* Name, float Size, float Life, const FString& Color)
	{
		const FName E = V.AddEmitter(TEXT("SimpleSpriteBurst"), Name);
		Once(V, E, Life + 0.05f);
		V.Set(E, ES, TEXT("SpawnBurst_Instantaneous"), TEXT("Spawn Count"), B::I(1));
		V.SetEnum(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Mode"), TEXT("Direct Set"));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime"), B::F(Life));
		V.SetEnum(E, PS, TEXT("InitializeParticle"), TEXT("Color Mode"), TEXT("Direct Set"));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.SetEnum(E, PS, TEXT("InitializeParticle"), TEXT("Sprite Size Mode"), TEXT("Uniform"));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size"), B::F(Size));
		return E;
	}

	/** Funkenregen: schnelle, in Flugrichtung gestreckte Funken mit Schwerkraft. */
	FName Sparks(B& V, const TCHAR* Name, int32 Count, float SpeedMin, float SpeedMax, float Size, const FString& Color)
	{
		const FName E = V.AddEmitter(TEXT("DirectionalBurst"), Name);
		Once(V, E, 1.2f);
		V.Set(E, ES, TEXT("SpawnBurst_Instantaneous"), TEXT("Spawn Count"), B::I(Count));
		V.SetEnum(E, PS, TEXT("InitializeParticle"), TEXT("Color Mode"), TEXT("Direct Set"));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Minimum"), B::F(SpeedMin));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Maximum"), B::F(SpeedMax));
		V.Renderer(E, 0, FString::Printf(TEXT("{\"Alignment\":\"VelocityAligned\"}")));
		return E;
	}

	/** Langsame Glutpunkte, die nachglimmen. */
	FName Embers(B& V, const TCHAR* Name, int32 Count, float Radius, const FString& Color)
	{
		const FName E = V.AddEmitter(TEXT("OmnidirectionalBurst"), Name);
		Once(V, E, 1.6f);
		V.Set(E, ES, TEXT("SpawnBurst_Instantaneous"), TEXT("Spawn Count"), B::I(Count));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Min"), B::F(0.5f));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Max"), B::F(1.3f));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Min"), B::F(1.5f));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Max"), B::F(3.5f));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(Radius));
		V.Set(E, PU, TEXT("GravityForce"), TEXT("Gravity"), B::V3(FVector3f(0.f, 0.f, -250.f)));
		return E;
	}

	/** Partikelwolke aus einem Burst (Rauch, Frostnebel, Flammenstoß): große weiche Sprites. */
	FName Puff(B& V, const TCHAR* Name, int32 Count, float SizeMin, float SizeMax, float LifeMin, float LifeMax, float SpeedMin, float SpeedMax,
		float GravityZ, const FString& Color)
	{
		const FName E = V.AddEmitter(TEXT("OmnidirectionalBurst"), Name);
		Once(V, E, LifeMax + 0.2f);
		V.Set(E, ES, TEXT("SpawnBurst_Instantaneous"), TEXT("Spawn Count"), B::I(Count));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Min"), B::F(LifeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Max"), B::F(LifeMax));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Min"), B::F(SizeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Max"), B::F(SizeMax));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(SizeMin * 0.5f));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Minimum"), B::F(SpeedMin));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Maximum"), B::F(SpeedMax));
		V.Set(E, PU, TEXT("GravityForce"), TEXT("Gravity"), B::V3(FVector3f(0.f, 0.f, GravityZ)));
		return E;
	}

	/** Dauerstrom hinter einem Geschoss (Weltraum, bleibt liegen): Rate pro Sekunde statt Burst. */
	FName Stream(B& V, const TCHAR* Name, float Rate, float SizeMin, float SizeMax, float LifeMin, float LifeMax, float SpeedMin, float SpeedMax,
		float GravityZ, const FString& Color, float Radius = 4.f)
	{
		const FName E = V.AddEmitter(TEXT("OmnidirectionalBurst"), Name);
		V.SetEnum(E, ES, TEXT("EmitterState"), TEXT("Loop Behavior"), TEXT("Infinite"));
		V.EnableModule(E, ES, TEXT("SpawnBurst_Instantaneous"), false);
		const FName R = V.AddModule(E, ES, TEXT("/Niagara/Modules/Emitter/SpawnRate"));
		V.Set(E, ES, *R.ToString(), TEXT("SpawnRate"), B::F(Rate));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Min"), B::F(LifeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Max"), B::F(LifeMax));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Min"), B::F(SizeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Max"), B::F(SizeMax));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(Radius));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Minimum"), B::F(SpeedMin));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Maximum"), B::F(SpeedMax));
		V.Set(E, PU, TEXT("GravityForce"), TEXT("Gravity"), B::V3(FVector3f(0.f, 0.f, GravityZ)));
		return E;
	}

	/** Leuchtender Kern, der am Geschoss klebt (lokaler Raum, Endlosschleife). */
	FName Core(B& V, const TCHAR* Name, float Size, const FString& Color)
	{
		const FName E = V.AddEmitter(TEXT("SingleLoopingParticle"), Name);
		V.EmitterData(E, TEXT("{\"bLocalSpace\":true}"));
		V.SetEnum(E, PS, TEXT("InitializeParticle"), TEXT("Color Mode"), TEXT("Direct Set"));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size"), B::F(Size));
		return E;
	}

	/** Leuchtband hinter dem Geschoss (Band aus der Flugbahn, Weltraum). */
	FName Trail(B& V, const TCHAR* Name, float Rate, float Width, float Life, const FString& Color)
	{
		const FName E = V.AddEmitter(TEXT("LocationBasedRibbon"), Name);
		V.SetEnum(E, ES, TEXT("EmitterState"), TEXT("Loop Behavior"), TEXT("Infinite"));
		const FName R = V.AddModule(E, ES, TEXT("/Niagara/Modules/Emitter/SpawnRate"));
		V.Set(E, ES, *R.ToString(), TEXT("SpawnRate"), B::F(Rate));
		V.SetEnum(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Mode"), TEXT("Direct Set"));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime"), B::F(Life));
		V.SetEnum(E, PS, TEXT("InitializeParticle"), TEXT("Color Mode"), TEXT("Direct Set"));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.SetEnum(E, PS, TEXT("InitializeParticle"), TEXT("Ribbon Width Mode"), TEXT("Direct Set"));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Ribbon Width"), B::F(Width));
		return E;
	}

	/** Material aus /Game/VFX/Materials auf Renderer 0 legen; Funken-Material richtet die Sprites an der Flugrichtung aus. */
	FName Mat(B& V, FName E, const TCHAR* Material)
	{
		const FString Path = EwigfelsVfxMaterials::Path(Material);
		const bool bStretch = FCString::Strstr(Material, TEXT("Spark")) != nullptr;
		V.Renderer(E, 0, FString::Printf(TEXT("{\"Material\":{\"refPath\":\"%s\"}%s}"), *Path, bStretch ? TEXT(",\"Alignment\":\"VelocityAligned\"") : TEXT("")));
		return E;
	}

	const FString Smoke = B::Col(FLinearColor(0.05f, 0.045f, 0.04f, 0.45f));

	// ---------------------------------------------------------------- Funkenstoß (Scintivox)

	bool Funkenstoss()
	{
		const FLinearColor Core(0.85f, 0.9f, 1.f), Glow(0.35f, 0.55f, 1.f), Spark(1.f, 0.85f, 0.45f);
		bool bOk = true;
		{
			B V;
			if (!V.Begin(TEXT("NS_funkenstoss")))
			{
				return false;
			}
			StandardUser(V, Core, Glow, Spark);
			V.UserVec3(TEXT("User.BeamEnd"), FVector3f(800.f, 0.f, 0.f));
			// Zittern addiert sich (Zufallsweg): kleine Schritte, damit der Blitz an Hand und Ziel bleibt
			Mat(V, Beam(V, TEXT("Saum"), 12, 20.f, 0.16f, Hdr(TEXT("User.GlowColor"), 12.f, 0.45f), 12.f, 0.05f), TEXT("M_EwVfx_Trail"));
			Beam(V, TEXT("Kern"), 18, 5.f, 0.16f, Hdr(TEXT("User.Color"), 60.f), 16.f, 0.04f);
			Mat(V, Beam(V, TEXT("Ast"), 12, 1.8f, 0.10f, Hdr(TEXT("User.SparkColor"), 40.f), 30.f, 0.03f), TEXT("M_EwVfx_Bolt"));
			Mat(V, Flash(V, TEXT("Muendung"), 18.f, 0.07f, Hdr(TEXT("User.GlowColor"), 12.f, 0.8f)), TEXT("M_EwVfx_Flare"));
			bOk &= V.Finish();
		}
		{
			B V;
			if (!V.Begin(TEXT("NS_funkenstoss_Impact")))
			{
				return false;
			}
			StandardUser(V, Core, Glow, Spark);
			Mat(V, Flash(V, TEXT("Blitz"), 90.f, 0.10f, Hdr(TEXT("User.GlowColor"), 25.f)), TEXT("M_EwVfx_Burst"));
			Mat(V, Sparks(V, TEXT("Funken"), 36, 350.f, 1100.f, 2.f, Hdr(TEXT("User.SparkColor"), 25.f)), TEXT("M_EwVfx_Spark"));
			Mat(V, Embers(V, TEXT("Glut"), 10, 6.f, Hdr(TEXT("User.SparkColor"), 8.f)), TEXT("M_EwVfx_Glow"));
			bOk &= V.Finish();
		}
		return bOk;
	}

	// ---------------------------------------------------------------- Frostweise (Gelucantus): Eislanze mit Frostnebel

	bool Frostweise()
	{
		const FLinearColor C(0.75f, 0.92f, 1.f), G(0.35f, 0.75f, 1.f), S(0.85f, 0.97f, 1.f);
		bool bOk = true;
		{
			B V;
			if (!V.Begin(TEXT("NS_frostweise"))) { return false; }
			StandardUser(V, C, G, S);
			Mat(V, Core(V, TEXT("Kern"), 42.f, Hdr(TEXT("User.Color"), 12.f)), TEXT("M_EwVfx_Glow"));
			Mat(V, Core(V, TEXT("Halo"), 110.f, Hdr(TEXT("User.GlowColor"), 3.f, 0.4f)), TEXT("M_EwVfx_Glow"));
			Mat(V, Trail(V, TEXT("Spur"), 60.f, 14.f, 0.35f, Hdr(TEXT("User.GlowColor"), 4.f, 0.5f)), TEXT("M_EwVfx_Trail"));
			Mat(V, Stream(V, TEXT("Nebel"), 45.f, 18.f, 36.f, 0.5f, 0.9f, 10.f, 40.f, 20.f, Hdr(TEXT("User.GlowColor"), 1.2f, 0.25f), 8.f), TEXT("M_EwVfx_Cloud"));
			Mat(V, Stream(V, TEXT("Kristalle"), 70.f, 1.2f, 3.f, 0.4f, 0.9f, 40.f, 160.f, -300.f, Hdr(TEXT("User.SparkColor"), 18.f), 10.f), TEXT("M_EwVfx_Glow"));
			bOk &= V.Finish();
		}
		{
			B V;
			if (!V.Begin(TEXT("NS_frostweise_Impact"))) { return false; }
			StandardUser(V, C, G, S);
			Mat(V, Flash(V, TEXT("Blitz"), 110.f, 0.12f, Hdr(TEXT("User.GlowColor"), 14.f)), TEXT("M_EwVfx_Burst"));
			Mat(V, Sparks(V, TEXT("Splitter"), 46, 300.f, 900.f, 2.5f, Hdr(TEXT("User.SparkColor"), 20.f)), TEXT("M_EwVfx_Spark"));
			Mat(V, Puff(V, TEXT("Frostwolke"), 14, 40.f, 80.f, 0.7f, 1.3f, 60.f, 160.f, 30.f, Hdr(TEXT("User.GlowColor"), 1.4f, 0.3f)), TEXT("M_EwVfx_Cloud"));
			Mat(V, Embers(V, TEXT("Glitzer"), 24, 30.f, Hdr(TEXT("User.SparkColor"), 12.f)), TEXT("M_EwVfx_Glow"));
			bOk &= V.Finish();
		}
		return bOk;
	}

	// ---------------------------------------------------------------- Glutweise (Ardesonus): Glutkugel mit Flammenschweif

	bool Glutweise()
	{
		const FLinearColor C(1.f, 0.55f, 0.15f), G(1.f, 0.3f, 0.05f), S(1.f, 0.75f, 0.3f);
		bool bOk = true;
		{
			B V;
			if (!V.Begin(TEXT("NS_glutweise"))) { return false; }
			StandardUser(V, C, G, S);
			Mat(V, Core(V, TEXT("Kern"), 48.f, Hdr(TEXT("User.Color"), 16.f)), TEXT("M_EwVfx_Glow"));
			Mat(V, Core(V, TEXT("Halo"), 130.f, Hdr(TEXT("User.GlowColor"), 4.f, 0.45f)), TEXT("M_EwVfx_Glow"));
			Mat(V, Stream(V, TEXT("Flammen"), 140.f, 20.f, 42.f, 0.2f, 0.45f, 20.f, 70.f, 180.f, Hdr(TEXT("User.Color"), 9.f, 0.7f), 9.f), TEXT("M_EwVfx_Fire"));
			Mat(V, Stream(V, TEXT("Rauch"), 30.f, 20.f, 42.f, 0.7f, 1.4f, 10.f, 40.f, 90.f, Smoke, 10.f), TEXT("M_EwVfx_Smoke"));
			Mat(V, Stream(V, TEXT("Funken"), 40.f, 1.2f, 2.8f, 0.3f, 0.8f, 80.f, 260.f, -200.f, Hdr(TEXT("User.SparkColor"), 25.f), 8.f), TEXT("M_EwVfx_Spark"));
			bOk &= V.Finish();
		}
		{
			B V;
			if (!V.Begin(TEXT("NS_glutweise_Impact"))) { return false; }
			StandardUser(V, C, G, S);
			Mat(V, Flash(V, TEXT("Blitz"), 140.f, 0.14f, Hdr(TEXT("User.Color"), 16.f)), TEXT("M_EwVfx_Burst"));
			Mat(V, Puff(V, TEXT("Flammenstoss"), 22, 30.f, 60.f, 0.25f, 0.55f, 150.f, 420.f, 150.f, Hdr(TEXT("User.Color"), 8.f, 0.7f)), TEXT("M_EwVfx_Fire"));
			Mat(V, Puff(V, TEXT("Rauch"), 12, 50.f, 100.f, 1.0f, 2.0f, 40.f, 140.f, 60.f, Smoke), TEXT("M_EwVfx_Smoke"));
			Mat(V, Sparks(V, TEXT("Funken"), 50, 250.f, 900.f, 2.5f, Hdr(TEXT("User.SparkColor"), 25.f)), TEXT("M_EwVfx_Spark"));
			Mat(V, Embers(V, TEXT("Glut"), 22, 25.f, Hdr(TEXT("User.SparkColor"), 12.f)), TEXT("M_EwVfx_Glow"));
			bOk &= V.Finish();
		}
		return bOk;
	}

	// ---------------------------------------------------------------- Sturmweise (Tempestivox): dicker, springender Blitz

	bool Sturmweise()
	{
		const FLinearColor C(0.85f, 0.8f, 1.f), G(0.55f, 0.35f, 1.f), S(0.7f, 0.75f, 1.f);
		bool bOk = true;
		{
			B V;
			if (!V.Begin(TEXT("NS_sturmweise"))) { return false; }
			StandardUser(V, C, G, S);
			V.UserVec3(TEXT("User.BeamEnd"), FVector3f(800.f, 0.f, 0.f));
			Mat(V, Beam(V, TEXT("Saum"), 14, 34.f, 0.2f, Hdr(TEXT("User.GlowColor"), 14.f, 0.5f), 16.f, 0.05f), TEXT("M_EwVfx_Trail"));
			Beam(V, TEXT("Kern"), 20, 8.f, 0.2f, Hdr(TEXT("User.Color"), 70.f), 22.f, 0.04f);
			Beam(V, TEXT("Kern2"), 16, 4.f, 0.14f, Hdr(TEXT("User.Color"), 50.f), 28.f, 0.035f);
			Mat(V, Beam(V, TEXT("Ast1"), 12, 2.f, 0.12f, Hdr(TEXT("User.SparkColor"), 40.f), 40.f, 0.03f), TEXT("M_EwVfx_Bolt"));
			Mat(V, Beam(V, TEXT("Ast2"), 10, 1.6f, 0.1f, Hdr(TEXT("User.SparkColor"), 40.f), 50.f, 0.03f), TEXT("M_EwVfx_Bolt"));
			Mat(V, Flash(V, TEXT("Muendung"), 26.f, 0.08f, Hdr(TEXT("User.GlowColor"), 14.f, 0.8f)), TEXT("M_EwVfx_Flare"));
			bOk &= V.Finish();
		}
		{
			B V;
			if (!V.Begin(TEXT("NS_sturmweise_Impact"))) { return false; }
			StandardUser(V, C, G, S);
			Mat(V, Flash(V, TEXT("Blitz"), 130.f, 0.12f, Hdr(TEXT("User.GlowColor"), 25.f)), TEXT("M_EwVfx_Burst"));
			Mat(V, Sparks(V, TEXT("Funken"), 60, 400.f, 1300.f, 2.f, Hdr(TEXT("User.SparkColor"), 30.f)), TEXT("M_EwVfx_Spark"));
			Mat(V, Embers(V, TEXT("Knistern"), 30, 40.f, Hdr(TEXT("User.Color"), 20.f)), TEXT("M_EwVfx_Glow"));
			bOk &= V.Finish();
		}
		return bOk;
	}

	int32 Build(const TArray<FString>& Ids)
	{
		struct FEntry { const TCHAR* Id; bool (*Fn)(); };
		const FEntry All[] = {
			{ TEXT("funkenstoss"), &Funkenstoss }, { TEXT("frostweise"), &Frostweise },
			{ TEXT("glutweise"), &Glutweise }, { TEXT("sturmweise"), &Sturmweise } };
		// Materialien zuerst, wenn eins fehlt (sonst gezielt: Ewigfels.VFX.Materialien)
		if (!UEditorAssetLibrary::DoesAssetExist(TEXT("/Game/VFX/Materials/M_EwVfx_GlowCloud")) || Ids.Contains(TEXT("materialien")))
		{
			EwigfelsVfxMaterials::BuildAll();
		}
		int32 Built = 0;
		for (const FEntry& Entry : All)
		{
			if (Ids.Num() == 0 || Ids.Contains(Entry.Id))
			{
				Entry.Fn();
				++Built;
			}
		}
		UE_LOG(LogTemp, Display, TEXT("[Ewigfels VFX] fertig, %d Weise(n)"), Built);
		return Built;
	}

	FAutoConsoleCommand GMaterials(TEXT("Ewigfels.VFX.Materialien"),
		TEXT("Effekt-Materialien /Game/VFX/Materials/M_EwVfx_* neu bauen"),
		FConsoleCommandDelegate::CreateLambda([]() { EwigfelsVfxMaterials::BuildAll(); }));

	FAutoConsoleCommand GBuild(TEXT("Ewigfels.VFX.Erstellen"),
		TEXT("Zauber-Effekte bauen: Ewigfels.VFX.Erstellen [id ...] (ohne id: alle) -> /Game/VFX/Spells/NS_<id>"),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { Build(Args); }));
}
