# Charakter-Editor und Personalisierung (Nocturna)

Stand 27.11.2026 (Umbau: fertige Köpfe, Kleiderschrank-Stil) · Code: `Source/Ewigfels/{Appearance,UI,World}` · Daten: `Content/Data/Appearance/` · Prüfen: `python Scripts/Tools/validate_appearance.py`

Ziel (GDD §13, E4): **Jede Spielfigur sieht anders aus.** Am Anfang baust du deine Figur im Charakter-Editor. Danach änderst du sie in Läden in der Welt, jeder Laden kann etwas anderes.

**Leitlinie (Nutzer, 27.11.2026):** einfach wie ein guter Kleiderschrank in modernen Open-World-Spielen. **Fertige Köpfe zum Aussuchen, keine Feinregler.** Bunt, immer mit magischem Touch. Controller und Maus/Tastatur gleich gut. Beim Spielstart **geführt** (Schritt für Schritt mit „Weiter“), im Laden frei.

## 1. Was man einstellen kann

Reihenfolge = Schritte der geführten Erstellung:

| # | Bereich | Inhalt (alles Kacheln oder Farbtupfer) | Später ändern bei |
|---|---|---|---|
| 1 | Kopf | 12 fertige Köpfe (alle Hauttöne, jeder bringt seinen Hautton mit), darunter Hautton (12 Tupfer) und Augenfarbe (12 Tupfer) | Spiegelhexe (Raunwald) |
| 2 | Haare | 10 Frisuren am Start (+6 im Laden), Haarfarbe, Brauen, Bart | Barbier & Haarstube |
| 3 | Statur | 3 Körperbauten, Größe in 3 Stufen (kleiner · mittel · größer) | Spiegelhexe |
| 4 | Zeichen | Sommersprossen, Muttermal, Narben, Lidstrich, Lippenfarbe, Rouge (antippen = an/aus, bis 8); im Laden Tattoos, Bemalung; ab Akt 3 leuchtende Klang-Runen | Tätowierer & Malerin |
| 5 | Kleidung | erst das Teil (10 Slots: Hut, Gesicht, Hals, Oberteil, Umhang, Gürtel, Handschuhe, Hose, Schuhe, Tasche), dann Farbe + zweite Farbe, „Diese Farben für alles“. 23 Stücke am Start, 11 im Laden, Uniform + Turm-Umhang über die Story | Schneiderei (alles), Handschuhmacher (Handschuhe), Turmzimmer-Spiegel (nur umfärben, gratis) |
| 6 | Ring | Metall + **Farbe deiner Magie** (Quarz, der Ring ist ein Erbstück von zu Hause) | Ringschmied Ambros |
| 7 | Stimme | hell / mittel / tief, antippen spielt sie vor | Spiegelhexe |
| 8 | Name | Name (tippen oder „Namen würfeln“), Anrede (nur Name · sie/ihr · er/ihm, frei kombinierbar) | Spiegelhexe |

Muster, Stoff, dritte Farbzone, zweifarbige Augen, Haar-Farbverlauf und Tonhöhe stecken weiter im Datenmodell (alte Spielstände, spätere Läden), der Editor zeigt sie bewusst nicht.

**Aussehen des Fensters:** links ein Bedienfeld mit Farbverlauf (Violett → Nachtblau) und schwebenden Funken, rechts die Figur im Porträtlicht. Jeder Bereich hat seine eigene Farbe (Kopf Rosa, Haare Bernstein, Statur Minze, Zeichen Flieder, Kleidung Himmelblau, Ring Gold, Stimme Türkis, Name Koralle). Rahmen: **Gold = gewählt, Türkis = Fokus (Controller), Flieder = Maus darüber**, die Kachel wächst leicht. Kacheln zeigen ein Vorschaubild `/Game/UI/Creator/Thumbs/T_<id>` (oder `thumb` im JSON). Fehlt es, steht dort ein farbiger Kreis (Köpfe/Metalle in ihrer Farbe, sonst mit Anfangsbuchstabe).

**Bedienung** (die Hinweise unten wechseln automatisch zum zuletzt benutzten Gerät):

| | Controller | Maus / Tastatur |
|---|---|---|
| Auswählen | Steuerkreuz/linker Stick + A | Klick, Pfeiltasten + Enter |
| Bereich wechseln | LB / RB | Q / E oder Klick auf den Bereich |
| Weiter / Fertig | Start | Knopf unten |
| Zurück (geführt) / Abbrechen (Laden) | B | Esc |
| Rückgängig / Wiederholen | X | Strg+Z / Strg+Y |
| Zufall (nur Erstellung) | Y | R |
| Figur drehen / zoomen | rechter Stick / LT, RT | Ziehen / Mausrad |

Kopf, Haare und Zeichen zoomen automatisch aufs Gesicht.

## 2. Ablauf im Spiel

**Spielbeginn:** Die Figur existiert noch nicht (`bCreated = false`) → der Editor öffnet sich von selbst und führt durch die 8 Schritte. Erzählerisch: Auf dem Nebelboot über den Ilsensee (M101) siehst du dein Spiegelbild im dunklen Wasser. „Fertig“ geht erst mit Namen (mind. 2 Zeichen). Danach: „Willkommen in Nocturna, …“, Weltstatus-Flag `Look_Created` ist gesetzt, und Dialoge können dich ab jetzt mit Namen ansprechen (§4).

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
| `skin` | nur Köpfe: passender Hautton `#RRGGBB` (wird beim Wählen des Kopfs gesetzt) |
| `color` | Kachelfarbe, z. B. bei Ring-Metallen |
| `thumb` | Vorschaubild der Kachel (optional, sonst `/Game/UI/Creator/Thumbs/T_<id>`) |

