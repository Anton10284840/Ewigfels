# GDD – Nocturna (Game Design Document)

Stand: 26.11.2026 · Phase 1 · Version 0.2 (E10: 430 Weisen, Akt-1-Schulleben)
Verwandte Dokumente: [STORY_BIBLE](STORY_BIBLE.md) · [CHARACTER_BIBLE](CHARACTER_BIBLE.md) · [ART_BIBLE](ART_BIBLE.md) · [ANIMATION_BIBLE](ANIMATION_BIBLE.md)

---

## 0. Grundsatzentscheidungen (bitte bestätigen oder umentscheiden)

Ich habe überall mit der **Empfehlung** weitergearbeitet. Wenn du anders entscheidest, ändere ich die Bibeln entsprechend (Namen sind per Suchen/Ersetzen schnell getauscht, das Magiesystem ist der teuerste Wechsel).

### E1 – Name der Akademie ✅ entschieden

**Nocturna** (lat. „die Nächtliche“, sprich nok-TUR-na). Vom Nutzer am 26.11.2026 gewählt, ist auch der **Spieltitel**.
Lore: Der Ewige Fels, auf dem die Akademie steht, summt am lautesten nachts unter den beiden Monden. Darum nannten die Gründer die Schule nach der Nacht.
Projekt-, Repo-, Ordner- und Code-Namen bleiben `Ewigfels` (Umbenennung würde Build und Pfade brechen).
Verworfen: Ewigfels, Stimmhorn, Ilsenhall, Hallwacht, Glockenklippe, Echospire, Hollowbell, Altisonus, Sonarium, Vox Nox, Cantus Altus, Alta Cantata, Sonora Aurora, Resona, Canora, Sonora.

### E2 – Grundregel des Magiesystems

| Option | Kern | Vorteil | Nachteil |
|---|---|---|---|
| **A: Resonanz (Klangmagie)** ⭐ Empfehlung | Magie ist Klang. Zauber („Weisen“) sind Akkorde aus Grundtönen, gespielt mit den Fingern (wie Tasten auf einem Akkordeon). Die verbotene alte Magie ist der **Urton**, der Ton unter allen Tönen: er löscht und schreibt Wirklichkeit um. | Sehr eigenständig, klar von anderen Zaubererwelten getrennt. Gibt Audio, VFX und Animation ein gemeinsames Leitmotiv. Dein Akkordeon wird Teil der Welt (Akkorde = Zauber, Soundtrack-Leitmotive = Weisen). | Ungewohnt. Muss in Audio sauber umgesetzt werden, sonst wirkt es nur wie ein Name. |
| B: Erinnerungsmagie | Magie ist das Gedächtnis der Welt. Verboten: Erinnerungen umschreiben. | Starke Story-Twists möglich. | Kampf-Zauber wirken aufgesetzt, schwer zu visualisieren. |
| C: Klassisches Mana über Kraftlinien | Energie aus Linien im Boden, Elemente. | Leicht verständlich, einfach zu bauen. | Generisch, sieht aus wie jedes andere Fantasy-RPG. |

### E3 – Häuser

