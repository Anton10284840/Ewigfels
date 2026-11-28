// Copyright Ewigfels. Alle Rechte vorbehalten.

// Zauber-Effekte per Code (docs/VFX_PLAN.md). "Ewigfels.VFX.Erstellen [id ...]" baut /Game/VFX/Spells/NS_<id> (+_Impact).
// Farben kommen zur Laufzeit über User.Color / User.GlowColor / User.SparkColor, Strahlen enden bei User.BeamEnd.
// Content/ ist nicht im Git: dieser Code IST die Quelle der Effekte, die Assets sind reproduzierbar.

#include "EwigfelsVfxBuilder.h"
#include "EwigfelsVfxMaterials.h"
#include "EditorAssetLibrary.h"
#include "NiagaraMeshRendererProperties.h"

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

	// ---------------------------------------------------------------- Bausteine für Kegel-, Flächen- und Selbst-Weisen

	/** Partikel fliegen im Kegel nach vorn (lokale X-Achse des Systems = Blickrichtung), erreichen etwa Range. */
	FName ConeBurst(B& V, const TCHAR* Name, int32 Count, float Range, float AngleDeg, float SizeMin, float SizeMax, float LifeMin, float LifeMax,
		const FString& Color, const TCHAR* Material, float Height = 60.f, float Drag = 0.6f)
	{
		const FName E = V.AddEmitter(TEXT("OmnidirectionalBurst"), Name);
		Once(V, E, LifeMax + 0.2f);
		V.Set(E, ES, TEXT("SpawnBurst_Instantaneous"), TEXT("Spawn Count"), B::I(Count));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Min"), B::F(LifeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Max"), B::F(LifeMax));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Min"), B::F(SizeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Max"), B::F(SizeMax));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(25.f));
		V.SetEnum(E, PS, TEXT("ShapeLocation"), TEXT("Offset Mode"), TEXT("Default"));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Offset"), B::V3(FVector3f(60.f, 0.f, Height)));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Offset Coordinate Space"), TEXT("/Script/NiagaraEditor.NiagaraExt_StackInputData_Enum(Enum=\"/Script/CoreUObject.Enum'/Script/Niagara.ENiagaraCoordinateSpace'\",EnumName=\"ENiagaraCoordinateSpace::Local\")"));
		V.EnableModule(E, PS, TEXT("AddVelocity"), false);
		const FName C = V.AddModule(E, PS, TEXT("/Niagara/Modules/Spawn/Velocity/AddVelocityInCone"));
		V.Set(E, PS, *C.ToString(), TEXT("Cone Axis"), B::V3(FVector3f(1.f, 0.f, 0.f)));
		V.Set(E, PS, *C.ToString(), TEXT("Cone Axis Coordinate Space"), TEXT("/Script/NiagaraEditor.NiagaraExt_StackInputData_Enum(Enum=\"/Script/CoreUObject.Enum'/Script/Niagara.ENiagaraCoordinateSpace'\",EnumName=\"ENiagaraCoordinateSpace::Local\")"));
		V.Set(E, PS, *C.ToString(), TEXT("Cone Angle"), B::F(AngleDeg * 0.5f));
		// Mit linearem Drag d legt ein Teilchen in der Zeit T die Strecke v/d·(1−e^(−dT)) zurück: v so wählen, dass es Range erreicht
		const float Travel = (1.f - FMath::Exp(-Drag * LifeMax)) / FMath::Max(Drag, 0.01f);
		V.Set(E, PS, *C.ToString(), TEXT("Velocity Strength"), B::F(Range / FMath::Max(Travel, 0.05f)));
		V.Set(E, PU, TEXT("GravityForce"), TEXT("Gravity"), B::V3(FVector3f(0.f, 0.f, 0.f)));
		V.Set(E, PU, TEXT("Drag"), TEXT("Drag"), B::F(Drag));
		return Mat(V, E, Material);
	}

	/** Bodenring, der sich nach außen ausbreitet (Druckwelle). Speed < 0: Teilchen laufen nach innen (Sog). */
	FName Ring(B& V, const TCHAR* Name, int32 Count, float Radius, float Speed, float SizeMin, float SizeMax, float LifeMin, float LifeMax,
		const FString& Color, const TCHAR* Material, float Height = 10.f, float GravityZ = 0.f)
	{
		const FName E = V.AddEmitter(TEXT("OmnidirectionalBurst"), Name);
		Once(V, E, LifeMax + 0.2f);
		V.Set(E, ES, TEXT("SpawnBurst_Instantaneous"), TEXT("Spawn Count"), B::I(Count));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Min"), B::F(LifeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Max"), B::F(LifeMax));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Min"), B::F(SizeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Max"), B::F(SizeMax));
		// flacher Ring: Kugel stark in Z gestaucht, Geschwindigkeit radial vom Mittelpunkt (Vorlage: "From Point")
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(Radius));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Surface Distribution"), B::F(1.f));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Non Uniform Scale"), B::V3(FVector3f(1.f, 1.f, 0.05f)));
		V.SetEnum(E, PS, TEXT("ShapeLocation"), TEXT("Offset Mode"), TEXT("Default"));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Offset"), B::V3(FVector3f(0.f, 0.f, Height)));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Minimum"), B::F(Speed * 0.8f));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Maximum"), B::F(Speed * 1.2f));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Origin Offset"), B::V3(FVector3f(0.f, 0.f, Height)));
		V.Set(E, PU, TEXT("GravityForce"), TEXT("Gravity"), B::V3(FVector3f(0.f, 0.f, GravityZ)));
		V.Set(E, PU, TEXT("Drag"), TEXT("Drag"), B::F(1.5f));
		return Mat(V, E, Material);
	}

	/** Wirbel: Teilchen kreisen um die Hochachse (Strudel, Wirbelwind). */
	FName Swirl(B& V, const TCHAR* Name, float Rate, float Radius, float Spin, float Pull, float SizeMin, float SizeMax, float Life,
		const FString& Color, const TCHAR* Material, float Seconds)
	{
		const FName E = V.AddEmitter(TEXT("OmnidirectionalBurst"), Name);
		Once(V, E, Seconds);
		V.EnableModule(E, ES, TEXT("SpawnBurst_Instantaneous"), false);
		const FName R = V.AddModule(E, ES, TEXT("/Niagara/Modules/Emitter/SpawnRate"));
		V.Set(E, ES, *R.ToString(), TEXT("SpawnRate"), B::F(Rate));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Min"), B::F(Life * 0.7f));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Lifetime Max"), B::F(Life));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Color"), Color);
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Min"), B::F(SizeMin));
		V.Set(E, PS, TEXT("InitializeParticle"), TEXT("Uniform Sprite Size Max"), B::F(SizeMax));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(Radius));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Surface Distribution"), B::F(1.f));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Non Uniform Scale"), B::V3(FVector3f(1.f, 1.f, 0.15f)));
		V.SetEnum(E, PS, TEXT("ShapeLocation"), TEXT("Offset Mode"), TEXT("Default"));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Offset"), B::V3(FVector3f(0.f, 0.f, 40.f)));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Minimum"), B::F(0.f));
		V.Set(E, PS, TEXT("AddVelocity"), TEXT("Velocity Speed|Maximum"), B::F(20.f));
		V.Set(E, PU, TEXT("GravityForce"), TEXT("Gravity"), B::V3(FVector3f(0.f, 0.f, 40.f)));
		const FName W = V.AddModule(E, PU, TEXT("/Niagara/Modules/Update/Forces/VortexForce"));
		V.Set(E, PU, *W.ToString(), TEXT("Vortex Axis"), B::V3(FVector3f(0.f, 0.f, 1.f)));
		V.Set(E, PU, *W.ToString(), TEXT("Vortex Force Amount"), B::F(Spin));
		V.Set(E, PU, *W.ToString(), TEXT("Origin Pull Amount"), B::F(Pull));
		return Mat(V, E, Material);
	}

	/** Aufsteigende Lichtpunkte um den Wirker (Heilung, Schlaf-Noten). */
	FName Rise(B& V, const TCHAR* Name, int32 Count, float Radius, float SizeMin, float SizeMax, float LifeMin, float LifeMax, float Up,
		const FString& Color, const TCHAR* Material)
	{
		const FName E = Puff(V, Name, Count, SizeMin, SizeMax, LifeMin, LifeMax, 10.f, 40.f, Up, Color);
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(Radius));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Non Uniform Scale"), B::V3(FVector3f(1.f, 1.f, 1.8f)));
		V.SetEnum(E, PS, TEXT("ShapeLocation"), TEXT("Offset Mode"), TEXT("Default"));
		V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Offset"), B::V3(FVector3f(0.f, 0.f, 90.f)));
		return Mat(V, E, Material);
	}

	/** Blitz aus dem Himmel auf den Systemursprung (Einschlag ohne BeamEnd: Ende relativ nach oben). */
	FName SkyBolt(B& V, const TCHAR* Name, int32 Points, float Width, float Life, const FString& Color, float Jitter)
	{
		const FName E = Beam(V, Name, Points, Width, Life, Color, Jitter, 0.03f);
		V.Set(E, ES, TEXT("BeamEmitterSetup"), TEXT("Absolute Beam End"), B::B(false));
		V.Set(E, ES, TEXT("BeamEmitterSetup"), TEXT("Beam End"), B::V3(FVector3f(0.f, 0.f, 1400.f)));
		return E;
	}

	/** Sprite-Renderer durch Mesh-Renderer ersetzen (echte 3D-Brocken), beleuchteter Stein. */
	FName AsMesh(B& V, FName E, const TCHAR* MeshPath, float Scale, const TCHAR* Material = TEXT("M_EwVfx_Rock"))
	{
		V.RemoveRenderer(E, 0);
		V.AddRenderer(E, UNiagaraMeshRendererProperties::StaticClass(), FString::Printf(
			TEXT("{\"Meshes\":[{\"mesh\":{\"refPath\":\"%s\"},\"scale\":{\"x\":%f,\"y\":%f,\"z\":%f}}],")
			TEXT("\"bOverrideMaterials\":true,\"OverrideMaterials\":[{\"explicitMat\":{\"refPath\":\"%s\"}}]}"),
			MeshPath, Scale, Scale, Scale, *EwigfelsVfxMaterials::Path(Material)));
		return E;
	}

	const TCHAR* RockMesh = TEXT("/Game/ParagonGideon/FX/Meshes/Heroes/Gideon/Abilities/SM_Meteor_Small.SM_Meteor_Small");
	const TCHAR* ChunkMesh = TEXT("/Game/ParagonGideon/FX/Meshes/Heroes/Gideon/Abilities/SM_Meteor_Chunk_Small.SM_Meteor_Chunk_Small");

	const FString Dust = B::Col(FLinearColor(0.35f, 0.3f, 0.24f, 0.5f));
	const FString Water = B::Col(FLinearColor(0.55f, 0.75f, 0.95f, 0.45f));
	// Kegel-Weisen bei Tageslicht: Staub und Wasser deutlich deckender
	const FString DustStrong = B::Col(FLinearColor(0.42f, 0.36f, 0.28f, 0.8f));
	const FString WaterStrong = B::Col(FLinearColor(0.6f, 0.8f, 1.f, 0.75f));

	bool Simple(const TCHAR* Name, const FLinearColor& C, const FLinearColor& G, const FLinearColor& S, TFunctionRef<void(B&)> Fill)
	{
		B V;
		if (!V.Begin(Name))
		{
			return false;
		}
		StandardUser(V, C, G, S);
		Fill(V);
		return V.Finish();
	}

	/** Geschoss-Weise: Flug (Kern, Halo, Spur, Strom) + Einschlag. */
	bool Projectile(const TCHAR* Id, const FLinearColor& C, const FLinearColor& G, const FLinearColor& S,
		TFunctionRef<void(B&)> Flight, TFunctionRef<void(B&)> Impact)
	{
		bool bOk = Simple(*FString::Printf(TEXT("NS_%s"), Id), C, G, S, Flight);
		bOk &= Simple(*FString::Printf(TEXT("NS_%s_Impact"), Id), C, G, S, Impact);
		return bOk;
	}

	// ---------------------------------------------------------------- Kampfweisen Gruppe 1 (Rest)

	bool Ziehklang()
	{
		// ruhiges, pulsierendes Klangband (glatt, ohne Zucken) mit Leuchtpunkten
		return Simple(TEXT("NS_ziehklang"), FLinearColor(0.7f, 1.f, 0.85f), FLinearColor(0.3f, 0.9f, 0.6f), FLinearColor(0.85f, 1.f, 0.9f), [](B& V)
		{
			V.UserVec3(TEXT("User.BeamEnd"), FVector3f(800.f, 0.f, 0.f));
			Mat(V, Beam(V, TEXT("Band"), 30, 70.f, 0.45f, Hdr(TEXT("User.GlowColor"), 10.f, 0.55f), 5.f, 0.08f), TEXT("M_EwVfx_Trail"));
			Mat(V, Beam(V, TEXT("Welle"), 24, 45.f, 0.45f, Hdr(TEXT("User.SparkColor"), 8.f, 0.5f), 14.f, 0.1f), TEXT("M_EwVfx_Trail"));
			Mat(V, Beam(V, TEXT("Kern"), 30, 14.f, 0.45f, Hdr(TEXT("User.Color"), 35.f), 3.f, 0.08f), TEXT("M_EwVfx_Trail"));
			Mat(V, Flash(V, TEXT("Hand"), 40.f, 0.3f, Hdr(TEXT("User.GlowColor"), 8.f, 0.7f)), TEXT("M_EwVfx_Glow"));
		});
	}

	bool Hammerton()
	{
		// Kegel-Druckwelle: goldene Klangbögen, Staub vom Boden, kleine Funken
		return Simple(TEXT("NS_hammerton"), FLinearColor(1.f, 0.8f, 0.4f), FLinearColor(1.f, 0.65f, 0.2f), FLinearColor(1.f, 0.9f, 0.6f), [](B& V)
		{
			ConeBurst(V, TEXT("Welle"), 30, 650.f, 70.f, 70.f, 130.f, 0.45f, 0.6f, Hdr(TEXT("User.GlowColor"), 7.5f, 0.75f), TEXT("M_EwVfx_GlowCloud"));
			ConeBurst(V, TEXT("Klang"), 50, 650.f, 70.f, 6.f, 12.f, 0.4f, 0.6f, Hdr(TEXT("User.Color"), 30.0f), TEXT("M_EwVfx_Spark"));
			ConeBurst(V, TEXT("Staub"), 18, 500.f, 80.f, 70.f, 140.f, 0.8f, 1.4f, DustStrong, TEXT("M_EwVfx_Smoke"), 20.f, 2.5f);
		});
	}

	bool Paukenschlag()
	{
		// Klatschen: Ring-Schockwelle am Boden, Staubring, Lichtringe in die Höhe
		return Simple(TEXT("NS_paukenschlag"), FLinearColor(1.f, 0.85f, 0.5f), FLinearColor(1.f, 0.6f, 0.3f), FLinearColor(1.f, 0.95f, 0.7f), [](B& V)
		{
			Mat(V, Flash(V, TEXT("Klatsch"), 160.f, 0.12f, Hdr(TEXT("User.GlowColor"), 10.f)), TEXT("M_EwVfx_Burst"));
			Ring(V, TEXT("Welle"), 60, 60.f, 900.f, 40.f, 70.f, 0.3f, 0.45f, Hdr(TEXT("User.GlowColor"), 4.f, 0.6f), TEXT("M_EwVfx_GlowCloud"));
			Ring(V, TEXT("Staub"), 36, 80.f, 600.f, 80.f, 150.f, 0.9f, 1.5f, Dust, TEXT("M_EwVfx_Smoke"), 5.f, 30.f);
			Ring(V, TEXT("Funken"), 50, 50.f, 1100.f, 4.f, 9.f, 0.25f, 0.45f, Hdr(TEXT("User.SparkColor"), 20.f), TEXT("M_EwVfx_Spark"), 30.f);
			Rise(V, TEXT("Lichtringe"), 14, 60.f, 20.f, 40.f, 0.5f, 0.9f, 500.f, Hdr(TEXT("User.Color"), 6.f, 0.7f), TEXT("M_EwVfx_Glow"));
		});
	}

	bool Haltelied()
	{
		// Geschoss: schwebende Notenringe (leuchtende Kugel mit kreisenden Punkten); Aufprall: Bodenriss-Staubwelle
		return Projectile(TEXT("haltelied"), FLinearColor(0.75f, 0.8f, 1.f), FLinearColor(0.5f, 0.55f, 1.f), FLinearColor(0.9f, 0.9f, 1.f),
			[](B& V)
			{
				Core(V, TEXT("Kern"), 40.f, Hdr(TEXT("User.Color"), 10.f));
				Core(V, TEXT("Halo"), 120.f, Hdr(TEXT("User.GlowColor"), 3.f, 0.4f));
				Mat(V, Trail(V, TEXT("Spur"), 50.f, 18.f, 0.4f, Hdr(TEXT("User.GlowColor"), 4.f, 0.5f)), TEXT("M_EwVfx_Trail"));
				Mat(V, Stream(V, TEXT("Noten"), 50.f, 3.f, 6.f, 0.5f, 0.9f, 30.f, 90.f, 0.f, Hdr(TEXT("User.SparkColor"), 14.f), 30.f), TEXT("M_EwVfx_Glow"));
			},
			[](B& V)
			{
				Mat(V, Flash(V, TEXT("Blitz"), 120.f, 0.14f, Hdr(TEXT("User.GlowColor"), 12.f)), TEXT("M_EwVfx_Burst"));
				Ring(V, TEXT("Staubwelle"), 30, 40.f, 500.f, 70.f, 130.f, 0.8f, 1.4f, Dust, TEXT("M_EwVfx_Smoke"));
				Ring(V, TEXT("Lichtring"), 40, 30.f, 700.f, 20.f, 40.f, 0.3f, 0.5f, Hdr(TEXT("User.GlowColor"), 5.f, 0.6f), TEXT("M_EwVfx_Glow"), 20.f);
			});
	}

	bool Glockenschild()
	{
		// schimmernde Kuppel: kreisende Lichtpunkte auf einer Halbkugel und ein weicher Glanz (Endlosschleife)
		return Simple(TEXT("NS_glockenschild"), FLinearColor(1.f, 0.9f, 0.6f), FLinearColor(1.f, 0.8f, 0.4f), FLinearColor(1.f, 0.95f, 0.8f), [](B& V)
		{
			// geschlossene Glocke: Kugel-Mesh mit Fresnel-Rand (Farbe aus User.Color), innen eine zweite, schwächere Schale
			// kurze Lebenszeit (0,25 s, ständig erneuert): beim Loslassen ist die Glocke sofort weg
			for (const TPair<const TCHAR*, float>& Shell : { TPair<const TCHAR*, float>(TEXT("Glocke"), 2.7f), TPair<const TCHAR*, float>(TEXT("Innen"), 2.55f) })
			{
				const bool bOuter = Shell.Value > 2.6f;
				const FName G = Core(V, Shell.Key, 1.f, bOuter ? Hdr(TEXT("User.Color"), 3.f, 1.f) : Hdr(TEXT("User.GlowColor"), 1.2f, 0.6f));
				V.SetEnum(G, ES, TEXT("EmitterState"), TEXT("Life Cycle Mode"), TEXT("Self"));
				V.SetEnum(G, ES, TEXT("EmitterState"), TEXT("Loop Behavior"), TEXT("Infinite"));
				V.Set(G, ES, TEXT("EmitterState"), TEXT("Loop Duration"), B::F(0.25f));
				AsMesh(V, G, TEXT("/Engine/BasicShapes/Sphere.Sphere"), Shell.Value, TEXT("M_EwVfx_ShieldFresnel"));
			}
			// dichte, kurzlebige Lichtpunkte auf einer Kugelschale (lokaler Raum: Kuppel wandert mit dem Wirker) + weicher Glanz
			Mat(V, Core(V, TEXT("Glanz"), 300.f, Hdr(TEXT("User.GlowColor"), 1.2f, 0.18f)), TEXT("M_EwVfx_GlowCloud"));
			const FName E = Stream(V, TEXT("Kuppel"), 600.f, 2.5f, 5.f, 0.2f, 0.35f, 0.f, 5.f, 0.f, Hdr(TEXT("User.Color"), 12.f), 130.f);
			V.Set(E, PS, TEXT("ShapeLocation"), TEXT("Sphere Surface Distribution"), B::F(1.f));
			V.EmitterData(E, TEXT("{\"bLocalSpace\":true}"));
			Mat(V, E, TEXT("M_EwVfx_Glow"));
			const FName W = Stream(V, TEXT("Wellen"), 40.f, 30.f, 60.f, 0.4f, 0.6f, 0.f, 5.f, 0.f, Hdr(TEXT("User.GlowColor"), 2.f, 0.3f), 125.f);
			V.Set(W, PS, TEXT("ShapeLocation"), TEXT("Sphere Surface Distribution"), B::F(1.f));
			V.EmitterData(W, TEXT("{\"bLocalSpace\":true}"));
			Mat(V, W, TEXT("M_EwVfx_GlowCloud"));
		});
	}

	bool ReinklangKonter()
	{
		// heller, reiner Lichtpfeil, weißgoldener Blitz beim Einschlag
		return Projectile(TEXT("reinklang_konter"), FLinearColor(1.f, 0.97f, 0.85f), FLinearColor(1.f, 0.85f, 0.5f), FLinearColor(1.f, 1.f, 0.9f),
			[](B& V)
			{
				Core(V, TEXT("Kern"), 36.f, Hdr(TEXT("User.Color"), 20.f));
				Core(V, TEXT("Stern"), 90.f, Hdr(TEXT("User.GlowColor"), 4.f, 0.6f));
				Mat(V, Trail(V, TEXT("Spur"), 80.f, 10.f, 0.3f, Hdr(TEXT("User.GlowColor"), 10.f, 0.8f)), TEXT("M_EwVfx_Trail"));
				Mat(V, Stream(V, TEXT("Funken"), 40.f, 2.f, 4.f, 0.2f, 0.5f, 40.f, 120.f, 0.f, Hdr(TEXT("User.SparkColor"), 20.f), 6.f), TEXT("M_EwVfx_Spark"));
			},
			[](B& V)
			{
				Mat(V, Flash(V, TEXT("Blitz"), 200.f, 0.16f, Hdr(TEXT("User.GlowColor"), 20.f)), TEXT("M_EwVfx_Flare"));
				Mat(V, Sparks(V, TEXT("Funken"), 60, 400.f, 1200.f, 2.f, Hdr(TEXT("User.SparkColor"), 25.f)), TEXT("M_EwVfx_Spark"));
				Ring(V, TEXT("Ring"), 40, 20.f, 800.f, 15.f, 30.f, 0.25f, 0.4f, Hdr(TEXT("User.Color"), 8.f), TEXT("M_EwVfx_Glow"), 40.f);
			});
	}

	// ---------------------------------------------------------------- Kampfweisen Gruppe 2 (Kampfweisen2.json)

	bool Splitterchor()
	{
		// Fächer aus Quarzsplittern: viele schnelle, gestreckte Splitter, Glitzern
		return Simple(TEXT("NS_splitterchor"), FLinearColor(0.95f, 0.85f, 1.f), FLinearColor(0.75f, 0.55f, 1.f), FLinearColor(1.f, 0.95f, 1.f), [](B& V)
		{
			ConeBurst(V, TEXT("Splitter"), 70, 900.f, 50.f, 8.f, 16.f, 0.25f, 0.45f, Hdr(TEXT("User.Color"), 40.0f), TEXT("M_EwVfx_Spark"), 80.f, 0.3f);
			ConeBurst(V, TEXT("Glitzer"), 40, 700.f, 55.f, 3.f, 6.f, 0.4f, 0.8f, Hdr(TEXT("User.SparkColor"), 45.0f), TEXT("M_EwVfx_Glow"), 80.f);
			ConeBurst(V, TEXT("Schimmer"), 10, 500.f, 45.f, 60.f, 100.f, 0.2f, 0.35f, Hdr(TEXT("User.GlowColor"), 6.2f, 0.65f), TEXT("M_EwVfx_GlowCloud"), 80.f);
		});
	}

	bool Bassbrecher()
	{
		// Stampfen: Bodenwelle, Staub, Brocken fliegen hoch
		return Simple(TEXT("NS_bassbrecher"), FLinearColor(0.8f, 0.6f, 1.f), FLinearColor(0.5f, 0.3f, 0.9f), FLinearColor(0.9f, 0.8f, 0.7f), [](B& V)
		{
			Mat(V, Flash(V, TEXT("Stoss"), 220.f, 0.15f, Hdr(TEXT("User.GlowColor"), 8.f)), TEXT("M_EwVfx_Burst"));
			Ring(V, TEXT("Welle"), 70, 60.f, 1000.f, 50.f, 90.f, 0.35f, 0.55f, Hdr(TEXT("User.GlowColor"), 4.f, 0.6f), TEXT("M_EwVfx_GlowCloud"));
			Ring(V, TEXT("Staub"), 40, 80.f, 700.f, 90.f, 180.f, 1.0f, 1.8f, Dust, TEXT("M_EwVfx_Smoke"), 5.f, 40.f);
			const FName Rocks = Puff(V, TEXT("Brocken"), 30, 8.f, 16.f, 0.7f, 1.2f, 300.f, 700.f, -1200.f, Dust);
			V.Set(Rocks, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(250.f));
			V.Set(Rocks, PS, TEXT("ShapeLocation"), TEXT("Non Uniform Scale"), B::V3(FVector3f(1.f, 1.f, 0.05f)));
			Mat(V, Rocks, TEXT("M_EwVfx_Smoke"));
		});
	}

	bool Wirbelschlag()
	{
		// Rundumschlag: schneller Windring nach außen, Funkenbogen
		return Simple(TEXT("NS_wirbelschlag"), FLinearColor(0.85f, 1.f, 0.95f), FLinearColor(0.5f, 0.95f, 0.85f), FLinearColor(1.f, 1.f, 1.f), [](B& V)
		{
			Ring(V, TEXT("Wind"), 50, 80.f, 1100.f, 60.f, 110.f, 0.3f, 0.5f, B::Col(FLinearColor(0.9f, 0.95f, 1.f, 0.35f)), TEXT("M_EwVfx_Wisp"), 70.f);
			Ring(V, TEXT("Bogen"), 80, 90.f, 1300.f, 5.f, 10.f, 0.2f, 0.35f, Hdr(TEXT("User.GlowColor"), 14.f), TEXT("M_EwVfx_Spark"), 90.f);
			Ring(V, TEXT("Staub"), 24, 80.f, 600.f, 80.f, 140.f, 0.7f, 1.2f, Dust, TEXT("M_EwVfx_Smoke"));
		});
	}

	bool Wiegenlied()
	{
		// sanfte, langsame Klangwolke nach vorn mit glitzernden Punkten
		return Simple(TEXT("NS_wiegenlied"), FLinearColor(0.85f, 0.75f, 1.f), FLinearColor(0.6f, 0.45f, 1.f), FLinearColor(1.f, 0.9f, 1.f), [](B& V)
		{
			ConeBurst(V, TEXT("Wolke"), 24, 1100.f, 45.f, 120.f, 220.f, 1.2f, 1.8f, Hdr(TEXT("User.GlowColor"), 4.0f, 0.60f), TEXT("M_EwVfx_Cloud"), 80.f, 1.2f);
			ConeBurst(V, TEXT("Sterne"), 50, 1000.f, 50.f, 4.f, 8.f, 1.0f, 1.6f, Hdr(TEXT("User.SparkColor"), 30.0f), TEXT("M_EwVfx_Glow"), 90.f, 1.2f);
		});
	}

	bool Atemwende()
	{
		// Selbstheilung: aufsteigende grüne Lichter, sanfter Ring am Boden
		return Simple(TEXT("NS_atemwende"), FLinearColor(0.6f, 1.f, 0.7f), FLinearColor(0.3f, 0.95f, 0.5f), FLinearColor(0.85f, 1.f, 0.8f), [](B& V)
		{
			Rise(V, TEXT("Lichter"), 50, 70.f, 4.f, 9.f, 0.8f, 1.6f, 160.f, Hdr(TEXT("User.SparkColor"), 12.f), TEXT("M_EwVfx_Glow"));
			Rise(V, TEXT("Hauch"), 10, 50.f, 60.f, 110.f, 0.8f, 1.4f, 120.f, Hdr(TEXT("User.GlowColor"), 1.5f, 0.35f), TEXT("M_EwVfx_Cloud"));
			Ring(V, TEXT("Ring"), 40, 40.f, 300.f, 18.f, 30.f, 0.6f, 0.9f, Hdr(TEXT("User.GlowColor"), 5.f, 0.6f), TEXT("M_EwVfx_Glow"), -80.f);
		});
	}

	bool Windstoss()
	{
		// kräftige Bö: weiße Schwaden und Staub schießen nach vorn
		return Simple(TEXT("NS_windstoss"), FLinearColor(0.9f, 0.97f, 1.f), FLinearColor(0.75f, 0.9f, 1.f), FLinearColor(1.f, 1.f, 1.f), [](B& V)
		{
			ConeBurst(V, TEXT("Boe"), 36, 1200.f, 55.f, 100.f, 180.f, 0.7f, 1.0f, B::Col(FLinearColor(0.95f, 0.97f, 1.f, 0.6f)), TEXT("M_EwVfx_Wisp"), 70.f, 0.8f);
			ConeBurst(V, TEXT("Linien"), 50, 1200.f, 50.f, 6.f, 12.f, 0.3f, 0.5f, Hdr(TEXT("User.GlowColor"), 10.0f, 0.85f), TEXT("M_EwVfx_Spark"), 80.f, 0.4f);
			ConeBurst(V, TEXT("Staub"), 20, 800.f, 60.f, 60.f, 120.f, 0.8f, 1.3f, DustStrong, TEXT("M_EwVfx_Smoke"), 20.f, 1.5f);
		});
	}

	bool Flutweise()
	{
		// Wasserschwall: Gischt, Tropfen, nasser Nebel
		return Simple(TEXT("NS_flutweise"), FLinearColor(0.5f, 0.75f, 1.f), FLinearColor(0.25f, 0.55f, 1.f), FLinearColor(0.85f, 0.95f, 1.f), [](B& V)
		{
			ConeBurst(V, TEXT("Schwall"), 40, 1000.f, 40.f, 70.f, 130.f, 0.4f, 0.7f, WaterStrong, TEXT("M_EwVfx_Wisp"), 70.f, 0.8f);
			const FName Drops = ConeBurst(V, TEXT("Tropfen"), 80, 1000.f, 45.f, 4.f, 8.f, 0.5f, 0.9f, Hdr(TEXT("User.SparkColor"), 6.f), TEXT("M_EwVfx_Spark"), 80.f, 0.3f);
			V.Set(Drops, PU, TEXT("GravityForce"), TEXT("Gravity"), B::V3(FVector3f(0.f, 0.f, -900.f)));
			ConeBurst(V, TEXT("Nebel"), 12, 700.f, 50.f, 120.f, 200.f, 1.0f, 1.6f, Hdr(TEXT("User.GlowColor"), 3.0f, 0.50f), TEXT("M_EwVfx_Cloud"), 60.f, 1.5f);
		});
	}

	bool Hagelchor()
	{
		// Hagelkugel, die über dem Ziel zerplatzt: Eisbrocken fallen, Frostwolke
		return Projectile(TEXT("hagelchor"), FLinearColor(0.8f, 0.92f, 1.f), FLinearColor(0.45f, 0.7f, 1.f), FLinearColor(0.95f, 1.f, 1.f),
			[](B& V)
			{
				Core(V, TEXT("Kern"), 50.f, Hdr(TEXT("User.Color"), 10.f));
				Core(V, TEXT("Halo"), 120.f, Hdr(TEXT("User.GlowColor"), 2.5f, 0.4f));
				Mat(V, Stream(V, TEXT("Kristalle"), 60.f, 2.f, 4.f, 0.4f, 0.8f, 40.f, 140.f, -400.f, Hdr(TEXT("User.SparkColor"), 16.f), 18.f), TEXT("M_EwVfx_Glow"));
				Mat(V, Stream(V, TEXT("Kaelte"), 25.f, 30.f, 50.f, 0.5f, 0.9f, 10.f, 30.f, 0.f, Hdr(TEXT("User.GlowColor"), 1.2f, 0.25f), 10.f), TEXT("M_EwVfx_Cloud"));
			},
			[](B& V)
			{
				Mat(V, Flash(V, TEXT("Blitz"), 140.f, 0.12f, Hdr(TEXT("User.GlowColor"), 12.f)), TEXT("M_EwVfx_Burst"));
				const FName Hail = Puff(V, TEXT("Hagel"), 70, 6.f, 12.f, 0.6f, 1.0f, 50.f, 250.f, -1400.f, Hdr(TEXT("User.SparkColor"), 10.f));
				V.Set(Hail, PS, TEXT("ShapeLocation"), TEXT("Sphere Radius"), B::F(260.f));
				V.SetEnum(Hail, PS, TEXT("ShapeLocation"), TEXT("Offset Mode"), TEXT("Default"));
				V.Set(Hail, PS, TEXT("ShapeLocation"), TEXT("Offset"), B::V3(FVector3f(0.f, 0.f, 300.f)));
				Mat(V, Hail, TEXT("M_EwVfx_Spark"));
				Mat(V, Puff(V, TEXT("Frost"), 16, 80.f, 160.f, 1.0f, 1.8f, 50.f, 160.f, 20.f, Hdr(TEXT("User.GlowColor"), 1.3f, 0.3f)), TEXT("M_EwVfx_Cloud"));
			});
	}

	bool Donnerkeil()
	{
		// aufgeladene Blitzkugel; Einschlag: Blitz aus dem Himmel, Druckring, Funken
		return Projectile(TEXT("donnerkeil"), FLinearColor(0.85f, 0.85f, 1.f), FLinearColor(0.5f, 0.45f, 1.f), FLinearColor(0.8f, 0.85f, 1.f),
			[](B& V)
			{
				Core(V, TEXT("Kern"), 55.f, Hdr(TEXT("User.Color"), 18.f));
				Core(V, TEXT("Halo"), 150.f, Hdr(TEXT("User.GlowColor"), 4.f, 0.5f));
				Mat(V, Stream(V, TEXT("Knistern"), 80.f, 3.f, 7.f, 0.1f, 0.25f, 150.f, 400.f, 0.f, Hdr(TEXT("User.SparkColor"), 25.f), 25.f), TEXT("M_EwVfx_Spark"));
			},
			[](B& V)
			{
				Mat(V, SkyBolt(V, TEXT("Himmelsblitz"), 24, 16.f, 0.25f, Hdr(TEXT("User.Color"), 60.f), 30.f), TEXT("M_EwVfx_Bolt"));
				SkyBolt(V, TEXT("Kern"), 24, 5.f, 0.2f, Hdr(TEXT("User.Color"), 80.f), 20.f);
				Mat(V, Flash(V, TEXT("Blitz"), 300.f, 0.18f, Hdr(TEXT("User.GlowColor"), 20.f)), TEXT("M_EwVfx_Burst"));
				Ring(V, TEXT("Druckring"), 60, 50.f, 1000.f, 40.f, 80.f, 0.3f, 0.5f, Hdr(TEXT("User.GlowColor"), 5.f, 0.6f), TEXT("M_EwVfx_GlowCloud"));
				Mat(V, Sparks(V, TEXT("Funken"), 70, 400.f, 1400.f, 2.f, Hdr(TEXT("User.SparkColor"), 30.f)), TEXT("M_EwVfx_Spark"));
			});
	}

	bool Wirbelflut()
	{
		// Strudel um den Wirker: Wasser kreist und zieht nach innen
		return Simple(TEXT("NS_wirbelflut"), FLinearColor(0.5f, 0.75f, 1.f), FLinearColor(0.25f, 0.55f, 1.f), FLinearColor(0.85f, 0.95f, 1.f), [](B& V)
		{
			Swirl(V, TEXT("Strudel"), 60.f, 700.f, 2500.f, 400.f, 80.f, 140.f, 1.0f, Water, TEXT("M_EwVfx_Wisp"), 1.2f);
			Swirl(V, TEXT("Gischt"), 120.f, 650.f, 3000.f, 500.f, 4.f, 8.f, 0.8f, Hdr(TEXT("User.SparkColor"), 8.f), TEXT("M_EwVfx_Spark"), 1.2f);
			Ring(V, TEXT("Sog"), 40, 750.f, -500.f, 60.f, 100.f, 0.8f, 1.2f, Hdr(TEXT("User.GlowColor"), 1.5f, 0.35f), TEXT("M_EwVfx_Cloud"));
		});
	}

	bool Felsfaust()
	{
		// Felsbrocken mit Staubschweif; Einschlag: Staubwolke, Splitter, Brocken
		return Projectile(TEXT("felsfaust"), FLinearColor(0.75f, 0.6f, 0.45f), FLinearColor(0.9f, 0.6f, 0.3f), FLinearColor(0.8f, 0.7f, 0.55f),
			[](B& V)
			{
				AsMesh(V, Core(V, TEXT("Fels"), 1.f, B::Col(FLinearColor(0.8f, 0.7f, 0.6f, 1.f))), RockMesh, 3.f);
				Mat(V, Stream(V, TEXT("Staub"), 40.f, 30.f, 60.f, 0.6f, 1.1f, 10.f, 40.f, -50.f, Dust, 20.f), TEXT("M_EwVfx_Smoke"));
				AsMesh(V, Stream(V, TEXT("Kiesel"), 14.f, 1.f, 1.f, 0.5f, 0.9f, 50.f, 150.f, -900.f, B::Col(FLinearColor(0.7f, 0.6f, 0.5f, 1.f)), 25.f), ChunkMesh, 0.3f);
			},
			[](B& V)
			{
				Mat(V, Puff(V, TEXT("Staubwolke"), 20, 80.f, 180.f, 1.2f, 2.0f, 100.f, 300.f, 30.f, Dust), TEXT("M_EwVfx_Smoke"));
				AsMesh(V, Puff(V, TEXT("Brocken"), 14, 1.f, 1.f, 0.9f, 1.4f, 300.f, 700.f, -1300.f, B::Col(FLinearColor(0.7f, 0.6f, 0.5f, 1.f))), ChunkMesh, 0.5f);
				Mat(V, Sparks(V, TEXT("Splitter"), 40, 300.f, 900.f, 3.f, Dust), TEXT("M_EwVfx_Smoke"));
				Mat(V, Flash(V, TEXT("Wucht"), 120.f, 0.08f, Hdr(TEXT("User.GlowColor"), 2.f, 0.4f)), TEXT("M_EwVfx_Burst"));
				Ring(V, TEXT("Bodenwelle"), 30, 40.f, 500.f, 60.f, 110.f, 0.8f, 1.3f, Dust, TEXT("M_EwVfx_Smoke"));
			});
	}

	bool Glutpeitsche()
	{
		// Feuerpeitsche: zuckendes Flammenband mit Glutfunken
		return Simple(TEXT("NS_glutpeitsche"), FLinearColor(1.f, 0.5f, 0.12f), FLinearColor(1.f, 0.3f, 0.05f), FLinearColor(1.f, 0.75f, 0.3f), [](B& V)
		{
			V.UserVec3(TEXT("User.BeamEnd"), FVector3f(800.f, 0.f, 0.f));
			Mat(V, Beam(V, TEXT("Glut"), 20, 80.f, 0.35f, Hdr(TEXT("User.GlowColor"), 8.f, 0.55f), 12.f, 0.05f), TEXT("M_EwVfx_Trail"));
			Mat(V, Beam(V, TEXT("Flammen"), 18, 50.f, 0.35f, Hdr(TEXT("User.Color"), 14.f, 0.8f), 18.f, 0.04f), TEXT("M_EwVfx_Fire"));
			Mat(V, Beam(V, TEXT("Kern"), 20, 16.f, 0.3f, Hdr(TEXT("User.SparkColor"), 40.f), 10.f, 0.05f), TEXT("M_EwVfx_Bolt"));
			Mat(V, Flash(V, TEXT("Hand"), 50.f, 0.15f, Hdr(TEXT("User.Color"), 10.f, 0.8f)), TEXT("M_EwVfx_Fire"));
		});
	}

	int32 Build(const TArray<FString>& Ids)
	{
		struct FEntry { const TCHAR* Id; bool (*Fn)(); };
		const FEntry All[] = {
			{ TEXT("funkenstoss"), &Funkenstoss }, { TEXT("frostweise"), &Frostweise },
			{ TEXT("glutweise"), &Glutweise }, { TEXT("sturmweise"), &Sturmweise },
			{ TEXT("ziehklang"), &Ziehklang }, { TEXT("hammerton"), &Hammerton }, { TEXT("paukenschlag"), &Paukenschlag },
			{ TEXT("haltelied"), &Haltelied }, { TEXT("glockenschild"), &Glockenschild }, { TEXT("reinklang_konter"), &ReinklangKonter },
			{ TEXT("splitterchor"), &Splitterchor }, { TEXT("bassbrecher"), &Bassbrecher }, { TEXT("wirbelschlag"), &Wirbelschlag },
			{ TEXT("wiegenlied"), &Wiegenlied }, { TEXT("atemwende"), &Atemwende }, { TEXT("windstoss"), &Windstoss },
			{ TEXT("flutweise"), &Flutweise }, { TEXT("hagelchor"), &Hagelchor }, { TEXT("donnerkeil"), &Donnerkeil },
			{ TEXT("wirbelflut"), &Wirbelflut }, { TEXT("felsfaust"), &Felsfaust }, { TEXT("glutpeitsche"), &Glutpeitsche } };
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
