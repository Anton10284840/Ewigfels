# Zauber-Effekte (Niagara) – Plan und Ablage

Ziel (Nutzer, 27.11.): weg vom „Grundformen-Look“, hin zu glänzenden, wuchtigen, fotorealistischen Effekten wie in großen
Spielen. Jede Weise hat ihre eigene Wirkung, Farbe und Geste. Eigenes Design, keine Nachbauten fremder Marken.

## Ablage und Anschluss (Code fertig)
- Pfad je Weise, **ohne JSON-Eintrag**: `/Game/VFX/Spells/NS_<id>` (Flug, Strahl, Schild, Welle) und
  `/Game/VFX/Spells/NS_<id>_Impact` (Einschlag). Mit `vfx.niagara` / `vfx.impact_niagara` lässt sich das überschreiben.
- Fehlt ein Asset, läuft der bisherige Platzhalter-Look (Lichtkugeln, Blitz-Strahl). Man kann also Weise für Weise umstellen.
- Der Code setzt diese **User-Parameter** (im System als User-Parameter anlegen, Namen exakt):
  - `User.Color` (LinearColor) Kern, `User.GlowColor` Saum/Glühen, `User.SparkColor` Funken (Akkord-Farbregel 1/2/3)
  - `User.BeamEnd` (Vector, Weltkoordinate) nur bei Strahl-Weisen (`delivery: beam`)
  - Skalierung kommt über die Komponenten-Skalierung (`vfx.scale`).
- Geschosse hängen `NS_<id>` an die Kollisionskugel (Schweif). Sobald es existiert, wird die Platzhalter-Kugel ausgeblendet.

## Quellen (Budget 0 €)
- **Paragon: Gideon** (Fab Standard License, schon im Projekt, CREDITS-Eintrag da): Texturen, Materialien und Partikel aus
  seinen Zaubern (Portal, Schwarzes Loch, Meteor, Geschosse) als Bausteine. Nur in Unreal nutzen (Lizenz).
  Cascade-Systeme bei Bedarf mit dem Engine-Plugin „Cascade to Niagara Converter“ umwandeln.
- Niagara-Vorlagen der Engine (`/Niagara/DefaultAssets/Templates/…`): Burst, Fountain, Beam, Dynamic Beam, Ribbon.
- ambientCG (CC0) für Rausch-/Wolkentexturen; eigene Flipbooks nur, wenn nötig.
- Jede neue externe Quelle vorher in `CREDITS.md`.

## Look je Weise (10 Kampfweisen)
| Weise | Datei(en) | Look |
|---|---|---|
| funkenstoss (Scintivox) | NS_funkenstoss (Beam), _Impact | Zickzack-Blitz aus den Fingern, heller Kern + farbiger Saum, kleine Äste, Funkenregen beim Einschlag; 3. Schlag dicker mit Druckring. Mündungsblitz klein (Figur darf nicht weiß überstrahlen). |
| sturmweise (Tempestivox) | NS_sturmweise (Beam), _Impact | Wie Funkenstoß, aber violett-blau, dicker, springt (Code macht die Sprünge, jedes Segment = eigener Beam). Elektrische Bögen am Ziel. |
| ziehklang (Trahisonus) | NS_ziehklang (Beam) | Ruhiges, pulsierendes Klangband (Ribbon) mit wandernden Notenringen zum Wirker hin. |
| frostweise (Gelucantus) | NS_frostweise, _Impact | Eislanze mit Frostnebel-Schweif und glitzernden Kristallsplittern; Einschlag: Eissplitter-Burst, Reif am Boden. |
| glutweise (Ardesonus) | NS_glutweise, _Impact | Glutkugel mit Flammen-Schweif, Funken und Hitzeflimmern; Einschlag: Flammenstoß + Glutfunken + Rauch. |
| hammerton (Malletonus) | NS_hammerton | Kegel-Druckwelle: verzerrte Luft, Staub vom Boden, goldene Klangbögen nach vorn. |
| paukenschlag (Tympaplausus) | NS_paukenschlag | Klatschen: Ring-Schockwelle am Boden, Staubring, kurze Lichtringe in die Höhe. |
| haltelied (Tenecantus) | NS_haltelied, _Impact | Geschoss = schwebende Notenringe; hält das Ziel mit kreisenden Lichtringen. Aufprall (Cadetonus): Bodenriss-Staubwelle. |
| glockenschild (Campanimur) | NS_glockenschild | Glockenförmige, schimmernde Kuppel (Fresnel), wandernde Klangwellen bei Treffern. |
| reinklang_konter (Purisonus) | NS_reinklang_konter, _Impact | Heller, reiner Lichtpfeil, weißgoldener Blitz beim Einschlag. |

