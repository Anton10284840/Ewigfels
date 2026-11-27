# Region 3 – Kesselbrunn

Stand: 27.11.2026 · Akt 1 (ab M101/M104) · Daten: `Content/Data/World/Regions/Kesselbrunn.json`, Quests `Content/Data/Quests/Akt1/Kesselbrunn/N121–N126.json`, Dialoge `Content/Data/Dialogues/Kesselbrunn/`
Grundlagen: [STORY_BIBLE §4](../STORY_BIBLE.md), [BESTIARIUM](../BESTIARIUM.md) (Egon Ruß, Schmelzer), [M101/M104](../missions/AKT1.md), [M206](../missions/AKT2.md) (Wintersperre). Gegenstücke: [ILSENSEE.md](ILSENSEE.md), [RAUNWALD.md](RAUNWALD.md).

> *„In Kesselbrunn ist es nie ganz kalt. Das Wasser ist warm, die Öfen sind warm, und Hanne ist warm, auch wenn sie so tut, als wär sie's nicht.“*

## 1. Überblick und Karte

Kesselbrunn liegt 15 Minuten südöstlich von Uferlicht in einem **Talkessel** zwischen bewaldeten Hängen. Heiße Quellen dampfen zwischen den Fachwerkhäusern, über dem Dorf liegt der Bergwerkshang mit der Mine. Hier ist der Spieler aufgewachsen: Die Leute kennen ihn, die Waisenkinder rennen hinterher, Hedda hat immer eine Zimtschnecke.

```
                     N
        Uferweg vom Ilsensee (15 min) ──┐
                                        │
      Waldhang ▲▲     KESSELBRUNN       │
            ▲▲   ┌──────────────────────┴───┐
          ▲▲     │ Marktplatz · Ambros' Werkstatt │
                 │ Waisenhaus · Heddas Backstube  │
                 │ Quellbad ≈≈ (heiße Quellen)    │
                 └──────┬─────────────────────┘
                        │ Grubenweg (Serpentinen, 5 min)
                 RUSSGRUBE (Bergarbeitersiedlung, Rauch) ✝
                        │
                 Alte Mine ●  (Schmelzhalle, Schmelzer)
                    S       Richtung Brandheim (gesperrt, Schild „KOLLEGIUM – KEIN DURCHGANG“)
✝ = Gruseldorf   ● = Dungeon/Boss
```

| Ort | Lage | Was dort ist | Schnellreise |
|---|---|---|---|
| **Kesselbrunn** | Talkessel | Markt, Werkstätten, heiße Quellen, Waisenhaus | Marktbrunnen |
| **Russgrube** | Hang über dem Dorf | Gruseldorf, Bergarbeitersiedlung im Rauch | Förderturm (nach Befreiung) |
| **Alte Mine** | unter der Russgrube | Stollen, Schmelzhalle, Regionsboss Schmelzer | – |
| **Brandheim-Schild** | Süden | gesperrte Straße, Kollegiums-Wache, verbotene Zone (Akt 2/5) | – |

**Stimmung:** Fachwerk in Ocker, Schieferdächer, Laternen in Orange (ART_BIBLE Region 3). Eng, gemütlich, laut. Dampf aus den Gullys. Im Winter liegt Schnee auf den Dächern, aber nie auf den Straßen (die Quellen wärmen den Boden).

**Musik:** Kesselbrunn = Akkordeon (Jupp, das Leitmotiv!), Blaskapelle, Kinderlieder. Russgrube = kein Lied, nur Hämmern im Takt, das langsam schneller wird, und ein Grubenpfiff in der Ferne. Wer genau hinhört: Das Hämmern ist das Kesselbrunner Bergmannslied, ohne Melodie.

---

## 2. Kesselbrunn – das Heimatdorf

**Aufbau:**
1. **Marktplatz mit Brunnen:** Stände, Jupps Ecke (spielt jeden Tag ab Mittag), Ausrufer mit Neuigkeiten.
2. **Ambros' Werkstatt „Kettler & Klang“:** Ringschmied, Bandoneon an der Wand, hunderte Quarze in Schubladen.
3. **Heddas Backstube:** Zimtschnecken, Brot, Pflaumenkuchen. Ein Fenster zur Gasse, aus dem sie Kindern Reste reicht.
4. **Waisenhaus „Zum Warmen Ofen“:** Hanne Brook leitet es. Sechs Kinder, ein Schlafsaal, der Spiegel im Flur (Charakter-Editor aus M101).
5. **Quellbad:** dampfende Becken, Bademeisterin Frieda Dampf; Rast mit Bonus (Atem-Regeneration).
6. **Handwerkergasse:** Barbier, Schneiderin, Möbeltischler (GDD §13 Personalisierung).
7. **Zunfthaus:** Sitz des örtlichen Zunftmeisters, mit Archiv im Keller (N124).

