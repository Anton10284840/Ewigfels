# Quests, Dialoge, Entscheidungen, Speichern – Format und Regeln (Phase 2)

Neue Quests sind **nur Daten**: eine JSON-Datei + optional SUDS-Dialoge. Kein C++, kein Blueprint.
Nach jeder Änderung: `.\Scripts\Tools\Validate-Data.ps1` (prüft alles und zeigt die Spielzeit).

## 1. Weltstatus (Entscheidungen und Konsequenzen)

`UEwigfelsWorldStateSubsystem` merkt sich drei Arten von Werten (alle Ganzzahlen):

| Bereich | Beispiel | Bereich | Bedeutung |
|---|---|---|---|
| `flag:` | `flag:E1_TrustEdda` | beliebig, 0 = nicht gesetzt | globale Schalter und Zähler (z. B. `NachhallLoad`) |
| `rel:` | `rel:Lio` | −100 … +100 | Beziehung zu einer Figur |
| `rep:` | `rep:Kollegium` | −100 … +100 | Ruf bei einer Fraktion |
| `quest:` | `quest:M101` | nur lesen | Quest-Status: 0 offen, 1 aktiv, 2 fertig, 3 gescheitert |

Ruf-Stufen (GDD §7.1): **Feind** ≤ −60 · **Misstrauisch** −59 … −20 · **Neutral** −19 … 19 · **Geschätzt** 20 … 59 · **Verbündet** ≥ 60.

Alle erlaubten Schlüssel stehen in [`Content/Data/World/WorldState.json`](../Content/Data/World/WorldState.json). Neues Flag = dort eine Zeile eintragen, sonst meldet das Prüfskript einen Tippfehler.

**Bedingungen** (Text): `flag:X` (gesetzt) · `!flag:X` (nicht gesetzt) · `rel:Lio>=30` · `rep:Kollegium>=Geschaetzt` · `quest:M101` (fertig) · `!quest:M101` · `quest:M101=active`. Vergleiche: `>= <= == != > < =`.
**Effekte** (Text): `flag:X=1` (setzen) · `flag:NachhallLoad+5` · `rel:Lio+10` · `rep:Zuenfte-15`.

## 2. Quest-Datei

Ort: `Content/Data/Quests/Akt<N>/<ID>.json`, Dateiname = ID. Beispiele: [M101](../Content/Data/Quests/Akt1/M101.json) (linear), [M110](../Content/Data/Quests/Akt1/M110.json) (Entscheidung mit Verzweigung).

```jsonc
{
  "id": "M110",                    // Pflicht, eindeutig
  "type": "main",                  // main | series | faction | side
  "act": 1, "region": "Nocturna", "giver": "Edda",
  "duration_min": 55,              // Pflicht, für den Spielzeit-Rechner
  "auto_start": true,              // startet von selbst, sobald "requires" passt
  "requires": ["quest:M109"],      // Bedingungen, alle müssen passen
  "title":   { "de": "…", "en": "…" },   // jeder Text braucht de + en
  "summary": { "de": "…", "en": "…" },
  "dialogues": ["D_M110_Edda"],    // .sud-Dateien, die dazugehören (wird geprüft)
  "start_stage": "s1_traum",       // optional, sonst die erste Etappe
  "stages": [
    {
      "id": "s1_traum",
      "text": { "de": "…", "en": "…" },          // steht im Questlog
      "objectives": [
        { "id": "o_schlafen", "kind": "interact", "target": "Bett_Turmzimmer",
          "count": 1, "marker": "Turmzimmer_Bett", "optional": false,
          "text": { "de": "Leg dich schlafen", "en": "Go to sleep" } }
      ],
      "on_complete": ["flag:Nym_Spoke=1"],       // Effekte, wenn alle Pflicht-Ziele erledigt sind
      "next": [                                  // erste passende Regel gewinnt
        { "if": ["flag:E1_TrustEdda"], "stage": "s3a_vertrauen" },
        { "stage": "s3b_verbergen" }             // Regel ohne "if" = Rückfall
      ]                                          // statt "stage": "end": "completed" | "failed"
    }
  ],
  "rewards": { "xp": 400, "gold": 50, "items": ["Ring_Doppelnacht"], "effects": ["rep:Hueter+5"] }
}
```

Ohne `next` endet die Quest nach der Etappe erfolgreich.

**Ziel-Arten (`kind`)** und wer sie meldet:

| kind | target | gemeldet von |
|---|---|---|
| `talk` | NPC-ID (`Edda`) | NPC-Interaktion: `ReportEvent("talk", "Edda")` |
| `dialogue` | Dialog-Name (`D_M101_Lio`) | automatisch, wenn der SUDS-Dialog endet |
| `reach` | Marker-ID | Trigger-Volumen in der Welt |
| `collect` / `defeat` / `interact` / `cast` | Item-, Gegner-, Objekt-, Weisen-ID | Inventar, Kampf, Interaktion, Magie |
| `custom` | frei | eigener Code / Konsole |
| `condition` | Bedingung (`flag:E1_Decided`) | erledigt sich selbst, sobald der Weltstatus passt |

`marker` = Wegpunkt für Kompass und Karte (UI folgt in Phase 5).

