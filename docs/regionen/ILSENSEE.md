# Region 1 – Nocturna & Ilsensee

Stand: 27.11.2026 · Version 1.0 · Akt 1 (wächst bis Akt 5) · Look: [ART_BIBLE §2](../ART_BIBLE.md) (Granitgrau, Kupfergrün, Kerzengold), Werte `Content/Data/Art/RegionLooks.json` (R01)
Orte und Marker als Daten: `Content/Data/World/Regions/Ilsensee.json` · Nebenquests: `Content/Data/Quests/Akt1/Ilsensee/N101–N106.json` · Gespräche: `Content/Data/Dialogues/Ilsensee/`

Die erste Region, in der der Spieler die meiste Zeit verbringt. Sie hat zwei Gesichter: **Uferlicht** am Ostufer, das fröhlichste Dorf des Spiels, und **Stillwasser** im Südschilf, ein Dorf, in dem niemand laut sprechen darf. Zwischen beiden liegt der See mit seiner versunkenen Glocke.

---

## 1. Überblick und Karte

Fläche: ca. 4 × 4 km (Kern des Akt-1-Gebiets, GDD §15). Der See ist 2,6 km lang und bis 90 m tief. Wasser ist dunkel, fast schwarz, aber klar: Man sieht nachts die Laternen im Wasser gespiegelt.

```
                         N
          Raunwald ▲▲▲   ┌───────────── Klippe ─────────────┐
           (Westen) ▲▲   │  NOCTURNA (Akademie, 312 Stufen) │
                ▲▲▲▲     └──────┬─────────────┬──────────────┘
                 ▲▲▲            │ Bootssteg    │ Gehege, Gewächshäuser
           Nebelharfe ◇         │              │
                 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
               ~~~   Reiherinsel ◇     ~~~~       ~~~~~~~~~   UFERLICHT
              ~~~~~        ~~~   ● Glockeninsel  ~~~~~~~~   (Ostufer, Kai,
              ~~~~~~   I L S E N S E E   (darunter: Seegrotte,  Laternenwerft,
              ~~~~~~~~~        versunkene Glocke) ~~~~~~~~~   Pfannkuchenplatz)
               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~      │
                 ~~~ Schilf ~~~~ Gezeitenstufen ◇ ~~~~~~        │ Uferweg
                    STILLWASSER (Südschilf, halb versunken)      │
                    Spiegelsteg ◇                         Straße nach Kesselbrunn (SO, 15 min)
                                                                 S
◇ = Rätselschrein   ● = Dungeon (Seegrotte, Eingang über Geheimgang 11 oder Glockeninsel)
```

| Ort | Lage | Was dort ist | Schnellreise |
|---|---|---|---|
| **Nocturna** | Nordklippe | Akademie (AKT1_SCHULLEBEN) | Bootssteg |
| **Uferlicht** | Ostufer | fröhliches Fischerdorf, Laternenwerft, Pfannkuchenplatz | Kai |
| **Stillwasser** | Südschilf | Gruseldorf, halb im See versunken | Kapelle (erst nach Befreiung) |
| **Glockeninsel** | Seemitte | kleine Felsinsel mit einem Glockenstuhl ohne Glocke; darunter die Seegrotte | – |
| **Reiherinsel** | Westen | Stelzreiher-Kolonie, Rätselschrein „Reiherruf“ | Insel-Steg |
| **Nebelharfe** | Westufer am Raunwaldrand | alte Kael-Harfe zwischen zwei Felsen, Rätselschrein | – |
| **Uferweg** | Ost/Süd | Straße Uferlicht → Kesselbrunn, Abzweig nach Stillwasser (Schild: „LEISE“) | – |

**Dichte:** alle 300–500 m etwas (GDD §15): Angelstellen, Laternenbojen, gesunkene Ruderboote mit Nachklängen, Flaschenpost, Glimmermotten-Wiesen am Westufer, ein Stelzreiher, der nur bei Nebel singt.

**Tageszeiten:** Morgens Nebel auf dem See (Uferlicht-Boote fahren raus), mittags klar, abends goldene Stunde über der Klippe, nachts Laternen. In Stillwasser ist es auch tagsüber dämmrig (Nebel hält sich im Schilf).

