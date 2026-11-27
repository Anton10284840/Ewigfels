# Akt 2 – Schatten im Inneren (Jahr 2, Herbst 813 bis Sommer 814)

Frage des Akts: **Wer jagt mich?** · Gefühl: Die Akademie ist noch schön, aber man schließt jetzt die Fenster. Angst, Misstrauen, erste Verluste.
Neue Regionen: **Graumoor**, **Glastiefen**. Neue Lehrkraft: **Corvin Hale**. Neue Reittiere: **Gischtrobbe** (Ilsensee, Sidequest), **Glutdachs** (Glastiefen, Sidequest).

| ID | Mission | Dauer | Ort | Kampf | Besonderheit |
|---|---|---|---|---|---|
| M201 | Neue Gesichter | 50 min | Nocturna | – | Corvin, erster Erstklässler-Schützling |
| M202 | Der Handschuh | 50 min | Nocturna, Kesselbrunn | Wilderer | Ermittlung |
| M203 | Im Graumoor | 55 min | Graumoor, Stelzenheim | Moorlicht-Unken | Mutter Quendel |
| M204 | Ruinen von Altwacht | 60 min | Altwacht | Stille Hand, Stummlinge | Dungeon, erster Nachhallschritt |
| M205 | Der Preis des Schlüssels ⚖ | 45 min | Stelzenheim | – | Entscheidung 2 |
| M206 | Wintersperre 🎬 | 60 min | Kesselbrunn | Stille Hand (groß) | Hanne stirbt |
| M207 | Lios Schwester | 45 min | Kesselbrunn, Nocturna | – | Mara erkannt |
| M208 | Glastiefen | 60 min | Glastiefen, Funkelgrund | Glasnattern, Stille Hand | Seelenecho |
| M209 | Die Taktgeberin | 50 min | Kael-Mine | Story-Boss Mara | „Brandheim-Kind“ |
| M210 | Masken 🎬 | 55 min | Nocturna | – (Schleichen/Dialog) | Inspektion, Aurelia schweigt |

---

## M201 – Neue Gesichter

**Ziel:** Rückkehr nach Hause, die Welt ist ein Jahr älter. Den Verräter so sympathisch machen, dass es in Akt 4 wehtut.

**Ablauf**
1. **Überfahrt** mit den neuen Erstklässlern. Der Spieler ist jetzt der, der sagt: „Warte, bis du die Treppe siehst.“ Ein kleiner Junge, **Wim Hasel** (11, Moosgrund-Anwärter, hat Angst vor Wasser), klammert sich an den Spieler und wird sein Schützling (kleine Questreihe durch Akt 2).
2. **Willkommensabend:** Bruni Kesselmayr ist in Rente (sie schickt einen explodierenden Kuchen als Abschied). Varga stellt **Corvin Hale** vor. Die Schüler lieben ihn nach dem ersten Satz.
3. **Erste Stunde bei Corvin:** Elementarkunde im Kesselkeller. Er lässt die Klasse einen Kessel absichtlich explodieren, „damit ihr wisst, wie es klingt, bevor es passiert“. Er erinnert sich an den Spieler vom Ilsenlicht (Dialogcheck: Funkensaft angenommen = er lacht über den Witz von damals).
4. **Je nach E1:**
   - `E1_TrustEdda`: Edda holt den Spieler nachts in die **Seegrotte** (Geheimgang 11). Erste Stunde „Halten“: den Urton spüren, ohne ihn rauszulassen (Atem-Minispiel, Herzschlag-Rhythmus). Edda erzählt nichts über Brandheim, nur: „Ich habe so etwas schon einmal gehört.“
   - `E1_HidePower`: Corvin bleibt nach der Stunde, schaut auf die Kerze des Spielers, die kurz grau flackert. „Ich hab nichts gesehen. Aber wenn du mal reden willst, mein Keller ist warm.“ (`flag:Corvin_Confidant=1`)
5. **Grell** zeigt Varga den schwarzen Handschuh (Spieler belauscht sie durch den Speisenaufzug, Geheimgang 8). Varga: „Kein Wort davon zu den Schülern. Und kein Wort zum Kollegium.“