### 2.1 Bewohner

| NPC | Wer | Eigenart | Was man bekommt |
|---|---|---|---|
| **Hanne Brook** | Waisenhausleiterin | trocken, warm unter der Schale, tut so, als wäre ihr alles egal | Briefe, Erinnerungen, Quest N121; stirbt in M206 |
| **Hedda** | Bäckerin | lacht laut, backt bei Kummer doppelt, hat ein Geheimrezept | Essen (Buffs), Quest N123 |
| **Jupp** | Straßenmusikant, Meister für Musikweisen | herzlich, derb, „Wer lernen will, gibt erst was zurück.“ | Liederheft, Musikweisen (M104, S114) |
| **Ambros Kettler** | Ringschmied | brummig, gemütlich, Musik-Metaphern | Stimmringe, Quarze, Gravuren; Quest N126 |
| **Frieda Dampf** | Bademeisterin | spricht nur in Temperaturen („Heute bin ich 38 Grad gelaunt.“) | Rast im Quellbad, Quest N122 |
| **Barbier Fridolin Kamm** | Barbier | erzählt Witze beim Schneiden, schneidet schief, wenn er lacht | Frisuren, Haarfarbe, Bart |
| **Schneiderin Mathilde Saum** | Schneiderin | kritisch, Nadeln im Mund, sagt die Wahrheit über Farben | Kleidung, Farbstoffe, Muster |
| **Hubert Hobel** | Möbeltischler | wortkarg, klopft auf Holz, bevor er antwortet | Turmzimmer-Möbel |
| **Zunftmeister Gustav Pfennig** | Zunftmeister von Kesselbrunn | freundlich, rund, zu freundlich | Schlüsselfigur der Russgrube-Schuld (N124) |
| **Die Waisenkinder** | Lene (9, Vater in der Russgrube), Bastian (12, will zur Akademie), Kalle (6, sammelt Steine), Ida (8), Tom und Tim (Zwillinge, 7) | rennen dem Spieler hinterher, wollen Zauber sehen | Quest N121, N124 (Lene) |

### 2.2 Was man in Kesselbrunn tut

- **Läden:** Ringschmied, Barbier, Schneiderin, Möbeltischler, Bäckerei, Markt (Kräuter, Quarze, Stoffe).
- **Minispiele:** Akkordeon mit Jupp (Rhythmus), Zimtschnecken backen (Hedda, Knet-Rhythmus), Steine flitschen am Quellbach (Kalle), Quellbad-Dampf-Bilder.
- **Rast:** Quellbad (Atem-Bonus) oder das alte Bett im Waisenhaus (Hanne: „Dein Bett. Ich hab's nicht weggegeben. Hab's nur nicht geschafft.“).
- **Feste:** **Bergfest** (Sommer, die Kapelle spielt, die Mine ist geschmückt), **Wintersonnenwende** (Akt 2, M206: das Fest, an dem die Stille Hand angreift).
- **Heimat-Gefühl:** Die Kinder schicken per Notenfalter Briefe nach Nocturna (alle zwei bis drei Spieltage, kurze lustige Zeilen; ab M206 hört es auf).

---

## 3. Russgrube – das Rauchdorf

> *„Nicht stehen bleiben. Wer stehen bleibt, schaut ihn an.“*

**Stimmung:** Eine Bergarbeitersiedlung am Hang: graue Schieferhäuser, ein Förderturm, eine Schmelzhütte mit hohem Schornstein. Der **Rauch** liegt zäh in den Gassen und formt **Gesichter**. Nachts drücken sie sich an die Fensterscheiben. Die Bergleute arbeiten ohne Pause, Tag und Nacht, mit leeren Augen. Sie essen im Stehen. Sie singen nicht. Das Hämmern hört nie auf.

**Das Unheimlichste:** **Die zwölf Gesichter.** Es sind immer dieselben zwölf. Wer sie zählt (es gibt einen Kodex-Eintrag dafür), merkt: Sie gehören zu den zwölf Namen auf der Gedenktafel am Förderturm. Die Toten des Grubenunglücks vor 20 Jahren. Sie schauen nicht böse. Sie schauen, als wollten sie etwas sagen.

