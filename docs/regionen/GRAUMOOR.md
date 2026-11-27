# Region 4 – Graumoor

Stand: 27.11.2026 · Akt 2 (ab M203) · Daten: `Content/Data/World/Regions/Graumoor.json`, Quests `Content/Data/Quests/Akt2/Graumoor/N201–N206.json`, Dialoge `Content/Data/Dialogues/Graumoor/`
Grundlagen: [STORY_BIBLE §4](../STORY_BIBLE.md), [BESTIARIUM](../BESTIARIUM.md) (Agathe Dornfeld, Moorkönigin), [M203–M205](../missions/AKT2.md). Vorgänger-Regionen: [ILSENSEE](ILSENSEE.md), [RAUNWALD](RAUNWALD.md), [KESSELBRUNN](KESSELBRUNN.md).

> *„Das Moor lügt nicht. Es stinkt nur.“* – Mutter Quendel

## 1. Überblick und Karte

Ein weites Moor südwestlich des Raunwalds, ca. 5 × 4 km. Grauer Nebel, schwarzes Wasser, Torfinseln, Birken mit weißer Rinde, Stege auf Pfählen. Das Moor ist **grau, aber nicht tot**: Frösche singen zweistimmig, Stelzreiher stehen im Nebel, Mooslinge rollen über die Bulten. Unter dem Moor läuft eine große **Ader**, und an einer Stelle ist die Welt dünn: Dort liegt Altwacht, die versunkene Festung, und dort kommt man in den Nachhall (M204).

```
                       N
          Raunwald-Rand ▲▲  (Wilderer-Lager, Akt 2)
                │
     Birkenhain ◇     Torfstich (Kollegium, nur E2_KeyCollegium)
                │
   MOORWINKEL ✝ ──── Bohlenweg ──── STELZENHEIM (Mitte, auf Pfählen)
   (Westen, Felder)                    │  Aderknoten ◇
                │                      │
        Irrlichtsenke  ~~~~~~~~  Königinnenteich ●  (Moorkönigin)
                │
          ALTWACHT ● (versunkene Festung, dünner Ort zum Nachhall)
                       S
◇ = Rätselschrein   ● = Dungeon/Boss   ✝ = Gruseldorf
```

| Ort | Lage | Was dort ist | Schnellreise |
|---|---|---|---|
| **Stelzenheim** | Moormitte | Pfahldorf der Hüter, Mutter Quendel | Froschsteg (nach M203) |
| **Moorwinkel** | Westen | Gruseldorf, Bauern als Marionetten auf den Feldern | Dorfbrunnen (nach Befreiung) |
| **Altwacht** | Süden | versunkene Festung, dünner Ort (M204) | Torruine (nach M204) |
| **Königinnenteich** | Südosten | Regionsboss Moorkönigin | – |
| **Irrlichtsenke** | Südwesten | tausend Irrlichter im Nebel, gefährlich | – |
| **Torfstich** | Norden | nur bei `E2_KeyCollegium`: Kanäle, Pumpen, Kollegiums-Ingenieure | Pumpenhaus |

**Klang-Navigation:** Im dichten Nebel sieht man 10 m weit. Orientierung über **Tonquellen**: Froschchöre (sicherer Boden), Stelzreiher (offenes Wasser), Glockenbojen der Hüter (Weg nach Stelzenheim). Wer auf Irrlichter zugeht, landet im Schlamm.

**Musik:** Stelzenheim = Froschchor als Rhythmus, Holzmaultrommel, Kehlgesang der Hüter, tief und warm. Moor = Bordun-Ton (ein tiefer Dauerton), über dem leise das Akt-2-Thema auf Bratsche liegt. Moorwinkel = ein Kinderlied (Ringelreihen), von Holzpuppen-Klappern begleitet, eine Spur zu langsam.