**Gegner:** keine. **Rätsel:** Speisenaufzug-Lauschposten.
**Dialoge:** D_M201_Corvin, D_M201_Wim, D_M201_Edda_Grotte / D_M201_Corvin_Keller.
**Belohnung:** 300 EP, Abzeichen 2. Jahr, Corvins erste Weise (*Siedepunkt*-Variante).
**Haken:** Wim fragt: „Stimmt es, dass du den Wald grau gemacht hast?“ Die Lauscher haben nichts vergessen.

## M202 – Der Handschuh

**Ziel:** Kleine Detektivarbeit. Der Spieler wird aktiv statt nur Opfer.

**Ablauf**
1. Lio will nicht warten: „Wenn die Leute mit dem Handschuh Varn umgedreht haben, können sie jeden umdrehen.“ Gemeinsam Varns Hütte am Waldrand durchsuchen (Varn ist in Kur in Kesselbrunn).
2. **Spuren:** Varns Notizbuch hat seit dem Frühjahr nur noch Striche im immer gleichen Takt. Eine Quittung eines „Moorhändlers“ für „einen Ruhestein gegen Schlaflosigkeit“.
3. **Kesselbrunn:** Varn befragen. Er ist freundlich, verwirrt, schämt sich. Mit Geduld (Dialog, kein Druck) erinnert er sich: ein Händler mit grauem Mantel, der Stein „summte so angenehm still“. Der Händler kam aus Graumoor.
4. **Die abgebrannte Scheune** (Haken aus M110): Der Bauer erzählt von Fremden, die nachts den Boden „abgehört“ haben. Unter der Asche: ein Bohrloch bis auf die Ader.
5. **Wilderer** am Rückweg: Sie fangen Mooslinge in Käfigen. Kampf oder schleichend die Käfige öffnen. (Anstoß Questreihe „Die Arche am See“ mit Saphira.)
6. **Erlaubnis:** Grell spricht mit Varga. Offiziell: „Exkursion Kreaturenkunde nach Graumoor“, Saphira begleitet (sie wollte ohnehin zu den Hütern).

**Gegner:** Wilderer (Klingenschnapper, Fallensteller, Wildererhund).
**Rätsel:** Notizbuch-Rhythmus entschlüsseln (Takt = Weg auf der Karte), Bohrloch finden.
**Dialoge:** D_M202_Varn, D_M202_Lio, D_M202_Grell.
**Belohnung:** 300 EP, Mooslinge (als Haustier wählbar), `rep:Hueter+5`.
**Haken:** Saphira packt heimlich ein Bündel ein, das winselt. (Echohund, ihr Geheimnis.)

## M203 – Im Graumoor

**Ziel:** Neue Region, neue Stimmung (Nebel, Stelzen, Frösche), eine Weise Frau, die mehr weiß als alle.

**Ablauf**
1. **Reise** per Boot und Fuß. Das Moor ist grau, aber nicht tot: Frösche singen zweistimmig, Stelzreiher stehen im Nebel.
2. **Stelzenheim:** Pfahldorf der Hüter. Großer Moment für Tilda, falls S108 gemacht: **Herr Quaak** wird von hunderten Fröschen begrüßt. Er ist der Froschkönig von Stelzenheim. Tilda: „Ich hab's immer gesagt!“ (Sie hat das Gegenteil gesagt.)
3. **Mutter Quendel** empfängt den Spieler, lacht mit Goldzahn, pustet Pfeifenrauch und sagt auf Kael etwas, das niemand versteht. Dann auf Deutsch: „Du klingst wie jemand, dem ich seit sechzig Jahren nicht antworte.“
4. **Die kranke Ader:** Mit Quendel und zwei Hütern einen Aderknoten „zurücksingen“ (Rhythmus-Minispiel, drei Stimmen, Spieler hält den Grundton). Moorlicht-Unken greifen den Knoten an.
5. **Der Moorhändler:** Quendel kennt ihn. Er kauft Kael-Splitter von Kindern und verschwindet nach Altwacht, wo „graue Männer graben“.
6. **Abend:** Frösche-Chor-Fest in Stelzenheim, Tanzen auf schwankenden Stegen.

