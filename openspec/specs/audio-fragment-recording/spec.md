# Audio Fragmenter Specification

## Purpose

Provides a stereo VST3 audio effect that records and plays short, deterministic fragments of the incoming signal with controllable mixing and timing.

## Requirements

### Requirement: Stereo-VST3-Insert
Das Plugin SHALL als VST3-Audioeffekt einen Stereo-Eingang und Stereo-Ausgang bereitstellen und kein Instrument oder MIDI-Ziel anbieten.

#### Scenario: Stereo-Layout
- **WHEN** der Host einen Stereo-Bus anfordert
- **THEN** verarbeitet das Plugin zwei Eingangskanäle und erzeugt zwei Ausgangskanäle

### Requirement: Parameter und State
Das Plugin SHALL `Fragment Length` in Millisekunden (20–2000 ms, Standard 222 ms) und `Dry/Wet` (0–100 %, Standard 100 %) als hostautomatisierbare Parameter anbieten. Beide Werte SHALL im Plugin-State gespeichert und wiederhergestellt werden.

#### Scenario: Parameter-State
- **WHEN** der Host beide Parameter setzt, speichert und restauriert den State
- **THEN** entsprechen die restaurierten Werte den gespeicherten Werten

### Requirement: Random Recent N=1
Das Plugin SHALL ausschließlich Random Recent mit festem `N = 1` implementieren. Nach jedem vollständigen Fragment SHALL genau das zuletzt vollständig aufgenommene Fragment als bearbeiteter Pfad sequenziell wiedergegeben werden. Vor dem ersten vollständigen Fragment ist der bearbeitete Pfad stumm.

#### Scenario: Recent-Auswahl
- **WHEN** ein vollständiges Fragment verfügbar wird
- **THEN** wird genau dieses zuletzt vollständige Fragment ausgewählt und sequenziell wiedergegeben

### Requirement: Zeitbasierte Fragmentierung
Fragment Length SHALL intern anhand der aktiven Samplerate in Samples umgerechnet werden. Fragmentgrenzen SHALL samplecursorbasiert und unabhängig von Host-Blockgrenzen sein. Eine neue Länge SHALL erst an der nächsten Fragmentgrenze aktiv werden; das aktuelle Fragment beendet sich mit seiner bisherigen Länge.

#### Scenario: Längenänderung
- **WHEN** Fragment Length mitten in einem Fragment geändert wird
- **THEN** wird die neue Sampleanzahl erst an der folgenden Fragmentgrenze aktiv

### Requirement: Echtzeitverarbeitung
Der Audiopfad SHALL vorallokierten begrenzten Speicher verwenden und keine Allokationen, Locks, Datei-I/O oder UI-Aufrufe ausführen. Reset und `prepareToPlay` SHALL flüchtige Audiodaten und Cursors löschen, aber Parameterwerte behalten.

#### Scenario: Reset
- **WHEN** der Host `prepareToPlay` erneut aufruft
- **THEN** beginnt die Fragmentverarbeitung mit leerem flüchtigem Audiostatus

### Requirement: Mischung
Die Ausgabe SHALL `((1 - wet) * input) + (wet * processedFragment)` entsprechen. Bei 0 % Wet ist sie ausschließlich trocken, bei 100 % Wet ausschließlich Fragment-Signal, und Zwischenwerte sind lineare Mischungen.

#### Scenario: Wet-Grenzwerte
- **WHEN** Dry/Wet auf 0 % oder 100 % gesetzt wird
- **THEN** ist die Ausgabe jeweils vollständig trocken oder vollständig fragmentiert

### Requirement: Deterministische Verarbeitung
Das Plugin SHALL bei identischem Audio und beliebigen Host-Blockgrößen identische Fragmentgrenzen und Ausgaben erzeugen.

#### Scenario: Blockgrößenunabhängigkeit
- **WHEN** identisches Audio mit unterschiedlichen Host-Blockgrößen verarbeitet wird
- **THEN** bleiben Fragmentgrenzen und Ausgaben deterministisch

#### Scenario: Stummer Start
- **WHEN** Wet auf 100 % steht und noch kein vollständiges Fragment verfügbar ist
- **THEN** bleibt der bearbeitete Pfad stumm
