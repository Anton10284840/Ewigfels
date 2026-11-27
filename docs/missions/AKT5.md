# Akt 5 – Nachhall (Jahr 5, Herbst 816 bis Sommer 817)

Frage des Akts: **Was mache ich mit der Macht?** · Gefühl: Abschied, Mut, Zusammenhalt, und am Ende die Wahl, die allein der Spieler trifft.
Neue Region: **Der Nachhall** (eigene Ebene: entsättigt, Licht von unten, Ton läuft rückwärts, schwebende Trümmer). Rückkehr nach **Brandheim**.
Alle Verbündeten, die noch leben und dem Spieler vertrauen, tauchen auf. Wer fehlt, fehlt sichtbar (leerer Platz am Lagerfeuer).

| ID | Mission | Dauer | Ort | Kampf | Besonderheit |
|---|---|---|---|---|---|
| M501 | Rückkehr nach Brandheim | 50 min | Brandheim | Stummlinge | Lagerfeuer mit allen |
| M502 | In den Nachhall 🎬 | 50 min | Nachhall | Leerchor | neue Gesetze |
| M503 | Die verlorenen Kael | 55 min | Nachhall, Echo-Vor-Kael | Prüfungen | Kael helfen |
| M504 | Das Haus, das es nicht gibt | 45 min | Nachhall-Brandheim | – | Eltern |
| M505 | Nymrael ⚖ | 45 min | Nyms Kammer | – | Entscheidung 6 |
| M506 | Die gefangenen Schüler | 55 min | schwebendes Brandheim | Stille Hand | Befreiung |
| M507 | Der Chor der Stillen Hand | 60 min | Aurenburg + Brandheim | Großschlacht | Verbündete |
| M508 | Der Dirigent | 60 min | Aurenburg-Glockenturm, Nachhall | Endboss | 3 Phasen |
| M509 | Der Grundton 🎬 | 30 min | Nachhall | – | Ende wählen |
| M510 | Epilog: Das nächste Jahr | 40 min | Nocturna | – | Abschlussfeier |

---

## M501 – Rückkehr nach Brandheim

**Ziel:** Alle zusammenholen. Die letzte ruhige Nacht.

**Ablauf**
1. **Aufbruch:** Der Spieler wählt, wer mitkommt. Pflicht: niemand. Möglich (wenn am Leben und mit genug Beziehung): Lio, Mara (`E3_MaraRedeemed`), Aurelia, Fink (`E5_Uprising`), Edda (nicht bei `E5_Negotiate`, da verhaftet, dann kommt sie mit Ausbruch durch Aurelia), Quendel (`E2_KeyKeepers`), Nell, Corvin (`flag:Corvin_Spared`), Ambros (bleibt am Rand der Sperrzone).
2. **Brandheim** ist dunkler als in Akt 3. Die Farbe ist aus einem Kilometer Umkreis verschwunden. Die Stille Hand hat die Stadt zu einem Resonanzkörper umgebaut (Bannglocken aus Aurenburg, gestohlene Kollegiumsglocken, ggf. die Stimmglocke).
3. **Lager am Stadtrand:** Aufklärung mit Seelenecho, Stummlinge fernhalten.
4. **Die Lagerfeuer-Szene** (Herz der Mission): Alle Verbündeten sitzen am Feuer. Jeder bekommt ein letztes Gespräch (Liste unten). Lio spielt Mundharmonika, Ambros (falls da) Bandoneon, der Spieler kann mit dem Akkordeon-Motiv einsetzen. **Die Lieder aller Freunde verschmelzen zum Wiegenlied.**
5. **Leere Plätze:** Wer tot ist, hat einen Stein am Feuer mit seinem Erinnerungsstück (Grells Handschuh, Saphiras Feder, Lios Netzknoten).

**Lagerfeuer-Gespräche:** Lio (Zukunft, Mara, „Wenn du bleibst, bleib ich auch“) · Mara (Schuld, die Fischer, „Ich habe ihn Vater genannt“) · Aurelia (wer sie ohne Namen ist) · Fink (Gedicht, schlecht, rührend) · Edda (Vergebung annehmen oder nicht) · Quendel (Kael-Lied, sie lehrt die erste Strophe für M503) · Nell (ihr Bruder, gefunden oder nicht) · Corvin (bereut, bittet um nichts).
**Gegner:** Stummlinge, Echohunde.
**Dialoge:** D_M501_Lagerfeuer (eine Datei, Verzweigungen pro Figur).
**Belohnung:** 800 EP, `flag:Ally_<Name>=1` pro Begleiter (bestimmt M507).

## M502 – In den Nachhall 🎬

**Ziel:** Eine neue Welt mit neuen Regeln. Schaurig schön.