**Musik:** Uferlicht = Akkordeon, Fiedel, Shanty-Chor (Dur, 6/8). Stillwasser = keine Musik, nur Wasser, Schilfrascheln, ganz leises Flüstern; manchmal ein einzelner Glockenton unter Wasser. See = das Akt-1-Thema auf Harfe, nachts mit Glockenwels-Brummen.

---

## 2. Uferlicht – das fröhliche Dorf

> *„In Uferlicht brennt immer irgendwo eine Laterne. Und meistens auch ein Pfannkuchen.“*

**Stimmung:** Bunte Holzhäuser auf Stelzen am Ufer, Fischernetze mit Glasschwimmern, Laternen in allen Farben an Leinen über den Gassen. Möwen, Katzen, Kinder. Es riecht nach Teer, Fisch und Butter. Abends sitzt das halbe Dorf am Kai und singt.

**Aufbau (von Nord nach Süd):**
1. **Laternenwerft:** Wenke Haffs Werkstatt, hier werden die Laternenboote für das Ilsenlicht gebaut. Bootsverleih.
2. **Kai:** Anleger für das Akademieboot, Fischmarkt, Bänke, die Shanty-Ecke.
3. **Pfannkuchenplatz:** Oles Pfanne (ein Kessel, groß wie ein Wagenrad) unter einem Laternendach. Hier ist das Pfannkuchenfest.
4. **Gasthaus „Zum Laternenhecht“:** Lotte Krabbe, Zimmer (Rast-Bonus), Gerüchte, Würfelspiel.
5. **Gassen:** Laternenmacher Ignaz Docht (Laternen + Turmzimmer-Deko), Netzflickerin Oma Fenna (Taschen, Gürtel), Fischhändler Bodo.
6. **Leuchtsteg:** langer Steg ins Wasser mit einer Laterne am Ende, die nie ausgeht. Liebespaare, Angler, Fotomodus.

### 2.1 Bewohner

| NPC | Wer | Eigenart | Was man bei ihr/ihm bekommt |
|---|---|---|---|
| **Kapitänin Wenke Haff** | Fischerin, baut Laternenboote, Meisterin für Wasser- und Reiseweisen | spricht wie ein Seewetterbericht mit Herz, rau, lacht über Stürme | Bootsverleih, Weisen *Segelwind*, *Laternenboot*, *Nebellicht*, *Ankerhalt*, Koop-Weise *Brückenlied*; Quest N101 |
| **Ole Butterfass** | Pfannkuchenbäcker, wirft Pfannkuchen bis aufs Dach | dick, flink, wettet um alles, verliert nie beim Wettwerfen (seit 11 Jahren) | Essen (Buffs), Backweisen; S112 |
| **Lotte Krabbe** | Wirtin „Zum Laternenhecht“, Shanty-Sängerin | laute Stimme, kennt jedes Gerücht, hat ein Herz für Streuner | Zimmer (Rast-Bonus), Gerüchte, Würfeln; Quest N103 (Glockenwels-Legende) |
| **Ignaz Docht** | Laternenmacher | redet nur über Licht, sieht schlecht, verwechselt Leute | Laternen (Licht-Upgrade), Turmzimmer-Deko |
| **Oma Fenna** | Netzflickerin, 90 | strickt beim Gehen, erzählt Geschichten aus der Zeit, als Stillwasser noch laut war | Taschen, Gürtel, Netzknoten-Schmuck; Hinweise zu N104 |
| **Bodo** | Fischhändler | brüllt Preise, hat zu allem eine Meinung | Fisch (Futter für Reittiere/Katzen), Köder |
| **Die Kaimöwen** | Kinderbande: **Pips** (Anführerin, 10), **Jule** (8, zählt alles), **Matti** (7, will mutig sein) | Mutproben, Streiche, laufen dem Spieler nach | Quest N102, Mutproben-Minispiele, Geheimnisse des Dorfs |
| **Tammo** | Fischerjunge, 14 | Angst vor dem Südschilf, rudert nie nach Süden | Angeln-Tutorial, Angelrute |

### 2.2 Was man in Uferlicht tut

