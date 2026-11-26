# GDD – Nocturna (Game Design Document)

Stand: 26.11.2026 · Phase 1 · Version 0.1
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
| **A: Resonanz (Klangmagie)** ⭐ Empfehlung | Magie ist Klang. Zauber („Weisen“) sind Akkorde aus Grundtönen, gespielt über einen Stimmstab. Die verbotene alte Magie ist der **Urton**, der Ton unter allen Tönen: er löscht und schreibt Wirklichkeit um. | Sehr eigenständig, klar von anderen Zaubererwelten getrennt. Gibt Audio, VFX und Animation ein gemeinsames Leitmotiv. Dein Akkordeon wird Teil der Welt (Akkorde = Zauber, Soundtrack-Leitmotive = Weisen). | Ungewohnt. Muss in Audio sauber umgesetzt werden, sonst wirkt es nur wie ein Name. |
| B: Erinnerungsmagie | Magie ist das Gedächtnis der Welt. Verboten: Erinnerungen umschreiben. | Starke Story-Twists möglich. | Kampf-Zauber wirken aufgesetzt, schwer zu visualisieren. |
| C: Klassisches Mana über Kraftlinien | Energie aus Linien im Boden, Elemente. | Leicht verständlich, einfach zu bauen. | Generisch, sieht aus wie jedes andere Fantasy-RPG. |

### E3 – Häuser

