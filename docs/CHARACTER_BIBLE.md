# CHARACTER BIBLE – Nocturna

Stand: 26.11.2026 · Phase 1 · Version 0.1
Inhalt: Pipeline · MetaHuman-Grundanleitung · 4 Türme (Häuser) · 15 Hauptfiguren · NPCs & Crowds · 20 Kreaturen + Reittier.
Concept-Prompts: `Scripts/ComfyUI/prompts/characters.json`, `houses.json`, `creatures.json` (Bilder landen in `art/concepts/…`).

---

## 1. Pipeline

### 1.1 Spielercharakter

| Baustein | Umsetzung |
|---|---|
| Gesicht | 8 eigene MetaHuman-Presets als Startpunkte (4 eher weiblich, 4 eher männlich, alle Hauttöne abgedeckt) + Regler im Spiel: Morph-Targets über eigene Blendshapes (Kiefer, Nase, Augenabstand, Wangen, Brauen). Kein Live-MetaHuman-Creator im Spiel (geht technisch nicht). |
| Haut | 6 Haut-Albedo-Varianten × Tönungsregler (Material-Parameter) |
| Haare | 10 Grooms (inkl. 2 Locken-, 1 Afro-, 1 Zopf-Variante), Farbe frei per Melanin/Rot-Regler |
| Augen | Irisfarbe frei, 2 Sonder-Varianten (Heterochromie, „Urton-Schimmer“ ab Akt 3 automatisch) |
| Narben/Male | 12 Decals im Gesichts-Material (Maske + Detail-Normal) |
| Stimme | 3 Varianten (hell, mittel, tief), Lautgeber nur in Kampf/Emotes, Dialoge ohne Spielerstimme (Textwahl) |
| Körper | 3 MetaHuman-Körpertypen × Größe (Skalierung ±5 %) |
| Outfit | **Modular:** Kopf, Oberkörper, Umhang, Hände, Beine, Schuhe = einzelne Skeletal Meshes, gemeinsames Skelett, Leader Pose Component (Kleidung folgt dem Körper ohne eigenes AnimBP) |
| Stoff | **Chaos Cloth** auf Umhang, Robensaum, Schal (Wind, Laufen, Gleiten, Fliegen). Budget: max. 2 Cloth-Meshes pro Figur gleichzeitig |
| Hände & Magie | Kein Stab. 4 Stimmringe als Static Meshes an Finger-Sockets (`ring_01_l` … `ring_04_r`), Quarzfarbe bestimmt Niagara-Farbe. Handschuhe als eigener Outfit-Slot. VFX-Sockets an allen 10 Fingerspitzen (`fx_tip_*`) |

