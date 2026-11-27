# Zauber-Effekte (Niagara) – Plan und Ablage

Ziel (Nutzer, 27.11.): weg vom „Grundformen-Look“, hin zu glänzenden, wuchtigen, fotorealistischen Effekten wie in großen
Spielen. Jede Weise hat ihre eigene Wirkung, Farbe und Geste. Eigenes Design, keine Nachbauten fremder Marken.

## Ablage und Anschluss (Code fertig)
- Pfad je Weise, **ohne JSON-Eintrag**: `/Game/VFX/Spells/NS_<id>` (Flug, Strahl, Schild, Welle) und
  `/Game/VFX/Spells/NS_<id>_Impact` (Einschlag). Mit `vfx.niagara` / `vfx.impact_niagara` lässt sich das überschreiben.
- Fehlt ein Asset, läuft der bisherige Platzhalter-Look (Lichtkugeln, Blitz-Strahl). Man kann also Weise für Weise umstellen.
- Der Code setzt diese **User-Parameter** (im System als User-Parameter anlegen, Namen exakt):
  - `User.Color` (LinearColor) Kern, `User.GlowColor` Saum/Glühen, `User.SparkColor` Funken (Akkord-Farbregel 1/2/3)
  - `User.BeamEnd` (Vector, Weltkoordinate) nur bei Strahl-Weisen (`delivery: beam`)
  - Skalierung kommt über die Komponenten-Skalierung (`vfx.scale`).
- Geschosse hängen `NS_<id>` an die Kollisionskugel (Schweif). Sobald es existiert, wird die Platzhalter-Kugel ausgeblendet.

## Quellen (Budget 0 €)
- **Paragon: Gideon** (Fab Standard License, schon im Projekt, CREDITS-Eintrag da): Texturen, Materialien und Partikel aus
  seinen Zaubern (Portal, Schwarzes Loch, Meteor, Geschosse) als Bausteine. Nur in Unreal nutzen (Lizenz).
  Cascade-Systeme bei Bedarf mit dem Engine-Plugin „Cascade to Niagara Converter“ umwandeln.
- Niagara-Vorlagen der Engine (`/Niagara/DefaultAssets/Templates/…`): Burst, Fountain, Beam, Dynamic Beam, Ribbon.
- ambientCG (CC0) für Rausch-/Wolkentexturen; eigene Flipbooks nur, wenn nötig.
- Jede neue externe Quelle vorher in `CREDITS.md`.

## Look je Weise (10 Kampfweisen)
| Weise | Datei(en) | Look |
|---|---|---|
| funkenstoss (Scintivox) | NS_funkenstoss (Beam), _Impact | Zickzack-Blitz aus den Fingern, heller Kern + farbiger Saum, kleine Äste, Funkenregen beim Einschlag; 3. Schlag dicker mit Druckring. Mündungsblitz klein (Figur darf nicht weiß überstrahlen). |
| sturmweise (Tempestivox) | NS_sturmweise (Beam), _Impact | Wie Funkenstoß, aber violett-blau, dicker, springt (Code macht die Sprünge, jedes Segment = eigener Beam). Elektrische Bögen am Ziel. |
| ziehklang (Trahisonus) | NS_ziehklang (Beam) | Ruhiges, pulsierendes Klangband (Ribbon) mit wandernden Notenringen zum Wirker hin. |
| frostweise (Gelucantus) | NS_frostweise, _Impact | Eislanze mit Frostnebel-Schweif und glitzernden Kristallsplittern; Einschlag: Eissplitter-Burst, Reif am Boden. |
| glutweise (Ardesonus) | NS_glutweise, _Impact | Glutkugel mit Flammen-Schweif, Funken und Hitzeflimmern; Einschlag: Flammenstoß + Glutfunken + Rauch. |
| hammerton (Malletonus) | NS_hammerton | Kegel-Druckwelle: verzerrte Luft, Staub vom Boden, goldene Klangbögen nach vorn. |
| paukenschlag (Tympaplausus) | NS_paukenschlag | Klatschen: Ring-Schockwelle am Boden, Staubring, kurze Lichtringe in die Höhe. |
| haltelied (Tenecantus) | NS_haltelied, _Impact | Geschoss = schwebende Notenringe; hält das Ziel mit kreisenden Lichtringen. Aufprall (Cadetonus): Bodenriss-Staubwelle. |
| glockenschild (Campanimur) | NS_glockenschild | Glockenförmige, schimmernde Kuppel (Fresnel), wandernde Klangwellen bei Treffern. |
| reinklang_konter (Purisonus) | NS_reinklang_konter, _Impact | Heller, reiner Lichtpfeil, weißgoldener Blitz beim Einschlag. |

## Reihenfolge
1. funkenstoss + Mündungsblitz (am häufigsten zu sehen), 2. frostweise, 3. glutweise, 4. sturmweise, 5. Rest.
Pro Weise: anlegen, im Spiel ansehen (Testarena), Bild machen, Nutzer zeigen.

## Erledigt im Code (27.11.)
- Mündungsblitz und Leucht-Halos deutlich schwächer (vorher wurde die Figur beim Funkenstoß ganz weiß).
- Alle drei Akkord-Farben gehen an Niagara; feste Ablage-Pfade ohne JSON.
