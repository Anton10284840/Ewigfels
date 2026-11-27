# Steuerung (Controller und Maus/Tastatur gleichwertig)

Die Anzeige wechselt die Tasten-Symbole automatisch auf das zuletzt benutzte Gerät
(`AEwigfelsCharacter::IsUsingGamepad`, erkannt über jede Taste, Maus-Bewegung und Sticks).

| Aktion | Controller | Maus/Tastatur |
|---|---|---|
| Laufen / Sprinten | linker Stick / L3 | WASD / Shift |
| Springen / Ausweichen | A / B | Leertaste / C (Alt) |
| Funkenstoß (Basis, 3er-Kette) | RT | linke Maus |
| Schild (kurz vor Treffer = Reinklang-Konter) | LT halten | rechte Maus halten |
| **Zauberrad** (6 Weisen) | RB halten, rechter Stick wählt, loslassen | Q halten, Maus wählt, loslassen |
| Gewählte Weise wirken | Y | F |
| Weise direkt wirken | Steuerkreuz = Plätze 1–4 | 1–6 |
| **Zauberbuch** (6 aus allen gelernten wählen) | Menü-Taste | B |
| Ziel aufschalten / wechseln | R3 / LB | Tab oder Mausrad-Klick / Mausrad |
| Sprechen / Dialog weiter | X | E |
| Hilfe | View | F1 |

## Zauberbuch
Oben die 6 Rad-Plätze, darunter alle gelernten Weisen als große Karten (Karussell).
Platz wählen (A / Klick), dann Karte wählen (A / Klick) = Weise liegt auf dem Platz; liegt sie schon woanders,
tauschen die Plätze. Tastatur: über einer Karte 1–6 drücken. Zurück: B / Esc / rechte Maus.
Solange das Buch offen ist, schluckt ein eigener Eingabe-Kontext (Priorität 100) alle Spieltasten.

## Code
- Aktionen und Tasten: `Character/EwigfelsInputSetup.cpp`
- Zauberrad, Geräte-Erkennung: `Character/EwigfelsCharacter.cpp` (`InputWheel*`, `UpdateSpellWheel`, `InputAnyKey`)
- Zauberbuch-Menü: `UI/EwigfelsSpellbookComponent.cpp`
- Zeichnen (Amulett, Weisen-Kugeln, Rad, Buch, Tasten-Symbole): `UI/EwigfelsHUD.cpp`
- Belegung ändern: `UEwigfelsSpellCasterComponent::RequestSetWheelSlot` → Server-RPC