**Personalisierung nach dem Start:** Alles oben ist später in Läden änderbar (Schneider, Barbier, Tätowierer, Ringschmied, Spiegelhexe, Stallmeister). Liste und Regeln: [GDD §13](GDD.md#13-personalisierung-und-läden). Technik dafür: Kleidung mit 3 Farbzonen (RGB-Maske) + Muster-Detailtextur, Haar-Farbverlauf im Groom-Material, Tattoos/Narben als Decal-Layer im Haut-Material (Position/Skalierung per UV-Offset).

### 1.2 Story-Figuren

Jede der 15 Hauptfiguren ist ein eigener MetaHuman (volle Qualität, LOD 0–1 in Cutscenes, LOD 2–3 im Spiel). Anleitung pro Figur unten im Blatt.

## 2. MetaHuman-Grundanleitung (gilt für alle Blätter)

Seit UE 5.6 läuft der MetaHuman Creator **im Editor** (kein Browser mehr). Die Reglernamen können sich zwischen 5.6 und 5.8 leicht unterscheiden. Wenn ein Begriff bei dir anders heißt, nimm den naheliegendsten und schreib mir den echten Namen, dann passe ich die Bibel an.

1. Plugins prüfen: *Edit → Plugins →* „MetaHuman Creator“ und „MetaHuman Core“ aktiv → Editor neu starten.
2. Content Browser → Ordner `Content/Characters/MetaHumans/<Name>` anlegen → Rechtsklick → *MetaHuman → MetaHuman Character* → Name `MHC_<Name>`.
3. Doppelklick → MetaHuman Creator öffnet sich.
4. **Preset:** Links *Presets/Templates* → das im Blatt genannte Ausgangsprofil wählen (Beschreibung der Merkmale steht dabei, weil die Presets keine festen Namen haben, die über Versionen gleich bleiben).
5. **Blend:** Im Gesichtsmodus *Blend* wählen → 2–3 weitere Presets in die Blend-Slots ziehen → einzelne Regionen (Augen, Nase, Mund, Kiefer) per Ziehen am Gesicht zu den Zielmerkmalen mischen.
6. **Sculpt/Move:** Feinkorrektur der Merkmale aus dem Blatt (z. B. „Kinn kantiger“).
7. **Skin:** Hautton-Feld (Wert im Blatt als Richtung: hell/mittel/dunkel + warm/kühl), Textur-Variante mit Sommersprossen/Falten nach Alter, Rauheit.
8. **Eyes:** Irisfarbe, Sklera leicht rötlich für ältere Figuren.
9. **Makeup:** nur wo angegeben (Augenringe, Rötung).
10. **Hair:** Groom laut Blatt, Farbe über Melanin/Rotanteil, Brauen/Bart passend.
11. **Body:** Körpertyp und Größe laut Blatt.
12. **Wardrobe:** vorerst Standardkleidung, unsere Kostüme kommen ab Phase 3 als eigene Meshes.
13. *Assemble/Build* → Qualität **Cinematic** für Story-Figuren, **Optimized** für Crowds → Ausgabe nach `Content/Characters/MetaHumans/<Name>`.
14. Screenshot vorne + seitlich nach `art/concepts/characters/<Name>/mh_*.png`, in `PROGRESS.md` Spalte „MetaHuman“ abhaken.

## 3. Die vier Türme (Häuser)

Nocturna hat vier Türme. Neue Schüler **wählen** ihren Turm nach Gesprächen mit den Turmsprechern (Mission M102). Jeder Turm wurde 402 n. E. von einer anderen Gruppe gestiftet.

| Turm | Lage & Stifter | Werte | Farben (Primär / Sekundär / Akzent) | Wappen | Gemeinschaftsraum | Uniform-Detail |
|---|---|---|---|---|---|---|
| **Aschenhain** | Ostturm, gestiftet von Überlebenden eines Waldbrandes (Köhler und Schmiede) | Beharrlichkeit, Wiederaufbau, Treue | Glutorange `#C4561D` / Anthrazit `#2B2B2E` / Messing `#B08D57` | Verkohlter Baumstumpf, aus dem ein grüner Trieb wächst | Alte Schmiede mit offenem Kamin, Ambosse als Tische | Ärmelsaum orange, Messingknöpfe |
| **Silberquell** | Nordturm, gestiftet von Gelehrten und Sternkundigen | Wissen, Wahrheit, Geduld | Tiefblau `#1F3A5F` / Silber `#C9CED6` / Perlweiß `#EDEBE6` | Eine Quelle unter zwei Monden (groß und klein) | Kuppelraum unter dem Observatorium, Sternkarten an der Decke | Silberner Kragenrand, Mondbrosche |
| **Sturmkamm** | Westturm, gestiftet von Seeleuten und Bergführern aus Tiedmar | Mut, Freiheit, Widerspruch | Sturmgrau `#4A5360` / Blitzgelb `#E3B23C` / Gischtweiß `#F2F4F5` | Seeschwalbe über einem Felsgrat, darunter ein gezackter Blitz | Offener Raum mit Fenstern nach Westen, Hängematten, Wind-Glockenspiel | Gelbe Paspel, Seemannsknoten am Gürtel |
| **Moosgrund** | Südturm, gestiftet von Hütern und Bauern | Fürsorge, Gemeinschaft, Erdung | Moosgrün `#4E6B3A` / Kupfer `#B0693A` / Leinen `#D9CFB8` | Eine Laterne zwischen drei Pilzen | Halb unterirdisch, Wurzeln an den Wänden, Kräuter trocknen an der Decke, Verbindung zu den Gewächshäusern | Kupferne Laternenbrosche, Leinenschal |

**Grunduniform (alle Türme):** knielanger dunkelgrauer Wollmantel mit Stehkragen, Weste, Hemd, Hose oder Rock (frei wählbar), Lederstiefel, Turmschal in Primärfarbe. Im Winter Kapuzenumhang. Kein Spitzhut, keine Krawatte mit Streifen.

## 4. Hauptfiguren (15)

Legende Akt-Bogen: A1 … A5. Stimme: TTS-Platzhalter mit Piper (männlich: `de_DE-thorsten`, Lizenz CC0; weibliche deutsche Stimmen: Lizenz pro Stimme in Phase 3 prüfen, siehe KI_LIZENZEN).

---

### 4.1 Magistra Edda Vornholt – Mentorin

| | |
|---|---|
| Alter / Rolle / Fraktion | 64 · Lehrerin für Klangkunde und Kael · ehem. Kollegium, heute neutral |
| Persönlichkeit | trocken, scharfsinnig, warmherzig nur mit Taten, nie mit Worten. Lacht selten, dann laut. |
| Ziel | das Kind aus Brandheim finden und schützen, ihre Schuld abtragen |
| Geheimnis | leitete 789 das Experiment, das Brandheim auslöschte. Hat den Spieler als Säugling aus den Trümmern getragen und ins Waisenhaus gegeben. |
| Bogen | A1 strenge Lehrerin, die zu viel weiß · A2 schützt den Spieler, lügt dafür · A3 Geständnis in Brandheim · A4 vom Kollegium verhaftet (bei Entscheidung „Verhandeln“) oder kämpft mit · A5 gibt ihren Urklang ab, um den Spieler zu stärken; stirbt in Ende B/C, lebt in A/D |
| Aussehen | schmales, kantiges Gesicht, hohe Wangenknochen, tiefe Lachfalten, eisgraue Augen, graues Haar zu einem strengen Knoten, eine weiße Strähne. Mittelgroß, aufrecht, hager. |
| Kleidung / Palette | lange dunkelgrüne Wollrobe, Lederweste mit vielen Taschen, Stimmgabel an einer Kette · Flaschengrün `#2F4A3A`, Asche `#8A8680`, Messing |
| MetaHuman | Preset: ältere Frau, schmales Gesicht, helle, kühle Haut · Blend: Kiefer kantiger, Augen tiefer liegend · Skin: hell-kühl, Altersfalten hoch, leichte Altersflecken · Eyes: hellgrau · Hair: Knoten (Bun), Grau 85 % · Body: schlank, 168 cm |
| Stimme | alt, klar, leise, spricht in kurzen Sätzen. TTS: weiblich, tiefere Lage, Tempo −10 % |
| Körpersprache / Idle | Hände hinter dem Rücken, klopft mit dem Finger den Takt auf den Unterarm; schaut über die Brille statt durch |

### 4.2 Lio Vandermeer – bester Freund, Begleiter

| | |
|---|---|
| Alter / Rolle / Fraktion | 15 (A1) · Mitschüler, Turm Sturmkamm · Zünfte (Familie: Netzmacher in Tiedmar) |
| Persönlichkeit | witzig, laut, loyal bis zur Dummheit, versteckt Angst hinter Witzen. Spielt Mundharmonika (Duette mit dem Akkordeon-Motiv). |
| Ziel | seine vor drei Jahren verschwundene Schwester Mara finden |
| Geheimnis | glaubt, Mara sei wegen ihm weggelaufen (Streit am Abend ihres Verschwindens) |
| Bogen | A1 Clown und erster Freund · A2 findet Mara bei den Feinden, bricht zusammen · A3 wird ernster, trainiert verbissen · A4 je nach E3: verlässt dich, stirbt beim Angriff oder steht fester denn je · A5 Lagerfeuer-Lied; im Ende D kann er sich opfern |
| Aussehen | rundes, offenes Gesicht, Sommersprossen, breites Lächeln mit Zahnlücke, dunkelbraune Locken, die nie liegen. Mittelbraune, warme Haut. Schlaksig, 172 cm, später 180 cm. |
| Kleidung / Palette | Uniform immer halb offen, gestrickter Fischerpullover darunter, Lederarmband mit Netzknoten · Senfgelb `#D6A23A`, Seegrau, Dunkelblau |
| MetaHuman | Preset: junger Mann, rundes Gesicht, mittlere warme Haut · Blend: Augen größer, Mund breiter · Skin: mittel-warm, Sommersprossen stark · Eyes: haselnussbraun · Hair: kurze, wilde Locken, dunkelbraun · Body: schlank, groß |
| Stimme | jung, schnell, bricht bei Aufregung. TTS: `de_DE-thorsten`, Tonhöhe +15 %, Tempo +10 % |
| Körpersprache / Idle | wippt auf den Fersen, dreht die Mundharmonika zwischen den Fingern, lehnt sich überall an |

### 4.3 Aurelia von Stauffen – Rivalin

| | |
|---|---|
| Alter / Rolle / Fraktion | 15 · Mitschülerin, Turm Silberquell, beste Duellantin ihres Jahrgangs · Kollegium (Tochter des Kanzlers) |
| Persönlichkeit | ehrgeizig, präzise, kalt nach außen, ehrlich bis zur Grausamkeit. Hasst Mitleid. |
| Ziel | ihrem Vater beweisen, dass sie mehr ist als sein Name |
| Geheimnis | weiß seit A2, dass der Spieler den Urton hat, und verrät es nicht. Hat einen verstoßenen Halbbruder (Fink), von dem sie nichts weiß. |
| Bogen | A1 Duell-Rivalin · A2 deckt den Spieler bei der Inspektion · A3 Turnierfinale, erstes ehrliches Gespräch · A4 muss zwischen Vater und Freunden wählen (E5) · A5 Verbündete oder Gegnerin, je nach Beziehung; Kanzlerin im Epilog möglich |
| Aussehen | ovales, symmetrisches Gesicht, schmale gerade Nase, hellblaue Augen, platinblondes Haar zu einem geflochtenen Kranz. Helle, kühle Haut. Schlank, sportlich, 170 cm. |
| Kleidung / Palette | perfekt sitzende Uniform, maßgeschneidert, silberne Manschetten, Duellhandschuh aus weißem Leder · Tiefblau, Silber, Weiß |
| MetaHuman | Preset: junge Frau, ovales Gesicht, helle Haut · Blend: Kinn feiner, Brauen gerade · Skin: hell-kühl, sehr glatt · Eyes: hellblau · Hair: Flechtkranz/Zopf, Platin · Body: athletisch-schlank |
| Stimme | klar, kontrolliert, langsam. TTS: weiblich, mittlere Lage, Tempo −5 % |
| Körpersprache / Idle | steht gerade, Gewicht auf einem Bein, zieht den Handschuh straff; verschränkt die Arme, wenn sie unsicher ist |

### 4.4 Magister Corvin Hale – Lehrer für Elementarkunde und Tränke, **Verräter (Akt 4)**

| | |
|---|---|
| Alter / Rolle / Fraktion | 38 · Lehrkraft ab A2 · heimlich Stille Hand (Choralist) |
| Persönlichkeit | charmant, witzig, „der coole Lehrer“, hört wirklich zu. Ehrlich überzeugt, das Richtige zu tun. |
| Ziel | den Urton des Spielers messen und für den Dirigenten (seinen Halbbruder) nutzbar machen |
| Geheimnis | Brandheim-Überlebender, Halbbruder von Theodric Vaal. Hat Varn (A1) manipuliert. |
| Bogen | A2 Vertrauensperson, hilft beim Verbergen · A3 schickt Briefe, rät zur Vorsicht · A4 öffnet die Tore in der Nacht der Glocken, Story-Boss · A5 tot oder, wenn verschont (Dialogcheck), erscheint er im Finale und bereut |
| Aussehen | gepflegter Dreitagebart, warme braune Augen, Lachfältchen, dunkelblondes, nach hinten gestrichenes Haar mit grauen Schläfen. Hellbraune, warme Haut. 183 cm, sportlich. |
| Kleidung / Palette | offene Weste, hochgekrempelte Ärmel, Lederschürze mit Brandflecken, Glasampullen am Gürtel · Weinrot `#6E2A2A`, Leder, Kupfer |
| MetaHuman | Preset: Mann Ende 30, freundliches Gesicht, mittlere Haut · Blend: Augen weicher, Lächelfalten · Skin: mittel-warm, leichte Falten · Eyes: warmes Braun · Hair: zurückgekämmt, dunkelblond, Schläfen grau; kurzer Bart · Body: athletisch, 183 cm |
| Stimme | warm, sympathisch, leicht ironisch. Im Verrat plötzlich ruhig und müde. TTS: `de_DE-thorsten`, Tempo normal |
| Körpersprache / Idle | lehnt am Pult, rührt beiläufig in einem Glas; berührt Leute an der Schulter. Nach dem Verrat: keine Berührungen mehr |

### 4.5 Meister Tobiah Grell – Lehrer für Schutzweisen und Duellkunde

| | |
|---|---|
| Alter / Rolle / Fraktion | 57 · Lehrkraft, Duellmeister · neutral, ehemaliger Grenzsoldat des Kollegiums |
| Persönlichkeit | knurrig, ungeduldig, fair. Hasst Feiglinge und Bürokraten gleichermaßen. Heimlicher Kreaturenfreund (füttert die Laternenkrähen). |
| Ziel | dass seine Schüler überleben |
| Geheimnis | hat beim Brandheim-Einsatz die Sperrzone bewacht und Befehle befolgt, die er bereut |
| Bogen | A1 harter Ausbilder · A2 erkennt die Gefahr, trainiert den Spieler im Geheimen · A3 Turnierrichter · A4 stirbt bei der Verteidigung der Halle (E4 = Halle) oder überlebt verletzt · A5 kämpft mit Krücke im Finale |
| Aussehen | breites, vernarbtes Gesicht, gebrochene Nase, kurzer grauer Vollbart, Glatze, ein milchiges Auge (links). Dunkle, warme Haut. 190 cm, massig. |
| Kleidung / Palette | gepolsterte Duelljacke, Lederschienen an den Unterarmen, schwere Kampfhandschuhe mit offenen Fingerspitzen · Dunkelbraun, Eisen, Ochsenblut |
| MetaHuman | Preset: älterer Mann, breites Gesicht, dunkle Haut · Blend: Nase breiter, Kiefer massig · Skin: dunkel-warm, Narben über Decal (Phase 3) · Eyes: braun, links milchig (Iris-Material) · Hair: Glatze, kurzer grauer Bart · Body: schwer, muskulös, 190 cm |
| Stimme | tief, rau, laut. TTS: `de_DE-thorsten`, Tonhöhe −15 %, Tempo −10 % |
| Körpersprache / Idle | breitbeinig, Arme verschränkt, knackt mit den Fingerknöcheln; reibt sich die Narbe am Kinn |

### 4.6 Dozentin Saphira Lindqvist – Lehrerin für Kreaturenkunde

| | |
|---|---|
| Alter / Rolle / Fraktion | 41 · Lehrkraft, Leiterin der Gehege · Hüter der Adern |
| Persönlichkeit | sanft, verträumt, stur wie ein Esel, wenn es um Tiere geht. Redet mit Kreaturen mehr als mit Menschen. |
| Ziel | ein Schutzgebiet am Ilsensee aufbauen (Questreihe „Die Arche am See“) |
| Geheimnis | versteckt in den Gehegen einen Nachhall-Echohund, den sie gesund pflegt |
| Bogen | A1 Ausflug in den Raunwald · A2 Wilderer-Spur · A3 Schutzgebiet wächst · A4 stirbt beim Schutz der Gehege (E4 = Corvin verfolgen) oder überlebt · A5 Kreaturen kämpfen mit |
| Aussehen | rundliches Gesicht, Sommersprossen, grüne Augen, rotblonde, dicke Haare zu einem lockeren Zopf mit Federn und Zweigen darin. Helle, warme Haut. 165 cm, kräftig. |
| Kleidung / Palette | gewachste Jacke, viele Schals, schwere Handschuhe, Taschen voller Futter · Moosgrün, Rost, Creme |
| MetaHuman | Preset: Frau um 40, rundes Gesicht, helle Haut · Blend: Wangen voller · Skin: hell-warm, Sommersprossen stark, Rötung Wangen · Eyes: grün · Hair: lockerer Zopf, Rotblond · Body: mittel, kräftig |
| Stimme | weich, singend, lacht oft. TTS: weiblich, mittlere Lage, Tempo −5 % |
| Körpersprache / Idle | hockt sich hin, statt zu stehen; summt; zupft Blätter aus den Haaren |

### 4.7 Rektorin Ysolde Varga – Leitung der Akademie

| | |
|---|---|
| Alter / Rolle / Fraktion | 69 · Rektorin von Nocturna · neutral, Diplomatin zwischen Kollegium, Hütern und Zünften |
| Persönlichkeit | würdevoll, kontrolliert, politisch, kann eiskalt sein. Liebt die Akademie mehr als jeden Menschen. |
| Ziel | Nocturna unabhängig halten |
| Geheimnis | wusste von Brandheim und hat Edda trotzdem eingestellt, als Pfand gegen das Kollegium |
| Bogen | A1 fern und mächtig · A2 schützt die Akademie, nicht den Spieler · A3 lässt das Turnier trotz Gefahr laufen · A4 tritt zurück oder bleibt (Ruf) · A5 hält in Ende A die Abschlussrede, in B schließt sie die Akademie |
| Aussehen | langes, edles Gesicht, Adlernase, schwarze Augen, kurzes weißes Haar, sehr dunkle Augenbrauen. Olivfarbene Haut. 178 cm, schlank. |
| Kleidung / Palette | hochgeschlossene schwarze Robe mit violettem Futter, Amtskette mit den vier Turmwappen · Schwarz, Violett `#4B2E5A`, Gold |
| MetaHuman | Preset: ältere Frau, langes Gesicht, olivfarbene Haut · Blend: Nase markanter (Adlernase), Augen schmaler · Skin: mittel-neutral, Alter hoch · Eyes: sehr dunkelbraun · Hair: kurzer Pixie, Weiß · Body: groß, schlank |
| Stimme | tief, ruhig, jedes Wort gesetzt. TTS: weiblich, tiefe Lage, Tempo −15 % |
| Körpersprache / Idle | sitzt sehr gerade, Fingerspitzen aneinander; dreht den Siegelring |

### 4.8 Theodric Vaal, „Der Dirigent“ – Antagonist, Anführer der Stillen Hand

| | |
|---|---|
| Alter / Rolle / Fraktion | 42 · Anführer · Stille Hand |
| Persönlichkeit | höflich, ruhig, belesen, nie laut. Tiefe Trauer, verwandelt in Überzeugung. Siezt alle, auch Kinder. |
| Ziel | den Urton einmal vollständig spielen, damit alle Magie verstummt. Kein Brandheim mehr, nie wieder. |
| Geheimnis | trägt die Stimmen seiner Familie als Echo in einem Glasquarz um den Hals; er hört sie jede Nacht |
| Bogen | A1 nur Gerüchte · A2 Briefe an den Spieler · A3 erstes Treffen in Brandheim (im Traum) · A4 zeigt sich nach dem Angriff · A5 Endboss; kann im Ende A verschont werden und lebt in Stille |
| Aussehen | schmales, blasses, sehr ruhiges Gesicht, tiefe Augenringe, graue Augen, schulterlanges schwarzes Haar mit einer silbernen Strähne, glatt rasiert. Sehr helle Haut. 185 cm, hager. |
| Kleidung / Palette | langer schwarzer Mantel mit hohem Kragen, schwarzer Handschuh nur rechts, ein Taktstock aus weißem Knochenholz (als Einziger wirkt er Magie über ein Werkzeug: er traut seinen eigenen Händen nicht) · Schwarz, Knochenweiß `#E8E2D4`, Glas |
| MetaHuman | Preset: Mann um 40, schmales Gesicht, sehr helle Haut · Blend: Wangen eingefallen, Augen tief · Skin: sehr hell-kühl, Augenringe (Makeup) · Eyes: grau · Hair: schulterlang glatt, Schwarz mit Strähne (Phase 3 Groom-Färbung) · Body: groß, sehr schlank |
| Stimme | leise, samtig, langsam, keine Emotion bis zum Finale. TTS: `de_DE-thorsten`, Tonhöhe −10 %, Tempo −20 % |
| Körpersprache / Idle | steht völlig still, bewegt nur die rechte Hand wie beim Dirigieren; berührt den Glasquarz am Hals |

### 4.9 Nym, die Echofrau (Nymrael) – mysteriöse Figur aus dem Nachhall

| | |
|---|---|
| Alter / Rolle / Fraktion | ~2000 Jahre (wirkt Mitte 30) · Hohe Sängerin der Kael · keine |
| Persönlichkeit | rätselhaft, sanft, manchmal kindlich, manchmal uralt. Spricht in Echos (letzte Worte wiederholen sich leise). |
| Ziel | zurück in die Welt, und den Fehler des Verstummens wiedergutmachen |
| Geheimnis | sie hat den Spieler „vorhergehört“ und den Urton in Brandheim zu ihm gelenkt. Sie ist mitschuldig an seiner Gabe. |
| Bogen | A1 Träume · A2 Stimme im Kopf beim Urton · A3 Erscheinung in der Kaelschlucht, zeigt die Wahrheit · A4 warnt vor Corvin (wenn Spieler ihr vertraut) · A5 Entscheidung 6: befreien oder binden |
| Aussehen | langes Gesicht, große dunkle Augen ohne sichtbare Iris im Nachhall, dunkelbraune Haut mit feinen goldenen Linien (Kael-Tätowierungen, leuchten beim Sprechen), schwarzes Haar sehr lang, schwebt wie unter Wasser. 175 cm. Rand der Figur flimmert (Material-Dither). |
| Kleidung / Palette | Gewand aus vielen dünnen Schichten, teilweise transparent, Goldschmuck an Stirn und Hals · Elfenbein, Gold, Nachhall-Violett `#B8A7D9` |
| MetaHuman | Preset: Frau Mitte 30, langes Gesicht, dunkle Haut · Blend: Augen größer, Wangenknochen hoch · Skin: dunkel-warm, sehr glatt · Eyes: sehr dunkel (später Material-Override) · Hair: langes, glattes Groom + Physik/Wind-Override für Schwebeeffekt · Body: schlank, groß |
| Stimme | weich, mehrstimmig (gleiche Aufnahme 3× leicht versetzt, Hall). TTS: weiblich, + MetaSounds-Echo-Kette |
| Körpersprache / Idle | schwebt 10 cm über dem Boden, Kopf leicht schräg, Hände öffnen und schließen sich langsam |

### 4.10 Ambros Kettler – Ringschmied und Händler in Kesselbrunn

| | |
|---|---|
| Alter / Rolle / Fraktion | 71 · Ringschmied, stimmt Stimmringe, handelt mit Quarzen · Freie Zünfte |
| Persönlichkeit | gemütlich, schlitzohrig, redet in Musik-Metaphern, spielt Bandoneon in der Werkstatt (Akkordeon-Leitmotiv!) |
| Ziel | in Ruhe arbeiten. Und das Versprechen halten, das er den Eltern des Spielers gab. |
| Geheimnis | kannte die Eltern des Spielers (beide Instrumentenbauer in Brandheim) und hat Edda den Säugling abgenommen; das Waisenhaus war seine Idee |
| Bogen | A1 stimmt die ersten Ringe · A2 sein Laden brennt beim Angriff · A3 baut den Meisterring · A4 schmiedet den „Brandheim-Quarz“ aus dem Instrument der Eltern · A5 spielt im Epilog bei der Feier |
| Aussehen | rundes, rotwangiges Gesicht, weißer, buschiger Schnurrbart, Brille auf der Nasenspitze, Glatze mit weißem Haarkranz. Helle, warme Haut. 165 cm, rundlich. |
| Kleidung / Palette | Lederschürze, Hemdsärmel, Ärmelschoner, Lupen an einer Stirnbrille · Braun, Leder, Dunkelrot |
| MetaHuman | Preset: alter Mann, rundes Gesicht, helle Haut · Blend: Nase knolliger, Wangen voll · Skin: hell-warm, Rötung Wangen/Nase, Falten hoch · Eyes: blau · Hair: Haarkranz weiß, großer Schnurrbart · Body: klein, rundlich |
| Stimme | brummig, gemütlich, kichert. TTS: `de_DE-thorsten`, Tonhöhe −5 %, Tempo −10 % |
| Körpersprache / Idle | poliert einen Quarz an der Schürze, schiebt die Brille hoch, klopft an Dinge und lauscht |

### 4.11 Kanzler Ruprecht von Stauffen – Oberhaupt des Hohen Kollegiums

| | |
|---|---|
| Alter / Rolle / Fraktion | 61 · Kanzler · Hohes Kollegium |
| Persönlichkeit | gewinnend, rhetorisch brillant, pragmatisch bis zur Skrupellosigkeit. Glaubt, Ordnung rechtfertigt alles. |
| Ziel | Kontrolle über den Urton, um das Kollegium unangreifbar zu machen |
| Geheimnis | Mitleiter des Brandheim-Experiments; hat die Vertuschung befohlen; hat einen unehelichen Sohn (Fink) verstoßen |
| Bogen | A1 Rede bei Ilsenlicht · A2 Inspektion · A3 Bietet Schutz an · A4 Kriegsrecht, Archiv-Enthüllung · A5 gestürzt, verhaftet oder (bei Verhandlung) gezwungen, abzudanken zugunsten Aurelias |
| Aussehen | quadratisches Gesicht, silbernes, akkurat gescheiteltes Haar, gepflegter kurzer Kinnbart, blaue Augen wie Aurelia. Helle, rosige Haut. 180 cm, breitschultrig. |
| Kleidung / Palette | weißer Amtsmantel mit Goldstickerei, Bannglocke als Brosche, schwere Ringe · Weiß, Gold `#C9A227`, Königsblau |
| MetaHuman | Preset: Mann um 60, kantiges Gesicht, helle Haut · Blend: Kiefer breit, Augen wie Aurelia (gleiches Preset im Augenbereich!) · Skin: hell-rosig, Falten mittel · Eyes: hellblau · Hair: kurzer Seitenscheitel, Silber; Kinnbart · Body: breit, 180 cm |
| Stimme | sonor, Redner. TTS: `de_DE-thorsten`, Tonhöhe −5 % |
| Körpersprache / Idle | Hände auf dem Rücken, Brust raus; legt Leuten die Hand auf die Schulter, um sie zu lenken |

### 4.12 Nell Tiedemann – Ermittlerin in Tiedmar

| | |
|---|---|
| Alter / Rolle / Fraktion | 34 · Hafenkommissarin der Zünfte · Freie Zünfte · Begleiterin in Tiedmar |
| Persönlichkeit | sarkastisch, müde, extrem genau, Pfeifenraucherin (kalte Pfeife, sie hat aufgehört). Keine Magie, nur Verstand. |
| Ziel | den Fall Salzwind lösen, egal wen es trifft |
| Geheimnis | ihr Bruder ist einer der verschwundenen Wirker |
| Bogen | A3 Questreihe „Der Fall Salzwind“ · A4 hilft beim Archiv-Einbruch · A5 bringt die Zünfte ins Finale |
| Aussehen | schmales, wettergegerbtes Gesicht, Schmiss über der Augenbraue, dunkle mandelförmige Augen, kurzes schwarzes Haar unter einer Schirmmütze. Ostasiatisch geprägte Züge, hell-neutrale Haut. 163 cm, drahtig. |
| Kleidung / Palette | langer Öltuchmantel, Halstuch, Dienstmarke der Zunft (Möwe in Anker) · Ölzeug-Gelb `#C9A64B`, Teer, Weiß |
| MetaHuman | Preset: Frau Mitte 30, schmales Gesicht, ostasiatische Merkmale · Blend: Wangen schmaler · Skin: hell-neutral, wettergegerbt (Rauheit hoch) · Eyes: dunkelbraun · Hair: kurz, schwarz · Body: drahtig, 163 cm |
| Stimme | trocken, nüchtern, leicht heiser. TTS: weiblich, mittlere Lage, Tempo +5 % |
| Körpersprache / Idle | kaut auf der kalten Pfeife, schreibt in ein kleines Notizbuch, scannt den Raum |

### 4.13 Fink – Anführer der Grauen Finken (Widerstand Aurenburg)

| | |
|---|---|
| Alter / Rolle / Fraktion | 26 · Widerstandsanführer · Graue Finken (Zünfte) · Begleiter in Aurenburg |
| Persönlichkeit | charismatisch, impulsiv, poetisch, schreibt Flugblätter in Reimen. Trägt Wut wie einen Mantel. |
| Ziel | das Kollegium stürzen |
| Geheimnis | unehelicher Sohn des Kanzlers, Aurelias Halbbruder. Er weiß es, sie nicht. |
| Bogen | A4 Questreihe „Graue Finken“, E5 Aufstand · A5 führt den Aufstand ins Finale oder verlässt Aurenburg nach der Verhandlung |
| Aussehen | markantes Gesicht mit Aurelias Nase und Kinn, blaue Augen, kurz geschorene Seiten, oben zerzaustes aschblondes Haar, Stoppelbart. Helle Haut, Narbe am Hals. 178 cm, sehnig. |
| Kleidung / Palette | geflickter grauer Mantel, Finkenfeder am Kragen, fingerlose Handschuhe · Grau, Ziegelrot, Ruß |
| MetaHuman | Preset: junger Mann, kantig, helle Haut · Blend: Nase/Kinn wie Aurelia (Blend mit ihrem Preset 30 %) · Skin: hell, rau, Narbe als Decal · Eyes: hellblau · Hair: Undercut, aschblond, Stoppeln · Body: sehnig, 178 cm |
| Stimme | jung, rau, kann von leise auf Rede umschalten. TTS: `de_DE-thorsten`, Tonhöhe +5 %, Tempo +5 % |
| Körpersprache / Idle | sitzt verkehrt herum auf Stühlen, zeichnet Kreide-Finken an Wände, ständig in Bewegung |

### 4.14 Mara Vandermeer, „die Taktgeberin“ – Lios Schwester, Stille Hand

| | |
|---|---|
| Alter / Rolle / Fraktion | 19 · Taktgeberin der Stillen Hand · Stille Hand (rekrutiert von Corvin) |
| Persönlichkeit | kühl, diszipliniert, innerlich zerrissen. Glaubt an den Dirigenten wie an einen Vater. |
| Ziel | Magie verstummen lassen, weil ihre Magie (unkontrolliert) einst ein Boot und zwei Fischer versenkte |
| Geheimnis | ist nicht weggelaufen wegen des Streits mit Lio, sondern weil sie die Fischer getötet hat und sich schämte |
| Bogen | A2 Angriff auf Kesselbrunn, Story-Boss 2 · A3 Entscheidung 3 · A4 je nach E3: tot, gefangen, Doppelagentin · A5 kämpft gegen oder neben dem Spieler |
| Aussehen | Lios Züge, aber schärfer: schmales Gesicht, Sommersprossen, gleiche Augen, kurzgeschorenes dunkles Haar, eine Seite länger. Mittelbraune Haut. 170 cm, athletisch. |
| Kleidung / Palette | schwarze Stille-Hand-Uniform, Metronom-Anhänger, Taktstock an der Hüfte, eine Haarsträhne mit Netzknoten (von Lio) · Schwarz, Dunkelblau, ein Fleck Senfgelb |
| MetaHuman | Preset: junge Frau, schmales Gesicht, mittlere warme Haut · Blend: Augen/Mund 40 % von Lios Preset (Geschwister!) · Skin: wie Lio, Sommersprossen · Eyes: haselnuss · Hair: asymmetrischer Kurzhaarschnitt, dunkelbraun · Body: athletisch |
| Stimme | beherrscht, knapp, bricht, wenn Lio spricht. TTS: weiblich, mittlere Lage |
| Körpersprache / Idle | tippt mit dem Taktstock gegen den Oberschenkel (Metronom-Rhythmus), Blick nie lange in Augen |

### 4.15 Mutter Quendel – Älteste der Hüter der Adern

| | |
|---|---|
| Alter / Rolle / Fraktion | 83 · Älteste der Hüter · Hüter der Adern |
| Persönlichkeit | schelmisch, derb, weise, Pfeifenkräuter-Raucherin. Redet mit Bäumen und mit dem Spieler gleich. |
| Ziel | die kranke Ader im Graumoor heilen, den Nachhall „zurücksingen“ |
| Geheimnis | die einzige Lebende, die Kael fließend spricht. Sie hört Nym seit 60 Jahren und hat ihr aus Angst nie geantwortet. |
| Bogen | A2 Graumoor, Schlüssel-Entscheidung (E2) · A3 lehrt Kael · A4 bringt die Hüter zur Verteidigung (wenn E2 = Hüter) · A5 singt den Weg in den Nachhall frei, Ende A: hilft beim Neustimmen |
| Aussehen | tief gefurchtes, braungebranntes Gesicht, schwarze, funkelnde Augen, dicke weiße Zöpfe mit Holzperlen, ein Goldzahn. Dunkle, warme Haut. 152 cm, gebeugt. |
| Kleidung / Palette | Schichten aus Moos-, Leinen- und Fellstoffen, Ketten aus Knochen und Quarz, Wurzelstab · Moosgrün, Ocker, Knochen |
| MetaHuman | Preset: sehr alte Frau, rundes Gesicht, dunkle Haut · Blend: Augen kleiner, Lachfalten tief · Skin: dunkel-warm, Alter maximal · Eyes: sehr dunkelbraun · Hair: zwei dicke Zöpfe, Weiß · Body: klein (Skalierung −5 %) |
| Stimme | krächzend, lachend, singt zwischendurch. TTS: weiblich, Tonhöhe −10 %, Tempo −10 % |
| Körpersprache / Idle | stützt beide Hände auf den Wurzelstab, wippt zu unhörbarer Musik, zeigt mit der Pfeife |

**Wichtige Nebenfiguren** (ohne volles Blatt, MetaHuman „Optimized“): Ausbilder Varn (Akt-1-Boss) · Hanne Brook (Waisenhausleiterin, stirbt in M206) · Ida Brack (Hafenmeisterin, Zunftrat) · die vier Turmsprecher Greta Kohlhammer (Aschenhain), Jonte Weißdorn (Silberquell), Fenja Brise (Sturmkamm), Hannes Wurzel (Moosgrund) · Großinquisitor Halmar Dräger (Regionsboss Aurenburg) · Bruder Stumm (Abt der Schweigemönche).

**Neu aus dem Missionsdesign (docs/missions/, 27.11.2026):** Wim Hasel (11, Erstklässler ab Akt 2, Schützling des Spielers, Angst vor Wasser) · Jorin und Elise (Eltern des Spielers, Instrumentenbauer in Brandheim, nur als Echo in M504; Nachname = im Editor wählbar, Standard Harlander) · Magister Lorenz Feil (Kollegiumsgesandter, M205) · Seline (junge Hüterin, Wurzelläuferin, M105) · Choralist Benedikt Raue (Kapitän der *Schweigenden Möwe*, M303) · Olga (Fischhändlerin, Tiedmar) · Ansgar Lenz (Ältester von Windnest) · Hedda (Bäckerin) und Jupp (Akkordeonspieler) in Kesselbrunn · Großmutter Morchel (Pilzhausen) · der Fährmann (spricht nur in Wetterberichten).

**Neu für den Akt-1-Schulvibe (E10, ohne volles Blatt):** Lehrkräfte Haldor Brams (Schlagweisen), Bruni Kesselmayr (Elementar/Tränke in Akt 1), Pim Ollendorf (Wandelkunde), Ilka Morgenroth (Sternkunde) · Personal Mutter Gundel (Küche), Herr Fennimore Staub (Bibliothek), Gärtnerin Rosa Dornbusch · Mitschüler Pankraz „Panki“ Holler, Mirela Sandor, Tilda Morn (mit Frosch Herr Quaak), Ruven Falk (Die Schiefen Töne) · lebendige Dinge: Ottilie (wandelnde Laterne), Kessel Klaus und Brüder (Teekessel), Sessel Barnabas, die Stimmglocke · 36 Meister in der Welt (Liste mit Ort und Bedingung: `Content/Data/Spells/Katalog/Lehrer.json`, Übersicht in WEISEN_KATALOG §5). Beschreibungen: AKT1_SCHULLEBEN.md §3 und §7.

## 5. NPCs und Crowds

| Thema | Plan |
|---|---|
| NPC-Generator | Datentabelle (`Content/Data/NPCs/NPCGenerator.json`, Phase 5): Gesichter (24 MetaHuman-Optimized-Köpfe) × Haut × Frisur × Outfit-Set × Farbe × Accessoire. Seed pro NPC = gleiche Person bei jedem Laden. Regel: zwei NPCs im selben Sichtfeld teilen nie Kopf + Frisur. |
| Schüler | Grunduniform × Turmfarbe × Jahrgang (Abzeichen 1–5) × Jahreszeit (Umhang im Winter) |
| Dorfbewohner, Händler, Wachen, Kollegiums-Beamte | je 4–6 Outfit-Sets pro Region (Kesselbrunn Wolle/Leder, Tiedmar Ölzeug/Leinen, Aurenburg Brokat/Uniform) |
| Mengen | Mass AI für Aurenburg und Tiedmar (Hunderte NPCs), Nah-NPCs als volle Characters, Fern-Crowds als Mass-Entities mit Vertex-Animation (Phase 7+) |
| Tagesabläufe | Smart Objects: Unterricht, Essen (Große Halle), Schlafen, Lernen (Bibliothek), Arbeiten (Markt, Hafen), Feste. Zeitplan aus JSON, abhängig von Tageszeit und Wetter |

## 6. Kreaturen (20, inkl. Reittier)

Alle Kreaturen sind Eigenentwürfe. Regeln: keine bekannten Fabelwesen-Namen aus anderen Zaubererwelten, keine Verwandtschaft zu bekannten Figuren. Basis für Modelle: Fab-Gratis-Rigs (Vierbeiner, Vogel, Fisch) + Blender + Control Rig. Concept-Prompts: `Scripts/ComfyUI/prompts/creatures.json`.

| # | Name | Lebensraum | Beschreibung | Verhalten | Zähmbar | Rolle im Spiel |
|---|---|---|---|---|---|---|
| 1 | **Sturmschwinge** (Reittier, siehe §7) | Weiße Zinnen, Treibende Inseln | Greifenartig: Vorderkörper einer Schnee-Eule (Kopf, Schwingen, Krallen), Hinterkörper eines Luchses, Federohren, Schwanz mit Federfächer. Spannweite 7 m. | scheu, stolz, folgt nur dem, der ihr die Federn ordnet | ja (Story, M305) | Flug-Reittier, Rennen |
| 2 | **Glimmermotte** | Raunwald, Akademie-Gärten | handgroße Motte, Flügel mit Leuchtmustern, die auf Musik reagieren | schwärmt bei Kerzenlicht, harmlos | nein (anlockbar) | Ambient-VFX, Lichträtsel |
| 3 | **Laternenkrähe** | Nocturna, Glockenturm | Krähe mit einer glühenden Kehle, die nachts wie eine Laterne leuchtet | neugierig, klaut glänzende Dinge, bringt Botschaften | ja | Haustier im Turmzimmer, Sammel-Quest |
| 4 | **Nebelfuchs** | Raunwald | silbergrauer Fuchs, löst sich bei Gefahr in Nebel auf | verspielt, führt Verirrte im Kreis | ja (selten) | Rätsel-Führer, Sidequest |
| 5 | **Borkenbär** | Raunwald, Zinnen-Ausläufer | Bär mit Fell, das wie Rinde und Moos verwächst, Pilze auf dem Rücken | territorial, schläft im Winter als „Hügel“ | nein | Gegner (Wild), Schutzgebiet-Quest |
| 6 | **Wurzelvater** | Raunwald-Herz | uralter, 12 m hoher Baumgeist aus verflochtenen Wurzeln, Laternen-Pilze als Augen | schläft, erwacht krank durch Nachhall | nein | Regionsboss |
| 7 | **Moosling** | Raunwald, Graumoor | katzengroßes, rundes Wesen aus Moos mit Kulleraugen und Blattohren | rollt sich zusammen, frisst Nachhall-Reste | ja | Begleiter-Kreatur im Garten, reinigt Belastung (Housing-Bonus) |
| 8 | **Moorlicht-Unke** | Graumoor | große Kröte, deren Rückenblasen wie Irrlichter leuchten und Opfer locken | lauert, springt aus dem Nebel | nein | Gegner; Königin = Regionsboss |
| 9 | **Stelzreiher** | Graumoor, Ilsensee | Reiher mit drei Meter langen Beinen und Kristallschnabel, singt zweistimmig | ruhig, flieht bei Lärm | nein | Ambient, Kodex, Klang-Rätsel (sein Ruf öffnet Schreine) |
| 10 | **Glasnatter** | Glastiefen | durchsichtige Schlange, Organe als leuchtende Kristalle sichtbar | jagt Wärme, versteinert Beute mit Klang | nein | Gegner |
| 11 | **Kristallwal** | Glastiefen (unterirdische Seen) | 20-m-Wal mit Kristallplatten statt Haut, Gesang erzeugt Licht in der Höhle | sanft, wird durch Kael-Minen gequält | nein | Regionsboss (erlösbar statt tötbar: Option) |
| 12 | **Salzkrake** | Küste Tiedmar | Krake mit salzverkrusteten Armen, Jungtiere in Gezeitentümpeln, Mutter riesig im Hafen | intelligent, sammelt Schiffsglocken | nein | Gegner, Regionsboss „Salzmutter“ |
| 13 | **Gischtrobbe** | Küste, Ilsensee | Robbe mit Flossen wie Segel, „surft“ auf Wellen | verspielt, warnt vor Sturm | ja | Schwimm-Begleiter, Rennen auf dem Wasser |
| 14 | **Firnwolf** | Weiße Zinnen | weißer Wolf mit Eiskristallen im Fell, Atem gefriert Luft | jagt im Rudel, Alpha mit Eisgeweih | nein (Welpen: ja, Questreihe) | Gegner, Regionsboss „Firnfürst“ |
| 15 | **Zinnadler** | Weiße Zinnen | Adler mit Metallglanz-Federn, Pfeifton beim Sturzflug | Rivale der Sturmschwinge | nein | Flug-Rennen-Gegner |
| 16 | **Dünengräber** | Kaelschlucht | sandfarbener Riesenwurm mit Klangtrichter-Maul, „hört“ Schritte | taucht unter Schritten auf, meidet Stille | nein | Umwelt-Gefahr (schleichen!), Mini-Boss |
| 17 | **Runenschildkröte** | Kaelschlucht, Treibende Inseln | Schildkröte mit Kael-Inschriften auf dem Panzer, die sie über Jahrhunderte „wachsen“ lässt | uralt, langsam, weise | ja (nur füttern) | Lore-Träger (Kael-Wörterbuch) |
| 18 | **Wolkenrochen** | Treibende Inseln | fliegender Rochen, durchscheinend, Wolken als Muster auf der Haut | gleitet in Schwärmen, Sturm-Alpha ist aggressiv | nein | Ambient, Regionsboss „Sturmrochen“ |
| 19 | **Echohund** | Nachhall, (versteckt in den Gehegen) | negativfarbener Hund, Konturen flimmern, sein Bellen läuft rückwärts | jagt Klang, loyal, wenn gerettet | ja (nur einer, Story) | Gegner im Nachhall; Saphiras Geheimnis |
| 20 | **Stummling** | Nachhall, dünne Orte | humanoid, porzellanglatt, kein Gesicht, nur ein runder Mund ohne Ton; Körper aus zusammengesetzten Scherben verlorener Dinge | saugt Klang aus der Umgebung (Ton wird dumpf in der Nähe) | nein | Haupt-Gegner im Nachhall |

**Reittier-Animationsset** (Sturmschwinge): siehe ANIMATION_BIBLE §6. Rig: eigener Vierbeiner-mit-Flügeln-Rig (Basis: Fab-Gratis-Vierbeiner + Vogelflügel-Kette in Blender, Control Rig für Flügel-IK).

## 7. Reittiere

Alle Reittiere sind eigene Erfindungen. Man zähmt sie in Quests, hält sie im **Stall der Akademie** und passt sie beim Stallmeister an (Sattel, Zaumzeug, Rüstung, Fellfarbe/Muster, Schmuck, Name). **Eigene Ideen vom Nutzer kommen hier dazu**, Vorlage: Name, Aussehen in 2 Sätzen, Bewegungsart, wo man es findet.

| # | Reittier | Bewegung | Aussehen | Besonderheit | Freischaltung |
|---|---|---|---|---|---|
| R1 | **Sturmschwinge** | Fliegen | Vorderkörper Schnee-Eule, Hinterkörper Luchs, Federohren, Schwanzfächer | schnellster Flieger, Sturzflug-Boost | Akt 3, Weiße Zinnen (Story) |
| R2 | **Perlsegler** | Fliegen + Gleiten | pferdegroß, langer Hals, vier breite Hautsegel zwischen den Beinen, Fell schimmert wie Perlmutt, Leuchtquaste am Schwanz; summt beim Fliegen | gleitet ewig ohne Ausdauer, kann aber schlecht steigen; nachts leuchtet die Quaste | Akt 2, Treibende Inseln (Sidequest) |
| R3 | **Moosbock** | Boden, Klettern | riesiger Bock mit Moospelz, Pilzen und kleinen Pflanzen auf dem Rücken, gedrehte Rindenhörner | läuft Steilwände hoch, Pflanzen auf seinem Rücken wachsen mit (Housing-Garten zum Mitnehmen) | Akt 1, Raunwald |
| R4 | **Dünenschreiter** | Boden, sehr schnell | Laufvogel, 3 m hoch, gepanzerte Schuppenbeine, Kopf mit Sonnenschirm-Kamm | schnellstes Bodentier, gräbt sich bei Sandsturm ein (Unterschlupf) | Akt 3, Kaelschlucht |
| R5 | **Gischtrobbe** | Wasser, Tauchen | Robbe mit segelartigen Flossen | surft auf Wellen, taucht in Höhlenseen | Akt 2, Tiedmar |
| R6 | **Glutdachs** | Boden, Graben | ponygroßer Dachs mit glimmenden Streifen im Fell, Funken bei jedem Schritt | gräbt verschüttete Gänge frei, wärmt im Schnee (kein Frieren) | Akt 4, Glastiefen |

Regeln: 1 Reittier aktiv, Wechsel per Pfiff-Rad (Taste halten). Jedes Reittier hat Ausdauer, Tempo, Spezialfähigkeit und eine Beziehung (füttern, putzen, gemeinsam Rennen = Bonus). Rennen: Flug-, Boden- und Wasser-Rennstrecken je Region.
