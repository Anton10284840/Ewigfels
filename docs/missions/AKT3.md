# Akt 3 – Die Reise (Jahr 3, Herbst 814 bis Sommer 815)

Frage des Akts: **Was ist der Urton wirklich?** · Gefühl: Weite, Abenteuer, Wahrheit, die wehtut, und am Ende Hoffnung.
Neue Regionen: **Tiedmar**, **Weiße Zinnen**, **Kaelschlucht**, dazu die verbotene Stadt **Brandheim**. Neue Reittiere: **Sturmschwinge** (Story, M305), **Dünenschreiter** (Kaelschlucht, Sidequest).
Das Jahr ist ein „Reisejahr“: Nocturna schickt Drittklässler auf Studienreise (offiziell). Edda begleitet am Anfang, danach reist der Spieler mit Lio, und in Tiedmar mit Nell.

| ID | Mission | Dauer | Ort | Kampf | Besonderheit |
|---|---|---|---|---|---|
| M301 | Brandheim 🎬 | 60 min | Brandheim | Stummlinge | Eddas Geständnis, Traum vom Dirigenten |
| M302 | Salz und Nebel | 50 min | Tiedmar, Möwenkai | – | Nell, Lios Eltern |
| M303 | Der Leuchtturm | 55 min | Tiedmar, Leuchtturm | Schmuggler, Stille Hand | Schiff der Stillen Hand |
| M304 | Turnier von Nocturna | 60 min | Nocturna | Duelle | Finale gegen Aurelia |
| M305 | Die Weißen Zinnen | 60 min | Weiße Zinnen | Firnwölfe | Sturmschwinge zähmen |
| M306 | Das Schweigen der Mönche | 45 min | Kloster | – | Entklang |
| M307 | Kaelschlucht | 55 min | Oase Sela, Vor-Kael | Dünengräber, Sandsänger | Klangtürme |
| M308 | Der Wächter von Kael | 60 min | Grab Nymraels | Story-Boss Wächter | Dungeon |
| M309 | Die Wahrheit über den Urton 🎬 | 45 min | Grab, Nachhall | – | Nym zeigt alles |
| M310 | Mara ⚖ | 55 min | Ausgang der Schlucht | Mara + Einheit | Entscheidung 3 |

---

## M301 – Brandheim 🎬

**Ziel:** Die Herkunft des Spielers. Eddas Schuld. Der erste Blick auf den Gegner als Mensch.

