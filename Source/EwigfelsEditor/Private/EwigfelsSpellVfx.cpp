// Copyright Ewigfels. Alle Rechte vorbehalten.

// Zauber-Effekte per Code (docs/VFX_PLAN.md). "Ewigfels.VFX.Erstellen [id ...]" baut /Game/VFX/Spells/NS_<id> (+_Impact).
// Farben kommen zur Laufzeit über User.Color / User.GlowColor / User.SparkColor, Strahlen enden bei User.BeamEnd.
// Content/ ist nicht im Git: dieser Code IST die Quelle der Effekte, die Assets sind reproduzierbar.

#include "EwigfelsVfxBuilder.h"

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
			Beam(V, TEXT("Saum"), 12, 20.f, 0.16f, Hdr(TEXT("User.GlowColor"), 12.f, 0.45f), 12.f, 0.05f);
			Beam(V, TEXT("Kern"), 18, 5.f, 0.16f, Hdr(TEXT("User.Color"), 60.f), 16.f, 0.04f);
			Beam(V, TEXT("Ast"), 12, 1.8f, 0.10f, Hdr(TEXT("User.SparkColor"), 40.f), 30.f, 0.03f);
			Flash(V, TEXT("Muendung"), 18.f, 0.07f, Hdr(TEXT("User.GlowColor"), 12.f, 0.8f));
			bOk &= V.Finish();
		}
		{
			B V;
			if (!V.Begin(TEXT("NS_funkenstoss_Impact")))
			{
				return false;
			}
			StandardUser(V, Core, Glow, Spark);
			Flash(V, TEXT("Blitz"), 90.f, 0.10f, Hdr(TEXT("User.GlowColor"), 25.f));
			Sparks(V, TEXT("Funken"), 36, 350.f, 1100.f, 2.f, Hdr(TEXT("User.SparkColor"), 25.f));
			Embers(V, TEXT("Glut"), 10, 6.f, Hdr(TEXT("User.SparkColor"), 8.f));
			bOk &= V.Finish();
		}
		return bOk;
	}

	int32 Build(const TArray<FString>& Ids)
	{
		struct FEntry { const TCHAR* Id; bool (*Fn)(); };
		const FEntry All[] = { { TEXT("funkenstoss"), &Funkenstoss } };
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

	FAutoConsoleCommand GBuild(TEXT("Ewigfels.VFX.Erstellen"),
		TEXT("Zauber-Effekte bauen: Ewigfels.VFX.Erstellen [id ...] (ohne id: alle) -> /Game/VFX/Spells/NS_<id>"),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args) { Build(Args); }));
}
