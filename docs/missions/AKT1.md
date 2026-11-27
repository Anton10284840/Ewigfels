# Akt 1 – Ankunft (Jahr 1, Herbst 812 bis Sommer 813)

Frage des Akts: **Wer bin ich?** · Gefühl: Staunen, Gemütlichkeit, Schabernack, dann der erste Schreck.
Zwischen den Missionen läuft das Schulleben (Unterricht, Glockenball, S101–S118, [AKT1_SCHULLEBEN](../AKT1_SCHULLEBEN.md)). Missionen starten automatisch, sobald die vorige fertig ist, aber erst nach einem **Schlaf** (der Spieler bestimmt das Tempo).
Quest-Daten: `Content/Data/Quests/Akt1/M101.json` … `M110.json`. Dialoge: `Content/Data/Dialogues/Akt1/`.

| ID | Mission | Dauer | Ort | Neue Weisen | Kampf |
|---|---|---|---|---|---|
| M101 | Das Boot über den Ilsensee 🎬 | 45 min | Kesselbrunn, Ilsensee | Funkenschnipp | – |
| M102 | Die Turmwahl | 50 min | Nocturna | Tischlied, Turmmelodie | – |
| M103 | Der erste Ton | 50 min | Nocturna | Stimmgabel, Leichtklang, Glockenschild, 1 Grundform | Übungspuppen |
| M104 | Deine ersten Ringe | 50 min | Kesselbrunn | Jupps Liederheft | – |
| M105 | Das Flüstern im Raunwald | 55 min | Raunwald, Pilzhausen | Wiegenlied, Lockruf | Pilzbrut |
| M106 | Ilsenlicht 🎬 | 55 min | Ilsensee, Uferlicht | Festweisen | Duell Aurelia |
| M107 | Die Stumme Kammer | 50 min | Nocturna bei Nacht | Klinkenlied | Schleichen |
| M108 | Erwachen 🎬 | 60 min | Raunwald im Winter | Verstummen (unfreiwillig) | Story-Boss Varn |
| M109 | Was hast du getan? | 50 min | Nocturna, Ilsensee-Eis | Frostweise-Spiele | Spuren lesen |
| M110 | Die Doppelnacht ⚖ | 55 min | Nocturna | Jahresakkord | – |

Summe ca. 8,6 Std. (nur Hauptmissionen).

---

## M101 – Das Boot über den Ilsensee 🎬

**Ziel:** Den Spieler verzaubern, bevor er irgendetwas tun muss. Charakter-Editor, Bewegung, erstes Schnippen.

**Ablauf**
1. **Kesselbrunn, Waisenhaus, Morgen.** Ein Notenfalter summt durch den Schlafsaal, sechs Kinder jagen ihn über die Betten (Spieler läuft mit: Bewegungs-Tutorial ohne Texttafeln). Er landet auf dem Kissen des Spielers. Hanne Brook, die Heimleiterin, liest den Brief vor, den sie selbst kaum glaubt: eine Einladung nach Nocturna, für ein fünfzehnjähriges Kind, das nie gezaubert hat.
2. **Spiegel im Flur:** Charakter-Editor (der Spiegel ist das Interface, Hanne kommentiert „Die Haare kriegst du da drüben eh nie gekämmt“).
3. **Abschied:** Die Kinder schenken eine Papierkrone, Hanne einen zu großen Wollschal. Sie sagt: „Wer pfeifen kann, kann zaubern. Du pfeifst schief, aber laut.“
4. **Steg am Ilsensee, Abend, Nebel.** Der Fährmann (spricht nur in Wetterberichten) wartet. Auf dem Steg: Laufen, Springen, Ausweichen über nasse Planken.
5. **Lio** sitzt schon im Boot, in einem zu großen Mantel, und hat gewettet, dass der Spieler umkippt (Dialog D_M101_Lio).
6. **Überfahrt (Cutscene + Steuerung):** Der Nebel reißt auf, Nocturna erscheint auf der Klippe, alle Fenster leuchten. Der **Glockenwels** taucht neben dem Boot auf, die versunkene Glocke läutet einmal unter Wasser. Lio wird blass, der Fährmann sagt: „Morgen Ostwind.“
7. **Erstes Schnippen:** Die Bootslaterne geht aus. Lio zeigt, wie man schnippt (*Funkenschnipp*). Beim dritten Versuch springt ein Funke über, die Laterne brennt, **in der Farbe des Spielers** (im Editor gewählt).
8. **Laternenkrähen** begrüßen das Boot, eine klaut die Papierkrone.
9. **Aufstieg zum Tor** (312 Stufen, Lio zählt). Optional Laternen am Pfad entzünden.

