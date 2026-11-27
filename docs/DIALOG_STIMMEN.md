# Dialog-Stimmen – wie jede Figur spricht

Stand: 27.11.2026 · Grundregeln: [STORY_BIBLE §11](STORY_BIBLE.md#11-ton-und-schreibstil) · Format: [QUEST_FORMAT §3](QUEST_FORMAT.md#3-dialoge-suds)
Dialoge liegen in `Content/Data/Dialogues/`: `Akt1/` (Missionen), `Schule/` (freie Gespräche in der Akademie, `D_Schule_<NPC>`).

## Grundregeln

- **Lehrkräfte siezen** Schüler. **Schüler duzen** sich. Der **Dirigent siezt alle**, auch Kinder.
- **Kurze Sätze.** Eine Dialogzeile passt in zwei Untertitelzeilen (max. ca. 160 Zeichen).
- **Jede Figur hat einen Tick**, der in fast jedem Gespräch einmal vorkommt (Tabelle unten). So erkennt man sie auch ohne Namen.
- **Witz in der Freundesgruppe, Ruhe bei den Erwachsenen.** Pathos nur, wenn es wehtun soll (M108, M206, M406, M504).
- **Spielerfigur:** Geschlecht ist frei. Nie „der/die Neue“, „Junge“, „Mädchen“ an den Spieler. Stattdessen Name (später per Variable), „du/Sie“, „Spätberufen“, „Kind“ (nur Hanne, Ambros, Kesselmayr), „Kleines“ (Jupp).
- **Antwortmöglichkeiten:** meist drei. Eine warm, eine frech oder ehrlich, eine still (`(Nichts sagen)`, `(Nicken)`). Keine ist „falsch“, aber sie bewegen Beziehungen.
- **Ausrufe der Welt:** „Bei beiden Monden!“, „Stimm dich ab!“ (= beruhig dich), „Hallkopf“ (= Dummkopf). Fluchen höchstens „verdammt“.
- **Emotion und Kamera** nur an Wendepunkten setzen (`[event emotion …]`, `[event camera …]`), nicht in jeder Zeile.

## Die Figuren

| Figur | Wie sie spricht | Tick / typischer Satz | Nie |
|---|---|---|---|
| **Lio** | schnell, laut, springt vom Witz zur Wahrheit und zurück | übertreibt Zahlen („Dreihundertzwölf Stufen. Zweimal gezählt.“), lenkt bei Mara mit einem Witz ab | lange ernste Reden (wenn doch, bricht er sie selbst ab) |
| **Aurelia** | kontrolliert, knapp, präzise, ehrlich bis zur Grausamkeit | sagt Dinge zweimal, wenn sie sie ernst meint („Danke. … Das war ernst gemeint. Danke.“) | jammern, Mitleid zeigen, „bitte“ (bis Akt 2) |
| **Edda** | leise, trocken, kurze Hauptsätze, siezt | korrigiert Lio mit vollem Nachnamen („Fünfzig, Herr Vandermeer.“) | Lob mit Adjektiv (sie sagt „Gut.“, nicht „Sehr gut!“) |
| **Grell** | laut, knapp, Befehlston | „Nächste Frage.“ · wiederholt einzelne Wörter mit Punkt („Kreis. Rund.“) | über Brandheim reden (bis Akt 3) |
| **Saphira** | weich, singend, redet über Tiere wie über Menschen | „Hört ihr das?“ · beschreibt Gefühle von Tieren | laut werden (außer, jemand tut einem Tier weh) |
| **Brams** | ZU LAUT, entschuldigt sich sofort, Glockengießer-Sprache | „GUT! Verzeihung. Gut.“ | flüstern (kann er nicht) |
| **Kesselmayr** | großmütterlich, lacht über Missgeschicke | nennt Schüler „Kind“ und „Rußnäschen“ | schimpfen über Explosionen |
| **Ollendorf** | nervös, höflich, verliert den Faden | erklärt, was sein Hut gerade ist („Mein Hut ist jetzt ein Igel.“) | jemanden bloßstellen |
| **Morgenroth** | ruhig, poetisch, nachts, sehr direkt | beschreibt alles über Klang statt Bild | „Schauen Sie mal“ (sie sagt „Hören Sie mal“) |
| **Varga** | wenige, gesetzte Worte, politisch | „Sie wünschen?“ · antwortet mit Grundsatz statt Gefühl | mehr als drei Sätze am Stück |
| **Corvin** | warm, witzig, ironisch, hört zu, stellt Gegenfragen | macht sich selbst klein („Klingt langweilig. Ist es auch.“) | drohen (bis M406; danach ruhig und müde) |
| **Ambros** | brummig, gemütlich, Musik-Metaphern, schweift ab | vergleicht Menschen mit Instrumenten („Klingst wie eine Glocke in Watte.“) | über die Eltern reden (bis M408) |
| **Kessel Klaus & Brüder** | Aschenhain brummig, Silberquell belehrend, Sturmkamm frech, Moosgrund mütterlich („Klärchen“) | „Ich merk mir alle Gesichter.“ · pfeift beleidigt | – |
| **Ottilie** | Gezeter, Großbuchstaben, Amtssprache | „Name, Turm, Ausrede! In dieser Reihenfolge!“ · zählt Ausreden | zugeben, dass sie sich über Komplimente freut |
| **Mirela** | leise, sachlich über unheimliche Dinge, sehr lieb | „Du stehst auf einem Nachklang.“ · erzählt Echos wie Freunde | gruselig sein wollen (sie ist es aus Versehen) |
| **Panki** | begeistert, technisch, optimistisch | „Theoretisch.“ · sagt Unfälle als Feature an | aufgeben |
| **Tilda** | ernst, stur, kindlich-würdevoll | übersetzt Herrn Quaak („Er sagt tschüss.“) | zugeben, dass Herr Quaak ein Frosch ist |
| **Ruven** | lässig, elegant, Geschäftston | „Ihr habt mich nie gesehen.“ · Regeln der Schiefen Töne | gemein sein |
| **Hanne** | trocken, warm unter der Schale | tut so, als wäre ihr alles egal | weinen (vor Kindern) |
| **Jupp** | herzlich, derb, Musikantensprüche | Meisterregel: „Wer lernen will, gibt erst was zurück.“ | ohne Gegenleistung lehren |
| **Mutter Gundel** | resolut, nennt alle „Schatz“ | Essen als Trost („Ein Knödel extra.“) | Leute hungrig gehen lassen |
| **Varn (manipuliert)** | Wörter dreimal, im Takt | „Gut. Gut. Gut.“ · „Eins. Zwei. Drei. Vier.“ | normale Sätze |
| **Nym** | sanft, rätselhaft, Echo am Satzende (kursiv, leiser) | „… *mich … mich …*“ | lange Erklärungen (bis M309) |
| **Der Dirigent** | höflich, samtig, langsam, siezt, nie laut | „Ich lade Sie ein, zuzuhören.“ · erklärt statt droht | schreien (einziger lauter Satz: Ende M508) |
| **Fährmann** | nur Wetterberichte | „Morgen Ostwind.“ | etwas anderes |

## Freie Gespräche (Hub-Aufbau)

Jeder wichtige NPC hat ein freies Gespräch (`D_Schule_<NPC>`), das man jederzeit führen kann:
1. **Begrüßung**, abhängig vom Fortschritt (`[if {quest.M108} == 2]` …).
2. **Themen-Menü** `:themen` mit 3–4 Themen; neue Themen erscheinen mit dem Fortschritt (`[if …]`-Block um die Auswahl).
3. **Abschied** als letzte Auswahl.
4. Kleine Beziehungs-Boni (`[event state `rel:Lio+1`]`) nur für echte Zuwendung (zuhören, Musik), nie fürs bloße Anklicken.

Vorhanden (Akt 1): Lio, Aurelia, Brams, Kesselmayr, Ollendorf, Morgenroth, Varga, Ottilie, Panki, Tilda, Mirela, Gundel. Edda, Grell und Saphira sprechen in Akt 1 vor allem in Missionen; ihre freien Gespräche folgen mit Akt 2 (dann mit Geheimnissen, die sich öffnen).