**Die Entscheidung 2 verändert die Region (M205):**
- **`E2_KeyKeepers`:** Die Ader heilt. Ab Akt 3 blüht das Moor: Lichtsumpf (Wasser leuchtet nachts grün), Stelzreiher-Schwärme, Seerosen so groß wie Boote. Moorkönigin ist nur krank (erlösbar).
- **`E2_KeyCollegium`:** Ab Akt 3 Torfstich und Kanäle, Wege aus Bohlen und Stein, Pumpenhäuser. Weniger Nebel, weniger Frösche, weniger Kreaturen. Stelzenheim halb leer (Familien ziehen nach Glastiefen und Raunwald). Moorkönigin greift aus Not an (Kampf ohne Erlösung).

---

## 2. Stelzenheim – das Pfahldorf

> *„Wer hier lebt, muss tanzen können. Die Stege schwanken immer.“*

**Stimmung:** Rundhäuser aus Schilf und Birkenholz auf hohen Pfählen, verbunden durch schwankende Stege und Hängebrücken. Unter den Häusern liegen Boote. Laternen aus getrockneten Kürbissen, Netze mit Moosballen, Räucherrauch. Überall Frösche, auf Dächern, in Fenstern, in Hüten. Die Hüter tragen Mäntel aus Moosfilz und Holzperlen im Haar.

**Aufbau:**
1. **Froschsteg:** Anlegestelle, Begrüßung durch einen Froschchor.
2. **Quendels Haus:** das höchste Haus, Pfeifenrauch, Kael-Inschriften an den Wänden, eine Kiste mit Kinderzeichnungen.
3. **Ratskreis:** runde Plattform, hier tagen die Hüter (M205 Entscheidung).
4. **Heilerhütte:** Kräuter, Moorbäder, Nachhall-Reinigung (`NachhallLoad` senken gegen Gold oder Gefallen).
5. **Schnitzerplattform:** Holzperlen, Masken, Stimmring-Fassungen aus Mooreiche.
6. **Aderknoten:** ein Ring aus Steinen im Wasser, unter dem die Ader pulsiert (M203, M205).

### 2.1 Bewohner

| NPC | Wer | Eigenart | Was man bekommt |
|---|---|---|---|
| **Mutter Quendel** | Älteste der Hüter, spricht Kael | lacht mit Goldzahn, Pfeife, sagt Unbequemes laut | Hauptfigur M203/M205; Kael-Unterricht ab Akt 3 |
| **Bruder Ilmo** | Heiler der Hüter | spricht sehr langsam, macht Pausen mitten im Satz, hat immer recht | Nachhall-Reinigung, Moorbäder, Quest N203 |
| **Wenna Bult** | Schnitzerin | schnitzt beim Reden, die Figur sieht am Ende immer aus wie der Gesprächspartner | Holzperlen, Masken (Personalisierung), Stimmring-Fassungen Mooreiche |
| **Fips und Fino** | Froschhüter-Zwillinge (12) | streiten sich, wer Frösche besser versteht | Quest N201, Froschrennen |
| **Seline** | junge Hüterin (aus dem Raunwald, N115) | knapp, ehrlich | kommt mit Akt 2 nach Stelzenheim, Fraktionsaufträge |
| **Herr Quaak** | Froschkönig (nach S108, M203) | sitzt auf einem Seerosenthron im Sommer | Tilda besucht ihn |

### 2.2 Was man in Stelzenheim tut

- **Läden:** Heilerhütte (Reinigung), Schnitzerin, Moorkräuter, Mooslinge (Haustiere).
- **Minispiele:** Froschrennen (Fips und Fino), Steg-Tanz (Balance-Rhythmus), Aderknoten-Gesang (Rhythmus mit drei Stimmen), Moorangeln (Aale, die singen).
- **Feste:** **Frösche-Chor-Fest** (M203, danach jede Vollmondnacht), **Nebelnacht** im Herbst (alle Laternen aus, man findet sich nur über Lieder).
- **Fraktion Hüter:** Ab M203 Ruf-Aufträge (Aderquellen reinigen, Kreaturen umsiedeln, Wilderer vertreiben).

---

## 3. Moorwinkel – das Marionettendorf

> *„Ringel, Ringel, Reihe, wir tanzen, bis wir dreie. Und wer stillsteht, bleibt.“*

