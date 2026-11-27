# Hauptgeschichte – Missionsdesign (Akt 1 bis 5)

Stand: 27.11.2026 · Version 1.0 · baut auf [STORY_BIBLE §8](../STORY_BIBLE.md#8-hauptgeschichte) (verbindliche Missionsliste), [CHARACTER_BIBLE](../CHARACTER_BIBLE.md) und [AKT1_SCHULLEBEN](../AKT1_SCHULLEBEN.md) auf.

| Datei | Inhalt |
|---|---|
| [AKT1.md](AKT1.md) | Ankunft · M101–M110 · Story-Boss Varn |
| [AKT2.md](AKT2.md) | Schatten im Inneren · M201–M210 · Story-Boss Mara |
| [AKT3.md](AKT3.md) | Die Reise · M301–M310 · Story-Boss Wächter von Kael |
| [AKT4.md](AKT4.md) | Verrat · M401–M410 · Story-Boss Corvin |
| [AKT5.md](AKT5.md) | Nachhall · M501–M510 · Endboss Dirigent, 4 Enden |
| [BOSSE.md](BOSSE.md) | Alle 5 Story-Bosse im Detail (Phasen, Arena, Musik, Koop) |

Jede Mission hat dieselben Felder wie im Master-Prompt verlangt: **Ziel, Ablauf, Orte, Gegner, Rätsel, Dialoge, Cutscenes, Entscheidungen, Belohnungen**. Quest-JSON entsteht daraus pro Akt in der Umsetzungsphase (Akt 1 liegt schon als JSON vor: `Content/Data/Quests/Akt1/`).

---

## 1. Die Geschichte in fünf Sätzen

1. **Akt 1:** Ein Waisenkind kommt an die schönste Schule der Welt, findet Freunde und entdeckt, dass in ihm ein verbotener Ton schläft, der die Welt farblos macht.
2. **Akt 2:** Eine Sekte, die alle Magie beenden will, jagt das Kind, und unter ihren Kämpfern ist die verschollene Schwester seines besten Freundes.
3. **Akt 3:** Auf einer Reise durch Velmark erfährt es, dass seine Lehrerin seine Heimatstadt ausgelöscht hat und dass der verbotene Ton nicht nur löschen, sondern auch **neu stimmen** kann.
4. **Akt 4:** Der liebste Lehrer der Schule öffnet dem Feind die Tore, die Akademie brennt, und das Kollegium zeigt sein wahres Gesicht.
5. **Akt 5:** Im Nachhall, der Welt alles Ungeschehenen, entscheidet der Spieler, ob die Magie bleibt, verstummt oder ihm allein gehört.

**Thema:** Darf man etwas Gefährliches zum Schweigen bringen, wenn man dabei alles Schöne mit verliert?
**Gefühlskurve:** Akt 1 Staunen → Akt 2 Angst → Akt 3 Weite und Wahrheit → Akt 4 Verlust → Akt 5 Mut und Abschied.

## 2. Die großen Wendungen (eine pro Akt, plus kleine dazwischen)

| Akt | Große Wendung | Wo vorbereitet | Wo aufgelöst |
|---|---|---|---|
| 1 | Im Spieler schläft der **Urton** | Kael-Buch summt seinen Herzschlag (M107), Ambros' Quarz flackert violett (M104), Träume von Nym (M106) | M108 Erwachen |
| 2 | Die vermisste **Mara** kämpft für die Stille Hand und weiß, wer der Spieler ist | Lio redet über seine Schwester ab M101, Netzknoten-Armband, schwarzer Handschuh (M109) | M207, M209 |
| 3 | **Edda** hat Brandheim ausgelöscht · der Urton kann **neu stimmen** · der Spieler wurde von Nym „vorhergehört“ | Edda starrt auf das Buch (M107), Mirelas Brandheim-Nachklang (S111), Grells Albträume | M301, M309 |
| 4 | **Corvin** ist Verräter und Halbbruder des Dirigenten · der **Kanzler** hat Brandheim unterschrieben · **Fink** ist Aurelias Halbbruder | Corvin auf dem Ilsenlicht (M106), sein Interesse an „Messungen“, Varns Taktstein (M109) | M404, M406, M407 |
| 5 | Das **Haus der Eltern** im Nachhall · der Dirigent trägt die Stimmen seiner Familie bei sich | Wiegenlied (Ambros' Bandoneon M104, Mirelas Nachklang), Glasquarz am Hals des Dirigenten | M504, M508 |

## 3. Vorbereitung und Auflösung (rote Fäden)

Damit Wendungen sich verdient anfühlen, wird jede vorher **mindestens zweimal** angedeutet.

| Faden | Andeutung 1 | Andeutung 2 | Auflösung |
|---|---|---|---|
| **Das Wiegenlied** (Leitmotiv der Eltern, 6 Töne) | M104 Ambros spielt es auf dem Bandoneon und bricht ab | S111 Mirelas Nachklang aus Brandheim summt es | M504 die Mutter singt es im Nachhall · im Finale wird es zum Gegenakkord gegen den Urton |
| **Der violette Schimmer** | M104 Ringquarz flackert violett | M106 Laterne des Spielers brennt einen Moment farblos | M108 Verstummen |
| **Corvin** | M106 charmanter Gast im Gefolge des Kanzlers, fragt nach dem „Spätberufenen“ | M109 Taktstein in Varns Tasche trägt ein Kollegiums-Prüfsiegel aus Corvins Labor | M406 Verrat, M407 Geständnis |
| **Die Stimmglocke** | Akt 1 hat sie Launen, klingt bei Lügen sauer | M210 sie klingt bei der Inspektion schief, als der Spieler lügt | M406 der Feind will sie stehlen, sie ist der größte Resonanzkörper Velmarks |
| **Nyms Stimme** | M106 erster Traum, nur Echo | M110 spricht den Namen des Spielers | M309 zeigt die Wahrheit, M505 Entscheidung 6 |
| **Grell und Brandheim** | M103 bricht beim Wort „Sperrzone“ ab | M304 erzählt als Turnierrichter von „einem Befehl, den ich nie wieder befolge“ | M406 opfert sich (E4 = Halle) |
| **Fink** | M402 Flugblatt mit Reim über den Kanzler | M403 trägt Aurelias Nase und Kinn | Questreihe „Graue Finken“ Teil 6 |
| **Die Stillen Wächter** (Rüstungen) | tanzen bei Festen (Akt 1) | Grell lehrt in M405 die *Belebung* | M406 marschieren sie zur Verteidigung |

## 4. Was die Entscheidungen verändern (Kurzfassung)

| Entscheidung | Sofort sichtbar | Später sichtbar |
|---|---|---|
| E1 (M110) Edda vertrauen / verbergen | Eddas Haltung in Akt 2 | Corvin wird Vertrauter (Hide), Akt-4-Boss härter; M301-Geständnis anders |
| E2 (M205) Schlüssel an Kollegium / Hüter | Graumoor trocken oder heil | Hüter verteidigen Nocturna in M406 nur bei „Hüter“ |
| E3 (M310) Mara töten / gefangen / zurückholen | Lios Reaktion | Lio in Akt 4 weg, tot oder an deiner Seite; Mara im Finale |
| E4 (M406) Halle / Corvin | Grell oder Saphira stirbt | Akademie-Zustand in Akt 5, Ort des Corvin-Kampfs |
| E5 (M409) Aufstand / Verhandlung | Aurenburg brennt oder bleibt geordnet | Verbündete im Finale, Aurelias Epilog |
| E6 (M505) Nym befreien / binden | Nym kämpft mit oder verlischt | welche Enden möglich sind |

## 5. Regeln für alle Missionen

- **Verspielt zuerst, dunkel danach.** Jede Mission hat mindestens einen warmen oder witzigen Moment, auch in Akt 4 und 5 (Lagerfeuer, Kessel Klaus, Lio).
- **Kein reines Holen und Bringen.** Jede Mission hat einen Twist, ein Rätsel oder eine Entscheidung.
- **Der Spieler wählt, wie er kämpft.** Fast jeder Kampf hat einen Schleich-, Rede- oder Rätselweg.
- **Koop (2–4):** Jede Mission funktioniert allein. Mit Freunden: Koop-Weisen öffnen Abkürzungen, Bosse bekommen mehr Leben und eine zusätzliche Angriffsart, Entscheidungen trifft der Gastgeber nach einer Abstimmung (jeder stimmt, Gastgeber hat das letzte Wort).
- **Länge:** 45–60 Minuten pro Mission, Akt 1 etwas kürzer, weil das Schulleben dazwischen viel Zeit füllt.