**Gegner:** Moorlicht-Unken (locken mit Lichtern, springen aus dem Nebel).
**Rätsel:** Aderknoten-Gesang, Nebel-Navigation über Tonquellen.
**Dialoge:** D_M203_Quendel, D_M203_Tilda_Quaak, D_M203_Saphira.
**Belohnung:** 350 EP, Stelzenheim als Schnellreisepunkt, Hüter-Weise *Rankenstrich*-Variante, `rep:Hueter+10`.
**Haken:** Nachts singt Quendel allein am Steg auf Kael. Aus dem Nebel antwortet eine Frauenstimme. Quendel hört auf und weint.

## M204 – Ruinen von Altwacht

**Ziel:** Erster großer Dungeon, erster Kontakt mit dem Nachhall.

**Ablauf**
1. **Die Festung** aus den Stimmkriegen liegt halb im Moor versunken. Die Stille Hand gräbt im Keller. Eingang: Schleichen über eingestürzte Türme oder Frontalangriff am Tor.
2. **Drei Flügel** mit je einem Rätsel: Klangfolge an alten Wachglocken, Wasserstand senken, Spiegel-Licht durch Schießscharten.
3. **Die Grabung:** Novizen und Schweiger bewachen ein Loch, aus dem Licht von unten kommt. Ein Choralist singt die Gruppe stark.
4. **Der Kael-Schlüssel:** Ein faustgroßer **Stimmstein**, der einen Ton hält, ohne zu verklingen.
5. **Erster Nachhallschritt:** Beim Greifen bricht der Boden. Der Spieler fällt **in den Nachhall** (dünner Ort). Alles ist farblos, Licht von unten, Ton läuft rückwärts. Die Festung steht dort heil, wie vor 800 Jahren. **Stummlinge** ziehen den Ton aus der Luft. Nym flüstert den Weg (Echo-Hinweise). Rückweg über einen Riss.
6. Zurück in der Welt: Saphira und Lio haben gesucht, nur zwei Minuten vergangen, für den Spieler fühlte es sich an wie eine Stunde.

**Gegner:** Novizen, Schweiger, Choralist (Mini-Boss), Stummlinge (im Nachhall).
**Rätsel:** Wachglocken, Wasserstand, Spiegel, Nachhall-Weg (nur dort sichtbare Brücken).
**Dialoge:** D_M204_Lio, D_M204_Nym_Echo.
**Belohnung:** 450 EP, Kael-Schlüssel, Kernweise *Nachhallschritt* (begrenzt), `flag:NachhallLoad+5`, Kodex „Der Nachhall“.
**Haken:** Unter den Grabungsplänen liegt eine Liste von Orten. Ganz oben: Kesselbrunn. Durchgestrichen. Daneben: „Wintersonnenwende“.

## M205 – Der Preis des Schlüssels ⚖

**Ziel:** Erste Entscheidung mit sichtbarer Folge in der Welt.

**Ablauf**
1. **Zurück in Stelzenheim:** Quendel will den Stimmstein in die kranke Ader setzen, damit sie heilt.
2. **Das Kollegium kommt:** Ein Gesandter (Magister Lorenz Feil) mit vier Ediktwachen. **Aurelia** begleitet ihn als Praktikantin ihres Vaters, sichtlich unwohl. Edikt: Kael-Artefakte gehören dem Kollegium. Angebot: Das Kollegium legt das Moor trocken und baut Wege, „die Ader wird dann sicher verwaltet“.
3. **Gespräche:** Quendel (laut, wütend, ehrlich), Feil (höflich, logisch, droht leise), Aurelia (privat: „Gib ihn ihm. Bitte. Ich muss hier was beweisen.“), Saphira (sagt nichts, schaut auf die Frösche).
4. **Entscheidung 2:**
   - `E2_KeyCollegium`: Ruf Kollegium +20, Hüter −20. Das Moor wird in Akt 3 trockengelegt (Region verändert: Kanäle, Wege, weniger Kreaturen, Stelzenheim halb leer). Aurelia `+10`.
   - `E2_KeyKeepers`: Ruf Hüter +20, Kollegium −20. Die Ader heilt, das Moor blüht (Lichtsumpf, Stelzreiher-Schwärme). Feil notiert den Namen des Spielers. Aurelia `−5`, sagt aber: „Du hast recht. Ich hasse, dass du recht hast.“