**Ablauf**
1. **Der Übergang:** Quendel (oder Nym-Echo) singt den Weg frei, der Spieler spielt den Urton bewusst. Die Welt kippt: Licht kommt von unten, Schatten sind hell, Ton läuft rückwärts.
2. **Neue Gesetze (Tutorial ohne Text):**
   - Weisen klingen rückwärts und wirken umgekehrt: *Funkenschnipp* macht kalt, *Glockenschild* zieht an statt abzuwehren. Nur Hall-Weisen und der Urton funktionieren normal.
   - Was man ungeschehen macht, kommt hier zurück: Zerstörte Brücken in der Welt sind hier heil.
   - Der Spieler sieht seinen eigenen Nachhall-Schatten, der Bewegungen um eine Sekunde verzögert nachmacht (Rätsel: Schatten auf Platten stellen).
3. **Schwebende Trümmer:** Häuser aus Brandheim, Kael-Tempel, verlorene Schiffe, Ollendorfs Hut von 790.
4. **Leerchor:** Schwarm aus verlorenen Stimmen, der Weisen verschluckt. Kampf nur mit Hall-Weisen und Rückklang.

**Gegner:** Leerchor, Stummlinge, Splitterwandler.
**Rätsel:** Schatten-Platten, rückwärts klingende Weisen nutzen.
**Cutscenes:** Übergang (60 s).
**Belohnung:** 800 EP, Nachhall-Karte.

## M503 – Die verlorenen Kael

**Ziel:** Die alte Zivilisation als Menschen kennenlernen. Hilfe verdienen.

**Ablauf**
1. **Echo-Vor-Kael:** Die Kael leben hier als Echos, wiederholen seit 2000 Jahren ihren letzten Tag.
2. **Drei Prüfungen** der Kael-Älteren (je eine Weise der Kael: Chor, Stille, Erinnerung):
   - **Chor:** eine Koop-Weise mit Verbündeten wirken (allein: mit Kael-Echos als Partner).
   - **Stille:** eine ganze Straße durchqueren, ohne einen Ton zu machen, während die Echos um Hilfe rufen.
   - **Erinnerung:** ein Kael-Kind findet seine Mutter nicht. Der Spieler hilft mit Seelenecho. (Spiegelt Nymraels Verlust.)
3. **Die Kael-Strophe:** Wer alle Prüfungen besteht und Quendels Strophe kennt (oder die Questreihe „Stimmen aus Stein“ abgeschlossen hat), lernt die **zweite Strophe des Grundtons**. Ohne sie ist Ende A nicht möglich.
4. Die Kael versprechen, im Finale zu singen.

**Gegner:** Prüfungs-Echos (nicht tödlich), Splitterwandler.
**Dialoge:** D_M503_KaelAeltester.
**Belohnung:** 800 EP, `flag:Kael_Song=1` (wenn bestanden), Kodex „Die Kael“ vollständig.

## M504 – Das Haus, das es nicht gibt

**Ziel:** Die emotionalste Szene des Spiels. Kein Kampf, nur Erinnerung.

**Ablauf**
1. **Nachhall-Brandheim:** Hier steht die Stadt in Farbe, am letzten Abend, eingefroren. Die Werkstatt aus M301 ist offen.
2. **Die Eltern:** Zwei Echos von Instrumentenbauern, **Jorin und Elise** (Nachname = vom Spieler im Editor wählbar, Standard „Harlander“). Sie bauen am Bandoneon. Sie sehen den Spieler nicht, sie sehen das Kind in der Wiege.
3. **Interaktion:** Der Spieler kann durch die Werkstatt gehen, Dinge berühren (jedes Ding = eine kurze Erinnerung: wie sie sich kennengelernt haben, Streit über den Namen des Kindes, das Lied).
4. **Das Wiegenlied:** Die Mutter singt es, ganz, zum ersten Mal mit allen Strophen. Der Spieler kann mitsummen (Eingabe, kein Zwang).
5. **Der Moment:** Für einen Herzschlag sieht die Mutter auf, direkt zum Spieler, und lächelt. Niemand weiß, ob sie ihn sieht. Dann der Urton, die Farbe läuft aus.
6. Der Spieler steht mit dem fertigen Lied in der Werkstatt. Ambros' Brandheim-Quarz am Finger leuchtet in der Farbe seiner Magie.

**Gegner:** keine.
**Dialoge:** D_M504_Eltern (Echos, keine Wahl), D_M504_Begleiter (wer mitkommt, wartet draußen und sagt danach ein Wort).
**Belohnung:** 600 EP, Wiegenlied komplett (wird zur Finale-Weise *Grundton*-Vorstufe), `flag:Heard_Lullaby=1`.