Köpfe stehen unter `heads` (das alte `face_presets` wird noch gelesen). Weitere Blöcke: `limits` (Größe, max. Male, Namenslänge), `palettes` (Farbvorschläge `skin`, `hair`, `eyes`, `dye`, `quartz`, `makeup`), `names` (Vorschläge für „Namen würfeln“), `defaults` (Startfigur, nur `start`-Teile, `face_b` = `face_a`). Feinregler (`sliders`) versteht der Code noch, der Katalog hat bewusst keine.

`Shops.json` – Läden. `categories` sagt, was der Spiegel dort ändern darf: `name`, `body`, `face`, `skin`, `eyes`, `hair`, `beard`, `brows`, `marks`, `voice`, `rings`, `outfit` (alle Kleidung), `outfit:<slot>` (nur ein Slot), `outfit_colors` (nur umfärben). Preis = `base_price` × geänderte Bereiche + Preise neu gewählter Teile. Statt Kronen geht `price_item` + `price_item_count` (Spiegelhexe: 5 Kräuterbunde).

Nach jeder Änderung: `python Scripts/Tools/validate_appearance.py` (läuft auch in der Cloud).

## 4. Technik

**Datenfluss:** `FEwigfelsAppearance` (nur Ids, Zahlen, Farben) → `UEwigfelsAppearanceComponent` an der Spielfigur (repliziert) → baut die Optik auf jedem Rechner. Änderungen laufen immer über den Server: `RequestChange` → `Server_RequestChange` → `Sanitize` (Zahlen begrenzen, unbekannte Ids ersetzen) → `CheckChange` (Laden darf das? freigeschaltet? Name?) → Spiegel in der Nähe? → bezahlen → setzen. Die Vorschau im Editor bleibt lokal.

**Was im Unreal-Editor später gebaut wird (Phase 5, Schritt für Schritt in eigener Anleitung):**

| Teil | Name / Ort | Wie |
|---|---|---|
| Körper | `/Game/Characters/Player/Body/SK_Body_{Schmal,Mittel,Kraeftig}` | 3 MetaHuman-Körper, gleiches Skelett |
| Köpfe | `/Game/Characters/Player/Heads/SK_Kopf_01` … `SK_Kopf_12` | 12 MetaHumans im MetaHuman Creator bauen (6 eher weiblich, 6 eher männlich, alle Hauttöne, Hautton wie `skin` im JSON), jeweils nur das Gesichts-Mesh exportieren. Hat die Figur ein Gesichts-Kind „Face“ (MetaHuman-Blueprint), bekommt es den Kopf, sonst hängt der Kopf als eigenes Teil am Körper. Kein Blender nötig |
| Vorschaubilder | `/Game/UI/Creator/Thumbs/T_<id>` (256 × 256) | Kopf/Frisur/Kleidung im Editor fotografieren (Skript folgt mit Phase 5) |
| Haut/Augen | Material-Parameter `SkinColor`, `EyeColorL`, `EyeColorR`, `HairColor`, `MagicColor` | im Haut- und Augen-Material als Vector-Parameter anlegen |
| Male | Parameter `Mark0Tex` … `Mark7Tex`, `Mark0Color` …, `Mark0Opacity` … | 8 Decal-Ebenen im Haut-Material |
| Haare/Bart/Brauen | Skeletal Meshes (Haar-Karten) unter `/Game/Characters/Player/{Hair,Beard,Brows}/`, Parameter `HairColor`, `HairTipColor` | folgen dem Körper per Leader Pose. Echte Grooms (Strähnen) hängt ein Blueprint über `OnAppearanceApplied` an |
| Kleidung | `/Game/Characters/Player/Outfit/SK_<id>`, Parameter `Dye1`, `Dye2`, `Dye3`, `PatternIndex`, `FabricIndex` | ein Master-Material mit RGB-Maske (3 Farbzonen) + Muster-/Stoff-Texturen nach Index |
| Stimmringe | `/Game/Characters/Player/Rings/SM_Stimmring`, Sockets `ring_01_r`, `ring_02_r`, `ring_01_l`, `ring_02_l`, Parameter `Quartz`, `MetalIndex` | Sockets am Hand-Skelett anlegen |
| Stimmproben | `/Game/Characters/Player/Voice/VS_{Hell,Mittel,Tief}` | Piper/Thorsten (Lizenz in docs/KI_LIZENZEN.md) |

Solange diese Assets fehlen, färbt die Komponente den Platzhalter-Körper: Robe = Umhang/Oberteil-Hauptfarbe, leuchtende Hände = Quarzfarbe, Augen = Augenfarbe, Größe und Kapuze stimmen auch.

**Warum fertige Köpfe:** Ein MetaHuman-Gesicht kann im Spiel nicht frei neu gerechnet werden. Fertige Köpfe sind einfacher zu bedienen (Wunsch des Nutzers), sehen immer gut aus und behalten die volle Mimik des Gesichts-Rigs.

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
- Vorschaubilder für alle Kacheln (Foto-Skript im Editor).
- Assets aus §4 bauen (Phase 5), Köpfe + Startkleidung zuerst.
