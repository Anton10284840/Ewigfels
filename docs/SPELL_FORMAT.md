# SPELL_FORMAT – Kampfwerte für Weisen, Status und Gegner

Technisches Format für alles, was im Kampf wirkt (Phase 3). Das **Design** der 430 Weisen steht in [WEISEN.md](WEISEN.md) und im Katalog (`Content/Data/Spells/Katalog/`). Diese Datei beschreibt die **spielbaren Werte**, die der C++-Lader liest.

- Lader: `UEwigfelsSpellSubsystem` (Weisen, Status, Start-Ausrüstung), `UEwigfelsCreatureSubsystem` (Gegner).
- Ordner: `Content/Data/Spells/**/*.json` (ohne `Katalog/`) und `Content/Data/Creatures/**/*.json`. Jede Datei darf `spells`, `statuses` und `loadout` enthalten.
- Prüfen (auch in der Cloud): `python Scripts/Tools/validate_combat.py`. Selbsttest: `python Scripts/Tools/test_validate_combat.py`.
- Im Spiel neu laden ohne Neustart: Konsole `Ewigfels.Spells.Reload`, Liste: `Ewigfels.Spells.List`, `Ewigfels.Creatures.List`.
- Felder mit `_` am Anfang sind Kommentare. Unbekannte Felder (z. B. `learn`, `combo` aus dem Katalog) landen in `Extra` und gehen nicht verloren.

## 1. Weise (`spells[]`)

```json
{
  "id": "glutweise",               // feste ID, klein, ohne Leerzeichen
  "catalog_id": "K25",             // Verweis auf die Kernweise im Katalog (Akkord + Name werden verglichen)
  "kind": "kern",                  // kern | alltag | koop | gegner  (Standard: kern)
  "name": { "de": "Glutweise", "en": "Ember Air" },
  "incantation": "Ardesonus",      // Zauberwort, lateinisch klingend, selbst erfunden (Prüfskript sperrt fremde Marken)
  "description": { "de": "…", "en": "…" },
  "school": "Elementar",
  "chord": ["Glut", "Glut", "Strom"],   // genau 3 Grundtöne; bei kind=gegner optional
  "unlock": "akt1",                // start | akt1…akt5 | hidden (nur als Konter/intern)
  "cost": 18,                      // Atem
  "cooldown": 3.5,                 // Sekunden
  "cast_time": 0.3,                // Ausholen bis zum Auslösen (Sek.), wird durch Status verlangsamt
  "recovery": 0.3,                 // Nachlauf (Sek.)
  "hands": "one",                  // one | two  (zwei Hände = langsamer laufen beim Zaubern)
  "delivery": { "type": "projectile", … },   // wie die Weise ankommt, §1.1
  "effects": [ … ],                // was beim Treffer passiert, §1.2
  "recast": { "window": 4, "effects": [ … ] },   // zweiter Druck auf gehaltene Ziele (Haltelied → Schmettern), §1.3
  "on_land": [ … ], "land_radius": 280,          // wirkt beim Aufprall eines geschleuderten/geschmetterten Ziels, §1.3
  "on_parry": [ … ],               // nur Schild: Effekte bei gelungener Parade
  "counter_spell": "reinklang_konter",       // nur Schild: wird nach der Parade automatisch ausgelöst
  "vfx": { "color": "#FF8A2A", "glow_color": "#…", "spark_color": "#…", "scale": 0.9, "shape": "orb",
           "beam_style": "bolt", "niagara": "/Game/VFX/…", "impact_niagara": "/Game/VFX/…" },
  "anim": { "montage": "/Game/Animations/Magic/AM_Magic_Glutweise", "gesture": "rub" },
  "sfx":  { "cast": "/Game/Audio/…", "impact": "/Game/Audio/…" }
}
```

**Jede Weise individuell (Ziel für alle ~430):** eigene Wirkung (Effekt-Baukasten §1.2/§1.3), eigene Farbe, eigene Geste, eigenes Zauberwort.