- **Läden:** Laternen, Deko, Taschen, Fisch, Köder, Bootsverleih.
- **Minispiele:** Pfannkuchen-Wettwerfen (Ole), Würfeln (Lotte), Angeln (Tammo, Kai und Leuchtsteg), Laternenboot-Rennen (ab M106 jederzeit), Shanty-Abend (Rhythmus-Spiel, Lotte), Mutproben (Kaimöwen).
- **Rast:** Zimmer im Laternenhecht oder Bank am Leuchtsteg (Nebel-Bonus: *Atem* regeneriert schneller).
- **Feste:** **Pfannkuchenfest** (einmal im Herbst, S112), **Ilsenlicht** (M106, Buden aus Uferlicht), **Laternennacht** (jeden Neumond: alle Laternen auf den See, man kann mitfahren).
- **Geschenke:** Wer oft da ist, bekommt Kleinigkeiten (Lotte: Fischbrötchen, Kinder: Muscheln, Wenke: einen alten Kompass).
- **Weltaufgabe aus dem Unterricht** (AKT1_SCHULLEBEN §5): „Bring drei Leute in Uferlicht zum Lachen, ohne Kitzelklang“ → Ole (Pfannkuchen auf den Kopf), Bodo (Möwe klaut seinen Hut), Kaimöwen (Grimassen-Duell).

---

## 3. Stillwasser – das Flüsterdorf

> *„Psst. Nicht so laut. Sie hört dich.“*

**Stimmung:** Ein Fischerdorf im Südschilf, halb versunken: Der Seespiegel ist seit 40 Jahren gestiegen, die unteren Häuser stehen bis zu den Fenstern im Wasser. Stege verbinden die Dächer. Alle Glocken sind mit Tüchern umwickelt. An jeder Tür ein Schild: **„LEISE“**. Die Bewohner sprechen nur im Flüsterton. Wer laut spricht, verliert die Stimme für einen Tag.

**Das Unheimlichste:** **Das Spiegelbild.** Schaut man ins Wasser, sieht man Stillwasser **anders**: hell erleuchtet, mit Musik, Lampions, tanzenden Menschen. Es ist der Abend des Sommerfests vor 40 Jahren. Die Spiegelleute bewegen sich. Manchmal schaut einer hoch, direkt zum Spieler. Und manchmal steht im Spiegel jemand neben dem Spieler, der in Wirklichkeit nicht da ist: ein Mann mit nassem Haar.

**Aufbau:**
1. **Schild am Uferweg:** „STILLWASSER – LEISE“. Ab hier dünnt die Musik aus. Vögel verstummen.
2. **Dachstege:** Wege über die versunkenen Häuser. Unter Wasser sieht man Möbel, Teller auf Tischen, eine Wiege.
3. **Waschplatz:** Drei Wäscherinnen waschen seit Jahren dasselbe Laken. Sie flüstern im Kanon: „Weiß. Weißer. Am weißesten.“
4. **Kapelle mit umwickelter Glocke:** Hier wohnt Agnes, die Älteste.
5. **Haus der Witwe:** das einzige Haus, das trocken auf einem Felsen steht. Ein Fenster mit Licht. Drinnen: Senta Grau.
6. **Spiegelsteg:** langer Steg ins Schilf, Rätselschrein (§5), hier ist das Spiegelbild am deutlichsten.

### 3.1 Bewohner

| NPC | Wer | Eigenart | Rolle |
|---|---|---|---|
| **Witwe Senta Grau** | Missklingerin (Fluchsängerin), Herrscherin über das Dorf | schöne, leise Stimme, freundlich, trägt Schwarz und eine Muschelkette, lächelt nie mit den Augen | Gegnerin in N104; kann ohne Kampf erlöst werden |
| **Flüsterfrau Agnes** | Älteste, 78 | flüstert, schreibt auf eine Schiefertafel, wenn es wichtig ist, mutiger, als sie aussieht | Questgeberin N104; nach Befreiung Meisterin für Flüsterweisen (spricht dann wieder laut und sehr gern) |
| **Linus** | Fährjunge, 12 | hat vor zwei Wochen laut gelacht und seitdem keine Stimme; schreibt mit Kreide auf Planken | hilft heimlich, zeigt Wege über die Dächer |
| **Die drei Wäscherinnen** | Schwestern Grete, Hedwig und Irmgard | waschen dasselbe Laken, sprechen im Kanon | Rätsel: Ihr Laken ist das Festtischtuch von damals (Hinweis in N104) |
| **Hinnerk** | alter Fischer | steht jede Nacht auf dem Spiegelsteg und schaut ins Wasser | Zeuge vom Abend des Unglücks, hat Konrad rufen hören und nichts gesagt |
| **Konrad Grau** (Spiegelbild) | Sentas Mann, ertrunken vor 40 Jahren | erscheint nur im Spiegel, nasses Haar, ruft lautlos | Schlüssel zur Erlösung |