**Ablauf**
1. **Reise mit Edda** über Kesselbrunn nach Osten. Wiederaufbau in Kesselbrunn (Ambros' neue Werkstatt, Wims Brief aus der Akademie). Edda redet unterwegs fast nichts. Einmal summt sie das Wiegenlied, bricht ab.
2. **Die Sperrzone:** Alte Kollegiumszäune, verrostete Bannglocken. Dahinter: **Brandheim**. Häuser stehen, aber farblos wie Bleistiftzeichnungen. Kein Ton. Die Schritte klingen, als ginge man auf Watte.
3. **Durch die Stadt:** Markt mit farblosen Äpfeln, ein Karussell, das sich lautlos dreht, Wäsche an Leinen. Mit *Seelenecho* hört man Bruchstücke vom letzten Abend: Kinder, die lachen, ein Hund, ein Streit über den Preis von Kerzen. Dann nichts.
4. **Stummlinge** (dünner Ort): Sie saugen den Ton aus der Umgebung. Kampf oder Schleichen durch Stille.
5. **Das Labor** auf dem Marktplatz: Ein Kreis aus geborstenen Bannglocken. Hier gesteht Edda (Dialog D_M301_Edda, Text variiert):
   - `E1_TrustEdda`: Sie erzählt alles, ruhig, ohne sich zu schonen. Dass sie das Experiment leitete. Dass Ruprecht es befahl. Dass sie einen Säugling aus einer Werkstatt trug und ihn Ambros gab. „Ich habe dich nicht gerettet. Ich habe dich übrig gelassen.“
   - `E1_HidePower`: Sie erzählt es, aber der Spieler hat den Brief von T. V. und kann sie mit Lücken konfrontieren. Es wird ein Streit. Sie gibt alles zu, weil sie nicht mehr kann.
6. **Die Werkstatt:** Ein Instrumentenbauer-Laden. An der Wand ist eine Melodie aus sechs Tönen eingeschnitzt: **das Wiegenlied**. Ein halbfertiges Bandoneon ohne Quarz. (Ambros hat den Quarz. Payoff Akt 4.)
7. **Nacht im Lager, Traum (Cutscene):** Der Spieler steht im Traum-Brandheim, in Farbe, lebendig, am letzten Abend. Ein Mann in Schwarz steht auf dem Marktplatz, sieht ihn an. **Theodric Vaal.** Höflich: „Sie waren ein Jahr alt. Ich war neunzehn. Wir haben beide alles verloren, aber nur einer von uns weiß, wer schuld ist. Ich lade Sie nicht ein, sich mir anzuschließen. Ich lade Sie ein, zuzuhören.“ Er hebt den Taktstock, die Farbe läuft aus der Stadt.

**Gegner:** Stummlinge, Echohunde.
**Rätsel:** Stille-Passagen (keinen Ton machen: nur schleichen, Weisen verraten dich), Seelenecho-Spuren zur Werkstatt.
**Dialoge:** D_M301_Edda (zwei Fassungen), D_M301_Vaal_Traum.
**Cutscenes:** Ankunft am Zaun (30 s), Werkstatt (40 s), Traum (90 s).
**Belohnung:** 600 EP, Werkstattschild (Turmzimmer), `flag:Knows_Origin=1`, `rel:Edda` ±15 je nach Gespräch, `flag:NachhallLoad+5`.
**Haken:** Edda bleibt in Kesselbrunn. „Den Rest des Weges gehst du ohne mich. Das ist richtig so.“

## M302 – Salz und Nebel

**Ziel:** Atem holen in einer lebendigen Stadt. Lio zu Hause. Ein neuer Fall.

**Ablauf**
1. **Ankunft in Tiedmar:** Hafenstadt an der Steilküste, Möwen, Shanty-Gesang, Zunftfahnen. Lio führt durch **Möwenkai** wie ein Fremdenführer und übertreibt alles.
2. **Lios Eltern:** Netzmacher, laut und liebevoll. Abendessen mit Fischsuppe. Niemand sagt Maras Namen, bis die Mutter einen Teller zu viel deckt. Lio entscheidet, ob er den Eltern von Mara erzählt (Spieler kann ihn bestärken oder bremsen, `rel:Lio`).
3. **Nell Tiedemann**, Hafenkommissarin, kalte Pfeife im Mund, braucht einen Wirker: Sieben Wirker sind in drei Monaten verschwunden. Einer ist ihr Bruder (das sagt sie erst in der Questreihe).
4. **Ermittlung:** Tatort Pier 9 mit *Seelenecho*, Zeugin befragen (Fischhändlerin Olga, lügt aus Angst), Spur zu den Schmugglern in **Ebbgrund** (Gruseldorf der Region, Teaser).
5. Ergebnis: Ein Teil der Verschwundenen wurde auf ein schwarzes Schiff gebracht, die *Schweigende Möwe*. Sie ankert nachts beim alten Leuchtturm.

**Gegner:** keine (optional Taschendiebe). **Rätsel:** Zeugenbefragung (Widersprüche finden), Seelenecho.
**Dialoge:** D_M302_Nell, D_M302_Lio_Eltern, D_M302_Olga.
**Belohnung:** 450 EP, Tiedmar-Schnellreise, Questreihe „Der Fall Salzwind“ startet, `rep:Zuenfte+10`.
**Haken:** Nell: „Zwei der Verschwundenen sind nicht auf dem Schiff. Die hat jemand anders gekauft.“ (Questreihe Salzwind.)

## M303 – Der Leuchtturm

**Ziel:** Action an der Küste, die Spur führt zur Kaelschlucht.

**Ablauf**
1. **Nachts zum Leuchtturm** mit Nell und Lio. Die Windglocke oben schweigt, das ist das Signal der Stillen Hand.
2. **Aufstieg** durch den Turm, Schmuggler im Sold der Stillen Hand. Oben: die Windglocke wieder zum Klingen bringen (Strom-Rätsel), damit die Hafenwache kommt.
3. **Auf das Schiff:** Mit Gischtrobbe oder Ruderboot zur *Schweigenden Möwe*. Unter Deck: drei gefangene Wirker in Taktstein-Trance. Befreien.
4. **Der Kapitän** (Choralist Benedikt Raue) flieht, verliert seine Seekarte: Kurs entlang der Küste, Ziel **Kaelschlucht, Grab der Nymrael**. Notiz: „Den Schlüssel finden, bevor das Kind ihn findet.“
5. Das Schiff brennt (Lio war das, „aus Versehen“).

**Gegner:** Klingenschnapper, Schmugglerwirker, Novizen, Choralist Raue (flieht nach Phase 1).
**Rätsel:** Windglocke stimmen, Laderaum-Schleichen.
**Dialoge:** D_M303_Nell, D_M303_Raue.
**Belohnung:** 500 EP, Seekarte, `rep:Zuenfte+5`.
**Haken:** Ein Notenfalter aus Nocturna: „Turnier in zwei Wochen. Ihr seid gemeldet. Ob ihr wollt oder nicht. Grell.“

## M304 – Turnier von Nocturna

**Ziel:** Heimkommen, glänzen, Aurelia als Mensch sehen.

**Ablauf**
1. **Das große Duellturnier:** Gäste aus ganz Velmark, Buden, Wettbüro (Fenja), die Stillen Wächter als Ordner. Die Akademie fühlt sich für einen Tag wieder an wie in Akt 1.
2. **Drei Runden** gegen Gegner mit eigener Taktik (aus der Questreihe „Vom Neuling zum Champion“): Jonte (liest während des Duells, kontert alles), Greta (Nahkampf mit Glutweisen), ein Gast aus Aurenburg (Kollegiums-Kadett, Bannweisen).
3. **Grell** als Richter. In der Pause erzählt er am Rand der Arena: „Ich hab mal einen Befehl befolgt. An einer Sperrzone. Ich ließ niemanden raus. Ich hab gehört, wie sie drinnen leiser wurden. Den Befehl befolge ich nie wieder.“
4. **Finale gegen Aurelia:** Das beste Duell des Spiels. Sie kämpft perfekt. Wenn sie verliert, wirft sie den Handschuh nicht weg, sie gibt dem Spieler die Hand. Wenn sie gewinnt, sagt sie: „Du hast dich zurückgehalten. Tu das nie wieder bei mir.“
5. **Abends auf der Mauer:** Erstes ehrliches Gespräch mit Aurelia. Über ihren Vater, über Brandheim (sie kennt nur die offizielle Version), über Angst. Wenn `rel:Aurelia>=40`: Sie erzählt, dass sie einen Bruder hätte haben können, „aber da gibt es nur eine Lücke im Stammbaum“.

**Gegner:** 4 Duellanten. **Rätsel:** keins.
**Dialoge:** D_M304_Grell, D_M304_Aurelia_Mauer.
**Belohnung:** 500 EP, Turnier-Titel (Kosmetik: Schärpe), bei Sieg Ring „Turniersieger“, `rel:Aurelia+10`.
**Haken:** Varga zur Lehrerschaft (belauscht über die Lauscher): „Das Kollegium will das Kind. Ich gebe ihnen das Kind nicht. Aber ich werde nicht für das Kind sterben.“

## M305 – Die Weißen Zinnen

**Ziel:** Bergabenteuer, das erste Flug-Reittier.

**Ablauf**
1. **Firnhütten:** Hirtendorf, Käserei, Schlittenrennen (optional). Lio und der Spieler kaufen Fellmäntel.
2. **Aufstieg** zum Kloster: Kletterpassagen, Eisbrücken (*Reifpfad*), Schneesturm (Sicht 10 m, Orientierung über Glockenklang des Klosters).
3. **Firnwölfe** greifen im Rudel an. Lawine: Lio löst sie aus Versehen aus, der Spieler muss sie mit *Sturmweise* ablenken oder Lio retten.
4. **Die Sturmschwinge:** In einer Felsnische liegt eine verletzte Sturmschwinge (Pfeil eines Wilderers im Flügel). Sie ist stolz und scheu. **Zähmen** (Minispiel aus CHARACTER_BIBLE §7): sich langsam nähern (summen), die zerzausten Federn ordnen (Rhythmus-Streichen), den Pfeil ziehen (Atem anhalten). Sie folgt nur dem, der ihr die Federn ordnet.
5. **Erster Flug** (Steuerung freigeschaltet): über die Gletscher zum Kloster. Musik schwillt an, Lio schreit vor Freude auf dem Rücken.

**Gegner:** Firnwölfe (Rudel, 3 Begegnungen), Wilderer (2).
**Rätsel:** Schneesturm-Navigation, Zähmen.
**Dialoge:** D_M305_Lio_Berg.
**Belohnung:** 600 EP, Reittier **Sturmschwinge** (R1), Fellmantel, Weiße-Zinnen-Schnellreise.
**Haken:** Vom Himmel aus sieht der Spieler zum ersten Mal die **Treibenden Inseln** über den Wolken.

## M306 – Das Schweigen der Mönche

**Ziel:** Eine ruhige, spirituelle Mission. Lernen, die Kraft zu halten.

**Ablauf**
1. **Kloster der Schweigemönche:** Niemand spricht. Alles läuft über Gesten und Glocken. Der Spieler muss sich ohne Worte verständigen (Gesten-Rad statt Dialog).
2. **Bruder Stumm**, der Abt, schreibt auf eine Schiefertafel: „Wir halten den Urton seit 600 Jahren unter der Zunge. Als Gebet. Nicht als Waffe.“
3. **Drei Prüfungen:** eine Kerze mit dem Urton löschen, ohne den Raum grau zu machen; eine Stunde reglos im Schnee sitzen (Zeitraffer, Gedanken als Echo-Bilder aus dem bisherigen Spiel); einen Glockenschlag „ungeschehen“ machen.
4. **Entklang (K57):** Die Mönche lehren, den Nachhall aus einem Ort oder Menschen zu ziehen. Ritual: Nachhall-Belastung des Spielers sinkt (`flag:NachhallLoad-10`).
5. **Abschied:** Bruder Stumm spricht ein einziges Wort, das erste seit 40 Jahren: „Kaelschlucht.“

**Gegner:** keine.
**Rätsel:** die drei Prüfungen.
**Dialoge:** D_M306_BruderStumm (Tafeltexte).
**Belohnung:** 400 EP, Kernweise *Entklang*, Mönchskutte (Kosmetik).
**Haken:** Lio hat die Stunde im Schnee nicht durchgehalten und Schneemänner gebaut. Die Mönche haben sie stehen lassen.

## M307 – Kaelschlucht

**Ziel:** Die alte Zivilisation sehen. Wüste nach Schnee: maximaler Kontrast.

**Ablauf**
1. **Flug in die Wüste.** Oase **Sela**: Karawanen, Gewürzmarkt, Tänzer. Gegenpol: **Sandgrab** am Rand, wo Stimmen aus dem Sand flüstern (Region-Quest).
2. **Durch den Canyon:** Der **Dünengräber** hört Schritte. Über Felsen springen, schleichen, Stille halten. Der Dünenschreiter (Reittier) wird hier gesehen (Sidequest zum Zähmen).
3. **Vor-Kael:** Ruinenstadt mit Klangtürmen. Drei Türme müssen gestimmt werden, damit das Tor zum Grab öffnet (Akkord-Rätsel: jeder Turm ein Grundton, der Wind spielt sie).
4. **Kael-Inschriften** sammeln (Questreihe „Stimmen aus Stein“). Eine Runenschildkröte trägt eine ganze Strophe auf dem Panzer.
5. Die Stille Hand ist schon da: Ihr Lager am Grabeingang ist leer, die Leute in Taktstein-Trance gegangen, hinein.

**Gegner:** Sandsänger (Kael-Konstrukte, Fernkampf mit Echos), Dünengräber (Umweltgefahr), Novizen.
**Rätsel:** Klangtürme stimmen, Dünengräber-Schleichen.
**Dialoge:** D_M307_Lio_Wueste, D_M307_Sela_Haendlerin.
**Belohnung:** 500 EP, Sela-Schnellreise, 5 Kael-Inschriften, Dünenschreiter-Spur.
**Haken:** Am Tor steht in Kael: *„Vor anu, thas kor.“* Der Spieler kann es lesen, ohne es gelernt zu haben.

## M308 – Der Wächter von Kael

**Ziel:** Dungeon und Story-Boss 3.

**Ablauf**
1. **Das Grab Nymraels:** Hallen wie das Innere eines Instruments. Gänge aus gespannten Steinsaiten.
2. **Drei Kammern** mit Kael-Rätseln: Echo-Labyrinth (blind, nur Ton), Zeitplatten (Nachhallschritt zwischen zwei Zuständen derselben Kammer), Licht von unten (Spiegel im Nachhall).
3. Die Stille Hand liegt in der Vorkammer, die Leute schlafen in Trance. Der Wächter hat sie nicht getötet, nur „angehalten“.
4. **Story-Boss: Der Wächter von Kael** (Details: [BOSSE.md](BOSSE.md#3-der-waechter-von-kael-akt-3)). Er prüft, ob der Spieler würdig ist, den Urton zu hören. Er spielt Akkorde vor, der Spieler muss mit Gegenakkorden antworten (Kampf + Rhythmus).
5. Der Wächter kniet und öffnet die letzte Kammer. In der Mitte: ein leerer Sarkophag aus Glas.

**Gegner:** Klangwächter, Scherbengolems, Story-Boss Wächter.
**Rätsel:** Echo-Labyrinth, Zeitplatten, Spiegel.
**Belohnung:** 700 EP, Wächterkern (Ring-Material für Ambros), Kodex „Nymrael“.
**Haken:** Der Sarkophag ist leer. Der Spieler hört hinter sich Nyms Stimme, diesmal ohne Echo: „Ich bin nicht hier. Ich war nie hier. Komm.“

## M309 – Die Wahrheit über den Urton 🎬

**Ziel:** Die größte Erkenntnis des Spiels. Hoffnung statt Fluch.

**Ablauf**
1. **Nym nimmt den Spieler in den Nachhall** (sicher, sie hält ihn). Licht von unten, Vor-Kael schwebt in Stücken.
2. **Die Vision (Cutscene, spielbar):** Der Spieler sieht das Verstummen vor 2000 Jahren durch Nymraels Augen. Ihr Kind ist gestorben. Sie spielt den Urton, um den Tod ungeschehen zu machen. Der Ton bricht. Die Kael fallen in den Nachhall, das Becken verdorrt.
3. **Die Wahrheit:** Nym erklärt: Der Urton löscht nicht. Er **stimmt um**. Sie hat ihn falsch gespielt, voller Angst, gegen etwas. Wer ihn **für** etwas spielt, kann die Welt neu stimmen, ohne etwas wegzunehmen. Darum brauchte sie einen Menschen, der beides in sich trägt: Urton und eine Melodie, an der er hängt.
4. **Das Geständnis:** Nym hat den Spieler „vorhergehört“. In der Nacht von Brandheim hat sie den brechenden Urton zu einem Säugling gelenkt, damit er nicht alles auslöscht. „Du warst das Einzige, das nicht stumm wurde. Du hast gesungen. Ein Lied aus sechs Tönen.“
5. **Der Spieler ist kein Fehler, sondern eine Stimmgabel.** Dialogwahl: wütend sein, dankbar sein, schweigen (beeinflusst `rel:Nym`).
6. **Zurück in der Welt:** Lio hat drei Stunden am Sarkophag gewartet und Proviant gegessen. „Und? Bist du jetzt ein Gott?“

**Gegner:** keine.
**Dialoge:** D_M309_Nym (der längste Nym-Dialog), D_M309_Lio.
**Cutscenes:** Das Verstummen (120 s), Brandheim aus Nyms Sicht (60 s).
**Belohnung:** 600 EP, Kernweise *Rückklang* (K58), `flag:Knows_Retune=1`, `rel:Nym` je nach Wahl.
**Haken:** Als sie das Grab verlassen, ist der Canyon voller schwarzer Gestalten. In der Mitte, ohne Maske: Mara.

## M310 – Mara ⚖

**Ziel:** Die zweite große Entscheidung über einen Menschen. Lios Schicksal hängt daran.

**Ablauf**
1. **Konfrontation:** Mara hat Befehl, den Spieler zum Dirigenten zu bringen. Ihre Einheit ist müde, verletzt, halb verdurstet. Ein Novize ist ein Kind (14).
2. **Kampf** gegen die Einheit, dann gegen Mara (kürzer als M209, sie ist erschöpft). Nach dem Kampf liegt sie im Sand.
3. **Lio** stellt sich zwischen sie und den Spieler. Mara erzählt endlich die Wahrheit: Sie lief nicht weg wegen des Streits mit Lio. Ihre Magie hat damals ein Boot versenkt, zwei Fischer ertranken. Sie hat sich geschämt. Der Dirigent hat ihr gesagt, dass es nicht ihre Schuld war, sondern die der Magie.
4. **Entscheidung 3:**
   - `E3_MaraKilled`: Der Spieler beendet es. Lio sagt kein Wort, nimmt den Netzknoten von ihrer Strähne und geht allein zur Oase. In Akt 4 verlässt er den Spieler (und kann in M406 sterben).
   - `E3_MaraPrisoner`: Mara wird an die Hafenwache/das Kollegium übergeben. Lio ist erleichtert und gebrochen zugleich. In Akt 4 wird sie im Archiv verhört und flieht.
   - `E3_MaraRedeemed`: Den Kampf beenden, die Einheit gehen lassen, Mara die Hand hinhalten. **Erfolg nur, wenn** Lio-Questreihe „Lied für Lio“ mindestens Teil 4 erreicht hat **oder** `rel:Lio>=60`: Lio spielt auf der Mundharmonika das Lied, das er jahrelang geübt hat. Mara kennt es, es ist das Lied, das sie ihm beigebracht hat. Sie bricht. Ohne diese Bedingung flieht Mara (zählt wie `E3_MaraPrisoner`, nur ohne Kollegium: sie taucht in M407 wieder auf).
5. **Rückreise nach Nocturna** auf der Sturmschwinge, Lio hinten drauf, still oder schlafend oder (bei Redeemed) mit Mara auf dem Dünenschreiter daneben.

**Gegner:** Novizen, Taktgeber, Mara (geschwächt).
**Dialoge:** D_M310_Mara, D_M310_Lio (drei Fassungen).
**Belohnung:** 700 EP, `rel:Lio` (−40 bis +20), `rel:Mara`, Ende Akt 3.
**Haken Akt 4:** Bei der Landung in Nocturna wartet ein Bote des Kollegiums mit Siegel: Einladung nach Aurenburg. Daneben Corvin, lächelnd: „Willkommen zu Hause. Wir haben dich vermisst.“