**Empfehlung:** 4 **Türme** statt „Häuser“-Begriff, Spieler **wählt** nach Gesprächen mit den vier Turmsprechern (keine magische Zuteilung). Details: [CHARACTER_BIBLE §3](CHARACTER_BIBLE.md#3-die-vier-türme-häuser).
Alternative: zufällige Zuteilung durch die Glocke der Akademie. Nachteil: nimmt dem Spieler die Wahl und erinnert zu stark an bekannte Vorbilder.

### E4 – Spielerfigur

**Empfehlung:** Frei gestaltbar (männlich/weiblich/divers über Körper- und Stimmwahl), Name frei, Alter 15 zu Beginn, 20 am Ende. Stimme: 3 Varianten (TTS-Platzhalter, später Sprecher).
Alternative: feste Figur mit Namen. Vorteil: stärkere Cutscenes. Nachteil: widerspricht dem Charakter-Editor im Master-Prompt.

### E5 – Zaubersystem-Technik (Entscheidung erst Phase 3, hier nur vorgemerkt)

GAS (Gameplay Ability System) vs. eigenes Komponentensystem. Vorläufige Empfehlung: **GAS**, weil Cooldowns, Kosten, Tags, Effekte (Nässe, Brennen) und Netzwerk-Robustheit fertig dabei sind.

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

## 3. Spielerfantasie und Kern-Loops

**Fantasie:** „Ich bin ein junger Klangwirker mit einer gefährlichen Gabe. Ich lerne, entdecke Geheimnisse, beschütze meine Freunde und entscheide, was aus der Magie wird.“

| Loop | Dauer | Inhalt |
|---|---|---|
| Moment | 30 s | Zauber-Akkorde kombinieren, parieren, Elementar-Reaktionen auslösen, klettern/gleiten |
| Erkundung | 5–15 min | Ort entdecken → Rätselschrein / Kampf / Lore-Fund → Belohnung (Material, Weise, Kodex) |
| Quest | 45–60 min | Mission mit Dialog, Ort, Rätsel, Kampf, Entscheidung, Cutscene |
| Akademie-Woche | 2–4 Std. | Unterricht (Minispiel) → neue Weise → Sidequests → Turmzimmer ausbauen → Story-Mission |
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

### 4.2 Weisen (Zauber): 25 in 5 Schulen

Steuerung: 4 aktive Weisen auf dem Schnellwahl-Ring, bis zu 4 Ringe (16 Slots), Wahlrad für Ringe. Basisangriff (Funkenstoß) immer verfügbar.

| # | Weise | Schule | Effekt | Interaktionen / Combos | Freischaltung |
|---|---|---|---|---|---|
| 1 | **Funkenstoß** | Schlag | Basisprojektil, schnelle 3er-Kette | Kette endet mit Rückstoß | Start |
| 2 | **Hammerton** | Schlag | Schwere Schockwelle nach vorne, wirft um | auf gezogene Gegner doppelter Schaden | Akt 1 |
| 3 | **Splitterchor** | Schlag | Fächer aus Quarzsplittern, nah stark | durchschlägt Eisrüstung | Akt 1 |
| 4 | **Ziehklang** | Schlag | Zieht Gegner/Objekt heran | danach Hammerton = Combo „Schlagzug“ | Akt 1 |
| 5 | **Zerrton** | Schlag | Kanalisierter Strahl, bricht Schilde | gegen Bannmagier-Schilde Pflicht | Akt 2 |
| 6 | **Glockenschild** | Schutz | Block; perfektes Timing = **Reinklang** (Konter + Atem zurück) | Basis jeder Verteidigung | Start |
| 7 | **Spiegelklang** | Schutz | Reflektiert Projektile 2 s lang | reflektierte Glut zündet Öl | Akt 1 |
| 8 | **Haltelied** | Schutz | Hält Gegner 4 s in der Luft fest | festgehaltene Gegner + Sturmweise = Kettenstrom | Akt 1 |
| 9 | **Atemwende** | Schutz | Heilt über Zeit, füllt Atem | Begleiter wird mitgeheilt (Talent) | Akt 2 |
| 10 | **Klangwand** | Schutz | Barriere, hält Projektile, 8 s | Glut durch die Wand = Feuerwand | Akt 2 |
| 11 | **Glutweise** | Elementar | Feuerball, Brennen-Effekt | Feuer + Öl = Explosion; Feuer + Eis = Dampfwolke (Sichtschutz) | Akt 1 |
| 12 | **Frostweise** | Elementar | Eislanze, Verlangsamung, friert Wasser | Eis + Wasser = Eisfläche/Plattform; gefrorene Gegner zerbrechen bei Hammerton | Akt 1 |
| 13 | **Sturmweise** | Elementar | Blitz springt auf 3 Ziele | Blitz + Nässe = doppelte Reichweite und Betäubung | Akt 2 |
| 14 | **Flutweise** | Elementar | Wasserschwall, macht nass, löscht Feuer | bereitet Sturm- und Frostweise vor | Akt 2 |
| 15 | **Erdweise** | Elementar | Felsdorn aus dem Boden, auch als Plattform | Erde + Wasser = Schlamm (verlangsamt) | Akt 3 |
| 16 | **Wiederklang** | Wandel | Repariert Zerbrochenes (Brücken, Statuen) | Kern vieler Rätsel | Akt 1 |
| 17 | **Leichtklang** | Wandel | Lässt Objekte schweben, werfen | geworfene Fässer + Glut | Start (Tutorial) |
| 18 | **Wachsklang** | Wandel | Lässt Ranken wachsen (Klettern, Fesseln) | Ranken + Glut = Feuerweg | Akt 2 |
| 19 | **Formwandel** | Wandel | Verwandelt schwachen Gegner 6 s in harmloses Tier, Objekte in andere | Tiere fliehen, Bosse immun | Akt 3 |
| 20 | **Schattenweise** | Wandel | Unsichtbarkeit/Tarnung für Schleichpassagen | Überraschungsangriff = 3× Schaden | Akt 2 |
| 21 | **Verstummen** | Ur (verboten) | Löscht alle Weisen in einem Radius (auch Gegnerschilde) | Kosten: Urklang + Nachhall 10 | Akt 1 (Erwachen) |
| 22 | **Nachhallschritt** | Ur | Kurzer Sprung durch den Nachhall (Teleport 8 m, durch Gitter) | Rätsel und Flucht | Akt 2 |
| 23 | **Seelenecho** | Ur | Zeigt Erinnerungen eines Ortes/Objekts (Ermittlung) | Detektiv-Questreihe | Akt 2 |
| 24 | **Entklang** | Ur | Löst Materie/Barrieren auf, enormer Schaden | Nachhall 20 | Akt 3 |
| 25 | **Grundton** | Ur | Ultimative Weise: Flächeneffekt, schreibt Gegner „um“ | Nachhall 40, nur mit vollem Urklang | Akt 4 |

**Akkord-Darstellung:** Jede Weise hat drei Grundtöne (siehe STORY_BIBLE §3.2). Im Kodex und im HUD als drei kleine farbige Noten sichtbar. Weisen mit gemeinsamem Grundton haben schnellere Übergänge (Combo-Fenster +0,3 s). So entsteht eine Lernkurve, ohne dass der Spieler Musik können muss.

### 4.3 Status-Effekte

Nass, Brennen, Gefroren, Betäubt, Schwebend, Verstummt (keine Weisen), Nachhall-berührt (nur Gegner aus dem Schattenreich, verwundbar gegen Urweisen).

## 5. Kampf

- **Lock-On** (Tab/rechter Stick drücken), Zielwechsel per Stick/Mausrad.
- **Parieren:** Glockenschild im Fenster von 0,2 s vor Treffer = Reinklang (Zeitlupe 0,3 s, Konterstoß, +15 Atem, +Urklang). Normal blocken kostet Atem.
- **Ausweichen:** Rolle oder kurzer Schritt (mit i-Frames), Ausdauer = Atem.
- **Schwachstellen:** Gegner haben Farb-Schilde (Glut-Schild nur mit Frost brechbar usw.) und körperliche Schwachpunkte (Kristalle, Rüstungsfugen).
- **Gegner-KI:** StateTree (UE 5.8) mit Gruppen-Koordinator (max. 2 greifen gleichzeitig nah an, Fernkämpfer flankieren).
- **Schwierigkeit:** Geschichte / Normal / Schwer / Meister + Einzeloptionen (Parierfenster, Gegnerschaden, Zielhilfe).

### 5.1 Gegnertypen (25)

| Gruppe | Typen |
|---|---|
| Stille Hand (Orden) | Novize · Schweiger (Nahkampf, verstummt Weisen) · Taktgeber (Fernkampf, synchronisiert Gruppen) · Choralist (stärkt Verbündete) · Dirigentenwache (Elite, Schild) |
| Wilderer & Schmuggler | Klingenschnapper (Messer, schnell) · Fallensteller (Netze, Öl) · Schmugglerwirker (Elementar) · Wildererhund (Tier) |
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

- Laufen, Sprinten, Springen, Ausweichen, **Klettern** (markierte Kanten + Ranken), **Schwimmen/Tauchen**, **Gleiten** mit dem Umhang (Leichtklang-Talent, ab Akt 1 Ende).
- **Reittier:** Sturmschwinge (greifenartig, siehe CHARACTER_BIBLE §5). Fliegen mit Ausdauer, Upgrades (Ausdauer, Tempo, Sturzflug-Boost), Flugrennen.
- **Karte:** Weltkarte mit Nebel des Krieges, Aussichtspunkte (Glockentürme) lüften Nebel, Schnellreise über Aderquellen.
- **Kompass** oben, ausblendbar. Marker nur für aktive Quest + gesetzte eigene Marker.
- **Rätselschreine** (40+): Mechaniken: Klangfolgen nachspielen, Licht mit Spiegeln umlenken, Wiederklang-Reparatur, Gewichte mit Leichtklang, Elementar-Schalter, Nachhall-Ebenen (nur im Nachhallschritt sichtbar), Zeit-Platten, Wasserstand, Echo-Labyrinth (blind, nur Ton).

## 7. Systeme (Übersicht, umgesetzt ab Phase 2)

| # | System | Kurz-Design | Phase | Modul |
|---|---|---|---|---|
| 1 | Third-Person-Controller | Motion Matching, Klettern, Schwimmen, Gleiten | 3 | Core |
| 2 | Zaubersystem | 25 Weisen, Ringe, Combos, Elementar | 3 | Magic |
| 3 | Kampf | Lock-On, Reinklang, Schwachstellen, StateTree | 3 | Combat, AI |
| 4 | Quests | JSON, Bedingungen, Verzweigung, Flags | 2 | Quest |
| 5 | Dialoge | Verzweigung, Skill-/Ruf-Checks, Emotions-Tags, Kamera | 2 | Dialogue |
| 6 | Entscheidungen | globale Flags, Beziehungswerte (−100…+100), Ruf bei 3 Fraktionen | 2 | Quest |
| 7 | Inventar & Ausrüstung | 6 Kleidungsslots + Stimmstab (Griff, Schaft, Quarz), Transmog, Seltenheit: Gewöhnlich, Gestimmt, Rein, Meisterhaft, Uralt | 2/5 | Core |
| 8 | Crafting | Tränke (Minispiel: Hitze + Rühr-Rhythmus), Pflanzenbeete im Turm, Stimmstab-Teile verbessern bei Ambros Kettler | 5 | Core |
| 9 | Progression | Level 1–40, Talentbaum je Schule (5 Bäume × 12 Talente), Weisen über Unterricht/Quests | 3/5 | Magic |
| 10 | Begleiter | 1 aktiver Begleiter (Lio, Aurelia ab Akt 2, Nell in Tiedmar, Fink in Aurenburg), eigene Kampf-KI, Kommentare | 5 | AI |
| 11 | Reittier | Sturmschwinge, Fliegen, Upgrades, Rennen | 5 | Core |
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
| **Die Freien Zünfte** | Tiedmar, Kesselbrunn | Händlerpreise, seltene Stimmstab-Teile, Schmuggelpfade |

### 7.2 Unterrichts-Minispiele

Tränkekunde (Rhythmus-Rühren + Hitze), Wandelkunde (Formen nachzeichnen), Sternkunde (Sternbilder und Mondphasen einstellen, Teleskop), Kreaturenkunde (füttern, beruhigen per Summen), Klangkunde (Akkord-Gehörtraining, Basis für neue Weisen), Duellkunde (Arena-Übungskämpfe).

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
| Aktivitäten (Rennen, Arena, Zähmen, Housing, Unterricht, Sammeln) | – | 6–10 Std. |
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