**Gegner:** keine. **Rätsel:** keins, nur Staunen.
**Dialoge:** D_M101_Lio (Boot), D_M101_Hanne (Abschied).
**Cutscenes:** Nebel reißt auf (40 s), Glockenwels (15 s).
**Belohnung:** 100 EP, Wollschal (Kosmetik, Hals), Weise *Funkenschnipp*, `rel:Lio+5`.
**Haken:** Am Tor wartet Edda. Sie schaut den Spieler eine Sekunde zu lang an und sagt nur: „Sie sind spät. Wie immer.“ („Wie immer?“ fragt Lio. Edda geht.)

## M102 – Die Turmwahl

**Ziel:** Zugehörigkeit. Der Spieler wählt seinen Turm selbst und hat am Ende ein Zuhause.

**Ablauf**
1. **Willkommensabend, Große Halle.** Speisefluss: Teller schweben in einem Band, Mutter Gundels Küchenchor singt. Alle summen das **Tischlied** (erste Weise ohne Kampf, Rhythmus-Eingabe). Die Stimmglocke summt zufrieden.
2. **Rektorin Varga** begrüßt die Neuen mit dem Satz über dem Tor: „Hier lernt man hören, bevor man spricht.“ Die Glocke brummt bestätigend.
3. **Vier Turmsprecher** (je ein kurzes Gespräch, Reihenfolge frei): Greta (Aschenhain) umarmt zu fest, Jonte (Silberquell) liest beim Reden, Fenja (Sturmkamm) will um alles wetten, Hannes (Moosgrund) verteilt Rosinenschnecken. Jeder stellt eine Frage, die Antwort färbt die Empfehlung, die Wahl bleibt frei.
4. **Aurelia** steht schon fest bei Silberquell und sagt über den Spieler laut genug: „Spätberufene. Das Kollegium nennt das Notaufnahme.“ Lio flüstert: „Ich mag sie jetzt schon nicht.“
5. **Die Wahl:** Der Spieler legt die Hand auf die Glocke seines Turms. Sie gibt einen neuen Ton dazu. `flag:Tower_<Name>`.
6. **Führung über die Notentreppe:** Der Turmsprecher zeigt, wie man die Melodie läuft. Ein falscher Ton macht die Treppe zur Rutsche (gewollt beim ersten Mal, alle lachen).
7. **Turmmelodie** summen, die Tür öffnet sich. **Kessel Klaus** (bzw. sein Bruder im gewählten Turm) begrüßt mit Dialekt.
8. **Turmzimmer:** Blick auf den See. Uniform liegt auf dem Bett (`flag:Look_Uniform`).
9. **Nacht:** Wer aus dem Fenster schaut, sieht **Ottilie**, die Laterne auf Messingbeinen, die singend den Hof abläuft.

**Gegner:** keine. **Rätsel:** Notentreppe (Melodie aus 4 Tönen nachlaufen).
**Dialoge:** D_M102_Turmsprecher (alle vier in einer Datei), D_M102_KesselKlaus.
**Belohnung:** 150 EP, Uniform + Turmschal, Weisen *Tischlied* und *Turmmelodie*, Turmzimmer.
**Haken:** Auf dem Kissen liegt ein Notenfalter ohne Absender: „Willkommen zu Hause.“ (Er ist von Edda. Das erfährt der Spieler in Akt 3.)

## M103 – Der erste Ton

**Ziel:** Das Magiesystem lernen, spielerisch. Erste Kernweise, erster Probier-Moment, erstes Kampf-Tutorial.