**Aufbau:**
1. **Grubenweg:** Serpentinen von Kesselbrunn, der Rauch beginnt an der dritten Kehre. Vögel drehen hier um.
2. **Die Gasse:** Häuser mit Rauch an den Fenstern, Bergleute laufen vorbei, ohne zu grüßen.
3. **Kantine:** Teller mit Essen, auf denen Ruß liegt. Ein Bergmann schläft im Stehen und hämmert im Schlaf.
4. **Förderturm mit Gedenktafel:** zwölf Namen. Eine Grubenlampe für jeden, alle aus.
5. **Schmelzhütte:** hier wohnt Egon Ruß, zwischen Amboss und Ofen.
6. **Mineneingang:** Weg zur Alten Mine (N125).

### 3.1 Bewohner

| NPC | Wer | Eigenart | Rolle |
|---|---|---|---|
| **Egon Ruß, der Rauchschmied** | Missklinger (Schattenweber), Herrscher | breit, Bart voller Ruß, leise, spricht in Schichten („Erste Schicht: Ich hör dir zu. Zweite: Ich glaub dir nicht.“) | Gegner in N124; erlösbar |
| **Kuno Hartmann** | Bergmann, Lenes Vater | arbeitet seit drei Monaten ohne Pause, erkennt seine Tochter nicht | wird nach der Befreiung neuer Steiger |
| **Fiete** | Grubenjunge, 14 | hat sich in einem Lorenwagen versteckt, als der Rauch kam; hustet | zeigt den Weg durch die Gassen, ohne vom Rauch gesehen zu werden |
| **Die zwölf Gesichter** | die Toten des Grubenunglücks | im Rauch, schweigend | Schlüssel: Sie wollen nicht Rache, sie wollen, dass man ihre Namen sagt |

### 3.2 Die Geschichte der Russgrube

Vor 20 Jahren war Egon Ruß Steiger der Kesselbrunner Mine. Die Zunft wollte schneller fördern und sparte an den Stützbalken. Egon schrieb drei Briefe an den Zunftmeister, Gustav Pfennig. Pfennig antwortete nie. Im Frühjahr brach Stollen 4 ein. Zwölf Mann starben. Die Zunft erklärte es zum Unglück, niemand war schuld. Pfennig ließ die Briefe in einer Akte im Zunftkeller verschwinden.

Egon ging fort und kam als Schattenweber zurück. Er webt den Rauch der Schmelze zu Gesichtern und hat die Bergleute in einen Takt gezwungen: Sie fördern ohne Pause, damit die Zunft endlich so viel bekommt, wie sie wollte. „Ihr wolltet Erz. Ihr kriegt Erz. Und wer es fördert, ist euch ja egal.“ Er will, dass Kesselbrunn spürt, was die Gier gekostet hat. Die Gesichter im Rauch hat er nicht gemacht. Die kamen von allein.

### 3.3 Grusel-Momente (je einmal)

1. **Die dritte Kehre:** Der Rauch beginnt. Ein Gesicht formt sich vor dem Spieler, ganz kurz, und weht weg.
2. **Der Schläfer:** In der Kantine hämmert ein schlafender Bergmann auf den Tisch, im Takt der Grube. Wenn man an ihm vorbeigeht, öffnet er die Augen und sagt: „Schicht.“
3. **Das Fenster:** Nachts im leeren Haus: Ein Rauchgesicht drückt sich an die Scheibe. Dann noch eins. Dann zwölf.
4. **Der Spiegel der Grubenlampen:** Am Förderturm leuchtet eine der zwölf Lampen kurz auf, wenn man den Namen liest. Dann wieder aus.
5. **Einziger echter Schreck** (BESTIARIUM §1.4): In der Schmelzhütte. Egon steht im Dunkeln direkt hinter dem Ofen, nur seine Augen leuchten im Glutschein, als der Spieler die Ofenklappe öffnet. Er sagt nur: „Zu.“

### 3.4 Die Befreiung (Quest N124) und was danach passiert

**Zwei Wege:**
- **Gerechtigkeit (kein Kampf):** Die alte Zunftakte mit Egons drei Briefen im Zunftkeller von Kesselbrunn finden (Schleich-Rätsel, Pfennig darf es nicht merken, oder mit Zustimmung von Jupp und Hanne offen fordern). Die Akte vor allen Leuten auf dem Marktplatz vorlesen lassen (Ausrufer). Dann die zwölf Namen am Förderturm laut vorlesen, einen nach dem anderen. Bei jedem Namen löst sich ein Gesicht im Rauch und lächelt. Egon hört zu, legt den Hammer weg. Der Takt bricht.
- **Kampf:** Egon Ruß als Mini-Boss (Schattenweber-Elite: Rauchschlingen, Rußgesichter als Ablenkung, Schmelzfunken). Danach zerreißt der Rauch, die Bergleute wachen auf. Egon geht in die Alte Mine und versiegelt den Eingang (N125 dann schwerer, Schmelzer ohne Abschaltung).