**Stimmung:** Ein Bauerndorf am Westrand des Moors, auf einem trockenen Hügel. Kartoffeläcker, Torfhütten, eine Windmühle, die sich ohne Wind dreht. Auf den Feldern arbeiten die **Bauern**: ruckartig, an **unsichtbaren Fäden**. Tag und Nacht. Sie hacken, säen, ernten, auch wenn nichts wächst. Ihre Gesichter sind ruhig, als schliefen sie. Zwischen den Feldern stehen **Vogelscheuchen**, die aussehen wie Kinder. Über allem tanzen Irrlichter.

**Das Unheimlichste:** **Die Fäden.** Mit *Seelenecho* oder bei Gegenlicht sieht man sie: Hunderte feine, glänzende Fäden, die von jedem Bauern zum Turm der Windmühle führen. Wenn der Spieler einen Faden berührt, spürt er einen Ruck in der eigenen Hand (Controller-Vibration), als hinge er selbst dran.

**Aufbau:**
1. **Bohlenweg von Stelzenheim:** endet an einem Gatter mit einem Schild: „FREMDE TANZEN MIT“.
2. **Die Felder:** Marionetten-Bauern, Vogelscheuchen-Kinder.
3. **Dorfplatz mit Ringelreihen-Kreis:** Ein Kreis aus Steinen. Nachts tanzen hier die Bauern im Kreis, zu dem Kinderlied, das niemand singt.
4. **Die Windmühle:** Agathe Dornfelds Haus. Die Flügel tragen die Fäden.
5. **Der Friedhof:** Gräber mit Moos, ein Grab ohne Namen, frisch geschmückt.

### 3.1 Bewohner

| NPC | Wer | Eigenart | Rolle |
|---|---|---|---|
| **Agathe Dornfeld** | Missklingerin (Hexenmutter), Herrscherin | uralt, krumm, Kleid aus Moos und Fäden, Puppen am Gürtel, nennt alle „mein Herzchen“, freundlich bis zur Gänsehaut | Gegnerin in N204; erlösbar |
| **Bäuerin Martha Ackerlein** | Marionette, früher Dorfvorsteherin | spricht im Schlaf, wenn man ihren Faden lockert | Zeugin der Verstoßung |
| **Tobi** | Junge, 10, nicht an Fäden | versteckt sich als Vogelscheuche zwischen den anderen, bewegt sich nur, wenn niemand hinsieht | Questgeber N204 |
| **Die Vogelscheuchen** | Puppen in Kinderkleidern | tragen Namensschilder der Dorfkinder | Hinweis: Die Kinder sind nach Stelzenheim geflohen, die Puppen sind ihr Ersatz |

### 3.2 Die Geschichte von Moorwinkel

Agathe Dornfeld war vor 50 Jahren eine junge Hüterin in Stelzenheim, Quendels beste Freundin. Sie hörte als Einzige die Stimmen aus dem dünnen Ort bei Altwacht: verlorene Kael-Echos, die um Hilfe baten. Sie sprach mit ihnen. Die Hüter fürchteten den Nachhall und verstießen sie. Quendel stimmte als Letzte zu. Agathe zog nach Moorwinkel und half dort den Bauern mit Wetterliedern.

Vor einem Jahr brach die Ader unter dem Moor (die Stille Hand bohrte bei Altwacht, M204). Die Felder von Moorwinkel versanken, die Ernte faulte. Die Bauern wollten fortziehen. Agathe hatte Angst, wieder allein zu sein. Sie band sie mit Fäden, „nur bis die Ernte wieder wächst“. Die Kinder schickte sie nach Stelzenheim, damit sie nicht gebunden werden. An ihrer Stelle stellte sie Vogelscheuchen auf. Das Grab ohne Namen ist ihr eigenes: Sie hat es sich gegraben, weil sie glaubt, dass niemand anderes es tun würde.

### 3.3 Grusel-Momente (je einmal)