## 3. Dialoge (SUDS)

Ort: `Content/Data/Dialogues/Akt<N>/<Name>.sud` (Text, im Repo). Im Editor importiert SUDS die Datei automatisch als Asset daneben (das Asset bleibt lokal). Sprache des Skripts: Deutsch; Englisch über den Localization Dashboard (SUDS legt dafür eine String Table an).
Grundsyntax: [SUDS-Doku](https://github.com/sinbad/SUDS/blob/master/docs/ScriptReference.md). Beispiele: [D_M101_Lio](../Content/Data/Dialogues/Akt1/D_M101_Lio.sud), [D_M110_Edda](../Content/Data/Dialogues/Akt1/D_M110_Edda.sud).

Verbindung zum Weltstatus (`UEwigfelsDialogueBridge`, hängt automatisch an jedem Dialog, der über `UEwigfelsDialogueBridge::StartDialogue` läuft):

| Im Skript | Wirkung |
|---|---|
| `[if {rel.Lio} >= 30]` · `[if {flag.E1_TrustEdda} > 0]` · `[if {quest.M101} == 2]` | liest den Weltstatus (Flags mit `> 0` prüfen) |
| `[set flag.E1_TrustEdda true]` · `[set rel.Lio {rel.Lio} + 10]` | schreibt in den Weltstatus |
| `[event state `rep:Kollegium+5`]` | Effekt, gleiche Syntax wie im Quest-JSON |
| `[event quest.start `M102`]` · `[event quest.report `talk`, `Edda`]` | Quest starten / Fortschritt melden |
| `[event quest.objective `M101`, `o_lio`]` · `[event quest.stage `M101`, `s2`]` · `[event quest.fail `M101`]` | Ziel abhaken / Etappe setzen / scheitern |
| `[event emotion `angry`]` · `[event camera `closeup`]` | geht an `OnCue` (Mimik, Dialogkamera: Phase 3) |
| `[set global.X …]` | SUDS-eigene globale Variable, wird mitgespeichert |

**Freie Gespräche** (NPC einfach ansprechen, ohne Quest) heißen `D_Schule_<NPC>`, `D_Dorf_<NPC>` oder `D_Welt_<NPC>` und brauchen keine Quest. Aufbau: Begrüßung je nach Fortschritt, dann ein Themen-Menü (`:themen` + `[goto themen]`). Themen, die erst später passen, stehen in einem `[if …]`-Block um die Auswahl (`[if {quest.M108} == 2]` / `* Text` / `[endif]`).

Sprecher-Anzeigenamen im Kopf setzen: `[set SpeakerName.Edda "Magistra Vornholt"]`.

## 4. Speichern und Laden

`UEwigfelsSaveSubsystem` speichert: Weltstatus, Quests (Status, Etappe, Ziel-Fortschritt, verfolgte Quest), globale Dialog-Variablen, Level, Spielerposition und alle Actors mit **`EwigfelsSaveable`**-Komponente.

- Slots: `Slot_1` … (manuell), `Auto_0` … `Auto_2` (Autosave alle 5 min, rotierend), `Quick`. Das Menü liest die Liste aus `GetSlots()`.
- Autosave sperren: `BlockAutosave("Kampf")` … `UnblockAutosave("Kampf")` (Kampf, Dialog, Cutscene).
- Actor speicherbar machen: Komponente **EwigfelsSaveable** hinzufügen, Variablen im Blueprint mit Häkchen **SaveGame** (Details > Advanced). Eingesammelt/zerstört: `DestroyPersistently()`. Optik nach dem Laden anpassen: Event `OnRestored`.
- World Partition: Entladene Actors behalten ihren Zustand, beim Wiedereinladen wird er hergestellt.
- Format-Version: `EwigfelsSave::CurrentVersion`. Bei Änderungen erhöhen und in `Migrate()` nachziehen, damit alte Spielstände weiter laden.

## 5. Konsolenbefehle zum Testen (im Spiel `~`)

```
Ewigfels.State.Dump                      Weltstatus anzeigen
Ewigfels.State.Apply rel:Lio+10          Wert ändern
Ewigfels.State.Check rep:Kollegium>=Geschaetzt
Ewigfels.Quest.List                      alle Quests mit Status
Ewigfels.Quest.Start M110                Quest erzwingen
Ewigfels.Quest.Report talk Edda          Fortschritt melden
Ewigfels.Quest.Reload                    JSONs neu einlesen (ohne Neustart)
Ewigfels.Save Slot_1 / Ewigfels.Load Slot_1 / Ewigfels.Save.List / Ewigfels.Autosave
```

## 6. Noch offen (spätere Phasen)

- Questlog-, HUD- und Lademenü-UI (CommonUI, Phase 5), Kompass-Marker in der Welt.
- XP/Gold/Items aus `OnQuestRewarded` abholen (Progression/Inventar).
- Dialog-Kamera und Mimik über `OnCue` (Phase 3).
- Quest-Texte kommen zurzeit direkt aus dem JSON (Sprache beim Laden). Später per Python-Skript in String Tables überführen, damit der Sprachwechsel im laufenden Spiel klappt.