**Danach:**
- Die Bergleute wachen auf und schlafen dann zwei Tage am Stück. Kuno Hartmann umarmt Lene.
- Der Rauch zieht ab, die Schieferdächer glänzen zum ersten Mal in der Sonne.
- **Kuno Hartmann** wird neuer Steiger (ersetzt die Nennung „Steiger Konrad“ aus AKT1_SCHULLEBEN §13).
- **Neuer Laden:** Grubenschmiede (Erz-Stimmringe, Grubenlampe als Licht-Upgrade).
- Bei Gerechtigkeit: Gustav Pfennig muss vor dem Zunftrat erscheinen (Ida Brack aus Tiedmar reist an, Akt 2). Er zahlt Renten an die Familien und lässt eine neue Gedenktafel gießen. Egon bleibt als Rauchschmied und bringt dem Spieler die Weise *Rauchschleier* bei (Sicht nehmen, Schleichen).
- Die Gedenktafel bekommt zwölf brennende Grubenlampen. Sie gehen nie aus.
- Im Akt 2 (M206, Wintersperre) schützen die befreiten Bergleute das Waisenhaus mit Grubenlampen. Der Brand wird kleiner, Hanne stirbt trotzdem (STORY_BIBLE), aber mehr Kinder überleben ungesehen leichter (kleiner Dialogunterschied, `flag:Russgrube_Freed`).

---

## 4. Nebenquests der Region (6)

| ID | Name | Wer | Ab | Dauer | Kern | Twist / Entscheidung | Lohn |
|---|---|---|---|---|---|---|---|
| N121 | **Das Dach vom Warmen Ofen** | Hanne | M104 | 30 min | Das Waisenhausdach leckt, Winter kommt. Mit den Kindern Schiefer holen, Dach flicken (Weisen *Schwebeklang*, *Ziegelruf*) | Hanne hat kein Geld und sagt es nicht. Die Kinder haben heimlich gespart, um dem Spieler eine Willkommensfeier zu machen. Entscheidung: das Geld fürs Dach nehmen (Kinder stolz) oder die Feier machen lassen und das Dach anders bezahlen (Jupp spielt Benefiz). | Hannes Wollschal (verbessert, +Kälteschutz), Brief-Sammlung |
| N122 | **Die kalten Quellen** | Frieda Dampf | M104 | 30 min | Die heißen Quellen werden jede Woche ein Grad kälter | Die Wärme fließt in die Alte Mine: Der Schmelzer zieht sie ab, seit die Russgrube ohne Pause fördert. Spur durch die Quellrohre bis zum Grubenhang. Öffnet N124/N125. | Quellbad-Dauerbonus, Kodex |
| N123 | **Heddas Geheimrezept** | Hedda | M104 | 25 min | Heddas Zimtschnecken-Rezept ist weg. Verdacht: Ole aus Uferlicht (Pfannkuchen-Rivalität) | Ole hat es nicht gestohlen. Er hat seit 40 Jahren eine Abschrift, von Heddas Großmutter an seinen Großvater, mit einem Liebesbrief auf der Rückseite. Das Original hat Kalle für seine Steinsammlung als Unterlage genommen. Entscheidung: Brief zeigen (Hedda und Ole backen zusammen beim Pfannkuchenfest) oder schweigen. | Rezept Zimtschnecke (Buff), Ole + Hedda Doppelstand (S112) |
| N124 | **Die Rauchgesichter** | Lene | M104 | 50 min | Russgrube befreien (§3.4) | Gerechtigkeit oder Kampf; Pfennig zur Rede stellen oder ihn laufen lassen | Russgrube befreit, Kuno als Steiger, `rep:Zuenfte+10` (Gerechtigkeit) |
| N125 | **Der Schmelzer** | Kuno | N124, M107 | 45 min | Alte Mine: Lava-Kanäle mit Hebeln umleiten, Rußgesichter als Ablenkung, Regionsboss Schmelzer | Bei Gerechtigkeit schaltet Egon den Schmelzer mitten im Kampf ab (Phase 2 entfällt, aber man muss ihn eine Minute schützen). Sonst: voller Kampf, mit Frost kühlen, Kern im Bauch. | Ring „Glutherz“, Kodex, Quellen wieder heiß |
| N126 | **Der summende Quarz** | Ambros | M104 | 25 min | Ambros braucht einen seltenen Quarz aus der Alten Mine (Eingang vor N125 nur Vorhalle) | Der Quarz summt sechs Töne. Der Spieler kennt sie nicht, aber Ambros wird blass und steckt ihn weg. „Falscher Quarz. Klingt wie eine Glocke in Watte.“ Er gibt einen anderen, besseren. (Saat für M408, Wiegenlied der Eltern.) | Stimmring-Quarz „Bernsteinglut“, Gravur gratis |