1. **Das Schild:** „FREMDE TANZEN MIT“. Nach dem Gatter zieht ein Faden kurz an der Hand des Spielers, die Figur macht einen ungewollten Tanzschritt.
2. **Die Vogelscheuche dreht den Kopf:** Tobi, aber das weiß man noch nicht.
3. **Der Ringelreihen:** Nachts tanzen alle Bauern im Kreis, und es ist ein Platz frei. Er bleibt frei, bis der Spieler weitergeht. Dann schließt sich der Kreis hinter ihm.
4. **Das Grab:** Frische Blumen, kein Name. Wenn man nach M204 Seelenecho wirkt, hört man Agathe: „Hier liegt niemand. Noch nicht.“
5. **Einziger echter Schreck** (BESTIARIUM §1.4): In der Windmühle hängt Agathe kopfüber von der Decke zwischen ihren Fäden, wie eine Spinne, als der Spieler die Treppe hochkommt. Sie sagt freundlich: „Da bist du ja, mein Herzchen.“

### 3.4 Die Befreiung (Quest N204) und was danach passiert

**Zwei Wege:**
- **Versöhnung (kein Kampf):** Quendel überzeugen, Agathe zurückzunehmen. Dafür braucht man **Ruf Hüter ≥ Geschätzt** (`rep:Hueter>=30`) und die Wahrheit über damals (Martha im Schlaf, Seelenecho am Ratskreis, Quendels Kinderzeichnungen: eine zeigt zwei Mädchen Hand in Hand). Quendel geht selbst nach Moorwinkel, die Treppe der Windmühle hoch, und sagt: „Ich hab als Letzte Ja gesagt. Ich sag als Erste Nein.“ Agathe lässt die Fäden los.
- **Kampf:** Agathe Dornfeld als Mini-Boss (Hexenmutter-Elite: Fluchkreise in drei Farben, Teleport zwischen Kreisen, ruft Irrlichter und Moorlicht-Unken, Phase 2: zwei Marionetten-Bauern, deren Fäden man trennt). Danach flieht sie in die Irrlichtsenke. Die Bauern fallen um und schlafen.

**Danach:**
- Die Fäden reißen, die Bauern sinken auf die Felder und schlafen einen Tag. Die Windmühle steht still.
- Die Kinder kommen aus Stelzenheim zurück und werfen die Vogelscheuchen um (Tobi zuerst).
- Bei Versöhnung: Agathe zieht nach Stelzenheim zurück, wird Lehrerin für **Fadenweisen** (Gegenstände an Fäden ziehen, Seile spannen) und spricht in Akt 5 mit den Echos im Nachhall (hilft beim Weg zu Nym). Sie streicht das Grab zu.
- Moorwinkel feiert jedes Jahr ein **Erntefest**, bei dem man nur tanzt, wenn man will.
- **Neuer Laden:** Webstube von Martha (Umhänge, Schals aus Moorwolle).

---

## 4. Nebenquests der Region (6)