## M505 – Nymrael ⚖

**Ziel:** Die letzte große Entscheidung vor dem Finale. Mitleid gegen Sicherheit.

**Ablauf**
1. **Nyms Kammer:** Ein umgedrehter Turm, in dem Nym seit 2000 Jahren gefangen ist. Ihre Fesseln sind Töne, die sie selbst gespielt hat.
2. **Nym ohne Maske:** Sie gibt alles zu, ohne Ausreden. Sie hat Brandheim nicht verursacht, aber den Ton zum Kind gelenkt. Sie will zurück in die Welt, alt werden, sterben dürfen.
3. **Die Wahl hängt am Finale:** Wird sie befreit, bleibt der Nachhall offen (sie ist der Riegel). Wird sie gebunden, wird sie zum Schloss, mit dem man den Nachhall verschließen kann, und verlischt dabei.
4. **Entscheidung 6:**
   - `E6_FreeNym`: Der Spieler löst die Töne. Nym fällt zum ersten Mal auf den Boden, statt zu schweben, und lacht. Sie kämpft im Finale mit. Ende D möglich, Ende A schwieriger (braucht hohe Nym-Beziehung).
   - `E6_BindNym`: Der Spieler stimmt ihre Töne zu einem Schloss. Nym nickt. „Das ist gerecht.“ Sie verlischt langsam bis M509. Ende A leichter möglich.

**Dialoge:** D_M505_Nym (lang, ruhig).
**Belohnung:** 600 EP, Flags E6.

## M506 – Die gefangenen Schüler

**Ziel:** Rettungsmission mit Zeitdruck. Für die Kinder, die man kennt.

**Ablauf**
1. **Schwebendes Brandheim:** Die Stille Hand hat Teile der Stadt zwischen Welt und Nachhall gehoben. Plattformen, Ketten, Glocken.
2. **Zwölf Schüler** in Taktstein-Käfigen auf verschiedenen Inseln. Jeder Käfig ein kleines Rätsel (Taktstein im richtigen Moment zerschlagen, Ketten mit Leichtklang lösen, Wachen synchron ausschalten).
3. Wer gerettet ist, hilft: Wim baut mit *Wachsklang* Brücken, Panki repariert mit Bastelweisen eine Gondel, Mirela findet mit Nachklängen versteckte Wege.
4. **Zeitdruck:** Die Glocken der Stillen Hand stimmen sich ein (Ton wird höher). Wer zu langsam ist, rettet nicht alle (fehlende Schüler kommen im Epilog nicht vor, aber niemand stirbt: Sie werden in M508 befreit, wenn der Dirigent fällt).

**Gegner:** Novizen, Taktgeber, Dirigentenwachen, Splitterwandler.
**Rätsel:** 12 Käfige.
**Belohnung:** 900 EP, `flag:Students_Saved` (Anzahl 0–12).

## M507 – Der Chor der Stillen Hand

**Ziel:** Die große Schlacht. Jede Entscheidung des Spiels zahlt sich aus.

**Ablauf**
1. **Zwei Schauplätze gleichzeitig:** Die Stille Hand marschiert auf Aurenburg (der Dirigent will die Bannglocken der Hauptstadt als zweiten Resonanzkörper), während der Spieler in Brandheim die Haupt-Resonanzglocke angreift.
2. **Verbündete erscheinen** (je nach Flags): Hüter über das Moor (`E2_KeyKeepers`), Ediktwachen unter Aurelia (`E5_Negotiate`) oder Graue Finken (`E5_Uprising`), Zünfte mit Nell, Mara mit abtrünnigen Novizen (`E3_MaraRedeemed`), die Stillen Wächter aus Nocturna, Kreaturen aus dem Schutzgebiet (Questreihe „Arche“), Kael-Echos (`flag:Kael_Song`), Nym (`E6_FreeNym`).
3. **Spielerisch:** Zwischen drei Fronten wechseln (Sturmschwinge), jede Front braucht Hilfe. Mehr Verbündete = mehr Fronten halten sich allein.
4. **Choralisten-Chor:** Hundert Stimmen der Stillen Hand singen gemeinsam ein Verstummen-Feld, das langsam alle Weisen löscht. Der Spieler muss die vier Chorführer ausschalten.
5. **Ende:** Die Resonanzglocke in Brandheim zerbricht. Aber der Dirigent ist schon in Aurenburg, auf dem Glockenturm.

**Gegner:** alle Stille-Hand-Typen, Leerchor.
**Belohnung:** 1000 EP.

## M508 – Der Dirigent

**Ziel:** Endboss. Ein Kampf gegen einen Menschen, der in vielem recht hat.

