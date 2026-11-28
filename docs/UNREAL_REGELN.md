# Unreal-Regeln (UE 5.8, C++)

Kurze Stolperfallen-Liste für Claude und alle, die am Code arbeiten. Ergänzt die Konventionen in `CLAUDE.md`
und gilt nur, wo sie unseren Entscheidungen nicht widerspricht (eigenes Zaubersystem ohne GAS, SUDS, eigenes Save-System,
server-autoritativ für Koop).

Angeregt durch die Sammlung „awesome-gamedev-agent-skills“ (Apache 2.0, Eintrag in `CREDITS.md`). Mit eigenen Worten
zusammengefasst und auf Nocturna zugeschnitten; nichts davon wird ausgeführt oder installiert.

## 1. Speicher und Zeiger (häufigste Absturzursache)
- Jeder Member, der ein `UObject`/Actor hält, ist `UPROPERTY()` mit `TObjectPtr<T>`. Auch Container: `UPROPERTY() TArray<TObjectPtr<…>>`.
  Ein roher `UObject*` ohne `UPROPERTY` wird vom Garbage Collector nicht gesehen und kann jederzeit verschwinden. `IsValid()` rettet einen hängenden Rohzeiger nicht.
- Referenzen, die das Ziel **nicht** am Leben halten sollen (Zielgegner, Zuhörer, gemerkter Spieler): `TWeakObjectPtr<T>`, vor jeder Nutzung prüfen.
- In Nicht-`UObject`-Klassen: `TStrongObjectPtr<T>` oder `FGCObject` mit `AddReferencedObjects` + `GetReferencerName`.
- `AddToRoot()` nur im Notfall, immer mit `RemoveFromRoot()`.

## 2. Klassen und Komponenten
- `GENERATED_BODY()` als erste Zeile im Klassenrumpf, `#include "Name.generated.h"` als **letztes** Include.
- Standard-Komponenten nur im Konstruktor mit `CreateDefaultSubobject` + `SetupAttachment`. Zur Laufzeit `NewObject` + `RegisterComponent()` + `AttachToComponent` (ohne `RegisterComponent` tickt/rendert nichts).
- Überschriebene Funktionen rufen immer `Super::` auf (`BeginPlay`, `EndPlay`, `Tick`, `GetLifetimeReplicatedProps`, `SetupPlayerInputComponent`, `NotifyControllerChanged`).
- Kein Gameplay im Construction Script (läuft bei jeder Editor-Änderung). Start-Logik in `BeginPlay`.
- Neues Modul eingebunden → Eintrag in `Ewigfels.Build.cs` (`Public…` wenn Header es brauchen, sonst `Private…`). „Unresolved external symbol“ heißt fast immer: Modul fehlt dort.

## 3. Koop/Netzwerk (ergänzt die Mehrspieler-Regel in `CLAUDE.md`)
- `bReplicates = true` im Konstruktor, sonst repliziert gar nichts. `#include "Net/UnrealNetwork.h"` für `DOREPLIFETIME`.
- Der GameMode existiert nur auf dem Server. Alles, was Clients sehen müssen, gehört in GameState, PlayerState oder replizierte Komponenten.
- Pro Spieler: Zustand in PlayerState, Eingabe/UI im PlayerController, Körper im Character, wiederverwendbares Verhalten in Komponenten.
- Optik (Niagara, Licht, Sound) auf Dedicated Servern nicht erzeugen; Spawn-Funktionen liefern dort ohnehin `nullptr`.

## 4. Eingabe (Enhanced Input)
- Mapping Context erst hinzufügen, wenn der Pawn einen lokalen Controller hat (`NotifyControllerChanged`/`PawnClientRestart` oder PlayerController), nicht in `BeginPlay`. So machen wir es schon in `EwigfelsCharacter.cpp`.
- `Triggered` für Dauerwerte (Bewegen, Umschauen), `Started`/`Completed` für Drücken/Loslassen. `Triggered` bei Tasten feuert jedes Bild.
- Wert mit dem passenden Typ lesen (`Axis2D` → `Get<FVector2D>()`), sonst immer 0.
- Den **Combo-Trigger** von Enhanced Input nicht nutzen (in 5.8 als unzuverlässig abgekündigt). Zauber-Combos bleiben in unserer eigenen Logik.
- Tastenbelegung ändern über die Enhanced-Input-User-Settings (pro Spieler gespeichert), nie das IMC-Asset selbst umschreiben.
- Gleiche Taste in zwei Contexts: der mit höherer Priorität schluckt sie. Beim Moduswechsel (Zauberrad, Kleiderschrank, Dialog) Context entfernen/hinzufügen.
- Prüfen im Spiel: Konsole `showdebug enhancedinput`.