- **Farben aus dem Akkord:** 1. Grundton = Kern (`color`), 2. = Glühen/Saum (`glow_color`), 3. = Funken/Splitter (`spark_color`). Jedes Feld in `vfx` überschreibt einzeln. Niagara bekommt `User.Color`; Gegnerweisen ohne Akkord nehmen überall `color`.
- **Geste:** `anim.gesture` = `flick` (Schnippen), `punch` (Stoß), `stiff` (Finger spreizen), `clap` (Klatschen), `lift` (Hand hebt), `slam` (Faust runter), `grab` (Greifen + Reißen), `rub` (Finger reiben + Wurf), `twitch` (Zucken). Ohne eigene `montage` spielt das Spiel `/Game/Animations/Magic/Gesten/AM_Geste_<gesture>`; die Zweitwirkung (`recast`) spielt immer `slam`.
- **Strahl-Look:** `beam_style` `bolt` (Zickzack-Blitz mit Ästen) oder `tether` (ruhiges, pulsierendes Klangband, z. B. Ziehklang).

Fehlt ein Niagara-System oder eine Montage, nimmt das Spiel einen Platzhalter (leuchtende Form in `vfx.color`, Körper-Pose). Man kann also Werte testen, bevor es Effekte gibt. `shape`: `orb` (Kugel), `lance`/`cylinder`, `cone`, `cube`.

### 1.1 `delivery`

| type | Felder (Standard) | Wirkung |
|---|---|---|
| `projectile` | `speed` 3000, `radius` 15, `range` 3000, `homing` 0–1 (0), `gravity` (0), `chain` (1), `chain_window` 0.6, `pierce` (0), `explode_radius` (0) | Geschoss aus den Fingern. `chain` = Schlagkette (z. B. 3er-Funken), `pierce` = durchschlägt n Gegner, `explode_radius` = Flächenschaden beim Aufprall |
| `beam` | `range` 3000, `radius` 15, `chain` (1), `chain_window` 0.6, `pierce` (0), `bounces` (0), `bounce_range` 900 | `bounces` = springt vom Ziel auf so viele weitere Gegner (Kettenblitz, Sturmweise). Sofortiger Strahl aus den Fingern bis zur ersten Wand (Funkenstoß als Blitz). Trifft das erste feindliche Ziel, mit `pierce` weitere dahinter. Optik: `vfx.niagara` bekommt den Endpunkt als `User.BeamEnd` (Vektor); ohne Niagara-System zeichnet das Spiel einen flackernden Blitz mit Verästelungen. Getroffene KI-Gegner frieren kurz ein (Treff-Stopp), der Wirkende bekommt einen Kamerastoß |
| `cone` | `range`, `angle` 60 | Kegel vor der Figur (Hammerton) |
| `area` | `radius` | Kreis um die Figur |
| `shield` | `parry_window` 0.2, `block_atem_per_damage` 0.5, `block_damage_mult` 0.2, `radius` | Halten = blocken (kostet Atem pro Schaden), im Fenster nach dem Hochnehmen = Parade |
| `self` | – | Wirkt nur auf die Zaubernde (Heilung, Atem) |

### 1.2 `effects[]`

| type | Felder | Wirkung |
|---|---|---|
| `damage` | `amount`, `element` (Grundton), `bonus {if_status, mult, consume}` | Schaden; Bonus z. B. ×2,5 gegen `frozen` und Eis zerbricht (`consume`) |
| `status` | `status`, `duration` | Status aus §2 setzen |
| `knockback` | `amount`, `up` | wegstoßen |
| `pull` | `amount` | heranziehen |
| `restore_atem` | `amount` | Atem zurück |
| `heal` | `amount` | Leben zurück |
| `stagger_attacker` | `amount` (Sek.) | Angreifer taumelt (bei Parade) |
| `lift` | `amount` (Höhe cm), `duration` | hebt das Ziel hoch und hält es in der Luft (Status `lifted`, Schweberinge) |
| `slam` | `amount` (cm/s nach unten) | schmettert ein gehaltenes Ziel zu Boden; beim Aufprall wirkt `on_land` |

Jeder Effekt kann `"chain_end_only": true` haben: wirkt nur beim letzten Schlag einer Kette.

### 1.3 Zweitwirkung und Aufprall

- `recast {window, effects}`: Hält die Weise ein Ziel (`lift`), löst der nächste Druck derselben Taste innerhalb von `window` s die `effects` auf alle gehaltenen Ziele aus – ohne Atem und Abklingzeit. Hält sie niemanden mehr, wird ganz normal neu gewirkt.
- `on_land` + `land_radius`: Wird ein Ziel von dieser Weise geschmettert (`slam`) oder gestoßen (`knockback`), wirken diese Effekte beim Bodenkontakt am Ziel und an allen Gegnern im Radius. Dazu gibt es Druckring, Funken und Kamerazittern.