5. **Abschied:** Quendel schenkt eine Holzperle aus ihrem Zopf: „Wenn du mal nicht weißt, wohin, roll sie.“

**Dialoge:** D_M205_Quendel, D_M205_Feil, D_M205_Aurelia.
**Belohnung:** 300 EP, Holzperle (Schmuck, zeigt bei Gefahr die Richtung nach Hause), Ruf je nach Wahl.
**Haken:** Auf dem Rückweg sieht der Spieler Rauch über den Bergen. In Richtung Kesselbrunn.

## M206 – Wintersperre 🎬

**Ziel:** Der erste echte Verlust. Ab hier ist niemand mehr sicher.

**Ablauf**
1. **Wintersonnenwende in Kesselbrunn.** Die Akademie ist zum Lichterfest eingeladen. Markt, Glühwein, Jupp spielt. Hanne Brook winkt aus dem Waisenhaus.
2. **Angriff (Cutscene):** Die Glocken des Dorfes verstummen eine nach der anderen. Schwarz gekleidete Gestalten kommen den Hang herunter, synchron im Schritt. Ziel: das Bohrloch unter dem Dorf (Ader) und der Spieler.
3. **Verteidigung (offener Kampf, 3 Zonen):** Marktplatz (Zivilisten retten), Ambros' Werkstatt (brennt, Ambros rettet die Quarze und eine **alte Instrumentenkiste**), Waisenhaus (Kinder rausholen, Zeitlimit, Rauch).
4. **Hanne:** Im Waisenhaus hält Hanne mit einem Kochtopf-Glockenschild (sie kann nur Alltagsweisen) die Tür gegen einen Schweiger. Der Spieler holt die Kinder raus. Die Decke kommt herunter. Hanne lächelt und sagt: „Pfeif laut, hab ich gesagt.“ Sie stirbt nicht dramatisch, sondern leise, im Schnee, mit den Kindern um sich.
5. **Rückzug der Stillen Hand:** Ein Signal aus einer Pfeife. Die Angreifer ziehen sich im Takt zurück.
6. **Wintersperre:** Varga schließt Nocturna. Niemand verlässt den Fels bis zum Frühling. Die Kinder aus dem Waisenhaus wohnen jetzt in der Akademie (kleine Gesten: sie helfen Mutter Gundel, schlafen im Moosgrund-Gemeinschaftsraum).

**Gegner:** Novizen, Schweiger, Taktgeber (synchronisieren Gruppen: alle Angriffe kommen gleichzeitig), Dirigentenwache (1).
**Rätsel:** Waisenhaus (Rauch, Wege mit *Leichtklang* und Wasserweisen freimachen).
**Dialoge:** D_M206_Hanne, D_M206_Ambros, D_M206_Varga_Sperre.
**Cutscenes:** Glocken verstummen (40 s), Hanne (50 s).
**Belohnung:** 500 EP, Hannes Kochlöffel (Erinnerungsstück im Turmzimmer), `rep:Zuenfte+10`.
**Haken:** Unter den Angreifern war eine Taktgeberin, die nicht angriff. Sie stand nur da und schaute auf Lio.

## M207 – Lios Schwester

**Ziel:** Die Wendung des Akts aus Lios Sicht. Ein Gespräch, kein Kampf.

