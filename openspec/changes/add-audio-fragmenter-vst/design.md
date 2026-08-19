## Kontext

Das Repository enthält derzeit nur Projektdokumentation und OpenSpec-Konfiguration. Dieser Change definiert einen Windows-VST3-Insert-Effekt für Ableton Live 12. Der fachliche Vertrag ist in `specs/audio-fragment-recording/spec.md` beschrieben.

## Ziele / Nicht-Ziele

**Ziele:**

- Einen Windows-VST3-Stereo-Insert-Effekt liefern, der offline getestet und in Ableton Live 12 per Smoke-Test geprüft werden kann.
- Das Eingangssignal kontinuierlich über einen begrenzten Ringpuffer mit deterministischer Zeitslicing-Verarbeitung aufnehmen und verarbeiten.
- Pufferlatenz und Verhalten während der Startbefüllung eindeutig festlegen.
- Den Audiocallback begrenzt, nach der Vorbereitung vorallokiert und unabhängig von UI- und Festplattenzugriffen halten.
- Die Wiedergabestrategie für spätere Fragmentauswahl, Wiederholung, Reverse, Transient-, Beat-, Granular- und MIDI-Funktionen erweiterbar halten.

**Nicht-Ziele:**

- Manuelle Record-/Stop-Sitzungen oder nachträgliche Analyse.
- Silence Detection, Transient Detection, Beat-, Tonhöhen-, Granular-, Reverse-, Zufalls-, Sortier-, Wiederholungs-, Stutter- oder MIDI-Trigger-Verhalten.
- Fragmentbibliotheken, Fragment-Metadatenverwaltung, Waveform-Browsing, WAV-Export oder Persistenz aufgezeichneter Audiodaten.
- Unterstützung von VST2, AU, AAX, Standalone-Paketen oder Host-Qualifizierung außerhalb von Windows/Ableton Live 12.

## Entscheidungen

### JUCE mit CMake und VST3 für Windows/Ableton Live 12 verwenden

JUCE stellt den VST3-Wrapper, die Parameterintegration, Audio-Buffertypen und eine Oberfläche für Offline-Prozessortests bereit. CMake ermöglicht reproduzierbare Windows-Builds und Testziele. Der erste Hostvertrag ist ein Stereo-Audio-Insert in Ableton Live 12; andere Hosts und Pluginformate bleiben zunächst außen vor.

Alternative: direkte Implementierung gegen das Steinberg SDK. Das würde die Framework-Abhängigkeit reduzieren, aber für dieses ansonsten leere Repository mehr Plattform-, Parameter- und Host-Integrationscode erfordern.

### Einen begrenzten Stereo-Ringpuffer als Live-Quelle verwenden

Der Prozessor schreibt jedes Eingangssample in einen vorallokierten Stereo-Ringpuffer und überschreibt bei voller Kapazität die ältesten Samples. Die Kapazität wird während der Vorbereitung aus einer begrenzten Maximaldauer und der maximalen Host-Blockgröße bestimmt. Sie MUSS mindestens der effektiven Fragment-Sampleanzahl plus der maximalen Host-Blockgröße entsprechen, damit ein fertiges Quellfragment lesbar bleibt, während ein neuer Block geschrieben wird. Die Fragmentlänge wird so begrenzt, dass diese Beziehung erhalten bleibt.

Alternative: unbegrenzte Aufnahmen sammeln und erst nach dem Stop analysieren. Das wird abgelehnt, weil der Effekt keine Aufnahmesitzung besitzt und während der Wiedergabe kontinuierlich arbeiten muss.

### Fragmentlänge extern in Millisekunden und intern in Samples darstellen

Der hostseitige Fragmentlängenparameter ist eine begrenzte Dauer in Millisekunden. Bei der Vorbereitung und nach einer Sampleratenänderung wird er anhand der aktiven Samplerate in eine begrenzte 64-Bit-Sampleanzahl umgerechnet. Fragmentgrenzen basieren auf einem monotonen Sample-Cursor, sodass Host-Blockgrenzen das Timing nicht verändern. Ändert sich der Wert während der Wiedergabe, wird das aktuelle Fragment mit seiner bisherigen Länge beendet; die neue effektive Länge beginnt an der nächsten Fragmentgrenze.

### Das zuletzt vollständig empfangene Fragment mit einer Fragmentlatenz wiedergeben

