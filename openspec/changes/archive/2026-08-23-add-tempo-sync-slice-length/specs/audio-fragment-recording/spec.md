## MODIFIED Requirements

### Requirement: Parameter und State
Das Plugin SHALL `Fragment Length` in Millisekunden (20-2000 ms, Standard 222 ms), `Dry/Wet` (0-100 %, Standard 100 %), `Recent Slices N` (Ganzzahl 1-8, Standard 4), den Modus `sliceLengthMode` mit `Milliseconds` und `Tempo Sync` (Standard `Milliseconds`) sowie `syncDivision` mit den verfügbaren geraden, punktierten und triolischen Notenwerten von 1/1 bis 1/16 als hostautomatisierbare Parameter anbieten. Alle Parameter SHALL im Plugin-State gespeichert und wiederhergestellt werden. Eine Änderung von `Recent Slices N`, `sliceLengthMode`, `syncDivision` oder `Fade` SHALL erst an der nächsten Fragmentgrenze wirksam werden.

#### Scenario: Parameter-State
- **WHEN** der Host alle Parameter setzt, speichert und restauriert den State
- **THEN** entsprechen die restaurierten Parameter den gespeicherten Werten

#### Scenario: N-Grenzen und Default
- **WHEN** das Plugin neu erzeugt wird oder der Host Werte außerhalb des Bereichs anfordert
- **THEN** ist N standardmäßig 4 und wird auf den Bereich 1-8 begrenzt

#### Scenario: Alter State ohne Fade-Parameter
- **WHEN** ein State aus einer älteren Plugin-Version ohne `Fade` geladen wird
- **THEN** wird `Fade` auf On gesetzt und die übrigen Werte werden unverändert restauriert

#### Scenario: Alter State ohne Sync-Parameter
- **WHEN** ein State aus einer Version ohne `sliceLengthMode` oder `syncDivision` geladen wird
- **THEN** wird `sliceLengthMode` auf `Milliseconds` gesetzt, der vorhandene Millisekundenwert bleibt unverändert und `syncDivision` erhält seinen definierten Default

#### Scenario: Verzögerte Parameterübernahme
- **WHEN** der Host N, Fade, den Modus oder die Notenwertdivision mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentverarbeitung unverändert und die neuen Werte gelten ab der nächsten Fragmentgrenze

#### Scenario: Verzögerte N-Übernahme
- **WHEN** der Host N mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentwiedergabe unverändert und der neue Wert gilt ab der nächsten Fragmentgrenze

#### Scenario: Verzögerte Fade-Übernahme
- **WHEN** der Host `Fade` mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentwiedergabe unverändert und der neue Wert gilt ab der nächsten Fragmentgrenze

### Requirement: Zeitbasierte Fragmentierung
Fragment Length SHALL im Modus `Milliseconds` anhand der aktiven Samplerate in Samples umgerechnet werden. Im Modus `Tempo Sync` SHALL die Fragmentlänge aus dem gültigen Host-BPM, der ausgewählten Notenwertdivision und der aktiven Samplerate in Samples berechnet werden. Gerade Divisionen SHALL 1/1, 1/2, 1/4, 1/8 und 1/16 umfassen; punktierte Varianten SHALL mit Faktor 1,5 und triolische Varianten SHALL mit Faktor 2/3 berechnet werden. 1/32 SHALL nicht angeboten werden. Fragmentgrenzen SHALL samplecursorbasiert und unabhängig von Host-Blockgrenzen sein. Eine neue Länge, ein Moduswechsel oder eine Tempoänderung SHALL erst an der nächsten Fragmentgrenze aktiv werden; das aktuelle Fragment beendet sich mit seiner bisherigen Länge.

#### Scenario: Längenänderung im Millisekundenmodus
- **WHEN** Fragment Length mitten in einem Fragment geändert wird
- **THEN** wird die neue Sampleanzahl erst an der folgenden Fragmentgrenze aktiv

#### Scenario: Längenänderung
- **WHEN** Fragment Length mitten in einem Fragment geändert wird
- **THEN** wird die neue Sampleanzahl erst an der folgenden Fragmentgrenze aktiv

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

#### Scenario: Gemeinsame Grenzübernahme
- **WHEN** Fragment Length, N, Modus, Division und Fade vor derselben Fragmentgrenze geändert wurden
- **THEN** werden alle neuen Werte gemeinsam für das nächste Fragment berücksichtigt

### Requirement: Kompakte Regleroberfläche
Die Plugin-Oberfläche SHALL einen Moduswähler mit den Anzeigen `Milliseconds` und `Tempo Sync`, einen Drehregler `Fragment Length` für Millisekunden, einen Drehregler `Tempo Division` für musikalische Werte, die drei bestehenden Bedienelemente und den `Fade`-Toggle darstellen. Im Modus `Milliseconds` SHALL nur `Fragment Length` aktiv und `Tempo Division` disabled sein. Im Modus `Tempo Sync` SHALL nur `Tempo Division` aktiv und `Fragment Length` disabled sein. Alle Bedienelemente SHALL mit ihren hostautomatisierbaren Parametern verbunden sein.

#### Scenario: Millisekundenansicht
- **WHEN** `Milliseconds` ausgewählt ist
- **THEN** zeigt `Fragment Length` einen Wert in ms und `Tempo Division` ist sichtbar, aber disabled

#### Scenario: Parameterdarstellung
- **WHEN** die Plugin-Oberfläche geöffnet wird
- **THEN** sind die bestehenden Bedienelemente, beide Längenregler, der Moduswähler, Beschriftungen und formatierte Werte sichtbar

#### Scenario: Tempo-Sync-Ansicht
- **WHEN** `Tempo Sync` ausgewählt ist
- **THEN** zeigt `Tempo Division` den ausgewählten Notenwert und `Fragment Length` ist sichtbar, aber disabled

#### Scenario: UI-Parameterbindung
- **WHEN** der Host oder die Oberfläche einen Parameterwert ändert
- **THEN** zeigt das zugehörige Bedienelement denselben Wert, Modus oder Disabled-Zustand an

## ADDED Requirements

### Requirement: Tempo-Sync-Notenwertdarstellung
Die Oberfläche SHALL gerade, punktierte und triolische Divisionen eindeutig unterscheiden und darf keine 1/32-Division anbieten. Die gespeicherte Division SHALL beim Wechsel zwischen den Modi erhalten bleiben.

#### Scenario: Divisionen auswählen
- **WHEN** der Benutzer `Tempo Sync` aktiviert
- **THEN** kann er 1/1, 1/2, 1/4, 1/8 oder 1/16 sowie deren punktierte und triolische Varianten auswählen

#### Scenario: Moduswechsel ohne Wertverlust
- **WHEN** der Benutzer zwischen `Milliseconds` und `Tempo Sync` wechselt
- **THEN** bleiben sowohl der zuletzt gewählte Millisekundenwert als auch die zuletzt gewählte Notenwertdivision gespeichert
