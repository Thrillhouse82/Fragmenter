## Zweck

AudioFragmenter ist ein Echtzeit-VST3-Insert-Effekt, der eingehendes Stereoaudio kontinuierlich zeitbasiert teilt und die deterministische Fragmentwiedergabe mit dem Live-Signal mischt.

## ADDED Requirements

### Requirement: Plugin arbeitet als Stereo-Insert-Effekt für Ableton

Das Plugin SHALL einen Stereo-Audioeingang und einen Stereo-Audioausgang als VST3-Effekt bereitstellen, der unter Windows auf einer Stereo-Audiospur in Ableton Live 12 eingesetzt werden kann.

#### Scenario: Ableton lädt den Insert-Effekt
- **WHEN** das Plugin in Ableton Live 12 unter Windows gescannt und auf einer Stereo-Audiospur eingefügt wird
- **THEN** wird es als Audioeffekt geladen, empfängt Stereosignale und erzeugt einen Stereoausgang

#### Scenario: Der Host stellt ein nicht unterstütztes Eingangslayout bereit
- **WHEN** der Host nicht den erforderlichen Stereo-Eingang bereitstellt
- **THEN** bleibt das Plugin sicher und erzeugt eine definierte stumme oder hostkompatible Ausgabe, ohne auf ungültige Kanäle zuzugreifen

### Requirement: Plugin puffert den Stereo-Eingang kontinuierlich und begrenzt

Das Plugin SHALL jedes eingehende Stereosample während der aktiven Audioverarbeitung in einen vorallokierten, begrenzten Ringpuffer schreiben und bei voller Kapazität die ältesten Samples überschreiben. Die vorbereitete Kapazität SHALL mindestens der effektiven Fragment-Sampleanzahl plus der maximalen Host-Audioblockgröße entsprechen.

#### Scenario: Der Ringpuffer empfängt Live-Audio
- **WHEN** der Host den Audioprozessor mit einem Stereoblock aufruft
- **THEN** wird der Eingang in Sample-Reihenfolge in den Ring geschrieben, während die Verarbeitung ohne unbegrenzte Speicheroperation fortgesetzt wird

#### Scenario: Der Ringpuffer erreicht seine Kapazität
- **WHEN** der Schreib-Cursor die begrenzte Kapazität erreicht
- **THEN** werden die ältesten Samples überschrieben und der Ringpuffer bleibt für die kontinuierliche Verarbeitung verfügbar

### Requirement: Plugin teilt Audio in Fragmente fester Länge

Das Plugin SHALL den gepufferten Datenstrom in aufeinanderfolgende Fragmente der konfigurierten Fragmentlänge teilen. Der benutzerseitige Wert wird in Millisekunden dargestellt, intern jedoch als aus der aktiven Samplerate abgeleitete Sampleanzahl verarbeitet.

#### Scenario: Die Fragmentlänge wird angewendet
- **WHEN** der Host Audio mit einer konfigurierten Fragmentlänge verarbeitet
- **THEN** liegen die Fragmentgrenzen an deterministischen Samplepositionen, unabhängig von den Host-Blockgrenzen

#### Scenario: Die Fragmentlänge liegt außerhalb des gültigen Bereichs
- **WHEN** der Host oder die UI eine Fragmentlänge außerhalb des unterstützten Bereichs oder größer als die vorbereitete Ringkapazität anfordert
- **THEN** begrenzt das Plugin sie auf einen gültigen effektiven Wert und setzt die Verarbeitung fort

#### Scenario: Die Fragmentverarbeitung läuft kontinuierlich
- **WHEN** der Host weiterläuft und den Audioprozessor wiederholt aufruft
- **THEN** schreibt, teilt und gibt das Plugin Fragmente ohne manuellen Record- oder Stop-Vorgang kontinuierlich weiter

### Requirement: Plugin gibt das zuletzt vollständig empfangene Fragment deterministisch wieder

Das bearbeitete Signal SHALL das zuletzt vollständig empfangene Quellfragment vom Anfang bis zum Ende sequenziell wiedergeben und anschließend zum nächsten vollständigen Fragment wechseln. Der MVP SHALL Fragmente nicht auswählen, umordnen, wiederholen, rückwärts abspielen oder triggern.

#### Scenario: Das erste vollständige Fragment wird verfügbar
- **WHEN** ein vollständiges Fragment empfangen wurde
- **THEN** beginnt der bearbeitete Pfad nach einer Pufferlatenz von einer Fragmentlänge mit der Wiedergabe dieses Fragments

#### Scenario: Aufeinanderfolgende Fragmente werden abgespielt
- **WHEN** das aktuelle vollständige Fragment endet und das nächste Fragment empfangen wurde
- **THEN** wechselt die Wiedergabe an der nächsten Fragmentgrenze ohne Auswahl oder Umordnung zum nächsten Fragment

#### Scenario: Der Puffer ist noch nicht gefüllt
- **WHEN** seit Start oder Reset weniger als ein vollständiges Fragment empfangen wurde
- **THEN** ist das bearbeitete Signal stumm und das Plugin liest keine nicht initialisierten oder veralteten Ringdaten