**Ablauf**
1. **Klangkunde bei Edda.** Die drei Gesetze („Alles schwingt. Jede Weise braucht Atem, Hände und einen Grundton. Kein Klang verschwindet.“). Minispiel: Stimmgabel anschlagen, Töne den Grundtönen zuordnen.
2. **Leichtklang:** Erst Federn schweben lassen, dann laufen **Ollendorfs Tische** aus der Wandelstube nebenan herein (er hat sich in der Tür geirrt). Mit *Leichtklang* die Tische einfangen und zurücktragen. Ollendorfs Hut wird vor Peinlichkeit zum Blumentopf.
3. **Probier-Modus:** Edda lässt die Klasse „einfach irgendwas“ greifen. Der Spieler kombiniert frei einen Grundton und eine Wirkart und entdeckt seine erste **Grundform** (welche, ist frei). Edda: „Das stand nicht im Lehrplan. Gut.“
4. **Nachmittag, Duellsaal bei Grell.** Glockenschild, Parieren im Takt (Reinklang), Ausweichen. Gegner: Übungspuppen, die mit Sandsäcken werfen. Grell knurrt bei jedem Treffer, lobt nie, nickt einmal.
5. Ein Mitschüler fragt Grell, ob er schon mal „echt“ gekämpft hat. Grell: „An einer Sperrzone. Nächste Frage.“ (Faden Grell/Brandheim.)
6. **Abend, Gemeinschaftsraum:** Lio zeigt seinen ersten Streich (*Kitzelklang* auf Kessel Klaus, der lacht pfeifend, bis er überkocht).

**Gegner:** Übungspuppen (3 Wellen). **Rätsel:** Tische einfangen (Physik mit Leichtklang).
**Dialoge:** D_M103_Edda (Unterricht), D_M103_Grell (Duellsaal), D_M103_Lio (Abend).
**Belohnung:** 200 EP, Weisen *Stimmgabel*, *Leichtklang*, *Glockenschild*, 1 Grundform, Ton für den Turm bei Bestnote.
**Haken:** Beim Probieren flackert die Kerze des Spielers kurz grau. Niemand merkt es außer Edda.

## M104 – Deine ersten Ringe

**Ziel:** Welt öffnen (erster Ausflug, Markt, Läden), Personalisierung zeigen, erster Meister.

