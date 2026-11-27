# Charakter-Editor und Personalisierung (Nocturna)

Stand 27.11.2026 · Code: `Source/Ewigfels/{Appearance,UI,World}` · Daten: `Content/Data/Appearance/` · Prüfen: `python Scripts/Tools/validate_appearance.py`

Ziel (GDD §13, E4): **Jede Spielfigur sieht anders aus, bis ins Detail.** Am Anfang baust du deine Figur im Charakter-Editor. Danach änderst du sie in Läden in der Welt, jeder Laden kann etwas anderes.

## 1. Was man einstellen kann

| Reiter | Inhalt | Später ändern bei |
|---|---|---|
| Wer bist du? | Name, Anrede (nur Name · sie/ihr · er/ihm). Anrede ist frei, egal welcher Körper und welche Stimme | Spiegelhexe (Raunwald) |
| Körper | 3 Körperbauten, Größe ±5 % (ca. 163–181 cm), Regler Schultern, Statur, Muskeln | Spiegelhexe |
| Gesicht | 2 von 8 Grundgesichtern wählen und mischen, dann 30 Regler (Kopf, Brauen, Augen, Nase, Wangen, Mund, Kiefer und Kinn, Ohren inkl. spitzer Ohren) | Spiegelhexe |
| Haut & Augen | Hautton (12 Vorschläge + frei per Farbton/Sättigung/Helligkeit), Augenfarbe, zweifarbige Augen | Spiegelhexe |
| Haare & Bart | 10 Frisuren am Start (+6 im Laden), Haarfarbe, Farbverlauf zu den Spitzen, Brauen, Bart | Barbier & Haarstube |
| Male & Schminke | bis zu 8 gleichzeitig: Sommersprossen, Muttermal, Narben, Lidstrich, Lippenfarbe, Rouge; im Laden Tattoos, Bemalung; ab Akt 3 leuchtende Klang-Runen. Jedes mit Farbe und Stärke | Tätowierer & Malerin |
| Stimme | hell / mittel / tief + Tonhöhe, Probehören | Spiegelhexe |
| Kleidung | 10 Slots (Kopf, Gesicht, Hals, Oberkörper, Umhang, Gürtel, Handschuhe, Beine, Schuhe, Tasche), jedes Stück mit 3 Farbzonen, Muster, Stoff. 23 Stücke am Start, 11 im Laden, Uniform + Turm-Umhang über die Story | Schneiderei (alles), Handschuhmacher (Handschuhe), Turmzimmer-Spiegel (nur umfärben, gratis) |
| Stimmring | Metall + **Quarzfarbe = Farbe deiner Magie** (der Ring ist ein Erbstück von zu Hause) | Ringschmied Ambros |

Bedienung: Maus ziehen dreht die Kamera um die Figur, Mausrad zoomt, Strg+Z / Strg+Y, „Zufall“, „Zurücksetzen“. Gesichts-Reiter zoomen automatisch aufs Gesicht, ein warmes Porträtlicht und ein kühles Kantenlicht leuchten die Figur aus.

## 2. Ablauf im Spiel

**Spielbeginn:** Die Figur existiert noch nicht (`bCreated = false`) → der Editor öffnet sich von selbst. Erzählerisch: Auf dem Nebelboot über den Ilsensee (M101) siehst du dein Spiegelbild im dunklen Wasser. „Fertig“ geht erst mit Namen (mind. 2 Zeichen). Danach: „Willkommen in Nocturna, …“, Weltstatus-Flag `Look_Created` ist gesetzt, und Dialoge können dich ab jetzt mit Namen ansprechen (§4).

**Läden:** Ein `AEwigfelsMirror` im Level mit `ShopId`. E drücken → derselbe Editor, aber nur mit den Reitern und Teilen, die dieser Laden anbietet. Unten steht der Preis. Bezahlt wird erst bei „Übernehmen“. In der Testarena steht ein Schneider-Spiegel links vom Startpunkt.

**Koop:** Jeder Spieler hat seine eigene Figur. Die zuletzt erstellte Figur liegt im Profil `EwigfelsLook` auf dem eigenen PC. Wer einem Spiel beitritt, sieht den Editor mit seiner Figur schon ausgefüllt und muss nur „Fertig“ drücken.

**Spielstand:** Das Aussehen steckt im Spielstand (Version 2). Alte Spielstände (Version 1) laden weiter, die Figur behält dann ihr Profil-Aussehen.