**Reihenfolge-Empfehlung:** N122 führt organisch zu N124. N125 nach N124. N126 jederzeit, aber am stärksten nach M104 (Ambros hat das Flackern gesehen).

---

## 5. Rätselschreine (3)

| Schrein | Ort | Mechanik | Lösung (kurz) | Lohn |
|---|---|---|---|---|
| **Quellorgel** | Quellbad | Dampfpfeifen in fünf Höhen | mit Quell- und Glut-Weisen Becken heizen und kühlen, bis die Pfeifen Jupps Melodie spielen | Atem-Kristall |
| **Lorenbahn** | Russgrube (nach Befreiung) | Weichen-Rätsel mit Klangschaltern | Loren so lenken, dass alle zwölf Lampen am Förderturm Erz bekommen | Stimmring-Quarz (Grubengold) |
| **Glockenbalken** | Waldhang über dem Dorf | Balken-Waage, auf der Glocken hängen | Glocken durch Töne leichter und schwerer machen (*Hallweite*, *Dämpfer*) | Kael-Inschrift (Questreihe „Stimmen aus Stein“) |

## 6. Sammelobjekte

| Was | Anzahl | Wo | Lore |
|---|---|---|---|
| **Kinderbriefe** | 10 | Waisenhaus, Markt, Notenfalter-Post | Briefe der Waisenkinder an den Spieler (lustig, nach M206 sehr traurig zu lesen) |
| **Grubenmarken** | 12 | Russgrube und Mine | Blechmarken der zwölf Toten, je eine kurze Geschichte; alle zwölf = Bonus-Szene am Förderturm |
| **Nachklänge** | 5 | Marktbrunnen, Waisenhaus-Flur, Förderturm, Schmelzhütte, Ambros' Werkstatt | zählen für S111 (der in Ambros' Werkstatt ist ein Wiegenlied-Fragment) |
| **Jupps Liedzettel** | 8 | überall im Dorf | neue Akkordeon-Lieder für das Rhythmus-Minispiel |

## 7. Gegner in der Region

- Kesselbrunn selbst ist in Akt 1 **friedlich** (kein Kampf im Heimatdorf).
- **Russgrube** nur im Kampf-Weg: Egon Ruß, **Rußgesichter** (Ablenkung, ungefährlich, aber sie verdecken die Sicht).
- **Alte Mine** (N125): **Erzkriecher** (Varianten von Pilzbrut, aus Stein), **Grubengeister** (Nachhall-Reste, harmlos, wenn man ihre Namen liest), Regionsboss **Schmelzer**.
- Ab Akt 2: Stille Hand (M206, großer Angriff), Kollegiums-Wachen am Brandheim-Schild.

## 8. Wie sich die Region über die Akte verändert

| Akt | Veränderung |
|---|---|
| 1 | wie oben; Briefe der Kinder nach Nocturna; Bergfest |
| 2 | M202 Scheunenbrand, M206 **Wintersperre**: Angriff der Stillen Hand, Waisenhaus brennt, Hanne stirbt, Ambros' Laden brennt. Kinder ziehen nach Uferlicht (Zelte am Kai). Danach Wiederaufbau als Weltaufgabe (Spieler hilft, Dorf wird Stück für Stück wieder bunt) |
| 3 | neues Waisenhaus „Hannes Ofen“, Bastian besteht die Aufnahme in Nocturna (wenn N121 gemacht) |
| 4 | Ambros schmiedet den Brandheim-Quarz (M404/M408); Kesselbrunn schickt Bergleute zur Verteidigung der Akademie (bei hohem Ruf Zünfte) |
| 5 | Epilog: Jupp spielt auf dem Marktplatz, alle Waisenkinder singen, eine Grubenlampe brennt für Hanne |