### 3.2 Die Geschichte von Stillwasser

Vor 40 Jahren war Stillwasser das lauteste Dorf am See: Glockengießer, Musikanten, das Sommerfest mit Lampions bis Uferlicht. In der Festnacht ruderte **Konrad Grau** hinaus, um die Festglocke auf der Glockeninsel zu läuten, und kenterte im Nebel. Er rief um Hilfe. Das Dorf sang so laut, dass niemand ihn hörte. Nur Hinnerk hörte einen Ruf und dachte, es sei ein Scherz.

Seine Frau **Senta** stand am nächsten Morgen am Ufer. Sie war Wirkerin, eine gute. Sie lernte von einem fahrenden Missklinger, wie man Stimmen verstimmt, und legte einen Fluch über das Dorf: **Wer laut ist, verliert seine Stimme.** Das Wasser stieg, weil der See den Ton des Dorfs nicht mehr trug (Lore: Seen in Velmark halten durch Klang ihren Pegel). Seitdem ist Stillwasser still.

Senta ist keine Bestie. Sie glaubt, sie beschützt das Dorf davor, noch einmal jemanden zu überhören. Sie hat sich selbst auch verflucht: Sie hat seit 40 Jahren nicht mehr laut geweint.

### 3.3 Grusel-Momente (geplant, je einmal)

1. **Das erste Spiegelbild:** Auf dem ersten Dachsteg schaut der Spieler (Kamera-Hinweis: kurzer Glanz im Wasser) ins Wasser und sieht das Fest. Niemand sagt etwas dazu.
2. **Die Wiege:** Unter Wasser in einem Haus schaukelt eine Wiege. Es gibt keine Strömung.
3. **Der laute Schritt:** Der Spieler stößt einen Eimer um (gescriptet). Alle Fenster des Dorfs gehen gleichzeitig auf. Alle Bewohner schauen ihn an und legen den Finger auf die Lippen. Dann gehen die Fenster wieder zu.
4. **Der Mann neben dir:** Auf dem Spiegelsteg steht im Spiegelbild Konrad direkt neben dem Spieler. Dreht man sich um: niemand. Schaut man wieder ins Wasser: Er zeigt auf die Glockeninsel.
5. **Einziger echter Schreck** (Regel BESTIARIUM §1.4): Im Haus der Witwe hängt ein Spiegel. Wenn man davorsteht, flüstert Senta direkt ins Ohr (Stereo, ganz nah): „Psst.“ Sie steht hinter dem Spieler.

### 3.4 Die Befreiung (Quest N104) und was danach passiert

**Zwei Wege:**
- **Erlösung (empfohlen, kein Kampf):** Konrads letzten Ruf finden. Er ist als Echo auf der Glockeninsel gefangen (Seelenecho erst ab Akt 2, darum in Akt 1: Mirelas Nachklang-Glas aus S111 fängt den Ruf ein, oder später mit Seelenecho). Senta den Ruf vorspielen. Sie hört ihn zum ersten Mal. Sie weint, laut. Der Fluch bricht.
- **Kampf:** Senta Grau als Mini-Boss (Fluchsängerin-Elite, singt Wellen aus dem See, Stimme-Raub). Danach ergibt sie sich und flieht in den Raunwald. Der Fluch bricht, aber das Dorf trauert und der Spiegel bleibt (das Fest im Wasser bleibt sichtbar, man hört jetzt aber leise die Musik).