**Empfehlung:** 4 **Türme** statt „Häuser“-Begriff, Spieler **wählt** nach Gesprächen mit den vier Turmsprechern (keine magische Zuteilung). Details: [CHARACTER_BIBLE §3](CHARACTER_BIBLE.md#3-die-vier-türme-häuser).
Alternative: zufällige Zuteilung durch die Glocke der Akademie. Nachteil: nimmt dem Spieler die Wahl und erinnert zu stark an bekannte Vorbilder.

### E4 – Spielerfigur

**Empfehlung:** Frei gestaltbar (männlich/weiblich/divers über Körper- und Stimmwahl), Name frei, Alter 15 zu Beginn, 20 am Ende. Stimme: 3 Varianten (TTS-Platzhalter, später Sprecher).
Alternative: feste Figur mit Namen. Vorteil: stärkere Cutscenes. Nachteil: widerspricht dem Charakter-Editor im Master-Prompt.

### E5 – Zaubersystem-Technik ✅ entschieden: eigenes System (Nutzer, 26.11.2026)

GAS (Gameplay Ability System) vs. eigenes Komponentensystem. Gewählt: **eigenes datengetriebenes Komponentensystem** (`UEwigfelsSpellCasterComponent` + `UEwigfelsCombatComponent`). Grund: Jede Weise ist reine JSON-Daten (Auslieferung + Liste von Effekten, Format [SPELL_FORMAT.md](SPELL_FORMAT.md)), dadurch bleiben 430 Weisen ohne 430 Blueprints machbar, und der Code ist für einen Laien lesbar. Kosten, Abklingzeiten, Status-Stapel und Netzwerk sind selbst gebaut und mit Automation-Tests abgesichert.

### E11 – Mehrspieler (Koop) ✅ vorbereitet, Einschalten später (Nutzer, 26.11.2026)

Koop für 2–4 Spieler (Listen-Server: ein Spieler ist Gastgeber). Jetzt noch nicht spielbar, aber **jede Spielmechanik wird ab Phase 3 so gebaut, dass Koop später nur eingeschaltet werden muss.** Architekturregel:

1. **Server entscheidet.** Schaden, Leben, Atem, Status, Abklingzeiten, Gegner-KI, Beute und Quest-Fortschritt werden nur auf dem Server (`HasAuthority()`) verändert.
2. **Clients bitten.** Eingaben gehen als Server-RPC („Weise X zaubern“, „ausweichen“), nie als fertiges Ergebnis.
3. **Zustand wird repliziert** (`DOREPLIFETIME`, `OnRep_…`), Effekte für alle per `NetMulticast` (nur Optik/Ton, keine Spielwerte).
4. **Bewegung über die CharacterMovementComponent** (Sprint als Saved-Move-Flag, Ausweichen als Root-Motion-Source), damit Vorhersage und Korrektur funktionieren.
5. **Kein „der Spieler“.** Code fragt nie nach Spieler 0; Gegner wählen ihr Ziel aus allen Spielern, Quests zählen für die Gruppe.
6. **Rein lokales** (Kamera, HUD, Dialogkamera, Zeitlupe) bleibt lokal und wird im Koop nicht angewendet, wo es andere stören würde (z. B. Zeitlupe nur allein).

### E6 – Ton ✅ entschieden (Nutzer, 26.11.2026)

**Verspielt und zum Eintauchen, mit dunklen, gruseligen Gegnern.** Die Welt soll Spaß machen und einladen (bunte Märkte, freundliche Dörfer, Tiere, Läden, Feste), der Kontrast sind echte Gruselorte und Schwarzmagier, gegen die man kämpft. Siehe ART_BIBLE §1 und §2.1.

### E7 – Magie aus den Fingern ✅ entschieden (Nutzer, 26.11.2026)

**Kein Zauberstab.** Magie bricht direkt aus den Fingerspitzen. Ausrüstung dafür: bis zu 4 **Stimmringe** und Handschuhe (STORY_BIBLE §3.3, ANIMATION_BIBLE §3.2).

### E8 – Reittiere ✅ entschieden (Nutzer, 26.11.2026)

**Mehrere selbst erfundene Reittiere** (Luft, Boden, Wasser), personalisierbar. Liste: [CHARACTER_BIBLE §7](CHARACTER_BIBLE.md#7-reittiere). Wenn du eigene Tiere erfinden willst, beschreib sie mir, dann kommen sie dazu.

### E9 – Weltgröße ✅ entschieden: Option A (Nutzer, 26.11.2026)

Ehrlich: GTA-Welten (GTA 5 hat grob 75–80 km², GTA 6 gilt als noch größer) bauen Teams aus über 1000 Leuten über viele Jahre. Allein mit 0 € kann man so eine Fläche **nicht** komplett von Hand füllen. Unreal schafft die Größe technisch (World Partition streamt nur, was nah ist), das Problem ist der Inhalt.

| Option | Fläche | Wie | Vorteil | Nachteil |
|---|---|---|---|---|
| **A: Groß mit dichten Kernen** ⭐ Empfehlung | ca. 64 km² (8 × 8 km), wächst pro Akt | Dörfer, Städte, Akademie, Dungeons von Hand; Wildnis dazwischen per **PCG** (prozedural: Wälder, Felsen, Wege, Ruinen, Camps automatisch) | Fühlt sich riesig an, ist aber überall voll. Machbar. Läuft auf deiner RTX 5070. | Kleiner als GTA (etwa GTA-5-Klasse statt GTA-6) |
| B: GTA-6-Größe | 150+ km² (12 × 12 km und mehr) | fast alles per PCG, Handarbeit nur an wenigen Orten | Riesige Fläche, langes Reiten und Fliegen | Viel leere, gleich aussehende Fläche, sehr lange Bauzeit, viel Speicher (Hunderte GB), längere Ladezeiten |
| C: Kompakt und sehr dicht | ca. 16 km² (4 × 4 km) | fast alles von Hand | Höchste Qualität pro Meter | Wirkt kleiner, Fliegen ist schnell vorbei |

**Empfehlung A**, gebaut in Ringen: Phase 5 (Akt 1) startet mit ca. 16 km² rund um die Akademie, jeder Akt fügt Regionen hinzu bis ca. 64 km². Wenn es gut läuft, können wir später Richtung B erweitern, World Partition erlaubt das ohne Neubau. Gewählt: **A** (Details §15).

### E10 – Hunderte Weisen ✅ entschieden: Option C (Nutzer, 26.11.2026)

Wunsch: Hunderte Zauber, lernbar im Unterricht oder bei Meistern, ohne Pflicht zu Zaubersprüchen. Gewählt: **Mischung** aus 60 handgemachten Kernweisen, 358 Alltagsweisen aus einem Baukasten (Grundton × Wirkart) und 12 Koop-Weisen. Optionen, Lernwege und ehrliche Animationskosten: [WEISEN.md §0](WEISEN.md#0-entscheidung-e10--hunderte-weisen--option-c-nutzer-26112026). Dazu: Akt 1 bekommt einen starken verspielten Schul-Vibe, siehe [AKT1_SCHULLEBEN.md](AKT1_SCHULLEBEN.md).

---

## 1. Überblick

| | |
|---|---|
| Titel | **Nocturna** (Code-/Projektname: Ewigfels) |
| Genre | Open-World-Action-RPG, Third-Person |
| Setting | Dunkles Märchen trifft Realismus. Kontinent **Velmark**, ca. 18.–19.-Jahrhundert-Technik ohne Industrie, zwei Monde |
| Plattform | Windows-PC, Tastatur/Maus + Gamepad |
| Engine | Unreal Engine 5.8, C++ |
| Ziel-Performance | 60 FPS in 1440p (RTX 5070), Presets Low–Cinematic |
| Umfang | ca. 100 Std. (Main 45–50, Questreihen/Sidequests 35–40, Aktivitäten 10–15) |
| Sprache | Deutsch + Englisch (Text), Platzhalter-Sprachausgabe per TTS |
| Altersfreigabe-Ziel | USK 12 / PEGI 12 (Fantasy-Gewalt, kein Blut, dunkle Themen) |

**Ein-Satz-Pitch:** Du kommst als Neuling an eine Zauberakademie auf einem singenden Felsen, und in dir erwacht der Urton, die verbotene Magie, die eine Zivilisation ausgelöscht hat und die jetzt ein Orden haben will, der die Welt für immer zum Schweigen bringen möchte.

## 2. Design-Säulen

1. **Magie, die man hört und spürt.** Jeder Zauber ist ein Akkord: eigener Klang, eigene Geste, eigene Lichtfarbe. Kampf hat Rhythmus (perfektes Parieren = „Reinklang“).
2. **Eine Akademie, die lebt und sich verändert.** Fünf Jahre, Feste, Unterricht, Zerstörung, Wiederaufbau. Der Hub ist das Herz des Spiels.
3. **Entscheidungen mit Gewicht.** Mindestens 6 große Entscheidungen verändern Figuren (Tod/Abwendung), Fraktionen und Regionen. 4 Enden.
4. **Cinematic-Look in Echtzeit.** Lieber eine Szene perfekt als zehn halb fertig (Art Bible).
5. **Verbotene Kraft mit Preis.** Der Urton ist stark, aber er nimmt der Welt Farbe und Klang. Der Spieler soll jedes Mal überlegen, ob er ihn benutzt.
6. **Verspielt eintauchen, dunkel kämpfen.** Läden, Feste, Tiere, Personalisierung und freundliche Dörfer machen Spaß. Dagegen stehen gruselige Orte und Schwarzmagier, die echtes Unbehagen erzeugen. Beides braucht das andere.
7. **Eine Schule zum Verlieben.** Akt 1 lässt sich Zeit: Unterricht, Streiche, Feste, lebendige Dinge, Geheimgänge und Hunderte kleine Weisen. Erst wenn der Spieler Nocturna liebt, wird es dunkel (AKT1_SCHULLEBEN.md).

## 3. Spielerfantasie und Kern-Loops

**Fantasie:** „Ich bin ein junger Klangwirker mit einer gefährlichen Gabe. Ich lerne, entdecke Geheimnisse, beschütze meine Freunde und entscheide, was aus der Magie wird.“

| Loop | Dauer | Inhalt |
|---|---|---|
| Moment | 30 s | Zauber-Akkorde kombinieren, parieren, Elementar-Reaktionen auslösen, klettern/gleiten |
| Erkundung | 5–15 min | Ort entdecken → Rätselschrein / Kampf / Lore-Fund → Belohnung (Material, Weise, Kodex) |
| Quest | 45–60 min | Mission mit Dialog, Ort, Rätsel, Kampf, Entscheidung, Cutscene |
| Weisen sammeln | laufend | Probieren (Grundton × Wirkart) → Entdeckung; Meister finden → Bedingung erfüllen → Liederheft lernen; Weisenheft füllen (430) |
| Akademie-Woche | 2–4 Std. | Unterricht (Minispiel) → neue Weise → Schulleben-Quests, Glockenball, Streiche, Nachtausflüge → Turmzimmer ausbauen → Story-Mission (AKT1_SCHULLEBEN §2) |
| Akt | 8–10 Std. | Ein Schuljahr, neue Regionen, Akt-Boss, große Entscheidung |

## 4. Magiesystem (Spielmechanik)

Lore-Regeln stehen in [STORY_BIBLE §3](STORY_BIBLE.md#3-magiesystem-resonanz). Hier die Spielregeln.

### 4.1 Ressourcen

| Ressource | Funktion | Regeneration |
|---|---|---|
| **Atem** (blau) | Kosten für normale Weisen. 100 Punkte, ausbaubar bis 200 | schnell außerhalb des Zauberns, langsamer im Kampf; Parieren gibt Atem zurück |
| **Cooldown** | pro Weise 2–30 s | – |
| **Urklang** (violett-weiß) | lädt sich durch Kampf und Reinklang-Paraden auf; Urweisen verbrauchen ihn | nur durch Kampf |
| **Nachhall** (Belastung, 0–100) | jede Urweise erhöht ihn. Hoher Wert: Bild entsättigt, Ton wird dumpf, Nachhall-Kreaturen tauchen auf. Langfristig: beeinflusst Enden und NPC-Reaktionen | sinkt langsam, Meditation an Aderquellen senkt ihn stark |

### 4.2 Weisen (Zauber): 430 – Kern, Alltag, Koop (E10)

**Vollständiges System:** [WEISEN.md](WEISEN.md) · **alle Weisen mit Akkord, Effekt, Combo und Lehrer:** [WEISEN_KATALOG.md](WEISEN_KATALOG.md) (automatisch aus `Content/Data/Spells/Katalog/*.json`).

Steuerung: 4 aktive Kernweisen auf dem Schnellwahl-Ring, bis zu 4 Ringe (16 Slots), Wahlrad für Ringe. Basisangriff (Funkenstoß) immer verfügbar. Alltagsweisen über das **Alltagsrad** (8 Favoriten) und den **Kontext-Vorschlag** am Objekt (WEISEN §5).

| Art | Anzahl | Kurz |
|---|---|---|
| Kernweisen | 60 | handgemacht: eigene Geste, eigener Effekt, Akkord aus 3 Grundtönen. Kampf, Story, große Rätsel |
| Alltagsweisen | 358 | Baukasten: Grundton × Wirkart (Schnippen, Tippen, Streichen, Kreisen, Pusten, Summen, Pfeifen + 5 Sonderformen). 49 Grundformen selbst entdecken, Rest bei Lehrern, Meistern, Mitschülern, Tieren, auf Festen |
| Koop-Weisen | 12 | Duette, Dreiklänge, Vierklänge für 2–4 Spieler; im Einzelspiel mit Begleiter |

Kernweisen nach Schule (✦ = schon in Version 0.1 dabei, Name und Funktion unverändert):

| Schule | Anzahl | Weisen |
|---|---|---|
| **Schlag** | 12 | Funkenstoß ✦, Hammerton ✦, Splitterchor ✦, Ziehklang ✦, Prellschlag, Zerrton ✦, Pfeilnote, Trommelfeuer, Stoßkadenz, Bassbrecher, Wirbelschlag, Paukenschlag |
| **Schutz** | 12 | Glockenschild ✦, Spiegelklang ✦, Haltelied ✦, Wiegenlied, Gleitschleier, Atemwende ✦, Klangwand ✦, Gegenklang, Bannkreis, Fesselkanon, Schutzchor, Stillpunkt |
| **Elementar** | 14 | Glutweise ✦, Frostweise ✦, Windstoß, Sturmweise ✦, Flutweise ✦, Nebelweise, Reifpfad, Funkenregen, Erdweise ✦, Glutpeitsche, Hagelchor, Donnerkeil, Wirbelflut, Felsfaust |
| **Wandel** | 14 | Leichtklang ✦, Wiederklang ✦, Laternenlicht, Spurklang, Klinkenlied, Lockruf, Schattenweise ✦, Wachsklang ✦, Kletterhauch, Echoköder, Schrumpfklang, Formwandel ✦, Spiegelbild, Belebung |
| **Ur (verboten)** | 8 | Verstummen ✦, Nachhallschritt ✦, Seelenecho ✦, Farbraub, Entklang ✦, Rückklang, Stummer Chor, Grundton ✦ |

**Akkord-Darstellung:** Jede Kernweise hat drei Grundtöne (STORY_BIBLE §3.2), jeder Akkord kommt nur einmal vor (das Prüfskript achtet darauf). Im Kodex und im HUD als drei kleine farbige Noten sichtbar. Weisen mit gemeinsamem Grundton haben schnellere Übergänge (Combo-Fenster +0,3 s). So entsteht eine Lernkurve, ohne dass der Spieler Musik können muss.

### 4.3 Status-Effekte

Nass, Brennen, Gefroren, Betäubt, Schwebend, Verstummt (keine Weisen), Nachhall-berührt (nur Gegner aus dem Schattenreich, verwundbar gegen Urweisen). Neu durch Alltagsweisen (E10): **Glatt** (rutscht aus), **Abgelenkt** (2–5 s, kichert, tanzt, flieht vor Gestank; Bosse immun, je Gegner einmal pro Kampf), **Laut**/**Leise** (Lärm lockt an, Stille hilft beim Schleichen). Details: WEISEN §8.

## 5. Kampf

- **Lock-On** (Tab/rechter Stick drücken), Zielwechsel per Stick/Mausrad.
- **Parieren:** Glockenschild im Fenster von 0,2 s vor Treffer = Reinklang (Zeitlupe 0,3 s, Konterstoß, +15 Atem, +Urklang). Normal blocken kostet Atem.
- **Ausweichen:** Rolle oder kurzer Schritt (mit i-Frames), Ausdauer = Atem.
- **Schwachstellen:** Gegner haben Farb-Schilde (Glut-Schild nur mit Frost brechbar usw.) und körperliche Schwachpunkte (Kristalle, Rüstungsfugen).
- **Gegner-KI:** StateTree (UE 5.8) mit Gruppen-Koordinator (max. 2 greifen gleichzeitig nah an, Fernkämpfer flankieren).
- **Schwierigkeit:** Geschichte / Normal / Schwer / Meister + Einzeloptionen (Parierfenster, Gegnerschaden, Zielhilfe).

### 5.1 Gegnertypen (30)

Ausführlich (Verhalten, Fähigkeiten, Schwächen, Grusel-Regeln, 10 Gruseldorf-Herrscher, Regionsbosse, Reittiere): [BESTIARIUM.md](BESTIARIUM.md).

| Gruppe | Typen |
|---|---|
| Stille Hand (Orden) | Novize · Schweiger (Nahkampf, verstummt Weisen) · Taktgeber (Fernkampf, synchronisiert Gruppen) · Choralist (stärkt Verbündete) · Dirigentenwache (Elite, Schild) |
| Wilderer & Schmuggler | Klingenschnapper (Messer, schnell) · Fallensteller (Netze, Öl) · Schmugglerwirker (Elementar) · Wildererhund (Tier) |
| **Missklinger (Schwarzmagier, Gruseldörfer)** | Fluchsänger (verflucht, lässt Spieler stolpern) · Knochenpfeifer (ruft Skelett-Krähen) · Schattenweber (unsichtbar bis zum Angriff) · Puppenmeister (steuert Dorfbewohner-Marionetten) · Hexenmutter (Elite, Fluchkreise) |
| Kollegium-Vollstrecker (nur bei schlechtem Ruf/Story) | Ediktwache (Hellebarde, Schild) · Bannmagier (Schild gegen Weisen) |
| Wilde Kreaturen | Firnwolf · Borkenbär · Moorlicht-Unke · Glasnatter · Salzkrake (Jungtier) · Pilzbrut (Schwarm) |
| Kael-Konstrukte | Klangwächter (Golem, reagiert auf Ton) · Scherbengolem · Sandsänger (Fernkampf, Echo) |
| Nachhall-Wesen | Stummling · Echohund · Leerchor (Schwarm aus Stimmen) · Splitterwandler (Teleport) |

### 5.2 Bosse

**10 Regionsbosse** (je 1 pro Region, im großen Dungeon):

| Region | Boss | Mechanik in einem Satz |
|---|---|---|
| Nocturna & Ilsensee | **Die Seeglocke** (versunkene Glocke, belebt) | Wellen von Schall, man pariert im Takt der Glockenschläge |
| Raunwald | **Der Wurzelvater** | Zwei Phasen: Ranken brennen, dann Kern freilegen mit Ziehklang |
| Kesselbrunn | **Schmelzer** (Hüttenkonstrukt in der Mine) | Lava-Kanäle umleiten, mit Frost abkühlen |
| Graumoor | **Moorkönigin** (riesige Moorlicht-Unke) | Irrlichter löschen, im Nebel nur über Ton orten |
| Glastiefen | **Kristallwal** | Kampf auf schwimmenden Kristallen, Resonanzpunkte treffen |
| Tiedmar | **Die Salzmutter** (Krake im Hafenbecken) | Tentakel binden, Hafenkräne als Waffen |
| Weiße Zinnen | **Firnfürst** (Alpha-Firnwolf + Lawine) | Lawinen auslösen/ablenken, Sturmweise auf vereiste Flanken |
| Kaelschlucht | **Der Letzte Sänger** (Kael-Konstrukt) | Spielt Akkorde vor, Spieler muss Gegenakkorde spielen |
| Treibende Inseln | **Sturmrochen** | Luftkampf auf dem Reittier |
| Aurenburg | **Der Großinquisitor** (Kollegium) | Bannschilde, nur mit Urweisen zu brechen, kostet Ruf |

**5 Story-Bosse** (je Akt): Akt 1 **Ausbilder Varn** (manipuliert, Stille Hand) · Akt 2 **Mara Vandermeer, „die Taktgeberin“** · Akt 3 **Der Wächter von Kael** · Akt 4 **Corvin Hale** (Verräter) · Akt 5 **Der Dirigent** (3 Phasen, letzte im Nachhall).

## 6. Erkundung und Bewegung

- Laufen, Sprinten, Springen, Ausweichen, **Klettern** (markierte Kanten + Ranken), **Schwimmen/Tauchen**, **Gleiten** mit dem Umhang (aus der Weise Gleitschleier, ab Akt 1 Ende).
- **Reittiere:** mehrere eigene Arten für Luft, Boden und Wasser (CHARACTER_BIBLE §7). Fliegen mit Ausdauer, Upgrades, Rennen, Aussehen anpassbar.
- **Karte:** Weltkarte mit Nebel des Krieges, Aussichtspunkte (Glockentürme) lüften Nebel, Schnellreise über Aderquellen.
- **Kompass** oben, ausblendbar. Marker nur für aktive Quest + gesetzte eigene Marker.
- **Rätselschreine** (40+): Mechaniken: Klangfolgen nachspielen, Licht mit Spiegeln umlenken, Wiederklang-Reparatur, Gewichte mit Leichtklang, Elementar-Schalter, Nachhall-Ebenen (nur im Nachhallschritt sichtbar), Zeit-Platten, Wasserstand, Echo-Labyrinth (blind, nur Ton).

## 7. Systeme (Übersicht, umgesetzt ab Phase 2)

| # | System | Kurz-Design | Phase | Modul |
|---|---|---|---|---|
| 1 | Third-Person-Controller | Motion Matching, Klettern, Schwimmen, Gleiten | 3 | Core |
| 2 | Zaubersystem | 430 Weisen (60 Kern, 358 Alltag, 12 Koop), Baukasten Grundton × Wirkart, Probier-Modus, Ringe, Combos, Elementar (WEISEN.md) | 3 | Magic |
| 3 | Kampf | Lock-On, Reinklang, Schwachstellen, StateTree | 3 | Combat, AI |
| 4 | Quests | JSON, Bedingungen, Verzweigung, Flags | 2 | Quest |
| 5 | Dialoge | Verzweigung, Skill-/Ruf-Checks, Emotions-Tags, Kamera | 2 | Dialogue |
| 6 | Entscheidungen | globale Flags, Beziehungswerte (−100…+100), Ruf bei 3 Fraktionen | 2 | Quest |
| 7 | Inventar & Ausrüstung | 10 Kleidungsslots + 4 Stimmringe + Handschuhe (siehe §13), Transmog, Seltenheit: Gewöhnlich, Gestimmt, Rein, Meisterhaft, Uralt | 2/5 | Core |
| 8 | Crafting | Tränke (Minispiel: Hitze + Rühr-Rhythmus), Pflanzenbeete im Turm, Stimmringe schmieden und verbessern bei Ambros Kettler | 5 | Core |
| 9 | Progression | Level 1–40, Talentbaum je Schule (5 Bäume × 12 Talente), Weisen über Unterricht, Meister, Mitschüler, Kreaturen, Feste, Probieren (WEISEN §4) | 3/5 | Magic |
| 10 | Begleiter | 1 aktiver Begleiter (Lio, Aurelia ab Akt 2, Nell in Tiedmar, Fink in Aurenburg), eigene Kampf-KI, Kommentare | 5 | AI |
| 11 | Reittiere | 6 Arten (Luft/Boden/Wasser), Stall in der Akademie, Upgrades, Rennen, Sattel/Rüstung/Fellfarbe im Laden | 5 | Core |
| 12a | Personalisierung & Läden | Charakter bis ins Detail über Läden in der Welt (Schneider, Barbier, Tätowierer, Ringschmied, Stallmeister …), siehe §13 | 2/5 | Core, UI |
| 12 | Housing | Turmzimmer: Möbel, Pflanzen, Kreaturen-Ecke, Trophäen | 5 | Core |
| 13 | Karte & Kompass | Nebel des Krieges, Marker, Schnellreise | 5 | UI |
| 14 | Speichern/Laden | mehrere Slots, Autosave, kompletter Weltstatus | 2 | Save |
| 15 | UI/UX | CommonUI, minimal-HUD, Kodex, Optionen, Barrierefreiheit | 2–5 | UI |
| 16 | Lokalisierung | String Tables DE/EN, alle Texte | 2 | Core |

### 7.1 Fraktionen und Ruf

Drei Fraktionen (Ruf −100 bis +100, Stufen: Feind, Misstrauisch, Neutral, Geschätzt, Verbündet). Jede hat eine eigene Questlinie (siehe STORY_BIBLE §6).

| Fraktion | Sitz | Belohnungen |
|---|---|---|
| **Das Hohe Kollegium** (Magierregierung) | Aurenburg | Zugang zu Archiven, legale Schnellreise in die Hauptstadt, Ediktwachen greifen nicht an |
| **Die Hüter der Adern** | Graumoor, Raunwald | Aderquellen als Schnellreise, Kreaturen-Zähmung, Nachhall-Reinigung |
| **Die Freien Zünfte** | Tiedmar, Kesselbrunn | Händlerpreise, seltene Stimmringe und Stoffe, Schmuggelpfade |

### 7.2 Unterrichts-Minispiele

Tränkekunde (Rhythmus-Rühren + Hitze), Wandelkunde (Formen nachzeichnen, Knopf-zu-Maus), Sternkunde (Sternbilder und Mondphasen einstellen, Teleskop, nur nachts), Kreaturenkunde (füttern, beruhigen per Summen), Klangkunde (Akkord-Gehörtraining, Basis für neue Weisen), Schlagweisen (Zielwerfen auf Glocken, Kegelbahn), Elementarkunde (Elemente kombinieren, Kerzenrennen), Schutzweisen und Duellkunde (Parier-Rhythmus, Arena-Übungskämpfe). Jede Stunde 5–10 min, endet mit einer Weise. Lehrkräfte, Stundenplan, Glockenball, Nachsitzen: [AKT1_SCHULLEBEN §5](AKT1_SCHULLEBEN.md#5-unterricht).

## 8. Steuerung (Standardbelegung)

| Aktion | Tastatur/Maus | Gamepad |
|---|---|---|
| Bewegen / Kamera | WASD / Maus | linker / rechter Stick |
| Sprinten / Gleiten | Shift / Leertaste in der Luft | L3 / A in der Luft |
| Springen / Klettern | Leertaste | A |
| Ausweichen | Alt | B |
| Basisangriff (Funkenstoß) | linke Maustaste | RT |
| Glockenschild / Parieren | rechte Maustaste | LT |
| Weise 1–4 | 1–4 | RB + A/B/X/Y |
| Ring wechseln | Mausrad / Q halten (Rad) | LB halten (Rad) |
| Lock-On | mittlere Maustaste | R3 |
| Interagieren | E | X |
| Urweise (Kontext) | F | LB + RB |
| Karte / Menü | M / Esc | Ansicht / Menü |

Umsetzung über **Enhanced Input** mit Input-Mapping-Contexts: Erkundung, Kampf, Flug, Schwimmen, Menü, Fotomodus. Alles frei belegbar.

## 9. UI/UX und Barrierefreiheit

- HUD: Atem-Bogen am Fadenkreuz, Urklang als feiner Ring, Gesundheit unten links, Kompass oben, alles einzeln ausblendbar.
- Menüs: Hauptmenü mit Live-Szene (Nocturna bei Nacht, Monde ziehen), Questlog, Inventar, Talentbäume, Kodex/Lore (Kreaturen, Orte, Personen, Weisen), Karte, Optionen.
- Barrierefreiheit: Untertitel (Größe, Hintergrund, Sprechername, Farbe), farbenblinde Modi für Elementar-Farben (zusätzlich Symbole), Zielhilfe, Parierfenster-Regler, Kamera-Wackeln aus, Motion Blur aus, Text-to-Speech für Menüs (Phase 11), **visuelle Klang-Hinweise** (wichtig, weil Rätsel auf Ton basieren: jede Ton-Mechanik hat ein visuelles Gegenstück).

## 10. Inhalt und Spielzeit

| Bereich | Menge | Spielzeit |
|---|---|---|
| Hauptmissionen | 50 (10 je Akt) | 45–50 Std. |
| Questreihen | 6 mit je 6–10 Quests (48 Quests) | 20–24 Std. |
| Fraktions-Questlinien | 3 × 5 Quests | 6–8 Std. |
| Sidequests | 60 (6 je Region) | 10–12 Std. |
| Rätselschreine | 40+ | 4–5 Std. |
| Schulleben-Quests Akt 1 (S101–S118) | 18 | ca. 8 Std. (Teil der Sidequests) |
| Aktivitäten (Rennen, Arena, Zähmen, Housing, Unterricht, Glockenball, Weisen sammeln) | 430 Weisen | 6–10 Std. |
| **Summe** | | **ca. 95–110 Std.** |

## 11. Produktionsplan und ehrliche Risiken

**Realität:** 100 Stunden Spielzeit mit Cinematic-Grafik ist für ein großes Studio ein Mehrjahresprojekt. Allein (mit mir) ist es nur machbar, wenn wir **streng in Scheiben** arbeiten und Inhalte maximal datengetrieben und prozedural erzeugen. Darum:

1. **Phase 4 (Benchmark-Szene)** ist der Qualitätsmaßstab. Erst wenn sie steht, wird in die Breite gebaut.
2. **Phase 5 (Vertical Slice, Akt 1)** ist das erste echte Ziel. Alles danach ist Erweiterung.
3. Regionen werden aus **Modul-Kits + PCG** gebaut, nicht von Hand Stein für Stein.
4. Dialoge und Quests sind Daten, die ich schreibe. Dein Engpass ist Editor-Arbeit, darum automatisieren wir mit Python und Unreal MCP so viel wie geht.

| Risiko | Wahrscheinlichkeit | Gegenmaßnahme |
|---|---|---|
| Umfang sprengt Zeit | hoch | Vertical Slice zuerst, Akte 2–5 können kürzer werden, ohne die Story zu brechen (Missionen sind als Kern/Optional markiert) |
| 12 GB VRAM für Cinematic + MetaHumans | mittel | LOD-Disziplin, MetaHuman-LODs, Budget-Tabelle in ART_BIBLE §9 |
| Gesichtsanimation ohne iPhone | mittel | Audio-driven MetaHuman Animator + Webcam-Mono-Video (SETUP 8a) |
| Konsistente Charakter-Concepts mit FLUX.1-schnell | mittel | fester Seed + sehr genaue Beschreibung; Concepts sind Referenz, nicht Endprodukt (Endprodukt ist der MetaHuman) |
| Kreaturen-Animation (Cascadeur Free nicht kommerziell) | mittel | Fab-Gratis-Rigs + Control Rig + Blender; Kreaturen bewusst mit wenigen, starken Bewegungen |
| Plugins brechen mit Engine-Updates | mittel | Versionen gepinnt in `Plugins.json`, Engine-Update nur zwischen Phasen |

## 12. Offene Punkte für Phase 2

- Flow vs. eigenes JSON-Questsystem (siehe [PLUGINS.md](PLUGINS.md)).
- JSON-Schema für Quest, Dialog, Item, Weise, Kreatur, NPC.
- Validierungs-Skript (fehlende IDs, tote Verzweigungen, Übersetzungen, Spielzeit-Summe).

## 13. Personalisierung und Läden

Ziel: **Jeder Spielercharakter sieht anders aus, bis ins kleinste Detail.** Der Charakter-Editor am Anfang ist nur der Start. Alles Weitere kauft, findet oder erarbeitet man in der Welt. Ein Spiegel in jedem Laden und im Turmzimmer zeigt Änderungen live.

| Laden / Ort | Was man ändern kann | Wo |
|---|---|---|
| **Schneiderei** | 10 Kleidungsslots: Kopf, Gesicht (Brille, Maske), Hals (Schal, Kette), Oberkörper, Umhang/Mantel, Gürtel, Handschuhe, Beine, Schuhe, Tasche. Jedes Teil mit 3 Farbzonen (frei färbbar mit Farbstoffen), Muster (Karo, Streifen, Stickerei), Material (Wolle, Leder, Samt, Leinen) | Kesselbrunn, Tiedmar, Aurenburg (je eigene Mode) |
| **Barbier & Haarstube** | Frisur (40+ Grooms), Haarfarbe inkl. Strähnen und Verlauf, Bart, Augenbrauen, Wimpern | Kesselbrunn, Tiedmar, Aurenburg |
| **Tätowierer & Malerin** | Tattoos (Körper + Gesicht, Position, Größe, Farbe), Narben, Sommersprossen, Schminke, Kriegsbemalung, leuchtende Klang-Runen (ab Akt 3) | Tiedmar, Kaelschlucht |
| **Ringschmied** (Ambros Kettler) | 4 Stimmringe: Metall, Quarzfarbe (= Farbe der Magie!), Gravur. Werte + Optik | Kesselbrunn |
| **Handschuhmacher** | Handschuhe, Armreife, Fingerkappen | Aurenburg |
| **Spiegelhexe** (freundliche Hexe im Raunwald) | Gesicht nachträglich ändern (Regler wie beim Start), Augenfarbe, Stimme | Raunwald, gegen Kräuter |
| **Stallmeister** | Reittiere: Sattel, Zaumzeug, Rüstung, Fellfarbe/Muster, Schmuck, Name | Akademie-Stall, jede Region 1 Stall |
| **Möbeltischler** | Turmzimmer: Möbel, Teppiche, Bilder, Pflanzen | Kesselbrunn, Tiedmar |
| **Farbstoff-Händler** | 60+ Farbstoffe (sammeln, kaufen, aus Pflanzen/Kreaturen brauen) | überall |

**Regeln:**
- **Transmog:** Werte eines Teils und Aussehen eines anderen kombinierbar. Optik-Sets speicherbar (5 Plätze, z. B. „Unterricht“, „Kampf“, „Fest“).
- **Magie-Farbe** kommt aus den Stimmringen: Ein Spieler mit rot-goldenen Ringen hat rot-goldene Funken. Teil der Personalisierung.
- **Nichts nur gegen Echtgeld**, keine Mikrotransaktionen. Alles erspielbar.
- **Kleidung reagiert:** nass bei Regen, Schnee auf den Schultern, Ruß nach Feuer, verblasst nicht dauerhaft.
- Technik: modulare Skeletal Meshes mit Leader Pose (CHARACTER_BIBLE §1.1), Farbzonen über Material-Masken (RGB-Maske = 3 Zonen), Muster als Detail-Textur. So reichen wenige Meshes für tausende Kombinationen.

## 14. Dörfer mit Kontrast

Jede Region hat **mindestens ein fröhliches und ein unheimliches Dorf** (Liste: STORY_BIBLE §4.1).

| | Fröhliche Dörfer | Gruseldörfer (Missklinger) |
|---|---|---|
| Stimmung | Musik auf dem Platz, Kinder spielen, Märkte, Feste, Laternen, warme Farben | Stille, Nebel, Krähen, verriegelte Fenster, Puppen an Türen, falsche Freundlichkeit |
| NPCs | grüßen, schenken Kleinigkeiten, laden zum Essen ein, Gerüchte, Minispiele (Tanzen, Würfeln, Wettessen) | starren, flüstern, folgen dem Spieler, manche sind Marionetten |
| Gameplay | Läden, Quests, Tiere, Housing-Deko, Rast (Bonus) | Rätsel, Schleichen, Fallen, Gruselmomente, Kampf gegen Schwarzmagier, Befreiung |
| Nach der Befreiung | – | Gruseldorf kann sich in ein fröhliches Dorf verwandeln (Farbe kehrt zurück, Musik spielt, neuer Laden). Belohnung, die man sieht. |

## 15. Weltgröße und Aufbau (E9 = Option A)

| Bereich | Größe | Inhalt |
|---|---|---|
| Kern um die Akademie (Akt 1) | ca. 16 km² | Nocturna, Ilsensee, Raunwald, Kesselbrunn + 2 Dörfer |
| Endausbau (Akt 5) | ca. 64 km² + Treibende Inseln (Luftraum) + Nachhall (eigene Ebene) | 10 Regionen, 25+ Dörfer/Weiler, 3 Städte |
| Dichte | alle 300–500 m etwas Sehenswertes | Dorf, Ruine, Höhle, Rätselschrein, Lager, Aussichtspunkt, Tiere, Händler unterwegs |
| Technik | World Partition (Zellen 128 m), HLODs für Fernsicht, PCG für Wildnis, Python-Skripte für Platzierung aus JSON | Landschaft 8 × 8 km mit 1 m Auflösung, Höhendaten selbst erzeugt |
| Fliegen | Höhenlimit 1200 m, Wolkendecke als Grenze | Fernsicht bis 6 km (Nebel + HLOD) |
| Speicher (geschätzt) | 80–150 GB Projekt, 30–50 GB fertiges Spiel | |
