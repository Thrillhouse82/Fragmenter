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
Das Plugin SHALL `Fragment Length` in Millisekunden (20-2000 ms, Standard 222 ms), `Dry/Wet` (0-100 %, Standard 100 %), `Recent Slices N` (Ganzzahl 1-8, Standard 4), den Modus `sliceLengthMode` mit `Milliseconds` und `Tempo Sync` (Standard `Milliseconds`) sowie `syncDivision` mit den verfügbaren geraden, punktierten und triolischen Notenwerten von 1/1 bis 1/16 als hostautomatisierbare Parameter anbieten. Alle Parameter SHALL im Plugin-State gespeichert und wiederhergestellt werden. Eine Änderung von `Recent Slices N`, `sliceLengthMode`, `syncDivision` oder `Fade` SHALL erst an der nächsten Fragmentgrenze wirksam werden.

#### Scenario: Parameter-State
- **WHEN** der Host alle Parameter setzt, speichert und restauriert den State
- **THEN** entsprechen die restaurierten Parameter den gespeicherten Werten

#### Scenario: N-Grenzen und Default
- **WHEN** das Plugin neu erzeugt wird oder der Host Werte außerhalb des Bereichs anfordert
- **THEN** ist N standardmäßig 4 und wird auf den Bereich 1-8 begrenzt

#### Scenario: Verzögerte N-Übernahme
- **WHEN** der Host N mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentwiedergabe unverändert und der neue Wert gilt ab der nächsten Fragmentgrenze

#### Scenario: Alter State ohne Fade-Parameter
- **WHEN** ein State aus einer älteren Plugin-Version ohne `Fade` geladen wird
- **THEN** wird `Fade` auf On gesetzt und die übrigen Werte werden unverändert restauriert

#### Scenario: Verzögerte Fade-Übernahme
- **WHEN** der Host `Fade` mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentwiedergabe unverändert und der neue Wert gilt ab der nächsten Fragmentgrenze

#### Scenario: Alter State ohne Sync-Parameter
- **WHEN** ein State aus einer Version ohne `sliceLengthMode` oder `syncDivision` geladen wird
- **THEN** wird `sliceLengthMode` auf `Milliseconds` gesetzt, der vorhandene Millisekundenwert bleibt unverändert und `syncDivision` erhält seinen definierten Default

#### Scenario: Verzögerte Parameterübernahme
- **WHEN** der Host N, Fade, den Modus oder die Notenwertdivision mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentverarbeitung unverändert und die neuen Werte gelten ab der nächsten Fragmentgrenze

### Requirement: Random Recent
Das Plugin SHALL an jeder Fragmentgrenze zufällig eines der letzten N vollständig aufgenommenen Fragmente auswählen. Sind weniger als N vollständige Fragmente verfügbar, SHALL die Auswahl auf die verfügbaren Fragmente beschränkt werden. Wenn N größer als 1 ist und mehrere Kandidaten verfügbar sind, SHALL eine unmittelbare Wiederholung des zuletzt ausgewählten Fragments vermieden werden, sofern dies möglich ist. Vor dem ersten vollständigen Fragment SHALL der bearbeitete Pfad stumm sein.

#### Scenario: Auswahl aus dem Recent-Fenster
- **WHEN** ein weiteres vollständiges Fragment verfügbar wird
- **THEN** wird genau ein Kandidat aus den letzten N vollständigen Fragmenten ausgewählt und sequenziell wiedergegeben

#### Scenario: Unmittelbare Wiederholung
- **WHEN** mindestens zwei zulässige Kandidaten vorhanden sind und N größer als 1 ist
- **THEN** ist das neu ausgewählte Fragment nicht identisch mit dem unmittelbar zuvor ausgewählten Fragment

#### Scenario: Unzureichende Historie
- **WHEN** weniger als N vollständige Fragmente vorhanden sind
- **THEN** werden ausschließlich die tatsächlich vorhandenen vollständigen Fragmente berücksichtigt

#### Scenario: Stummer Start
- **WHEN** Wet auf 100 % steht und noch kein vollständiges Fragment verfügbar ist
- **THEN** bleibt der bearbeitete Pfad stumm

### Requirement: Zeitbasierte Fragmentierung
Fragment Length SHALL im Modus `Milliseconds` anhand der aktiven Samplerate in Samples umgerechnet werden. Im Modus `Tempo Sync` SHALL die Fragmentlänge aus dem gültigen Host-BPM, der ausgewählten Notenwertdivision und der aktiven Samplerate in Samples berechnet werden. Gerade Divisionen SHALL 1/1, 1/2, 1/4, 1/8 und 1/16 umfassen; punktierte Varianten SHALL mit Faktor 1,5 und triolische Varianten SHALL mit Faktor 2/3 berechnet werden. 1/32 SHALL nicht angeboten werden. Fragmentgrenzen SHALL samplecursorbasiert und unabhängig von Host-Blockgrenzen sein. Eine neue Länge, ein Moduswechsel oder eine Tempoänderung SHALL erst an der nächsten Fragmentgrenze aktiv werden; das aktuelle Fragment beendet sich mit seiner bisherigen Länge.