Zum Eingabezeitpunkt `t` schreibt das Plugin das aktuelle Sample in den Ringpuffer und verwendet als bearbeitetes Signal das abgeschlossene Quellintervall unmittelbar vor dem aktuellen Fragment. Das erste vollständige Quellfragment wird nach einer konfigurierten Fragmentlänge hörbar, wodurch eine deterministische Latenz von einem Fragment entsteht. Der Wiedergabe-Cursor läuft dieses Fragment der Reihe nach durch und wechselt anschließend zum nächsten vollständigen Fragment. Im MVP gibt es keine Auswahl, Wiederholung oder Umordnung.

### Flüchtigen Status bei Host-Reset und Sampleratenänderungen zurücksetzen

Ringpufferinhalt, Sample-Cursor, aktueller Fragment-Cursor und Status der Startbefüllung sind flüchtig. Ein Host-Reset leert sie und startet die Verarbeitung mit einem leeren Puffer neu. Bei einer Sampleratenänderung bleibt die benutzerseitige Fragmentlänge in Millisekunden erhalten; die Sampleanzahl wird neu berechnet, der begrenzte Speicher während der Vorbereitung angepasst oder neu initialisiert und das Audio gelöscht, damit keine Samples verschiedener Sampleraten vermischt werden. Nur gewöhnliche Plugin-Parameter dürfen durch den Host persistiert werden; Audiodaten werden niemals serialisiert.

### Trockenes und bearbeitetes Signal explizit mischen

Für den Wet-Anteil `w` im Bereich [0, 1] lautet die Ausgabe `((1 - w) * input) + (w * processedFragment)`. Bei 0 % Wet besteht die Ausgabe nur aus dem Live-Eingang, bei 100 % Wet nur aus dem bearbeiteten Fragmentsignal. Während der Startphase ist das bearbeitete Signal so lange stumm, bis ein vollständiges Fragment verfügbar ist. Zwischenwerte mischen daher Live-Eingang und Stille; bei 100 % Wet bleibt die Ausgabe während dieser Phase stumm.

### Die Wiedergabe hinter einer erweiterbaren Strategieschnittstelle kapseln

Der MVP verwendet einen sequenziellen Cursor für vollständige Fragmente. Auswahl und Traversierung der Fragmentquellen werden jedoch von Ringpufferaufnahme und Ausgangsmischer getrennt. Spätere Strategien können Fragmente auswählen, sortieren, wiederholen, rückwärts abspielen, triggern oder transformieren, ohne den Audio-I/O-Vertrag oder den Dry/Wet-Mischer zu ändern.

## Risiken / Abwägungen

- [Eine vollständige Fragmentlatenz kann sich bei großen Einstellungen lang anfühlen] -> Fragmentlänge klar anzeigen, effektive Latenz ausweisen und die dokumentierte Verzögerung um ein Fragment testen.
- [Die Ringkapazität kann für eine angeforderte Fragmentlänge zu klein sein] -> Den Parameter auf die vorbereitete begrenzte Kapazität beschränken und den effektiven Wert für Host/UI sichtbar machen.
- [Reset oder Sampleratenänderungen können gepufferte Audiodaten ungültig machen] -> Flüchtigen Status während Vorbereitung und Reset löschen und mit dem definierten Startverhalten fortfahren.
- [Hosts können sich unterschiedlich verhalten] -> Das erste Abnahmeziel auf Ableton Live 12 unter Windows begrenzen und dort die definierte Smoke-Test-Checkliste ausführen.
- [Lizenz- und Versionsentscheidungen des Frameworks und SDKs beeinflussen die Distribution] -> JUCE- und VST3-SDK-Versionen in der Build-Konfiguration festschreiben und die Weitergabebedingungen vor dem Packaging prüfen.

## Migrationsplan

Es gibt keine bestehende Implementierung und keinen zu migrierenden persistenten Audiostatus. Das Windows-CMake/JUCE-Projekt hinzufügen, das VST3-Ziel und die Offline-Tests bauen, anschließend das Plugin in einem Entwicklungs-VST3-Scanpfad installieren. Vor der Einführung zusätzlicher Wiedergabestrategien wird das Plugin in Ableton Live 12 anhand der Smoke-Test-Kriterien validiert.

## Offene Fragen

- Die genauen Standardwerte für Fragmentlänge und Ringkapazität können während der Implementierung abgestimmt werden, solange die festgelegten Grenzen, die effektive Latenz und das Startverhalten unverändert bleiben.