**Ablauf**
1. **Morgen nach dem Angriff.** Lio ist verschwunden. Der Spieler sucht ihn (Spuren im Schnee, Lauscher fragen, Kessel Klaus weiß es: „Der Junge ist zum Glockenturm, barfuß, der Hallkopf.“).
2. **Glockenturm:** Lio sitzt zwischen den Laternenkrähen. In der Hand: eine Haarsträhne mit einem Netzknoten, die er im Schnee in Kesselbrunn fand. „Den Knoten hab ich ihr beigebracht. Mit sieben. Den kann sonst keiner.“
3. **Lio bricht zusammen.** Wut, dann Weinen, dann Schweigen. Der Spieler wählt, wie er da ist (zuhören, festhalten, Witz machen, Wut teilen). Alles ist richtig, jedes verändert Lio leicht (bitter, mutig, weich).
4. **Corvin** kommt dazu (er hat sie gesucht), setzt sich, sagt das Richtige und bringt Kakao. Er erwähnt „zufällig“, dass die Stille Hand in den **Glastiefen** Kael-Quarz abbaut. (Er schickt den Spieler absichtlich zu Mara, um die Kraft zu messen.)
5. **Edda** verbietet die Reise. **Grell** sagt nichts, aber lässt in der Nacht den Bootssteg unbewacht.

**Gegner:** keine.
**Dialoge:** D_M207_Lio_Glockenturm (wichtigster Dialog des Akts), D_M207_Corvin, D_M207_Edda.
**Belohnung:** 250 EP, `rel:Lio` je nach Wahl (+10 bis +20), `flag:Mara_Revealed=1`.
**Haken:** Lio spielt um Mitternacht auf der Mundharmonika das Lied für Mara. Aus dem Nachhall antwortet niemand. Aber aus dem See antwortet der Glockenwels mit einem einzigen Glockenschlag.

## M208 – Glastiefen

**Ziel:** Neue Region, Höhlenwunder, neues Werkzeug.

**Ablauf**
1. **Heimlich los** mit Lio (und optional Aurelia, wenn `rel:Aurelia>=20`: Sie hat den Steg beobachtet und kommt „aus Neugier“ mit).
2. **Funkelgrund:** Kristallschleifer-Familien, alles glitzert, die Kinder singen Echolieder in die Schächte. Händler Grimm Glanz verkauft Höhlenlaternen.
3. **Abstieg:** Kristallseen, leuchtende Pilze, der ferne Gesang des **Kristallwals**. Glasnattern jagen Wärme (Atem anhalten, kalt bleiben).
4. **Seelenecho:** In einer Kael-Halle steht ein Stein, der die letzten Worte des Ortes festhält. Ein Kael-Echo (alte Minenarbeiterin) lehrt den Spieler, Echos von Orten zu hören (**Seelenecho**, K55). Erste Anwendung: sehen, wo die Stille Hand vor einer Stunde lang ging.
5. **Die Mine:** Die Stille Hand zwingt Bergleute aus Hohlstollen, Kael-Quarz zu brechen. Daraus werden **Taktsteine**. Befreien (Schleichen oder Kampf).

**Gegner:** Glasnattern, Novizen, Taktgeber, Bergleute unter Taktstein-Zwang (nicht töten, Stein zerschlagen).
**Rätsel:** Kristall-Resonanz (Töne in Kristallen spiegeln, um Brücken aus Licht zu bauen), Seelenecho-Spur.
**Dialoge:** D_M208_Lio, D_M208_Aurelia (optional), D_M208_KaelEcho.
**Belohnung:** 450 EP, Kernweise *Seelenecho*, Funkelgrund-Schnellreise, Glutdachs-Spur (Sidequest).
**Haken:** Im Seelenecho der Mine hört der Spieler eine Stimme, die Befehle zählt: „Eins. Zwei. Drei. Vier.“ Lio erstarrt. „Das ist ihr Takt.“

## M209 – Die Taktgeberin

**Ziel:** Story-Boss 2. Ein Kampf, in dem man nicht gewinnen will.