#### Scenario: Längenänderung
- **WHEN** Fragment Length mitten in einem Fragment geändert wird
- **THEN** wird die neue Sampleanzahl erst an der folgenden Fragmentgrenze aktiv

#### Scenario: Gemeinsame Grenzübernahme
- **WHEN** Fragment Length und N vor derselben Fragmentgrenze geändert wurden
- **THEN** werden beide neuen Werte erst an dieser Grenze für das nächste Fragment berücksichtigt

#### Scenario: Tempo-Sync-Länge
- **WHEN** der Modus `Tempo Sync` aktiv ist und der Host ein BPM von 120 liefert
- **THEN** entspricht eine Viertelnote 500 ms, eine Achtelnote 250 ms und eine punktierte Achtelnote 375 ms vor Sample-Rundung

#### Scenario: Triolische Länge
- **WHEN** eine triolische Division ausgewählt ist
- **THEN** wird die entsprechende gerade Division mit dem Faktor 2/3 verkürzt

#### Scenario: Tempoänderung
- **WHEN** sich das Host-BPM während eines laufenden Fragments ändert
- **THEN** bleibt das laufende Fragment unverändert und die neue tempoabhängige Länge gilt ab der nächsten Fragmentgrenze

#### Scenario: Fehlendes Host-Tempo
- **WHEN** der Host kein gültiges BPM liefert
- **THEN** wird der letzte gültige BPM-Wert verwendet, ersatzweise ein definierter Startwert von 120 BPM

#### Scenario: Gemeinsame Grenzübernahme mit Tempo-Sync
- **WHEN** Fragment Length, N, Modus, Division und Fade vor derselben Fragmentgrenze geändert wurden
- **THEN** werden alle neuen Werte gemeinsam für das nächste Fragment berücksichtigt

### Requirement: Echtzeitverarbeitung
Der Audiopfad SHALL vorallokierten begrenzten Speicher verwenden und keine Allokationen, Locks, Datei-I/O oder UI-Aufrufe ausführen. Speicher und Metadaten SHALL bis zu acht Recent-Fragmente sowie deren unterschiedliche Längen sicher verwalten. Reset und `prepareToPlay` SHALL flüchtige Audiodaten, Historie, Zufallszustand und Cursors löschen, aber Parameterwerte behalten.

#### Scenario: Reset
- **WHEN** der Host `prepareToPlay` erneut aufruft
- **THEN** beginnt die Fragmentverarbeitung mit leerem flüchtigem Audiostatus und den gespeicherten Parameterwerten

#### Scenario: Maximales Recent-Fenster
- **WHEN** N auf 8 steht und acht oder mehr Fragmente aufgenommen wurden
- **THEN** bleiben alle auswählbaren Fragmente bis zum Ende ihrer Wiedergabe gültig, ohne Ringpufferüberschreibung

### Requirement: Mischung
Die Ausgabe SHALL `((1 - wet) * input) + (wet * processedFragment)` entsprechen. Bei 0 % Wet ist sie ausschließlich trocken, bei 100 % Wet ausschließlich fragmentiert, und Zwischenwerte sind lineare Mischungen.

#### Scenario: Wet-Grenzwerte
- **WHEN** Dry/Wet auf 0 % oder 100 % gesetzt wird
- **THEN** ist die Ausgabe jeweils vollständig trocken oder vollständig fragmentiert

### Requirement: Deterministische Verarbeitung
Das Plugin SHALL bei identischem Audio, identischem Startzustand und beliebigen Host-Blockgrößen identische Fragmentgrenzen, Zufallsauswahlen und Ausgaben erzeugen. Zufallsauswahl SHALL ausschließlich an samplecursorbasierten Fragmentgrenzen stattfinden.

#### Scenario: Blockgrößenunabhängigkeit
- **WHEN** identisches Audio mit unterschiedlichen Host-Blockgrößen verarbeitet wird
- **THEN** bleiben Fragmentgrenzen, Auswahlfolge und Ausgaben identisch

#### Scenario: Stummer Start
- **WHEN** Wet auf 100 % steht und noch kein vollständiges Fragment verfügbar ist
- **THEN** bleibt der bearbeitete Pfad stumm