## 2. Status (`statuses[]`)

```json
{ "id": "chilled", "name": {"de": "Unterkühlt", "en": "Chilled"}, "duration": 3.0,
  "damage_per_second": 0, "move_speed_mult": 0.55, "action_speed_mult": 0.75, "atem_regen_mult": 1,
  "stops_actions": false, "cancels": ["burning"], "escalate_at": 3, "escalate_to": "frozen", "color": "#9AD8FF" }
```

- Erneutes Setzen verlängert und zählt Stapel hoch. Bei `escalate_at` Stapeln wird daraus `escalate_to` (3× Unterkühlt = Gefroren).
- `cancels` hebt andere Status auf (Feuer taut Eis).
- `look`: wie der Status am Ziel aussieht – `ice` (Eisblock, Animation bleibt stehen), `notes` (kreisende Noten über dem Kopf), `flames` (flackernde Flammen), `rings` (Schweberinge), `tint` (nur Farbe im HUD). Farbe = `color`.
- Vorhanden: `burning` (flames), `chilled`, `frozen` (ice), `wet`, `stunned` (notes), `cursed`, `lifted` (rings, von `lift` gesetzt).

## 3. Start-Ausrüstung (`loadout`)

`"loadout": ["funkenstoss", "glockenschild", "hammerton", "frostweise", "haltelied", "ziehklang", "glutweise", "sturmweise"]` – Start-Belegung: Basis-Weise (linke Maus / RT), Schild (rechte Maus / LT), danach die **6 Plätze des Zauberrads** (Tasten 1–6, Controller: Rad mit RB, Plätze 1–4 auch aufs Steuerkreuz). Mehr als 6 werden ignoriert. Im Spiel wählt der Spieler im **Zauberbuch** (B / Menü-Taste) aus allen gelernten Weisen seine 6 (`UEwigfelsSpellCasterComponent::RequestSetWheelSlot`, server-autoritativ; gelernt = `KnownSpells`, in der Testarena alle Weisen ohne `"unlock": "hidden"` und ohne `"kind": "gegner"`). Steuerung: `docs/STEUERUNG.md`.

## 4. Gegner (`creatures[]` in `Content/Data/Creatures/`)

```json
{ "id": "Fluchsaenger", "name": {"de": "Fluchsänger", "en": "Hexsinger"},
  "faction": "Missklinger", "behavior": "ranged",      // melee | ranged | dummy
  "health": 60, "walk_speed": 240, "run_speed": 420,
  "aggro_radius": 3200, "attack_range": 2200, "preferred_range": 1300, "flee_range": 420,
  "spells": ["fluch_kugel", "fluch_kreis"],              // Gegnerweisen (kind: gegner)
  "element_mult": { "Glut": 1.5 },                       // Schwächen/Resistenzen
  "stagger_threshold": 12,                               // so viel Schaden auf einmal = Taumeln
  "look": { "body": "#1E1426", "accent": "#8A4DFF", "eyes": "#C9A2FF", "height": 1.1, "hood": true,
            "mesh": "/Game/…", "anim_class": "/Game/….…_C" } }
```

- `id` ist zugleich das Quest-Ziel: `{"kind": "defeat", "target": "Fluchsaenger"}`.
- Fernkämpfer: `flee_range` < `preferred_range` ≤ `attack_range` (prüft das Skript).
- Nahkämpfer greifen höchstens zu zweit gleichzeitig an (Kampf-Regie), die anderen kreisen.

## 5. Mehrspieler-Regel

Alle Werte wirken nur auf dem Server: Schaden, Status, Atem und Abklingzeiten werden dort berechnet und an alle repliziert. Clients schicken nur „ich will Weise X zaubern“. Eine neue Weise braucht deshalb keinen eigenen Netzwerk-Code, solange sie mit den Bausteinen oben auskommt. Neue Effekt- oder Delivery-Typen werden in `EwigfelsSpellEffects.cpp` bzw. `EwigfelsSpellCasterComponent.cpp` ergänzt und hier eingetragen.