**Nach der Befreiung (sichtbare Belohnung, GDD §14):**
- Die Tücher fallen von den Glocken. Ganz Stillwasser läutet gleichzeitig, man hört es in Nocturna. Die Stimmglocke der Akademie antwortet.
- Das Wasser sinkt in den nächsten Tagen um zwei Meter, die unteren Häuser tauchen auf, Bewohner räumen auf und singen dabei.
- **Lautfest:** Ein neues Dorffest, bei dem alle so laut schreien, singen und lachen dürfen, wie sie wollen. Agnes gewinnt immer.
- Agnes wird Meisterin für **Flüsterweisen** (und spricht jetzt ununterbrochen).
- Linus bekommt seine Stimme zurück und wird Fährmann zwischen Stillwasser und Uferlicht (neue Schnellreise per Boot).
- **Neuer Laden:** Die Wäscherinnen eröffnen eine Färberei (Farbstoffe für die Personalisierung, GDD §13). Das alte Festtischtuch hängt als Fahne über dem Dorf.
- Bei Erlösung: **Senta** läutet jeden Abend bei Sonnenuntergang die Festglocke auf der Glockeninsel, für Konrad. Wer sie besucht, kann mit ihr reden (sie lehrt eine einzige Weise: *Ruf über das Wasser*, die man auf dem See auch im Nebel hört).

---

## 4. Nebenquests der Region (6)

Regel (STORY_BIBLE §10): kein reines Holen und Bringen, jede Quest hat einen Twist, ein Rätsel oder eine Entscheidung.

| ID | Name | Wer | Ab | Dauer | Kern | Twist / Entscheidung | Lohn |
|---|---|---|---|---|---|---|---|
| N101 | **Das Boot, das allein fährt** | Wenke Haff | M104 | 30 min | Laternenboote für das Ilsenlicht bauen (Werkstatt-Minispiel) | Ein Boot fährt nachts allein nach Stillwasser. Der Spieler folgt ihm: Es fährt zur Glockeninsel und bleibt dort stehen, als warte es. (Anstoß für N104) | Weisen *Laternenboot*, *Segelwind*; eigenes Ruderboot |
| N102 | **Die Mutprobe der Kaimöwen** | Pips | M104 | 25 min | Die Kinderbande stellt Mutproben (Leuchtsteg im Dunkeln, Ottilie eine Feder klauen, Oles Pfanne läuten) | Letzte Mutprobe: die Glocke in Stillwasser auswickeln. Der Spieler entscheidet: mitmachen, verbieten oder selbst gehen. Matti geht heimlich allein und verliert seine Stimme. | Kaimöwen-Abzeichen, Geheimversteck der Bande (Kiste mit Schätzen) |
| N103 | **Der Glockenwels** | Lotte Krabbe | M103 | 30 min | Angel-Legende: Wer den Glockenwels fängt, hat einen Wunsch frei. Köder, Stellen, Wetter herausfinden. | Man kann ihn nicht fangen. Wenn er anbeißt, zieht er das Boot zur Glockeninsel und taucht. Dort unten liegt die versunkene Glocke. Loslassen = er bringt einen Schlüssel (Eingang Seegrotte). Festhalten = Leine reißt, Spieler fällt ins Wasser (lustig), Schlüssel später über Geheimgang 11. | Seegrotten-Schlüssel, Legende im Kodex |
| N104 | **Das Flüsterdorf** | Agnes | N102 oder N101 | 50 min | Stillwasser befreien (§3.4) | Erlösung oder Kampf. Hinnerks Schuld: ihn verraten oder ihn selbst gestehen lassen. | Stillwasser befreit, Agnes als Meisterin, Färberei, `rep:Hueter+10` (Erlösung) |
| N105 | **Die Seeglocke** | – (Dungeon) | N103, M107 | 45 min | Seegrotte unter dem See: Wasserstand-Rätsel, Echo-Gänge, Glockenwels als Helfer | Regionsboss **Seeglocke**: Sie ist nicht böse, sie ist gesprungen und schreit vor Schmerz. Zerschlagen (schneller) oder mit *Wiederklang* heilen (schwerer). Geheilt läutet sie jede Vollilse. | Ring „Seeglocke“, Weise *Glockenruf*-Variante, Kodex |
| N106 | **Die Robbe im Netz** | Tammo | M201 (Akt 2) | 35 min | Fischer klagen über eine Robbe, die Netze zerreißt | Sie befreit ihr Junges. Netze mit Robben-Tor knüpfen (Lio hilft) oder die Robbe vertreiben. Mit Robben-Tor: Wettschwimmen, sie wird Reittier. | Reittier **Gischtrobbe** (R5) |

