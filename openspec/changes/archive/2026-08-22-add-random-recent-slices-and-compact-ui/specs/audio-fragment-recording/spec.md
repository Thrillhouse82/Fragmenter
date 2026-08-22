## MODIFIED Requirements

### Requirement: Parameter und State
Das Plugin SHALL `Fragment Length` in Millisekunden (20–2000 ms, Standard 222 ms), `Dry/Wet` (0–100 %, Standard 100 %) und `Recent Slices N` (Ganzzahl 1–8, Standard 4) als hostautomatisierbare Parameter anbieten. Alle drei Werte SHALL im Plugin-State gespeichert und wiederhergestellt werden. Eine Änderung von `Recent Slices N` SHALL erst an der nächsten Fragmentgrenze wirksam werden.

#### Scenario: Parameter-State
- **WHEN** der Host alle drei Parameter setzt, speichert und restauriert den State
- **THEN** entsprechen die restaurierten Parameter den gespeicherten Werten

#### Scenario: N-Grenzen und Default
- **WHEN** das Plugin neu erzeugt wird oder der Host Werte außerhalb des Bereichs anfordert
- **THEN** ist N standardmäßig 4 und wird auf den Bereich 1–8 begrenzt

#### Scenario: Verzögerte N-Übernahme
- **WHEN** der Host N mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentwiedergabe unverändert und der neue Wert gilt ab der nächsten Fragmentgrenze

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
Fragment Length SHALL intern anhand der aktiven Samplerate in Samples umgerechnet werden. Fragmentgrenzen SHALL samplecursorbasiert und unabhängig von Host-Blockgrenzen sein. Eine neue Länge SHALL erst an der nächsten Fragmentgrenze aktiv werden; das aktuelle Fragment beendet sich mit seiner bisherigen Länge. Änderungen von N SHALL nach denselben Grenzsemantiken an dieser Fragmentgrenze übernommen werden.

#### Scenario: Längenänderung
- **WHEN** Fragment Length mitten in einem Fragment geändert wird
- **THEN** wird die neue Sampleanzahl erst an der folgenden Fragmentgrenze aktiv

#### Scenario: Gemeinsame Grenzübernahme
- **WHEN** Fragment Length und N vor derselben Fragmentgrenze geändert wurden
- **THEN** werden beide neuen Werte erst an dieser Grenze für das nächste Fragment berücksichtigt

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

## ADDED Requirements

### Requirement: Kompakte Regleroberfläche
Die Plugin-Oberfläche SHALL Fragment Length, Dry/Wet und Recent Slices N als drei beschriftete Drehregler mit sichtbarer Wertanzeige darstellen. Fragment Length SHALL in Millisekunden, Dry/Wet in Prozent und Recent Slices N als Ganzzahl angezeigt werden. Jeder Regler SHALL mit seinem hostautomatisierbaren Parameter verbunden sein.

#### Scenario: Parameterdarstellung
- **WHEN** die Plugin-Oberfläche geöffnet wird
- **THEN** sind alle drei Drehregler, Beschriftungen und formatierten Werte sichtbar

#### Scenario: UI-Parameterbindung
- **WHEN** der Host oder die Oberfläche einen Parameterwert ändert
- **THEN** zeigt der zugehörige Drehregler denselben formatierten Wert an und verwendet denselben Parameterwert