**Ablauf**
1. **Die tiefe Mine:** Ein Kael-Klangraum, rund wie ein Trommelfell, Kristalle an den Wänden.
2. **Mara** steht in der Mitte, mit sechs Novizen im Takt. Lio ruft ihren Namen. Sie antwortet nicht ihm, sondern dem Spieler: „Du bist das, wofür er uns nach Kesselbrunn geschickt hat.“
3. **Story-Boss Mara** (Details: [BOSSE.md](BOSSE.md#2-mara-vandermeer-die-taktgeberin-akt-2)). Lio kämpft mit, kann sich aber nicht überwinden, Mara direkt anzugreifen. Wenn Lio getroffen wird, zögert Mara jedes Mal (Öffnung für den Spieler).
4. **Ende des Kampfs:** Der Raum bricht. Mara könnte den Spieler unter Geröll lassen, zieht ihn aber raus. Dann flieht sie und sagt im Gehen: „Frag deine Lehrerin nach Brandheim, **Brandheim-Kind**.“
5. Lio hat es gehört. Der Spieler weiß nicht, was Brandheim ist. „Aderbrand“, steht in jedem Buch. „Eine Unglücksstadt.“

**Gegner:** Mara (Story-Boss), Novizen im Takt.
**Dialoge:** D_M209_Mara (Kampfrufe), D_M209_Lio_Nachher.
**Belohnung:** 600 EP, Maras Metronom-Anhänger (sie verliert ihn), `flag:Heard_Brandheim=1`, `rel:Mara+5`.
**Haken:** Auf dem Rückweg hält Aurelia (falls dabei) dem Spieler stumm die Hand hin, als er strauchelt. Dann lässt sie los, als sei nichts gewesen.

## M210 – Masken 🎬

**Ziel:** Spannung ohne Kampf. Der Spieler muss sich verstecken, während die Macht im Haus ist.

**Ablauf**
1. **Das Kollegium inspiziert Nocturna** (wegen des Angriffs und des Ausflugs in die Glastiefen). Großinquisitor **Halmar Dräger** mit Bannglocken, die auf Urton reagieren. Jeder Schüler wird „angehört“.
2. **Schleichteil:** Vor der Anhörung muss der Spieler Maras Metronom und die Notizen über Brandheim aus seinem Turmzimmer bringen, während Ediktwachen die Türme durchsuchen (Geheimgänge nutzen).
3. **Die Anhörung (Dialog-Duell):** Dräger stellt Fragen. Die **Stimmglocke** hängt über dem Raum und klingt sauer bei Lügen. Der Spieler muss die Wahrheit so sagen, dass sie nichts verrät (Wortwahl-Rätsel). Ein Fehler lässt die Bannglocken anschlagen.
4. **Aurelia** ist als Kanzlerstochter Zeugin. Wenn die Bannglocken anschlagen, lässt sie absichtlich ihren Duellhandschuh fallen, die Glocke ist abgelenkt. Später: „Ich weiß, was du bist. Ich hab's in der Mine gesehen. Ich sag's nicht. Frag nicht, warum.“
5. **Corvin** steht während der Inspektion hinter Dräger und macht Notizen. (Er misst.)
6. **Ende des Jahres:** Edda kommt zum Spieler auf den Steg. Sie weiß, dass er Brandheim gehört hat. „Nächstes Jahr zeige ich es dir. Ich verspreche es. Dann darfst du mich hassen.“

**Gegner:** keine (Ediktwachen beim Schleichen: erwischt = Neustart am Abschnitt).
**Rätsel:** Wahrheit ohne Verrat (Dialog-Rätsel), Durchsuchung umgehen.
**Dialoge:** D_M210_Draeger, D_M210_Aurelia, D_M210_Edda_Steg.
**Cutscenes:** Ankunft des Kollegiums (Bannglocken über dem See, 40 s), Edda am Steg (40 s).
**Belohnung:** 500 EP, `rel:Aurelia+15`, `flag:Aurelia_Knows=1`, Ende Akt 2.
**Haken Akt 3:** In den Ferien bekommt der Spieler einen Brief, ohne Falter, mit der Post. Eine sehr saubere Handschrift: „Sie sollten wissen, woher Sie kommen. Ihre Lehrerin wird es Ihnen nicht vollständig sagen. Hochachtungsvoll, T. V.“