**Ablauf**
1. **Bootsausflug** mit Lio nach Kesselbrunn (Freizeit-Erlaubnis von Varga). Der Spieler ist hier aufgewachsen: Leute erkennen ihn, Bäckerin **Hedda** schenkt eine Zimtschnecke, die Waisenkinder rennen hinterher.
2. **Markt-Tutorial:** Kaufen, Verkaufen, Gerüchte. Barbier und Schneiderin zeigen die Personalisierung (Spiegel, Farbstoffe).
3. **Jupp mit dem Akkordeon** spielt auf dem Platz. Er ist der erste **Meister**: Er lehrt nur, wer ihm ein Lied zurückspielt (Rhythmus-Minispiel). Lohn: Liederheft (3 Musikweisen).
4. **Ambros Kettler, Ringschmiede.** Werkstatt voller Quarze und tickender Stimmgeräte. Er klopft gegen die Finger des Spielers wie gegen eine Glocke, misst, summt. Dann **stimmt er den ersten Ring**: Der Spieler wählt Metall und Quarzfarbe, das ist ab jetzt die Farbe seiner Magie.
5. **Der Moment:** Beim Stimmen flackert der Quarz für einen Herzschlag **violett-weiß**. Ambros lässt fast die Zange fallen. Er lacht es weg („Billigquarz aus Tiedmar“) und schenkt dem Spieler einen zweiten Ring „für den Weg“.
6. Während der Spieler den Spiegel testet, spielt Ambros auf seinem Bandoneon eine kurze Melodie aus sechs Tönen, bricht ab, schaut aus dem Fenster. (**Das Wiegenlied.** Wer will, kann fragen, die Antwort: „Altes Lied. Hab vergessen, wie's weitergeht.“)
7. **Rückweg:** Lio kauft heimlich eine Mundharmonika-Zunge für seine Schwester, „falls sie wiederkommt“. Erstes ernstes Gespräch über Mara.

**Gegner:** keine (optional Taschendieb auf dem Markt, Verfolgungsjagd mit *Leichtklang*-Fässern).
**Rätsel:** Jupps Lied nachspielen.
**Dialoge:** D_M104_Ambros, D_M104_Jupp, D_M104_Lio_Mara.
**Belohnung:** 200 EP, 2 Stimmringe, Liederheft, 50 Gold Taschengeld, `rel:Lio+5`.
**Haken:** Nachts schreibt Ambros einen Brief. Wir sehen nur die Anrede: „Edda.“

## M105 – Das Flüstern im Raunwald

**Ziel:** Erster echter Ausflug in die Wildnis, erste Kämpfe, Kreaturen lieben lernen, erster Gruselkontrast.

**Ablauf**
1. **Kreaturenkunde-Ausflug mit Saphira.** Sie summt den Bäumen zu, die Bäume raunen zurück. Weise *Wiegenlied*: einen aufgeregten Borkenbär-Jungen beruhigen.
2. **Glimmermotten-Wiese:** Mit *Lockruf* einen Schwarm Motten zu einem Lichtbild formen (Minispiel: Tonfolge halten). Sehr schöne Stelle für den Fotomodus.
3. **Lio verirrt sich:** Er folgt einem **Nebelfuchs**, der ihn im Kreis führt. Der Spieler folgt Spuren (Spurklang), findet Lio, der zum dritten Mal am selben Pilz vorbeiläuft und schwört, es seien drei verschiedene Pilze.
4. **Pilzhausen:** Häuser in Riesenpilzen, Großmutter **Morchel** bewirtet die Klasse. Kinder zeigen den Mottenreigen.
5. **Erste Kämpfe:** Ein **Pilzbrut**-Schwarm überfällt die Vorratskammer von Pilzhausen. Kampf mit *Glockenschild*, *Funkenschnipp* (Glut zerstört Sporen) und den Grundformen.
6. **Hüter-Begegnung:** Eine Hüterin (Seline, junge Wurzelläuferin) taucht auf, bedankt sich knapp und warnt Saphira: „Die Ader unter dem Hain klingt falsch. Seit dem Frühjahr.“
7. **Waldrand, Rückweg:** Hinter den Bäumen sieht man **Krähenhain**: Puppen in allen Ästen, kein Vogel singt. Eine Puppe dreht langsam den Kopf. Saphira: „Da gehen wir nicht hin. Nicht heute.“ (Region-Quest, siehe Region Raunwald.)

**Gegner:** Pilzbrut (Schwarm, 3 Wellen), Pilzbrut-Mutter (Mini-Boss, platzt in Sporenwolken).
**Rätsel:** Mottenbild, Fuchsspuren.
**Dialoge:** D_M105_Saphira, D_M105_Morchel, D_M105_Lio_Fuchs.
**Belohnung:** 250 EP, *Wiegenlied*, *Lockruf*, Kodex-Einträge (5 Kreaturen), Pilzhausen als Schnellreisepunkt.
**Haken:** In der Nacht träumt der Spieler von einem Wald ohne Farbe. Nur Rauschen, kein Wort.

## M106 – Ilsenlicht 🎬

**Ziel:** Das schönste Fest des Spiels. Danach weiß der Spieler, was auf dem Spiel steht.

**Ablauf**
1. **Herbstfest am See.** Tausende Laternen, Buden aus Uferlicht, Kürbisgrinsen (Kürbisse mit Weisen schnitzen), Festweisen. Die **Stillen Wächter** (Rüstungen) tanzen zur Musik der Kesselbrunner Kapelle.
2. **Laternenregatta:** Jeder Turm schickt ein Laternenboot über den See, gesteuert mit Wind- und Wasserweisen (Rennen, 3 Runden um die Inseln).
3. **Kanzler Ruprecht von Stauffen** hält eine Rede auf dem Steg: Ordnung, Sicherheit, „Magie braucht Hüter, keine Helden“. Aurelia steht hinter ihm und klatscht genau zweimal.
4. **Corvin Hale** ist Teil des Gefolges (Tränke-Gutachter des Kollegiums, weinroter Schal). Er spricht den Spieler an, freundlich, witzig, kennt sich mit Spätberufenen aus, „die sind oft die Interessantesten“. Er bietet ein Glas Funkensaft an. (Spieler kann annehmen oder nicht, ohne Folgen, aber Corvin merkt sich die Antwort: Dialogcheck in M201.)
5. **Aurelia fordert zum Duell** (vor Publikum, nach Festregeln: Glockenschilde, kein Schaden, wer drei Glocken trifft). Sieg oder **faires** Verlieren öffnet später ihre Stilweisen. Wer unfair kämpft (Streichweisen), verliert ihren Respekt (`rel:Aurelia-10`).
6. **Laternen fliegen lassen:** Spieler und Lio lassen gemeinsam eine Laterne steigen. Lio schreibt „Mara“ drauf. Die Laterne des Spielers brennt einen Moment **farblos**. Lio sieht es nicht, er schaut seiner nach.
7. **Nacht, Traum (Cutscene):** Ein schwarzer Raum, Licht kommt von unten. Eine Frau mit goldenen Linien auf der Haut, Haar wie unter Wasser. Sie sagt nichts. Nur ihre Lippen bewegen sich, und ein Echo kommt Sekunden später: „…hörst du mich… mich… mich…“ Aufwachen, Kessel Klaus pfeift die Weckglocke.

**Gegner:** Aurelia (Duell, nicht tödlich).
**Rätsel:** Regatta-Steuerung, Kürbisgrinsen.
**Dialoge:** D_M106_Aurelia_Duell, D_M106_Corvin, D_M106_Ruprecht (Rede, Zuschauer), D_M106_Lio_Laterne.
**Cutscenes:** Laternen steigen (60 s, Trailer-Shot!), Nym-Traum (45 s).
**Belohnung:** 250 EP, Festweisen, Regatta-Preis (Bootswimpel), bei fairem Duell `rel:Aurelia+10`.
**Haken:** Morgens liegt auf der Fensterbank Raureif, obwohl es Herbst ist. Er formt ein Kael-Zeichen: *nym*.

## M107 – Die Stumme Kammer

**Ziel:** Schabernack + Geheimnis. Nachts unterwegs, Regeln brechen, ein Buch, das den Spieler kennt.

**Ablauf**
1. Der Spieler zeichnet das Kael-Zeichen aus dem Raureif ab. Lio: „Sowas steht nur in der Stummen Kammer. Da kommt keiner rein.“ Pause. „Also heute Nacht?“
2. **Nach der Sperrstunde:** An Ottilie vorbeischleichen (sie singt, man hört sie kommen; Licht = gesehen). Die Notentreppe muss leise gelaufen werden.
3. **Flüsterkorridor:** Die Lauscher tratschen über jeden, der vorbeigeht. Lio muss sich auf die Zunge beißen, als ein Lauscher seinen Gesang kritisiert.
4. **Die Schiefen Töne helfen:** Ruven Falk sitzt im Kreuzgang und verlangt einen Gefallen (Brams' Stuhl mit einer *Quietschnote* präparieren), dafür lehrt er das **Klinkenlied**.
5. **Bücherschlund:** In der Bibliothek ein verlorenes Buch zurück ins Regal K stellen (es versteckt sich, Mini-Jagd), Klinkenlied singen, das Regal schluckt die beiden.
6. **Die Stumme Kammer:** Kein Ton dringt hinein, die Schritte sind lautlos. Bücher mit Ketten. Ein Kael-Buch, **das Buch der Vorhörung**, beginnt zu summen, als der Spieler es berührt: im Rhythmus seines Herzschlags. Auf einer Seite steht das Zeichen *nym* und daneben ein Zeichen, das noch niemand übersetzt hat.
7. **Rückweg:** Ottilie erwischt beide auf der Notentreppe und zetert durch drei Stockwerke. Nachsitzen bei Edda: 50 Notenfalter falten.
8. **Nachsitzen:** Edda fragt, was die beiden gesucht haben. Wenn der Spieler das Buch erwähnt, schaut sie lange darauf (sie hat es auf dem Tisch, Herr Staub hat es ihr gebracht), dann schließt sie es. „Manche Bücher lesen zurück.“

**Gegner:** keine (Schleichen: Ottilie, 2 Bibliotheks-Lauscher, Herr Staub auf Nachtrunde).
**Rätsel:** Notentreppe leise, Bücherschlund, verlorenes Buch.
**Dialoge:** D_M107_Lio_Nacht, D_M107_Ruven, D_M107_Edda_Nachsitzen.
**Belohnung:** 250 EP, *Klinkenlied*, Geheimgang 7 und 12, *Notenfalter* (Weise) durchs Nachsitzen.
**Haken:** Wer nach dem Nachsitzen zurück ins Klassenzimmer schleicht, sieht Edda mit dem Buch am Fenster, sie weint. (Optional, Kodex-Eintrag „Was Edda weiß“.)

## M108 – Erwachen 🎬

**Ziel:** Der Stimmungsbruch des Akts. Der Ort, den der Spieler lieben gelernt hat, wird für einen Moment tot.

**Vorbereitung:** Es ist Frühwinter, erster Schnee. **Ausbilder Varn** (Waldläufer, leitet seit Jahren die Winterübung im Raunwald) holt die Klasse ab. Er ist freundlich, aber er blinzelt zu selten und tippt mit dem Finger einen gleichmäßigen Takt gegen seinen Gürtel.

**Ablauf**
1. **Winterübung:** Spurensuche im Schnee, Feuer machen mit Glut-Grundform, Eis prüfen. Varn trennt die Gruppe: Spieler und Lio gehen den „Nordpfad“.
2. Unterwegs wird es still. Kein Vogel. Der Schnee knirscht nicht mehr. Lio macht Witze, die immer leiser werden.
3. **Falle:** Varn wartet an einer Lichtung, wo die Ader unter dem Hain falsch klingt (Hinweis aus M105). Er wirft einen **Taktstein** (schwarzer Stein mit Metronomzeiger) in den Schnee. Ein Verstummen-Feld legt sich über die Lichtung: **Die Weisen des Spielers funktionieren nicht mehr.**
4. **Story-Boss Varn, Phase 1:** Varn kämpft mit Schweigeweisen und einem Holzspeer. Er greift nicht den Spieler an, sondern Lio. Er sagt dabei in fremdem Takt: „Zeig es. Zeig es. Zeig es.“ (Details: [BOSSE.md](BOSSE.md#1-ausbilder-varn-akt-1)).
5. **Phase 2:** Lio wird getroffen und fällt in den Schnee. Der Spieler hat keine Weise, nur Ausweichen und Schild. Das Herz pocht (Rumble/Ton).
6. **Der Bruch (Cutscene):** Die Finger des Spielers krampfen sich zur Faust. Kein Ton. Dann ein tiefer Ton unter allen Tönen. In einem Ring von 200 m verliert der Raunwald seine Farbe. Schnee wird grau, Vögel fallen lautlos von den Ästen und fliegen benommen weiter, das Feuer im Lager erlischt ohne Rauch. Varns Taktstein zerspringt, Varn bricht zusammen.
7. **Stille.** Dann kommt die Farbe langsam zurück, von außen nach innen. Lio hustet, lebt, sieht den Spieler an und sagt nichts. Zum ersten Mal sagt Lio nichts.
8. **Edda** kommt durch den Wald gerannt (sie ist der Klasse gefolgt). Sie sieht den grauen Ring, kniet sich zum Spieler, legt ihm ihren Mantel um und sagt leise: „Nicht sprechen. Das war ich. Hörst du? Das war ich.“

**Gegner:** Varn (Story-Boss), 2 Wellen manipulierte Firnfüchse (vor dem Boss, Augen gleichmäßig blinkend).
**Rätsel:** keins.
**Dialoge:** D_M108_Varn (Kampfrufe), D_M108_Edda_Lichtung.
**Cutscenes:** Verstummen (60 s, Schlüsselszene), Edda kommt (30 s).
**Belohnung:** 400 EP, `flag:Urton_Awakened=1`, `flag:NachhallLoad+10`, Kernweise *Verstummen* (gesperrt, nur in Story-Momenten), Kodex „Der Urton“.
**Haken:** Varn wacht im Krankenflügel auf und erinnert sich an nichts seit dem Sommer.

## M109 – Was hast du getan?

**Ziel:** Folgen spüren, Einsamkeit, dann Freundschaft als Antwort. Und die erste Spur des Feindes.

**Ablauf**
1. **Gerüchte:** Die Lauscher tragen es durch die Akademie. Jedes Mal anders: „Es hat den Wald verbrannt.“ „Es hat Varn verflucht.“ „Es kann Leute verschwinden lassen.“ Die **Stimmglocke klingt schief**, sobald der Spieler die Halle betritt. Mitschüler weichen aus, Aurelia setzt sich demonstrativ an einen anderen Tisch.
2. **Edda deckt den Spieler:** Offiziell war es ein „Aderbruch“, den Edda gebannt hat. Varga glaubt es nicht und lässt es trotzdem so stehen.
3. **Lio** meidet den Spieler zwei Tage. Dann klopft er nachts ans Turmzimmer, mit zwei Tassen Kakao (Schaumtier obenauf): „Du hast mich gerettet. Ich hab nur nicht gewusst, wie man danke sagt zu jemandem, der den Wald grau macht.“ (Wichtigster Lio-Dialog des Akts.)
4. **Firnnacht:** Die Freunde holen den Spieler zurück: Schneeballschlacht der Türme mit Frostweisen, Eisfiguren-Wettbewerb (Panki baut einen Eisdrachen, der sofort umfällt), **Eis-Glockenball** auf dem See, Kakao am Kamin. Wer mitmacht, hört die Stimmglocke wieder normal klingen.
5. **Spur:** Mit Lio und Mirela in den Krankenflügel schleichen. In Varns Sachen: ein **schwarzer Handschuh** (Zeichen der Stillen Hand) und ein Splitter des Taktsteins mit einem winzigen Prüfsiegel des Kollegiums (Laborzeichen, das in Akt 4 Corvin zugeordnet wird).
6. **Grell** erwischt sie. Statt Strafe sieht er den Handschuh, wird grau im Gesicht und nimmt ihn an sich: „Das bleibt unter uns. Ich kenne diese Leute.“

**Gegner:** keine (Firnnacht-Spiele).
**Rätsel:** Krankenflügel-Schleichen, Taktstein-Splitter untersuchen (Stimmgabel zeigt den gleichmäßigen Takt).
**Dialoge:** D_M109_Lio_Kakao, D_M109_Grell_Handschuh, D_M109_Mirela.
**Belohnung:** 300 EP, Winterumhang, `rel:Lio+15`, `flag:Found_BlackGlove=1`.
**Haken:** Ein Notenfalter ohne Absender, schwarz gefaltet: „Sie haben etwas, das uns gehört. Wir sind geduldig.“ Der Falter zerfällt zu Asche.

## M110 – Die Doppelnacht ⚖

**Ziel:** Das Jahr feiern, dann die erste große Entscheidung.

**Ablauf**
1. **Nach der Stimmprobe:** Zeugnisse (Kosmetik), der **Glockenwettstreit** wird gezählt. Der Turm mit den meisten Tönen spielt den **Jahresakkord**, die ganze Halle singt mit. (Hat der Spieler-Turm gewonnen, spielt er die Melodiestimme.)
2. **Mondtau:** Nur in Doppelnächten fällt Tau, der leuchtet. Mit Lio, Mirela, Panki, Tilda auf dem Dach des Nordturms (Geheimgang 4) Tau sammeln, Wünsche flüstern.
3. **Schiefe-Töne-Finale:** Feuerwerk aus allen Kaminen (falls im Club: Spieler zündet den letzten Kamin).
4. **Nym spricht:** Im Bett. Der Traum ist diesmal klar. Licht von unten, schwebende Häuser, ein Kinderwagen ohne Kind. Nym sagt den Namen des Spielers und einen Satz: „Du bist nicht falsch gestimmt. Du bist zu früh.“ (`flag:Nym_Spoke`)
5. **Edda** wartet morgens um fünf vor dem Turm (Dialog D_M110_Edda). **Entscheidung 1:** Ihr die Wahrheit über die Stimme sagen oder die Kraft verbergen.
6. **Abschied in die Ferien:** Das Boot fährt zurück über den See. Lio spielt auf der Mundharmonika das Lied, das er „für Mara“ übt. Der Fährmann: „Herbst kommt früh dieses Jahr.“

**Entscheidung 1:**
- `E1_TrustEdda`: Edda trainiert den Spieler ab Akt 2 offen im Halten der Kraft (Urweisen früher, Nachhall-Belastung steigt langsamer).
- `E1_HidePower`: Edda ahnt es, aber der Spieler steht allein. In Akt 2 wird Corvin sein Vertrauter.

**Dialoge:** D_M110_Edda (vorhanden), D_M110_Freunde_Dach, D_M110_Nym.
**Belohnung:** 400 EP, Ring „Doppelnacht“, Jahresakkord (Festweise), `rep:Hueter+5`.
**Haken Akt 2:** In den Ferien brennt in Kesselbrunn eine Scheune. In der Asche: der Abdruck einer Hand in schwarzem Handschuh.