## Reihenfolge
1. funkenstoss + Mündungsblitz (am häufigsten zu sehen), 2. frostweise, 3. glutweise, 4. sturmweise, 5. Rest.
Pro Weise: anlegen, im Spiel ansehen (Testarena), Bild machen, Nutzer zeigen.

## Erledigt im Code (27.11.)
- Mündungsblitz und Leucht-Halos deutlich schwächer (vorher wurde die Figur beim Funkenstoß ganz weiß).
- Alle drei Akkord-Farben gehen an Niagara; feste Ablage-Pfade ohne JSON.

## Effekte bauen (Editor-Modul, seit 28.11.)
- Quelle der Effekte ist Code: `Source/EwigfelsEditor/Private/EwigfelsSpellVfx.cpp` (Bausteine Strahl/Blitz, Leuchtfleck,
  Funkenregen, Glut) über `FEwigfelsVfxBuilder` (UE-5.8-Schnittstelle `UNiagaraExternalEditUtilities`).
  `Content/` liegt nicht im Git – die Assets entstehen reproduzierbar aus dem Code.
- Bauen (Editor geschlossen): `UnrealEditor-Cmd.exe Ewigfels.uproject -run=pythonscript -script=Scripts/Python/create_spell_vfx.py`
  (nur bestimmte Weisen: Umgebungsvariable `EWIGFELS_VFX="funkenstoss frostweise"`). Konsole im Editor: `Ewigfels.VFX.Erstellen [id]`.
- Ansehen ohne Gegner/Wirkzeit: im Spiel `Ewigfels.Test.VFX <id> [quit]` → `Saved/Screenshots/VFX`.
- Auslesen: `Ewigfels.VFX.Vorlagen` (Engine-Vorlagen), `Ewigfels.VFX.Auslesen /Game/VFX/Spells/NS_<id>` → `Saved/VFX/*.txt`.
- Stolpersteine: vorhandene Systeme werden geleert statt gelöscht (Löschen+Neuanlegen ergab kaputte Dateien);
  `GetStackIssues` nicht benutzen (danach gespeicherte Systeme stürzten beim Laden ab); User-Parameter vor dem Start
  setzen (`SpawnNiagaraAt` startet erst nach `User.BeamEnd`); Blitz-Zickzack = `JitterPosition` mit abgeschaltetem `UpdateBeam`.

## Stand 28.11. (alle 22 Kampfweisen haben NS_<id>)
- Materialien: `/Game/VFX/Materials/M_EwVfx_*` aus Gideon-Texturen (Glow, Flare, Burst, Spark, Bolt, Trail, Fire-Flipbook 8×16,
  Smoke-Flipbook 8×8 deckend, Wisp, Cloud, GlowCloud). Bau: `Ewigfels.VFX.Materialien` (oder automatisch, wenn eins fehlt).
- Bausteine in `EwigfelsSpellVfx.cpp`: Beam (Zickzack), Flash, Sparks, Embers, Puff, Stream, Core, Trail, ConeBurst (Kegel nach
  lokal +X), Ring (Bodenwelle, negativ = Sog), Swirl (Wirbel), Rise (aufsteigend), SkyBolt (Blitz von oben auf den Einschlag).
- Gruppe 2 (Kampfweisen2.json), abgeleitete Looks: splitterchor Quarzsplitter-Fächer · bassbrecher Bodenwelle+Brocken ·
  wirbelschlag Windring · wiegenlied langsame Klangwolke mit Sternen · atemwende aufsteigende Heil-Lichter · windstoss
  Schwaden-Bö · flutweise Wasserschwall+Tropfen · hagelchor Eiskugel, Hagel fällt aufs Ziel · donnerkeil Blitzkugel, Himmelsblitz
  beim Einschlag · wirbelflut Strudel mit Sog · felsfaust Felsbrocken mit Staubschweif · glutpeitsche Flammenband.
- Offen: `NS_glockenschild` existiert (schimmernde Kuppel, Endlosschleife), wird vom Code aber noch nicht gespawnt (Schild nutzt
  den alten Mesh-Look). Geschosse (`SpawnNiagaraAttached`) setzen die Farben erst nach dem Start – erstes Bild in Standardfarbe.