#### Scenario: Die Wiedergabe des aktuellen Fragments ist deterministisch
- **WHEN** dieselben Stereo-Eingangsdaten mit Fragmentlänge, Samplerate und Reset-Sequenz zweimal verarbeitet werden
- **THEN** haben die bearbeiteten Ausgaben dieselben Fragmentgrenzen und Samplewerte, abgesehen von der definierten Mischung

### Requirement: Benutzer kann Fragmentlänge und Dry/Wet-Mischung konfigurieren

Das Plugin SHALL einen Fragmentlängenparameter und einen Dry/Wet-Parameter bereitstellen. Dry/Wet SHALL als normierte Mischung von 0 % bis 100 % interpretiert werden.

#### Scenario: Trockene Mischung ist ausgewählt
- **WHEN** Dry/Wet auf 0 % gesetzt ist
- **THEN** enthält die Ausgabe ausschließlich das unbearbeitete Live-Eingangssignal ohne Beitrag der Fragmentverarbeitung

#### Scenario: Nasse Mischung ist ausgewählt
- **WHEN** Dry/Wet auf 100 % gesetzt ist
- **THEN** enthält die Ausgabe ausschließlich das bearbeitete Fragmentsignal und bleibt vor dem ersten vollständigen Fragment stumm

#### Scenario: Eine Zwischenmischung ist ausgewählt
- **WHEN** Dry/Wet zwischen 0 % und 100 % liegt
- **THEN** ist die Ausgabe die entsprechende lineare Mischung aus Live-Eingang und bearbeitetem Fragmentsignal

#### Scenario: Parameter ändern sich während der Wiedergabe
- **WHEN** der Host Fragmentlänge oder Dry/Wet während der Audioverarbeitung ändert
- **THEN** wird Dry/Wet ohne Verzögerung angewendet, das aktuelle Fragment mit seiner bisherigen Länge beendet, die neue Fragmentlänge an der nächsten Fragmentgrenze wirksam und keine unbegrenzte Allokation oder Ringbeschädigung verursacht

### Requirement: Plugin setzt den flüchtigen Verarbeitungsstatus sicher zurück

Das Plugin SHALL gepufferte Audiodaten und Wiedergabe-Cursor bei Host-Reset und Sampleratenänderungen löschen, dabei die Fragmentlänge in Millisekunden erhalten und ihre Sample-Repräsentation neu berechnen.

#### Scenario: Der Host setzt die Verarbeitung zurück
- **WHEN** der Host den Prozessor zurücksetzt oder neu vorbereitet
- **THEN** werden Schreib-Cursor, Fragment-Cursor und Status der Startbefüllung gelöscht; der bearbeitete Ausgang bleibt stumm, bis erneut ein vollständiges Fragment verfügbar ist

#### Scenario: Der Host ändert die Samplerate
- **WHEN** der Host die Samplerate ändert und das Plugin neu vorbereitet
- **THEN** bleibt die Fragmentlänge in Millisekunden erhalten, die Sampleanzahl wird neu berechnet, Audiodaten der vorherigen Samplerate werden gelöscht und die Verarbeitung startet mit der definierten Latenz erneut

### Requirement: Audioverarbeitung bleibt echtzeitfähig

Das Plugin SHALL im Echtzeit-Audiopfad keine Datei-I/O, UI-Aufrufe, unbegrenzten Allokationen oder blockierenden Synchronisationen ausführen.

#### Scenario: Nicht-Audio-Arbeit findet während der Verarbeitung statt
- **WHEN** Host- oder UI-Parameteraktivität stattfindet, während der Host den Audioprozessor aufruft
- **THEN** läuft die Audioverarbeitung ohne Warten auf Nicht-Echtzeit-Arbeit und ohne vermeidbare Aussetzer weiter

#### Scenario: Der Host ändert die Blockgröße
- **WHEN** der Host den Prozessor mit einer anderen maximalen Audioblockgröße vorbereitet
- **THEN** wird der begrenzte Verarbeitungsspeicher für die neue Blockgröße vorbereitet und das Fragmenttiming bleibt samplegenau

### Requirement: Das Ableton-Live-12-Smoke-Verhalten ist prüfbar

Das Plugin SHALL einen Windows-Smoke-Test in Ableton Live 12 für Discovery, Insert, Stereo-Signalfluss, Dry/Wet-Verhalten, konfigurierbare Fragmentlänge, Startbefüllung und stabile kontinuierliche Verarbeitung bestehen.

#### Scenario: Der Live-Smoke-Test besteht
- **WHEN** das Plugin gescannt, auf einer Stereo-Audiospur eingefügt, mit einem anhaltenden Stereo-Testsignal gespeist und mit 0 %, Zwischenwerten sowie 100 % Wet geprüft wird
- **THEN** lädt Live das Plugin, ist die Ausgabe bei 0 % Wet unbearbeitet, bleibt die bearbeitete Ausgabe bis zur Startbefüllung stumm und folgt danach der deterministischen Fragmentwiedergabe, mischen Zwischenwerte beide Signale und tritt während des Tests kein hörbarer Verarbeitungsfehler auf