## 5. Niagara (Zauber-Optik)
- Von Gameplay setzbare Werte (Farbe, Stärke) nur im Namensraum `User.` anlegen; der Name muss exakt passen (bei uns `User.Color`).
- Spawn-Rückgabe immer auf `nullptr` prüfen (kein Asset, Server, Skalierung hat es weggeschnitten).
- Einmal-Effekte nicht-loopend mit Auto-Destroy; häufige Einmal-Effekte (Treffer, Funken, Schritte) mit **Pooling** (`ENCPoolMethod::AutoRelease`) und den zurückgegebenen Zeiger dann nicht behalten.
- Loopende Effekte: Komponente als `UPROPERTY` merken und mit `Deactivate()` beenden, sonst stapeln sie sich.
- Achtung Argument-Reihenfolge: bei `SpawnSystemAttached` steht `PoolingMethod` **vor** `bAutoActivate`, bei `SpawnSystemAtLocation` danach.
- CPU-Emitter für alles, worauf Gameplay reagiert; GPU nur für reine Optik in großer Menge, dann mit festen Bounds (sonst verschwindet der Effekt am Bildrand).
- Kosten prüfen: `stat Niagara`, `fx.Niagara.Debug.Hud Enabled=1`.

## 6. KI (Gegner)
- Pro Gegner veränderlicher Zustand nie in Membern nicht-instanzierter BT-Tasks (die teilen sich alle Gegner) → Node-Memory oder StateTree-Instanzdaten.
- Blackboard-Schlüssel nie als freie Strings tippen (Tippfehler scheitern still) → `FBlackboardKeySelector` oder eine gemeinsame Konstante.
- Latente Tasks immer mit `FinishLatentTask` beenden, auch im Abbruch-Pfad. Decorator ohne „Observer Aborts“ reagiert nicht auf Schlüsseländerungen.
- `MoveTo` scheitert sofort → Navmesh fehlt oder Ziel liegt daneben. EQS läuft nur auf dem Server.
- StateTree nach Änderungen neu kompilieren, bevor man im PIE testet.

## 7. Blueprints (wenn wir welche nutzen)
- Ketten aus `Cast To` und harten Referenzen laden ganze Asset-Bäume mit. Lieber Interfaces, Dispatcher, Soft References. Prüfen mit Reference Viewer/Size Map.
- Dispatcher-Zuhörer in `EndPlay`/`Destruct` wieder abmelden.
- Heiße Logik gehört in C++ (Blueprint-Nativierung gibt es in UE5 nicht mehr).

## 8. Bauen und Testen
- Bauen nur bei geschlossenem Editor (steht schon in `Build-Ewigfels.ps1`). Live Coding nur für Änderungen in Funktionsrümpfen; nach Header-Änderungen Editor zu und voll bauen.
- „Cannot open include file …generated.h“: Generated-Include nicht zuletzt, `GENERATED_BODY()` fehlt, oder Datei bei offenem Editor angelegt (Projektdateien neu erzeugen).
- Testbare Regeln in einfache Structs/Funktionen legen, damit die meisten Tests keine Welt brauchen.
- Nach dem Testlauf prüfen, dass die erwartete **Anzahl** Tests lief: ein falscher Filter führt nichts aus und sieht trotzdem grün aus.
- Nie „sollte kompilieren“ melden: sagen, was gebaut/getestet wurde, mit den Ergebniszeilen.

## 9. Packen (später, Phase Release)
- Startkarte in Project Settings setzen; Karten, die nur per Name geladen werden, und nur per Pfad geladene Assets (z. B. aus unseren JSON-Daten!) in die Cook-Liste aufnehmen, sonst fehlen sie im fertigen Spiel.
- Vor dem Packen `-run=DataValidation` laufen lassen. Mit UAT `BuildCookRun … -pak` bauen; nur der gestagte/archivierte Ordner ist auslieferbar.
- Shipping hat keine Konsole und kein `quit`: das Spiel braucht einen eigenen „Beenden“-Knopf.

## Offene Punkte aus dem Abgleich mit unserem Code
- [ ] `EwigfelsSpellVisuals::SpawnNiagaraAt` erzeugt Treffer-/Funkeneffekte ohne Pooling. Wenn viele Zauber gleichzeitig fliegen, auf `ENCPoolMethod::AutoRelease` umstellen (vorher prüfen, dass niemand den Rückgabezeiger behält). Braucht PC-Build.
- [ ] `Test-Ewigfels.ps1` meldet nur „bestanden/fehlgeschlagen“. Ergänzen: Anzahl gelaufener Tests ausgeben und bei 0 als Fehler werten.
- [ ] Vor dem ersten Paket-Build: Cook-Liste für alle Assets, die nur über `Content/Data/**/*.json` per Pfad geladen werden.