### Requirement: Kompakte Regleroberfläche
Die Plugin-Oberfläche SHALL einen Moduswähler mit den Anzeigen `Milliseconds` und `Tempo Sync`, einen Drehregler `Fragment Length` für Millisekunden, einen Drehregler `Tempo Division` für musikalische Werte, die drei bestehenden Bedienelemente und den `Fade`-Toggle darstellen. Im Modus `Milliseconds` SHALL nur `Fragment Length` aktiv und `Tempo Division` disabled sein. Im Modus `Tempo Sync` SHALL nur `Tempo Division` aktiv und `Fragment Length` disabled sein. Alle Bedienelemente SHALL mit ihren hostautomatisierbaren Parametern verbunden sein.

#### Scenario: Parameterdarstellung
- **WHEN** die Plugin-Oberfläche geöffnet wird
- **THEN** sind die bestehenden Bedienelemente, beide Längenregler, der Moduswähler, Beschriftungen und formatierte Werte sichtbar

#### Scenario: UI-Parameterbindung
- **WHEN** der Host oder die Oberfläche einen Parameterwert ändert
- **THEN** zeigt das zugehörige Bedienelement denselben Wert, Modus oder Disabled-Zustand an

#### Scenario: Millisekundenansicht
- **WHEN** `Milliseconds` ausgewählt ist
- **THEN** zeigt `Fragment Length` einen Wert in ms und `Tempo Division` ist sichtbar, aber disabled

#### Scenario: Tempo-Sync-Ansicht
- **WHEN** `Tempo Sync` ausgewählt ist
- **THEN** zeigt `Tempo Division` den ausgewählten Notenwert und `Fragment Length` ist sichtbar, aber disabled

### Requirement: Tempo-Sync-Notenwertdarstellung
Die Oberfläche SHALL gerade, punktierte und triolische Divisionen eindeutig unterscheiden und darf keine 1/32-Division anbieten. Die gespeicherte Division SHALL beim Wechsel zwischen den Modi erhalten bleiben.

#### Scenario: Divisionen auswählen
- **WHEN** der Benutzer `Tempo Sync` aktiviert
- **THEN** kann er 1/1, 1/2, 1/4, 1/8 oder 1/16 sowie deren punktierte und triolische Varianten auswählen

#### Scenario: Moduswechsel ohne Wertverlust
- **WHEN** der Benutzer zwischen `Milliseconds` und `Tempo Sync` wechselt
- **THEN** bleiben sowohl der zuletzt gewählte Millisekundenwert als auch die zuletzt gewählte Notenwertdivision gespeichert

### Requirement: Steuerbare Wet-Signal-Hüllkurve
Bei `Fade On` SHALL jedes wiedergegebene Wet-Fragment an Anfang und Ende mit einer Half-Cosine-Hüllkurve versehen werden. Die Fade-Dauer je Rand SHALL `min(20 ms, Fragmentlänge / 8)` betragen, in Samples sampleratenabhängig berechnet werden und innerhalb der tatsächlichen Fragmentlänge bleiben. Bei `Fade Off` SHALL das Wet-Fragment ohne Hüllkurvenmultiplikation ausgegeben werden.

#### Scenario: Fade-in und Fade-out
- **WHEN** ein Fragment bei aktiviertem Fade wiedergegeben wird
- **THEN** beginnt und endet sein Wet-Anteil deterministisch mit dem vorgesehenen Half-Cosine-Verlauf bei null und erreicht zwischen den Rändern den vollständigen Pegel

#### Scenario: Begrenzung der Fade-Dauer
- **WHEN** Fragmente 20 ms, 100 ms oder mindestens 160 ms lang sind
- **THEN** beträgt die nominale Randdauer entsprechend 2,5 ms, 12,5 ms beziehungsweise höchstens 20 ms, abzüglich unvermeidbarer Sample-Rundung

#### Scenario: Sehr kurze Fragmente und Sampleraten
- **WHEN** ein Fragment sehr kurz ist oder mit verschiedenen Sampleraten verarbeitet wird
- **THEN** wird die Fade-Länge ausschließlich in Samples begrenzt, es entstehen keine ungültigen Indizes oder Divisionen durch null, und die Verarbeitung bleibt deterministisch

#### Scenario: Harte Glitch-Grenzen
- **WHEN** Fade deaktiviert ist
- **THEN** werden Anfang und Ende des Wet-Fragments nicht durch die De-Click-Hüllkurve abgeschwächt

#### Scenario: Unveränderte Fragmentlogik
- **WHEN** identisches Audio mit Fade On oder Fade Off verarbeitet wird
- **THEN** bleiben Fragmentgrenzen, Recent-Kandidaten, Auswahlfolge, PRNG-Folge und Dry/Wet-Mischung unverändert

#### Scenario: Blockgrößenunabhängigkeit
- **WHEN** identisches Audio mit unterschiedlichen Host-Blockgrößen verarbeitet wird
- **THEN** bleiben Wet-Ausgabe, Hüllkurve und Fade-Übernahme an Fragmentgrenzen identisch