Dazu in der Region: S103 (Lampenöl aus Uferlicht), S112 (Pfannkuchen-Meisterschaft), S108 (Froschbucht am Westufer).

---

## 5. Rätselschreine (4)

| Schrein | Ort | Mechanik | Lösung (kurz) | Lohn |
|---|---|---|---|---|
| **Spiegelsteg** | Stillwasser | Spiegelbild-Rätsel: Plattformen existieren nur im Spiegelbild | Im Wasser schauen, wo der Spiegel-Steg ist, blind darauf laufen; *Laternenlicht* macht ihn kurz sichtbar | Atem-Kristall (+Atem) |
| **Reiherruf** | Reiherinsel | Klangfolge: Die Stelzreiher singen zweistimmig, man muss die zweite Stimme pfeifen | Hören, nachpfeifen (Pfeifen-Wirkart), drei Runden länger werdend | Stimmring-Quarz (Farbe Reiherblau) |
| **Nebelharfe** | Westufer | Kael-Harfe: Nebel-Saiten, die nur bei Nebel da sind | Morgens bei Nebel kommen, *Nebellicht* zeigt Saiten, Akkord Strom-Hall-Quell spielen | Kael-Inschrift (Questreihe „Stimmen aus Stein“) |
| **Gezeitenstufen** | Südschilf, zwischen Stillwasser und Uferweg | Wasserstand: Stufen tauchen bei Ebbe auf | Mit *Ankerhalt* Boote als Stufen festhalten, Wasser mit Quell-Weisen senken | Bootswimpel „Gezeiten“ |

## 6. Sammelobjekte

| Was | Anzahl | Wo | Lore |
|---|---|---|---|
| **Flaschenpost** | 12 | im See treibend, im Schilf, an der Glockeninsel | Briefe von Uferlicht-Kindern über 300 Jahre (lustig, dann einer aus Stillwasser vom Festabend: „Ich hab Papa rufen hören, aber Mama sagt, das ist der Wind.“) |
| **Nachklänge** | 6 | Leuchtsteg, Glockeninsel, Stillwasser-Kapelle, Laternenwerft, Reiherinsel, Seegrotte | zählen für S111 |
| **Laternen-Muster** | 8 | Ignaz' verlorene Entwürfe | neue Laternen-Optik fürs Turmzimmer |
| **Kodex-Kreaturen** | Glockenwels, Stelzreiher, Gischtrobbe, Laternenkrähe, Glimmermotte | beobachten | Kreaturen-Kodex |

## 7. Gegner in der Region

Die Region ist in Akt 1 fast friedlich (Schul-Vibe). Kämpfe gibt es nur:
- in **Stillwasser** (N104, nur im Kampf-Weg: Senta Grau + 2 Fluchsänger-Lehrlinge, Flüstermöwen als Alarm),
- in der **Seegrotte** (N105: Scherbengolem-Jungtier aus Glockenbronze, Moorlicht-Unken-Kaulquappen, Regionsboss Seeglocke),
- nachts am **Westufer**: Pilzbrut aus dem Raunwald (wenige).
Ab Akt 2 (nach der Wintersperre) kommen Patrouillen der Stillen Hand über den See (Boote im Nebel, Metronom-Ticken über dem Wasser) und Wilderer am Westufer.

## 8. Wie sich die Region über die Akte verändert

| Akt | Veränderung |
|---|---|
| 1 | wie oben; Ilsenlicht (M106); Winter: See friert am Rand zu, Eis-Glockenball (M109) |
| 2 | Wintersperre: Uferlicht nimmt Flüchtlinge aus Kesselbrunn auf (Zelte am Kai). Stille-Hand-Boote nachts. Gischtrobbe (N106). |
| 3 | Turnier (M304): Uferlicht baut Tribünen am Ufer, Wettbüro bei Ole. |
| 4 | Nacht der Glocken (M406): Feinde kommen über den See. Uferlicht schickt Boote zur Rettung (bei hohem Ruf Zünfte). Danach Trauerlaternen. |
| 5 | Epilog (M510): Laternennacht mit allen Überlebenden. Ein neues Boot im Nebel. |
