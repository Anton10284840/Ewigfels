# BESTIARIUM – Gegner, Bosse und Reittiere

Stand: 27.11.2026 · Version 1.0 · baut auf [GDD §5](GDD.md#5-kampf) (Gegnerliste), [CHARACTER_BIBLE §6–7](CHARACTER_BIBLE.md#6-kreaturen-20-inkl-reittier) (Kreaturen, Reittiere) und [STORY_BIBLE §6.1](STORY_BIBLE.md#61-missklinger-schwarzmagier) (Missklinger) auf.
Daten: `Content/Data/Creatures/Design/Bestiarium.json` (Design-Werte aller Gegner und Bosse) und `Content/Data/Creatures/Design/Reittiere.json`. Kampfwerte für die Engine (Leben, Tempo, Weisen) stehen getrennt im Format von Phase 3 (`Content/Data/Creatures/Phase3_Gegner.json`, docs/SPELL_FORMAT.md) und werden pro Akt aus diesen Design-Daten abgeleitet. Die IDs sind dieselben.
Story-Bosse: [missions/BOSSE.md](missions/BOSSE.md).

---

## 1. Grusel-Regeln (gelten für alle dunklen Gegner)

Nocturna ist verspielt. Darum muss der Grusel **echt** sein, sonst wirkt er wie Kostüm. Unsere Welt ist aus Klang gemacht, also ist unser Grusel vor allem **Klang, der fehlt oder falsch ist**.

1. **Hören vor Sehen.** Jeder dunkle Gegner kündigt sich zuerst über den Ton an (Tabelle „Erkennung“). Wer gut zuhört, ist vorgewarnt. Kopfhörer lohnen sich.
2. **Stille ist die Warnung.** Bei Stille-Hand- und Nachhall-Gegnern wird zuerst die Umgebung leiser: Vögel, Wind, Musik dünnen aus. Das ist unheimlicher als jedes Brüllen.
3. **Schief ist böse.** Missklinger klingen verstimmt: Ihre Weisen sind einen Viertelton daneben, ihre Melodien kippen. Das Ohr merkt es, bevor der Kopf es versteht.
4. **Kein Schreck ohne Vorwarnung.** Keine billigen Jumpscares. Grusel entsteht aus Erwartung: Die Puppe hat sich bewegt, als man nicht hinsah. Ausnahme: maximal ein echter Schreckmoment pro Gruseldorf, gut vorbereitet.
5. **Menschen, keine Monster.** Missklinger und Stille Hand sind Menschen mit Gründen. Viele ergeben sich bei wenig Leben (`surrender`), manche kann man befreien oder überreden. Nur Nachhall-Wesen und Konstrukte kämpfen bis zum Ende.
6. **Opfer sind keine Gegner.** Marionetten-Dorfbewohner, Bergleute unter Taktstein-Zwang und verzauberte Tiere werden **nicht** getötet: Man löst den Zauber (Faden kappen, Stein zerschlagen, Wiegenlied). Wer es trotzdem tut, verliert Ruf und das Dorf erinnert sich.
7. **Grusel-Regler** in den Optionen (Barrierefreiheit): „Voll“, „Mild“ (keine Gesichter im Rauch, Puppen ohne Augen, Echos leiser), „Ruhig“ (Grusel nur angedeutet). Das Gameplay bleibt gleich.

**Klang-Erkennungszeichen der vier dunklen Gruppen**

| Gruppe | Man hört zuerst … | Man sieht zuerst … |
|---|---|---|
| Stille Hand | ein Metronom-Ticken, dann verstummen Vögel im Umkreis | schwarze Handschuhe, synchrone Schritte |
| Missklinger | eine bekannte Melodie, die schief wird (Kinderlied, Tischlied) | Fäden, Puppen, verriegelte Fenster, zu breite Lächeln |
| Nachhall-Wesen | Töne, die rückwärts laufen; das eigene Echo kommt zu spät | Farbe verschwindet, Licht kommt von unten |
| Kael-Konstrukte | tiefes Summen in Stein, Saiten, die sich spannen | Glas und Stein, der atmet |

---

## 2. Gegnertypen (30)

Spalten: **Akt** = ab wann · **Verhalten** = was die KI tut (StateTree, GDD §5) · **Schwäche** = Grundton oder Taktik · **Grusel** = der Moment, den der Spieler sich merkt.

### 2.1 Stille Hand (Orden, Akt 1–5)

Ziel: Magie verstummen lassen. Kämpfen diszipliniert, im Takt, gemeinsam. Waffen: Stäbe, Klingen, **Taktsteine** (schwarze Steine mit Metronomzeiger, verstummen Weisen).

| ID | Name | Akt | Aussehen | Verhalten | Fähigkeiten | Schwäche | Grusel |
|---|---|---|---|---|---|---|---|
| `Novize` | Novize | 1 | graue Kutte, Kapuze, schwarzer Handschuh rechts, Gesicht halb verhüllt | Nahkampf in Gruppen, greift auf Befehl des Taktgebers an, ergibt sich bei 15 % Leben | Hieb, Ansturm, wirft Taktstein-Splitter (verstummt 2 s) | Hall (Glockenschild-Konter), allein schwach | Sie sprechen nie. Sie zählen. |
| `Schweiger` | Schweiger | 2 | schwarze Kutte, Mund mit Leder verbunden, Arme voller Taktstein-Ringe | Nahkampf, sucht den Wirker mit der meisten Magie (Spieler), drängt in die Ecke | **Verstummen-Griff** (packt, die nächste Weise schlägt fehl), Schweigekegel, Rammstoß | Strom (springt über die Ringe), Abstand halten | Im Umkreis von 8 m wird der Ton dumpf, auch die Musik |
| `Taktgeber` | Taktgeber | 2 | Mantel mit Metronom auf der Brust, Taktstock | hält Abstand, **synchronisiert** alle Verbündeten: sie greifen exakt gleichzeitig an | Taktbefehl (4 Schläge, auf 4 alle), Pfeilnote, Takt-Schild für Verbündete | laute Weise auf Schlag 3 bringt die ganze Gruppe aus dem Takt; zuerst ausschalten | Das Ticken wird lauter, je näher man kommt |
| `Choralist` | Choralist | 2 | weiße Halbmaske ohne Mund, langer Talar | hinten, singt Verbündete stärker, flieht vor Nahkampf | Stärkungschor (+30 % Schaden Verbündete), Stiller Chor (Feld, in dem Weisen doppelt Atem kosten), Heilsummen | Unterbrechen (jeder Treffer bricht den Gesang), Glut | Sein Gesang ist schön. Das macht es schlimmer. |
| `Dirigentenwache` | Dirigentenwache | 3 | schwarze Plattenrüstung mit Notenlinien, Turmschild aus Taktstein | Elite, schützt Choralisten, blockt Weisen frontal | Schildwall (frontal immun), Schildstoß, Verstummen-Welle (Kreis 6 m) | von hinten, Leichtklang hebt das Schild an, Grund-Weisen brechen den Stand | Steht still, bis man sich bewegt |

### 2.2 Missklinger (Schwarzmagier, Gruseldörfer, Akt 1–5)

Wirker, die Weisen absichtlich **verstimmen**. Sie herrschen durch Angst über abgelegene Dörfer. Erkennungszeichen: Eine Melodie, die schief wird. Jeder Gruseldorf-Herrscher ist eine Variante dieser fünf Typen (Liste §3).

| ID | Name | Akt | Aussehen | Verhalten | Fähigkeiten | Schwäche | Grusel |
|---|---|---|---|---|---|---|---|
| `Fluchsaenger` | Fluchsänger | 1 | dürr, hohe Stimme, Mund zu breit, Kleider aus verblichenen Notenblättern | Fernkampf, hält Abstand, flieht bei Nähe, verflucht dann aus dem Versteck | **Stolperfluch** (Spieler stolpert beim nächsten Ausweichen), Fluchkugel, Fluchkreis (Boden: langsamer) | Glut (Notenblätter brennen), Hall (Fluch zurückspiegeln) | Singt das Kinderlied aus dem Dorf. Falsch. |
| `Knochenpfeifer` | Knochenpfeifer | 1 | Federmantel aus Krähenfedern, Flöte aus Knochen, Vogelschädel-Maske | Beschwörer, bleibt hinten, ruft Schwärme | **Skelettkrähen** (3–6, Schwarm, picken), Pfeifenstoß (Betäubung), Krähenwand | Strom (Schwarm platzt), Flöte zerstören (Schwachpunkt am Gürtel) | Man hört die Flöte, bevor man ihn sieht. Die Krähen landen erst um dich herum. |
| `Schattenweber` | Schattenweber | 2 | unsichtbar bis zum Angriff, dann: Gestalt aus gewebten Schatten mit Nadelfingern | Lauerjäger: unsichtbar, schleicht, greift von hinten an, verschwindet wieder | Webstich (Hinterhalt, hoher Schaden), Schattenfaden (fesselt 1,5 s), Lichtfraß (löscht Laternen) | *Laternenlicht*, Leuchtfarbe (*Farbtausch* auf Staub macht ihn sichtbar), Spurklang | Die Laternen gehen eine nach der anderen aus. Hinter dir. |
| `Puppenmeister` | Puppenmeister | 1 | elegant, lange Finger mit Fadenringen, Lächeln wie gemalt | steuert 2–4 **Marionetten** (verzauberte Dorfbewohner), versteckt sich hinter ihnen | Fadenzug (Marionette greift an), Fadennetz, Tausch (tauscht Platz mit einer Marionette) | Fäden kappen (Glut/Strom auf die Fäden über den Köpfen), Marionetten nicht verletzen! | Die Marionetten flüstern „Hilf mir“, während sie angreifen |
| `Hexenmutter` | Hexenmutter | 2 | uralt, krumm, Kleid aus Moos und Fäden, Puppen am Gürtel | Elite, legt Fluchkreise, ruft Missklinger und Kreaturen, teleportiert zwischen Kreisen | **Fluchkreis** (Bodenfläche, je nach Farbe: Gift, Stille, Wurzeln), Puppenzauber (Puppe = Spielerkopie, die angreift), Kesselruf | Kreise mit *Entklang* oder Gegen-Grundton löschen, dann ist sie verwundbar | Sie hat eine Puppe, die aussieht wie du |

### 2.3 Wilderer und Schmuggler (Akt 2–4)

| ID | Name | Akt | Verhalten | Fähigkeiten | Schwäche | Besonderes |
|---|---|---|---|---|---|---|
| `Klingenschnapper` | Klingenschnapper | 2 | schnell, flankiert, weicht aus | Doppelmesser-Kombo, Wurfmesser, Rückzug in Rauch | Haltelied (verlangsamt), Reinklang | ergibt sich oft, verrät dann den Lagerplatz |
| `Fallensteller` | Fallensteller | 2 | legt Netze, Öl, Bärenfallen, lockt in Fallen | Netzwurf, Ölfass (+Glut = Feuer), Bärenfalle | Spurklang zeigt Fallen, Öl gegen ihn selbst | seine Fallen kann man umdrehen |
| `Schmugglerwirker` | Schmugglerwirker | 3 | Fernkampf mit Elementarweisen, wechselt Element | Glut/Frost/Strom je nach Fass am Gürtel | Fass treffen (explodiert in seinem Element) | handelt mit gestohlenen Kael-Quarzen |
| `Wildererhund` | Wildererhund | 2 | Rudel mit Wilderer, beißt, hält fest | Biss + Festhalten, Heulen (ruft Wilderer) | Wiegenlied beruhigt ihn (wird friedlich!), Lockruf lenkt ab | beruhigte Hunde laufen zum Schutzgebiet (Questreihe Arche) |

### 2.4 Kollegium-Vollstrecker (nur bei schlechtem Ruf oder Story, Akt 2–5)

| ID | Name | Verhalten | Fähigkeiten | Schwäche |
|---|---|---|---|---|
| `Ediktwache` | Ediktwache | Formation, Hellebarde und Schild, ruft Verstärkung | Hellebardenstoß (Reichweite), Schildwall, Festnahme (Griff, bei Erfolg: Gefängnis statt Tod) | Formation sprengen (Windstoß, Prellschlag) |
| `Bannmagier` | Bannmagier | Fernkampf hinter Wachen, schützt sie | **Bannschild** (blockt Weisen eines Grundtons, wechselt alle 10 s), Bannglocke (Weisen 3 s aus), Fesselkanon | Grundton wechseln, wenn sein Schild wechselt; Urweisen brechen jeden Bann (kostet Ruf) |

### 2.5 Wilde Kreaturen (Akt 1–5)

Nicht böse, nur wild. Viele kann man beruhigen statt besiegen (Wiegenlied, Lockruf, Futter).

| ID | Name | Akt | Wo | Verhalten | Fähigkeiten | Friedlicher Weg |
|---|---|---|---|---|---|---|
| `Pilzbrut` | Pilzbrut (Schwarm) | 1 | Raunwald | Schwarm aus faustgroßen Pilzkriechern, frisst Vorräte, greift in Wellen an | Sporenwolke (Husten = Weisen verzögert), Anspringen, Mutter platzt in Brut | Glut zerstört Sporen; Feuerkreis hält sie fern |
| `Borkenbaer` | Borkenbär | 1 | Raunwald | territorial, warnt (Brüllen, Boden schlagen), greift erst nach 3 s an | Prankenhieb, Ansturm, Rindenpanzer (Rücken immun) | rückwärts weggehen; Wiegenlied; Honig |
| `Moorlicht_Unke` | Moorlicht-Unke | 2 | Graumoor | lauert im Nebel, Rückenblasen leuchten wie Irrlichter und locken, springt dann | Lichtlockung (Spieler läuft automatisch 1 s darauf zu, wenn nicht ausgewichen), Zungenpeitsche, Giftblase | Irrlichter mit Frost löschen |
| `Glasnatter` | Glasnatter | 2 | Glastiefen | jagt Wärme, sieht den Spieler nur, wenn er Glut wirkt oder rennt | Versteinerungston (Lähmung 2 s), Biss | still und kalt bleiben (schleichen), Frost-Aura macht dich unsichtbar für sie |
| `Salzkrake_Jung` | Salzkrake (Jungtier) | 3 | Küste Tiedmar | Gezeitentümpel, klaut Glänzendes (auch Ringe!), zieht ins Wasser | Tintenstoß (blind), Armgriff | glänzenden Köder werfen |
| `Firnwolf` | Firnwolf | 3 | Weiße Zinnen | Rudel (3–6), Alpha mit Eisgeweih koordiniert, umzingelt | Eisbiss (verlangsamt), Rudelruf, Frostatem (Alpha) | Alpha besiegen → Rudel flieht; Welpen retten (Questreihe) |

### 2.6 Kael-Konstrukte (Akt 2–5)

Wächter der alten Zivilisation. Reagieren auf Ton, nicht auf Sicht.

| ID | Name | Verhalten | Fähigkeiten | Schwäche |
|---|---|---|---|---|
| `Klangwaechter` | Klangwächter | Golem aus Stein und Saiten, wacht an Kael-Orten, greift nur an, wenn Ton gemacht wird | Saitenschlag, Resonanzstampf (Welle), Echo-Schild | leise bleiben; Kern hinter der Brustplatte, öffnet sich bei Gegenakkord |
| `Scherbengolem` | Scherbengolem | aus Kristallscherben, zerfällt und setzt sich neu zusammen | Splitterhagel, Neu-Zusammensetzen (heilt), Scherbenwirbel | Grund-Weisen (Gewicht) halten die Scherben am Boden |
| `Sandsaenger` | Sandsänger | Fernkampf-Konstrukt in der Wüste, singt Echos | Echoschuss (trifft 1 s später nochmal am selben Ort), Sandwand | sich bewegen (Echo trifft den alten Ort), Quell-Weisen machen Sand schwer |

### 2.7 Nachhall-Wesen (Akt 2–5, die unheimlichsten Gegner)

Wesen aus allem, was ungeschehen gemacht wurde. Licht von unten, Ton rückwärts. Sie haben keine Gründe mehr, nur Hunger nach Klang.

| ID | Name | Akt | Aussehen | Verhalten | Fähigkeiten | Schwäche | Grusel |
|---|---|---|---|---|---|---|---|
| `Stummling` | Stummling | 2 | humanoid, porzellanglatt, kein Gesicht, nur ein runder Mund ohne Ton; Körper aus Scherben verlorener Dinge (Löffel, Spielzeug, Schlüssel) | langsam, kommt näher, wenn man Ton macht; bleibt stehen, wenn man still ist | **Klangsog** (zieht Ton aus der Umgebung, Weisen in 6 m werden schwächer), Umarmung (Griff, Atem sinkt) | Hall-Weisen, Rückklang; Schleichen | Wenn er dich umarmt, hört man ganz leise das, was er verschluckt hat: ein Kinderlachen, einen Löffel, ein Lied |
| `Echohund` | Echohund | 2 | negativfarbener Hund, Konturen flimmern, Bellen läuft rückwärts | jagt Klang, Rudel von 2–3, sehr schnell, verliert Spur bei Stille | Echobiss (trifft zweimal, das zweite Mal 1 s später), Sprung | Echoköder (jagt den Köder), Stillpunkt | Man hört sein Bellen, bevor er bellt |
| `Leerchor` | Leerchor (Schwarm) | 4 | Wolke aus halbdurchsichtigen Mündern, die stumm singen | Schwarm, umhüllt den Spieler, verschluckt Weisen | **Verschlucken** (Weise wirkungslos, kommt als Echo gegen dich zurück), Chor-Druckwelle | Hall-Weisen, Koop-Weisen (mehrstimmig verwirrt ihn) | Wenn er dich umhüllt, hörst du deine eigene Stimme rückwärts |
| `Splitterwandler` | Splitterwandler | 4 | gespaltene Gestalt, eine Hälfte in der Welt, eine im Nachhall | teleportiert zwischen den Ebenen, greift aus dem Nichts an | Ebenenwechsel, Splitterstoß, Doppelgänger (Kopie aus der anderen Ebene) | *Nachhallschritt* folgt ihm; Seelenecho zeigt, wo er auftaucht | Er steht hinter dir im Spiegel, aber nicht im Raum |

---

## 3. Die Herren der Gruseldörfer (10 Missklinger)

Jedes Gruseldorf (STORY_BIBLE §4.1) hat einen Missklinger als Herrscher. Befreit man das Dorf, verwandelt es sich (GDD §14). Jeder hat eine Geschichte, die ihn verständlich macht, und einen Weg ohne Tod.

| Dorf | Herrscher | Typ | Wie das Dorf leidet | Geschichte | Weg ohne Tod | Akt |
|---|---|---|---|---|---|---|
| **Stillwasser** (Ilsensee) | **Witwe Senta Grau** | Fluchsängerin | Bewohner können nur flüstern; wer laut spricht, verliert die Stimme für einen Tag | Ihr Mann ertrank im See, während das Dorf beim Fest so laut sang, dass niemand seine Rufe hörte | Ihr den letzten Ruf ihres Mannes vorspielen (Seelenecho am Ufer) | 1 |
| **Krähenhain** (Raunwald) | **Mauritz Knochenfeld** | Knochenpfeifer | Puppen in allen Bäumen, jede ist ein Dorfbewohner, der „ungehorsam“ war und jetzt als Puppe im Baum schläft | Puppenmacher-Lehrling, den das Dorf als Kind verlacht hat | Seine erste Puppe finden (im Baum über der Kirche) und ihm zurückgeben | 1 |
| **Russgrube** (Kesselbrunn) | **Egon Ruß, der Rauchschmied** | Schattenweber | Der Rauch der Schmelze formt Gesichter, die nachts durch die Fenster schauen; Bergleute arbeiten ohne Pause | Steiger, der bei einem Grubenunglück 12 Mann verlor, weil die Zunft billig gebaut hat | Die alte Zunftakte nach Kesselbrunn bringen (Gerechtigkeit statt Rache) | 1 |
| **Moorwinkel** (Graumoor) | **Agathe Dornfeld** | Hexenmutter | Bauern als Marionetten auf den Feldern, auch nachts | Hüterin, die verstoßen wurde, weil sie mit dem Nachhall sprach | Quendel überzeugen, sie zurückzunehmen (Ruf Hüter ≥ Geschätzt) | 2 |
| **Hohlstollen** (Glastiefen) | **Meister Lidlos** | Puppenmeister | Bergleute blinzeln nicht mehr, Augen trocken und glänzend | Kristallschleifer, der Angst vor der Dunkelheit hat und will, dass niemand je wieder die Augen schließt | Ihm eine Höhlenlaterne schenken, die nie ausgeht (Funkelgrund-Quest) | 2 |
| **Ebbgrund** (Tiedmar) | **Kapitän Jorgen Moor** | Fluchsänger | ertrunkene Glocken läuten bei Ebbe, wer sie hört, läuft ins Wasser | Kapitän, dessen Schiff das Kollegium versenkte, weil es Kael-Quarz schmuggelte | Die versunkene Schiffsglocke heben und ihm zurückgeben | 3 |
| **Frostwacht** (Weiße Zinnen) | **Reifhild** | Schattenweberin | Bewohner stehen still im Eis, lebendig, sie hören alles | Hirtin, deren Kinder in einer Lawine starben; sie hält jetzt alle fest, damit niemand mehr geht | Sie die Lawine aus ihrer Erinnerung noch einmal sehen lassen und loslassen (Entklang) | 3 |
| **Sandgrab** (Kaelschlucht) | **Der Sandflüsterer** | Schattenweber | Stimmen aus dem Sand verraten jedes Geheimnis der Bewohner, alle misstrauen allen | Kael-Gelehrter, der zu lange Echos gehört hat und nicht mehr weiß, welche Stimme seine ist | Seine eigene Stimme finden (Seelenecho in seinem alten Haus) | 3 |
| **Leerfall** (Treibende Inseln) | **Der Umgekehrte** | Hexenmeister (Hexenmutter-Variante) | Schwerkraft ist falsch, Häuser hängen kopfüber, Kinder fallen nach oben | Windnest-Junge, der fliegen wollte und die Insel „umgestimmt“ hat | Ihm auf dem Perlsegler zeigen, wie Fliegen ohne Zwang geht | 4 |
| **Die Senke bei Nacht** (Aurenburg) | **Lorenzo Fadenweis** | Puppenmeister | Nach der Sperrstunde laufen Menschen wie an Fäden durch die Gassen und arbeiten für das Kollegium | Arbeitet heimlich für den Kanzler (Arbeitskräfte nachts) | Beweis an Fink geben (Questreihe Graue Finken) oder Fäden zum Kanzler zurückverfolgen | 4 |

---

## 4. Regionsbosse

Je einer pro Region, am Ende des großen Dungeons. Jeder Boss hat 2–3 Phasen und einen **Weg der Erlösung**, wenn es ein Wesen ist (statt Tod).

| Region | Boss | Phase 1 | Phase 2 | Phase 3 / Ende | Erlösung statt Tod | Akt |
|---|---|---|---|---|---|---|
| Ilsensee | **Die Seeglocke** (belebte versunkene Glocke, Seegrotte) | Schallwellen im Takt der Glockenschläge: Reinklang im Takt | Glockenwels greift ein (Wellen), Plattformen sinken | Riss in der Glocke freilegen, mit *Wiederklang* heilen | Ja: Glocke heilen statt zerschlagen, sie läutet danach jede Vollilse | 1 |
| Raunwald | **Der Wurzelvater** (12 m Baumgeist, krank durch Nachhall) | kranke Ranken brennen (Glut) | Kern freilegen mit *Ziehklang*, Pilzbrut schützt ihn | – | Ja: Nachhall mit *Entklang* (später) oder Wiegenlied + Hüter-Gesang aus dem Kern ziehen | 1 |
| Kesselbrunn | **Schmelzer** (Hüttenkonstrukt in der Mine) | Lava-Kanäle mit Hebeln umleiten | Kühlen mit Frost, Panzer bricht | Kern im Bauch, Rußgesichter lenken ab | Nein (Konstrukt), aber Egon Ruß kann ihn abschalten, wenn Russgrube befreit ist | 1 |
| Graumoor | **Moorkönigin** (riesige Moorlicht-Unke) | im Nebel nur über Ton orten | Irrlichter löschen (Frost) | Zungenangriffe, Sumpf steigt | Ja: bei `E2_KeyKeepers` ist sie nur krank, Quendel singt sie gesund | 2 |
| Glastiefen | **Kristallwal** | Kampf auf schwimmenden Kristallen, Resonanzpunkte treffen | Kael-Minenketten an seinem Körper zerschlagen | – | Ja (empfohlen): Ketten lösen statt Wal verletzen, er singt dann Licht in die Höhle | 2 |
| Tiedmar | **Die Salzmutter** (Riesenkrake im Hafenbecken) | Tentakel binden (Haltelied) | Hafenkräne als Waffen (Leichtklang) | sie sammelt Schiffsglocken: ihr Nest im Wrack zerstören oder ihr eine Glocke schenken | Ja: die versunkene Glocke aus Ebbgrund schenken | 3 |
| Weiße Zinnen | **Firnfürst** (Alpha-Firnwolf + Lawine) | Rudel, Alpha koordiniert | Lawinen auslösen oder ablenken (Sturmweise) | Eisgeweih brechen | Ja: Welpen aus der Questreihe zurückbringen, er zieht ab | 3 |
| Kaelschlucht | **Der Letzte Sänger** (Kael-Konstrukt) | spielt Akkorde vor, Spieler spielt Gegenakkorde | Kanon (zweistimmig) | Stille-Phase (nur Echoköder) | Nein, aber er ergibt sich und lehrt eine Kael-Weise | 3 |
| Treibende Inseln | **Sturmrochen** | Luftkampf auf dem Reittier, Blitzbahnen ausweichen | Wolkenrochen-Schwarm | Sturz durch die Wolkendecke, Sturzflug-Treffer | Ja: Gewitterquarz aus seiner Stirn ziehen, er wird friedlich | 4 |
| Aurenburg | **Großinquisitor Halmar Dräger** (Kollegium) | Bannschilde, wechselt Grundton | Bannglocken-Turm (Glocken stimmen) | Urweisen brechen den letzten Bann (kostet Ruf Kollegium) | Er ergibt sich bei 10 % (nur bei `E5_Uprising` als Pflichtboss) | 4 |

---

## 5. Reittiere (6, alle eigene Erfindungen)

Ein Reittier ist ein Freund, kein Fahrzeug. Jedes hat **Persönlichkeit, Vorlieben und eine Beziehung** zum Spieler. Bessere Beziehung = mehr Ausdauer, neue Tricks, und es kommt schneller, wenn man pfeift.

**Allgemeine Regeln**
- Rufen: Pfiff-Rad (Taste halten), das Tier kommt aus der Nähe angerannt/angeflogen. Hat es Angst (Nachhall, Gruseldorf), kommt es nicht und man hört es von Weitem schnauben.
- **Werte** (1–5): Tempo, Ausdauer, Wendigkeit, Mut (wie nah es an Gefahr geht).
- **Beziehung** 0–100: füttern (Lieblingsfutter doppelt), putzen (Minispiel am Stall), gemeinsam Rennen, Streicheln nach Gefahr. Stufen: Scheu (0) · Vertraut (30) · Treu (60) · Seelenverwandt (90, Spezial-Trick).
- **Upgrades** (3 Stufen pro Tier) beim Stallmeister: Sattel (Ausdauer), Zaumzeug (Wendigkeit), Rüstung (Mut, weniger Schaden). Optik frei: Fellfarbe/Muster (Farbstoffe), Schmuck, Name.
- **Koop:** Jedes Reittier trägt einen Mitreiter (Gastspieler oder Begleiter). Der Mitreiter kann vom Rücken aus Weisen wirken.
- **Rennen:** pro Region Strecken für Luft, Boden, Wasser. Rivalen mit eigenen Tieren (z. B. Fenja auf einer Gischtrobbe, ein Zinnadler-Reiter aus Firnhütten).
- Kein Reittier im Inneren der Akademie (Ausnahme: Glutdachs darf im Moosgrund-Keller schlafen, wenn Beziehung „Treu“).

| # | Reittier | Akt / Ort | Bewegung | Tempo | Ausd. | Wend. | Mut | Spezialfähigkeit | Seelenverwandt-Trick |
|---|---|---|---|---|---|---|---|---|---|
| R3 | **Moosbock** | 1 · Raunwald | Boden, Klettern | 3 | 4 | 3 | 4 | **Wandlauf:** läuft markierte Steilwände hoch | **Wandelnder Garten:** Pflanzen auf seinem Rücken wachsen (Kräuter ernten unterwegs) |
| R5 | **Gischtrobbe** | 2 · Ilsensee | Wasser, Tauchen | 4 (Wasser) / 1 (Land) | 3 | 5 | 3 | **Wellenritt:** surft auf Wellen, springt über Hindernisse, taucht in Höhlenseen | **Sturmwarnung:** spürt Gewitter 2 Spielstunden vorher (Karten-Symbol) |
| R6 | **Glutdachs** | 2 · Glastiefen | Boden, Graben | 3 | 3 | 2 | 5 | **Graben:** gräbt verschüttete Gänge frei, findet Schätze im Boden | **Glutpelz:** wärmt im Schnee (kein Frieren), Funken vertreiben Glasnattern |
| R1 | **Sturmschwinge** | 3 · Weiße Zinnen (Story M305) | Fliegen | 5 | 3 | 4 | 4 | **Sturzflug-Boost:** aus der Höhe stürzen = Tempo ×2 | **Sturmruf:** fliegt kurz durch Gewitter ohne Schaden |
| R4 | **Dünenschreiter** | 3 · Kaelschlucht | Boden, sehr schnell | 5 | 4 | 2 | 2 | **Sandsprint:** auf Sand dauerhaft Höchsttempo, Dünengräber hören ihn nicht | **Sonnenschirm:** gräbt sich bei Sandsturm mit dem Spieler ein (Unterschlupf, Speicherpunkt) |
| R2 | **Perlsegler** | 4 · Treibende Inseln | Fliegen + Gleiten | 3 | 5 | 3 | 3 | **Ewiges Gleiten:** verbraucht beim Gleiten keine Ausdauer, steigt aber schlecht | **Leuchtquaste:** nachts Licht in 30 m, Stummlinge weichen zurück |

> **Änderung gegenüber CHARACTER_BIBLE §7 (alte Fassung):** Akte angepasst, damit jedes Tier dort erscheint, wo seine Region im Spiel freigeschaltet wird: Perlsegler Akt 4 statt 2 (Treibende Inseln sind erst ab Akt 4 erreichbar), Gischtrobbe Akt 2 am Ilsensee statt Tiedmar (Tiedmar kommt erst in Akt 3), Glutdachs Akt 2 statt 4 (Glastiefen ist Akt 2). Reihenfolge jetzt: 1 Tier in Akt 1, 2 in Akt 2, 2 in Akt 3, 1 in Akt 4.

### 5.1 Die sechs Tiere im Detail

**R3 Moosbock – der gemütliche Kletterer (Akt 1)**
- **Aussehen:** riesiger Bock (Schulterhöhe 1,8 m), dichter Moospelz, kleine Pilze und Farne auf dem Rücken, gedrehte Hörner aus Rinde. Augen bernsteinfarben, schläfrig.
- **Persönlichkeit:** gemütlich, stur, verfressen. Bleibt stehen, wenn er eine Blume sieht, die er mag. Lehnt sich an den Spieler wie eine Katze.
- **Verhalten in der Welt:** grast neben dem Spieler, schüttelt sich nach Regen (Pilze wachsen sichtbar), meckert bei Krähenhain und weigert sich, hineinzugehen.
- **Zähmen (Sidequest N116 „Der Bock mit dem Garten“, Raunwald):** Ein Moosbock frisst seit Wochen die Gärten von Pilzhausen leer. Großmutter Morchel will ihn verjagen. Der Spieler findet heraus, warum: Sein Fels-Hang im Raunwald wurde von Wilderern mit Fallen gespickt. Fallen entschärfen, ihm Wurzelknollen bringen, dreimal neben ihm grasen (warten-Minispiel: man darf sich nicht bewegen), dann darf man aufsteigen. Morchel schenkt einen Sattel aus Pilzleder.
- **Lieblingsfutter:** Waldmeister. **Mag nicht:** Lärm, Feuer.
- **Upgrades:** Kletterklauen (steilere Wände), Gartentaschen (2 Pflanzplätze mehr), Rindenpanzer.

**R5 Gischtrobbe – die verspielte Schwimmerin (Akt 2)**
- **Aussehen:** Robbe so groß wie ein Pony, Flossen wie Segel (halbdurchsichtig, mit Adern), silbergraues Fell mit dunklen Flecken, Schnurrhaare, die bei Wind summen.
- **Persönlichkeit:** verspielt, neugierig, eifersüchtig. Spritzt den Spieler nass, wenn er zu lange mit anderen redet. Bringt Muscheln als Geschenke.
- **Verhalten:** springt aus dem Wasser, wenn der Spieler am Ufer pfeift, rollt sich in der Sonne, jagt Fische, warnt mit Bellen vor Sturm.
- **Zähmen (Sidequest N106 „Die Robbe im Netz“, Ilsensee, ab Akt 2):** Fischer in Uferlicht klagen über eine Robbe, die Netze zerreißt. In Wahrheit befreit sie ihr Junges, das sich immer wieder verfängt. Das Junge befreien, die Fischer überzeugen, ein Robben-Tor ins Netz zu knüpfen (Lio hilft, Netzknoten!), dann ein Wettschwimmen gegen die Mutter gewinnen.
- **Lieblingsfutter:** Ilsensee-Hering. **Mag nicht:** Stille (wird unruhig in Stillwasser).
- **Upgrades:** Segelflossen (schneller), Atemkiemen-Geschirr (länger tauchen), Muschelpanzer.

**R6 Glutdachs – der mutige Gräber (Akt 2)**
- **Aussehen:** ponygroßer Dachs, schwarzes Fell mit glimmenden orangefarbenen Streifen, die bei Aufregung heller leuchten. Funken bei jedem Schritt. Breite Grabkrallen.
- **Persönlichkeit:** mutig bis leichtsinnig, beschützend, grummelig, schnarcht laut. Stellt sich vor den Spieler, wenn Gefahr droht.
- **Verhalten:** schnüffelt nach Schätzen (bleibt stehen und scharrt, wo etwas vergraben ist), wärmt den Spieler bei Kälte, faucht Nachhall-Wesen an.
- **Zähmen (Sidequest im Rahmen von M208, Glastiefen):** Ein Glutdachs ist in einem eingestürzten Kael-Stollen gefangen, die Stille Hand hat ihn als „Wärmequelle“ angekettet. Ketten lösen, ihm einen Weg freisprengen (Prellschlag), dann folgt er dem Spieler bis Funkelgrund und will nicht mehr weg.
- **Lieblingsfutter:** Glutwurzeln (Glastiefen). **Mag nicht:** Wasser (weigert sich zu schwimmen).
- **Upgrades:** Grabkrallen aus Kristall (härterer Fels), Glutgeschirr (Funkenspur verbrennt Fallen), Steinpanzer.

**R1 Sturmschwinge – die stolze Fliegerin (Akt 3)**
- **Aussehen:** Vorderkörper einer Schnee-Eule (weißer Kopf, gelbe Augen, Schwingen 7 m), Hinterkörper eines Luchses (geflecktes Fell, Pinselohren als Federohren), Schwanz mit Federfächer.
- **Persönlichkeit:** stolz, scheu, eitel. Folgt nur dem, der ihr die Federn ordnet. Dreht beleidigt den Kopf um 180 Grad, wenn sie zu lange warten muss.
- **Verhalten:** putzt sich ständig, jagt Zinnadler aus ihrem Revier, landet auf Turmspitzen, hasst Regen.
- **Zähmen:** Story M305 (verletzt in einer Felsnische, Pfeil eines Wilderers).
- **Lieblingsfutter:** Firnforelle. **Mag nicht:** nasse Federn, Lärm beim Landen.
- **Upgrades:** Sturmsattel (Ausdauer), Federhaube (Wendigkeit in Böen), Wolkenrüstung.

**R4 Dünenschreiter – der schnelle Angsthase (Akt 3)**
- **Aussehen:** Laufvogel, 3 m hoch, gepanzerte Schuppenbeine, kleiner Kopf mit großem fächerförmigem Kamm (Sonnenschirm, faltet sich auf), sandfarbenes Gefieder mit roten Spitzen.
- **Persönlichkeit:** schnell, nervös, schreckhaft, unglaublich treu, wenn er einmal vertraut. Rennt bei Gefahr erst weg und kommt dann zurück, schämt sich (senkt den Kamm).
- **Verhalten:** stellt den Kamm in der Mittagssonne auf und spendet dem Spieler Schatten, tanzt (Balztanz), wenn er sich freut.
- **Zähmen (Sidequest in der Kaelschlucht, Oase Sela):** Eine Karawane hat einen jungen Dünenschreiter verloren, er wird von einem Dünengräber gejagt. Den Gräber mit Stille ablenken (Schleichen), dann den Schreiter mit einem Rennen um die Oase überzeugen.
- **Lieblingsfutter:** Dattelkäfer. **Mag nicht:** Dunkelheit, Höhlen.
- **Upgrades:** Sandschuhe (Ausdauer), Kammgeschirr (Wendigkeit), Schuppenpanzer (mehr Mut).

**R2 Perlsegler – der stille Gleiter (Akt 4)**
- **Aussehen:** pferdegroß, langer Hals, vier breite Hautsegel zwischen den Beinen, Fell wie Perlmutt (schimmert je nach Licht rosa bis grün), Leuchtquaste am Schwanz. Summt beim Fliegen.
- **Persönlichkeit:** sanft, träumerisch, weise. Summt Melodien, die er unterwegs gehört hat, nach (auch das Wiegenlied, wenn der Spieler es kennt).
- **Verhalten:** gleitet in Kreisen, schläft im Flug, lässt Windnest-Kinder auf sich klettern.
- **Zähmen (Sidequest in Leerfall/Windnest, Treibende Inseln):** Teil der Befreiung von Leerfall: Der Umgekehrte hat einen Perlsegler „falsch gestimmt“, er fliegt kopfüber und kann nicht landen. Den Fluch lösen (Rückklang), ihn zu Boden singen.
- **Lieblingsfutter:** Wolkenbeeren. **Mag nicht:** Kälte, Eile.
- **Upgrades:** Segelspanner (besseres Steigen), Quastenschmuck (Leuchtweite), Perlmuttrüstung.

### 5.2 Reittiere in der Geschichte

- **Akt 1:** Der Moosbock ist das erste Tier und langsam, aber kann klettern: Er öffnet den Raunwald.
- **Akt 3:** Die Sturmschwinge ist der große Moment (M305). Ab hier ist Velmark von oben erreichbar.
- **Akt 4, M406:** In der Nacht der Glocken rufen alle gezähmten Tiere nach dem Spieler. Wer „Treu“ ist, kommt und kämpft mit (Glutdachs am Tor, Sturmschwinge über dem Glockenturm, Gischtrobbe im See gegen die Boote).
- **Akt 5:** Im Nachhall können Reittiere nicht folgen, außer bei „Seelenverwandt“ (sie erscheinen als Lichtgestalt und tragen den Spieler über eine Lücke in M502).
- **Epilog:** Jedes Tier mit Beziehung ≥ 60 bekommt einen Satz im Epilog (Schutzgebiet-Baustein).