## 3. Daten (`Content/Data/Appearance/`)

`Appearance.json` – alles Wählbare. Neue Frisur, neues Kleidungsstück, neue Farbe = nur ein JSON-Eintrag, kein Code.

```json
{ "id": "haar_flechtkranz", "name": { "de": "Flechtkranz", "en": "Braided crown" },
  "asset": "/Game/Characters/Player/Hair/haar_flechtkranz.haar_flechtkranz",
  "unlock": "shop", "price": 60 }
```

| Feld | Bedeutung |
|---|---|
| `id` | eindeutig, nur `a-z 0-9 _` |
| `name` | Deutsch + Englisch |
| `asset` | Skeletal Mesh (Körper, Haare, Kleidung), Textur (Male), Sound (Stimmprobe). Fehlt das Asset noch, läuft trotzdem alles (Platzhalter) |
| `unlock` | `start` = Editor und Läden · `shop` = nur im Laden · `flag:<Flag>` = sobald der Weltstatus-Flag gesetzt ist (Flag muss in `Content/Data/World/WorldState.json` stehen) |
| `price` | Kronen im Laden (zusätzlich zum Grundpreis des Ladens) |
| `slot` | nur Kleidung: einer der 10 Slots |
| `kind` | nur Male: `freckles`, `scar`, `makeup`, `tattoo`, `paint`, `rune` |
| `morph` | nur Grundgesichter: Morph-Target im Gesicht (Gesicht 1 = Grundform, leer) |

Weitere Blöcke: `limits` (Größe, max. Male, Namenslänge), `sliders` (Regler mit `morph_pos`/`morph_neg`; Körperregler beginnen mit `koerper_`), `palettes` (Farbvorschläge `skin`, `hair`, `eyes`, `dye`, `quartz`, `makeup`), `defaults` (Startfigur, nur `start`-Teile).

`Shops.json` – Läden. `categories` sagt, was der Spiegel dort ändern darf: `name`, `body`, `face`, `skin`, `eyes`, `hair`, `beard`, `brows`, `marks`, `voice`, `rings`, `outfit` (alle Kleidung), `outfit:<slot>` (nur ein Slot), `outfit_colors` (nur umfärben). Preis = `base_price` × geänderte Bereiche + Preise neu gewählter Teile. Statt Kronen geht `price_item` + `price_item_count` (Spiegelhexe: 5 Kräuterbunde).

Nach jeder Änderung: `python Scripts/Tools/validate_appearance.py` (läuft auch in der Cloud).

## 4. Technik

**Datenfluss:** `FEwigfelsAppearance` (nur Ids, Zahlen, Farben) → `UEwigfelsAppearanceComponent` an der Spielfigur (repliziert) → baut die Optik auf jedem Rechner. Änderungen laufen immer über den Server: `RequestChange` → `Server_RequestChange` → `Sanitize` (Zahlen begrenzen, unbekannte Ids ersetzen) → `CheckChange` (Laden darf das? freigeschaltet? Name?) → Spiegel in der Nähe? → bezahlen → setzen. Die Vorschau im Editor bleibt lokal.

**Was im Unreal-Editor später gebaut wird (Phase 5, Schritt für Schritt in eigener Anleitung):**

| Teil | Name / Ort | Wie |
|---|---|---|
| Körper | `/Game/Characters/Player/Body/SK_Body_{Schmal,Mittel,Kraeftig}` | 3 MetaHuman-Körper, gleiches Skelett |
| Grundgesichter | Morph-Targets `Preset_02` … `Preset_08` im Gesichts-Mesh | 8 MetaHumans im Creator bauen (gleiche Topologie), Gesicht 2–8 in Blender als Shape Key relativ zu Gesicht 1 speichern, als ein Gesichts-Mesh mit Morphs importieren |
| Regler | Morph-Targets `<Name>_Pos` / `<Name>_Neg` (z. B. `NaseLaenge_Pos`), Liste in `Appearance.json` | in Blender als Shape Keys modellieren. Körperregler (`Koerper*`) auch auf jedem Kleidungs-Mesh, damit Kleidung mitwächst |
| Haut/Augen | Material-Parameter `SkinColor`, `EyeColorL`, `EyeColorR`, `HairColor`, `MagicColor` | im Haut- und Augen-Material als Vector-Parameter anlegen |
| Male | Parameter `Mark0Tex` … `Mark7Tex`, `Mark0Color` …, `Mark0Opacity` … | 8 Decal-Ebenen im Haut-Material |
| Haare/Bart/Brauen | Skeletal Meshes (Haar-Karten) unter `/Game/Characters/Player/{Hair,Beard,Brows}/`, Parameter `HairColor`, `HairTipColor` | folgen dem Körper per Leader Pose. Echte Grooms (Strähnen) hängt ein Blueprint über `OnAppearanceApplied` an |
| Kleidung | `/Game/Characters/Player/Outfit/SK_<id>`, Parameter `Dye1`, `Dye2`, `Dye3`, `PatternIndex`, `FabricIndex` | ein Master-Material mit RGB-Maske (3 Farbzonen) + Muster-/Stoff-Texturen nach Index |
| Stimmringe | `/Game/Characters/Player/Rings/SM_Stimmring`, Sockets `ring_01_r`, `ring_02_r`, `ring_01_l`, `ring_02_l`, Parameter `Quartz`, `MetalIndex` | Sockets am Hand-Skelett anlegen |
| Stimmproben | `/Game/Characters/Player/Voice/VS_{Hell,Mittel,Tief}` | Piper/Thorsten (Lizenz in docs/KI_LIZENZEN.md) |