| ID | Name | Wer | Ab | Dauer | Kern | Twist / Entscheidung | Lohn |
|---|---|---|---|---|---|---|---|
| N201 | **Das große Froschrennen** | Fips und Fino | M203 | 25 min | Froschrennen zwischen den Zwillingen, jeder will den Spieler im Team | Die Frösche laufen nur, wenn man ihnen vorsingt. Fips' Frosch ist taub. Entscheidung: Fips verraten, ihm ein Lied mit Klopfrhythmus zeigen (er gewinnt fair) oder Fino helfen. | Froschpfeife (lockt Frösche, zeigt sicheren Boden) |
| N202 | **Die Wilderer am Birkenhain** | Seline | M203 | 35 min | Wilderer fangen Mooslinge und Stelzreiher | Sie haben einen Vertrag mit Aurenburg: Kreaturen mit Kael-Quarz im Magen. (Anstoß Questreihe „Die Arche“, Verbindung zu Fraktionskonflikt) | Arche-Questreihe, `rep:Hueter+10` |
| N203 | **Bruder Ilmos Bad** | Ilmo | M204 | 25 min | Nachhall-Reinigung: Ilmo braucht drei Zutaten (Moorlilie, Stelzreiherfeder, Irrlicht im Glas) | Das Irrlicht ist ein verlorenes Kind-Echo aus Moorwinkel. Freilassen (Seelenecho zeigt den Weg) oder ins Bad geben (stärkere Reinigung). | `NachhallLoad-10` oder Echo-Kodex + Agathe-Hinweis |
| N204 | **Moorwinkel** | Tobi | M204 | 50 min | Moorwinkel befreien (§3.4) | Versöhnung (Ruf Hüter ≥ 30) oder Kampf | Moorwinkel befreit, Fadenweisen, `rep:Hueter+10` |
| N205 | **Die Moorkönigin** | Quendel | M205 | 45 min | Riesige Moorlicht-Unke greift Stelzenheim an | Bei `E2_KeyKeepers`: Sie ist krank vom Bohren bei Altwacht, Quendel singt sie gesund (Spieler hält Irrlichter fern). Bei `E2_KeyCollegium`: Kampf, sie flieht vor den Pumpen. | Ring „Moorkrone“, Kodex |
| N206 | **Die Nebelnacht** | Wenna Bult | M203 | 30 min | Nebelnacht-Fest: Alle Laternen aus, man findet seine Leute nur über ihr Lied | Man muss die Lieder von Lio, Aurelia (falls dabei) und Wim erkennen. Am Ende singt einer, den man nicht kennt: Quendel am Steg, auf Kael, und aus dem Nebel antwortet eine Frau (Saat aus M203). | Nebellaterne, Kodex „Lieder der Freunde“ |

---

## 5. Rätselschreine (3)

| Schrein | Ort | Mechanik | Lösung (kurz) | Lohn |
|---|---|---|---|---|
| **Aderknoten** | Stelzenheim | drei Steine, drei Stimmen | Grundton halten, Ober- und Mittelstimme mit Stimmringen wechseln | Atem-Kristall |
| **Birkenorgel** | Birkenhain | hohle Birken, durch die der Wind pfeift | Löcher mit Moos stopfen, bis die Birken das Froschlied spielen | Stimmring-Quarz (Irrlichtgrün) |
| **Wachglocke von Altwacht** | Altwacht, äußerer Turm | Glocke, die im Nachhall heil ist und draußen gesprungen | Nachhallschritt, dort läuten, draußen klingt sie mit | Kael-Inschrift (Questreihe „Stimmen aus Stein“) |

## 6. Sammelobjekte

| Was | Anzahl | Wo | Lore |
|---|---|---|---|
| **Irrlichter im Glas** | 10 | überall im Nebel, nur nachts | jedes ist ein Echo eines Satzes, der im Moor gesagt wurde |
| **Hüterperlen** | 12 | bei Hütern, an Aderquellen | jede Perle trägt ein Symbol, alle zusammen = Hüter-Kette (Bonus Ruf) |
| **Nachklänge** | 5 | Ratskreis, Aderknoten, Windmühle, Grab, Altwacht-Tor | zählen für S111 |

## 7. Gegner in der Region

- **Moorlicht-Unken** (überall im Nebel), **Irrlichter** (harmlos, aber locken in Schlamm).
- **Wilderer** und **Wildererhunde** am Birkenhain (N202).
- **Stille Hand** bei Altwacht (M204): Novizen, Schweiger, Choralist; im Nachhall Stummlinge.
- **Moorwinkel** nur im Kampf-Weg: Agathe Dornfeld, Marionetten-Bauern (Fäden trennen, nicht verletzen).
- **Moorkönigin** (N205).

## 8. Wie sich die Region über die Akte verändert

| Akt | Veränderung |
|---|---|
| 2 | wie oben; Frösche-Chor-Fest, Wilderer, Altwacht |
| 3 | E2-Folgen werden sichtbar (Lichtsumpf oder Torfstich); Quendel lehrt Kael in Stelzenheim |
| 4 | Bei `E2_KeyKeepers` kommen die Hüter zur Verteidigung der Akademie (M406), bei Kollegium bleiben sie fern |
| 5 | Quendel singt den Weg in den Nachhall frei (M502), Agathe (bei Versöhnung) spricht mit den Echos |