**Ablauf**
1. **Aurenburg-Glockenturm, Doppelnacht.** Unter dem Turm kämpfen die Verbündeten.
2. **Story-Boss: Der Dirigent** (Details: [BOSSE.md](BOSSE.md#5-der-dirigent-theodric-vaal-akt-5)). Phase 1 und 2 auf dem Turm. Am Ende von Phase 2 spielt er den Urton mit allen Bannglocken. Der Turm, die Stadt, der Himmel werden farblos.
3. **Phase 3 im Nachhall:** Spieler und Dirigent fallen in den Nachhall, auf eine Insel aus Brandheim. Hier spielt er nicht mehr gegen den Spieler, sondern um ihn herum: Er versucht, den Spieler als Saite zu benutzen.
4. **Gegenakkord:** Das Wiegenlied (Brandheim-Quarz) gegen den Urton des Dirigenten. Jeder Verbündete, der am Lagerfeuer war, gibt eine Stimme dazu (sichtbar: ihre Farben in der Melodie).
5. **Der Dirigent bricht:** Sein Glasquarz zerspringt. Die Stimmen seiner Familie sind frei, sie klingen einen Moment über dem Nachhall. Er sitzt still.
6. **Wahl in Dialog:** Den Dirigenten verschonen (`flag:Vaal_Spared`) oder nicht. Nur verschonen öffnet die Szene, in der er im Ende A in Stille lebt.

**Belohnung:** 2000 EP.

## M509 – Der Grundton 🎬

**Ziel:** Das Ende wählen. Spielerisch, nicht als Menü.

**Ablauf**
1. Der Spieler steht im Zentrum des Nachhalls, der Urton liegt in seinen Händen. Alle sieben großen Aderknoten (Aderkarte aus M401) sind hörbar.
2. **Die Wahl ist eine Weise:** Der Spieler wählt, **was er spielt**. Verfügbar ist nur, was die Bedingungen erlauben (siehe STORY_BIBLE §9):

| Ende | Was der Spieler spielt | Bedingungen |
|---|---|---|
| **A: Neue Stimmung** | Urton + Wiegenlied + Kael-Strophe an allen Aderknoten | NachhallLoad < 50, mind. 2 Fraktionen ≥ Geschätzt, `Has_AderMap`, `Kael_Song`, und `E6_BindNym` oder `rel:Nym>=60` |
| **B: Die Stille** | Den Urton des Dirigenten zu Ende | immer möglich (Dialogoption „Er hatte recht“), automatisch wenn zu wenige Verbündete (< 2 Ally-Flags) |
| **C: Der Erste Ton** | Urton für sich selbst, die Welt nach eigenem Willen | NachhallLoad > 75 |
| **D: Das Opfer** | Urton nach innen, im Nachhall bleiben und von innen verschließen | `E6_FreeNym` und (`rel:Lio>=60` oder `rel:Aurelia>=60`); bei Werten ≥ 80 bietet sich Lio/Aurelia an, an Stelle des Spielers zu bleiben (wählbar) |

3. **Cutscene je Ende** (90–120 s), dann Epilog.

## M510 – Epilog: Das nächste Jahr

**Ziel:** Abschied, Lohn, Ausblick. Der Spieler soll lächeln oder weinen, am besten beides.

**Ablauf**
1. **Rückkehr nach Nocturna** (in Ende D: die Freunde kehren zurück, der Spieler sieht es als Echo).
2. **Abschlussfeier** in der Großen Halle (Zustand je nach E4 und Ende): Zeugnisse, Varga oder Edda oder Ollendorf hält die Rede, Kessel Klaus pfeift die Hymne, die Stimmglocke läutet von selbst (wenn vorhanden). Ambros spielt das Wiegenlied auf dem fertigen Bandoneon der Eltern.
3. **Freies Spiel:** Der Spieler kann durch die Akademie gehen und mit allen reden, jeder hat einen Abschiedssatz.
4. **Epilog-Bausteine** (STORY_BIBLE §9), je 20–40 s Standbild + Erzählerstimme. Reihenfolge: Nocturna → Lio & Mara → Aurelia → Edda → Graumoor → Aurenburg → Hüter → Zünfte → Schutzgebiet → Turnier → Tiedmar → Kael.
5. **Letztes Bild:** Ein neues Boot im Nebel über den Ilsensee. Ein Kind sitzt darin. Der Spieler (in A, B, C) steht als Lehrkraft auf dem Steg und sagt: „Du bist spät. Wie immer.“ (Spiegel zu M101.)
6. **Nach dem Abspann:** Freie Welt bleibt spielbar (außer Ende B: Magie bleibt aus, man kann nur noch spazieren, schwimmen, reiten).

**Belohnung:** Titel nach Ende, Neues Spiel+ (Weisen und Kosmetik bleiben).