Solange diese Assets fehlen, färbt die Komponente den Platzhalter-Körper: Robe = Umhang/Oberteil-Hauptfarbe, leuchtende Hände = Quarzfarbe, Augen = Augenfarbe, Größe und Kapuze stimmen auch.

**Ehrliche Grenze:** Ein MetaHuman-Gesicht kann im Spiel nicht frei neu gerechnet werden (das kann nur der MetaHuman Creator im Editor). Deshalb die Lösung mit 8 Grundgesichtern + eigenen Morph-Targets. Die Gesichtsanimation (Mimik) läuft weiter über das Gesichts-Rig. Bei extremen Reglerwerten kann die Mimik leicht ungenau werden, das testen wir in Phase 5 und begrenzen die Regler notfalls im JSON.

**Dialoge:** Name und Anrede stehen als SUDS-Variablen bereit: `{global.SpielerName}` und `{global.SpielerGeschlecht}` (Textgeschlecht für Formen wie „er/sie“; bei „Nur Name“ neutral). Die bestehenden Beispiel-Dialoge nutzen das noch nicht, das kommt mit Phase 5.

## 5. Konsole (im Spiel `~`)

| Befehl | Wirkung |
|---|---|
| `Ewigfels.Look.Editor` | Editor öffnen (z. B. zum Testen) · `Ewigfels.Look.Editor haarstube` = Laden-Spiegel |
| `Ewigfels.Look.Random` | Zufallsfigur im offenen Editor |
| `Ewigfels.Look.Code` | Look-Code der eigenen Figur ins Log + in die Zwischenablage (`NOC1:…`) |
| `Ewigfels.Look.Import NOC1:…` | Look-Code in den offenen Editor laden (Figur mit Freunden teilen) |
| `Ewigfels.Look.Reset` | Figur vergessen, Editor öffnet sich wie bei neuem Spiel |
| `Ewigfels.Look.Reload` | `Content/Data/Appearance` neu laden |

Zum schnellen Testen ohne Editor: in `Config/DefaultGame.ini` unter `[/Script/Ewigfels.EwigfelsAppearanceComponent]` `bOpenCreatorOnStart=False`.

## 6. Tests

`Ewigfels.Look.Catalog`, `.Sanitize`, `.Rules`, `.RandomAndCode`, `.ServerChecks` (Automation, `Scripts/Tools/Test-Ewigfels.ps1`) und `validate_appearance.py`.

## 7. Offen (spätere Phasen)

- Magiefarbe aus dem Stimmring in die Weisen-Effekte geben (Platzhalter-Hände zeigen sie schon). Kommt, wenn Phase 3 gebaut und getestet ist.
- Bezahlen: `UEwigfelsAppearanceSubsystem::PaymentHandler`/`MoneyQuery` ans Inventar hängen (Phase 5). Bis dahin ist alles gratis (Log-Hinweis).
- Besitz: Gekaufte Kleidung landet später im Inventar; Turmzimmer + Transmog + 5 Optik-Sets (GDD §13) bauen darauf auf.
- Gamepad-Bedienung des Editors mit CommonUI (Phase 5, zusammen mit dem restlichen Menü).
- Assets aus §4 bauen (Phase 5), Grundgesichter + Startkleidung zuerst.
