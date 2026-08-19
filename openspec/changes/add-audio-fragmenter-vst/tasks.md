## 1. Windows-VST3-Projekt einrichten

- [ ] 1.1 Das CMake-Projekt einrichten, JUCE/VST3-Abhängigkeiten festlegen und ein Windows-VST3-Effektziel mit Offline-Tests konfigurieren.
- [ ] 1.2 Einen Stereo-Eingangsbus und einen Stereo-Ausgangsbus mit Ableton-Live-12-Insert-Metadaten konfigurieren; kein Instrument- oder MIDI-Ziel bereitstellen.
- [ ] 1.3 Ein Offline-Prozessor-Harness hinzufügen, das deterministische Stereoblöcke einspeist und Ausgaben für samplegenaue Prüfungen erfasst.

## 2. Ringpuffer und Parameter

- [ ] 2.1 Einen begrenzten, vorallokierten Stereo-Ringpuffer mit Schreib-Cursor, Kapazität, Überschreiben der ältesten Daten und Erkennung der Startbefüllung implementieren.
- [ ] 2.2 Den Fragmentlängenparameter in Millisekunden definieren, ihn bei Vorbereitung und Sampleratenänderungen in eine begrenzte Sampleanzahl umrechnen und den effektiven Wert bereitstellen.
- [ ] 2.3 Den Dry/Wet-Parameter als normierten, hostautomatisierbaren Wert mit Semantik für 0 %, Zwischenwerte und 100 % definieren.
- [ ] 2.4 Host-Reset und erneute Vorbereitung bei Sampleratenänderung implementieren, sodass flüchtiger Ringpuffer- und Wiedergabestatus gelöscht und keine Audiodaten persistiert werden.

## 3. Deterministische Fragmentwiedergabe

- [ ] 3.1 Sample-Cursor-basierte Fragmentgrenzen implementieren, die über beliebige Host-Blockgrößen hinweg stabil bleiben.
- [ ] 3.2 Eine Latenz von einem Fragment und die sequenzielle Wiedergabe des zuletzt vollständig empfangenen Fragments implementieren.
- [ ] 3.3 Das Startverhalten implementieren, bei dem der bearbeitete Pfad bis zur Verfügbarkeit eines vollständigen Fragments stumm bleibt.
- [ ] 3.4 Fragmentauswahl und -traversierung hinter einer erweiterbaren Strategieschnittstelle kapseln, ohne Zufall, Umordnung, Wiederholung, Reverse, Transient-, Beat-, Granular- oder MIDI-Verhalten zu implementieren.

## 4. Audiomischung und Echtzeitsicherheit

- [ ] 4.1 `output = (1 - wet) * input + wet * processedFragment` für beide Stereokanäle implementieren.
- [ ] 4.2 Prüfen, dass im Audiocallback keine Datei-I/O, UI-Aufrufe, blockierende Synchronisation oder unbegrenzte Allokation stattfindet.
- [ ] 4.3 Deterministische Tests für Ringüberschreiben, Kapazitätssicherheit, Fragmenttiming, Startbefüllung, Latenz von einem Fragment, Dry/Wet-Endpunkte, Zwischenmischung, Änderungen der Fragmentlänge während der Wiedergabe, Blockgrößenänderungen, Reset und Sampleratenänderungen hinzufügen.
- [ ] 4.4 Einen Stresstest mit kontinuierlichem Stereosignal und Parameteränderungen hinzufügen, der Aussetzer, ungültige Lesezugriffe und Speicherwachstum erkennt.

## 5. Validierung in Ableton Live 12

- [ ] 5.1 Das Windows-VST3 bauen, in einem Entwicklungs-Scanpfad installieren und prüfen, dass Ableton Live 12 es erkennt.
- [ ] 5.2 Den Effekt auf einer Stereo-Audiospur einfügen und den Stereo-Eingangs-/Ausgangsfluss mit einem anhaltenden Testsignal prüfen.
- [ ] 5.3 Prüfen, dass 0 % Wet ausschließlich das Live-Signal ausgibt, 100 % Wet bis zur Startbefüllung stumm bleibt und danach deterministische Fragmente wiedergibt und Zwischenwerte beide Signale mischen.
- [ ] 5.4 Prüfen, dass eine Änderung der Fragmentlänge die effektive Schnittdauer und Latenz ohne Abstürze oder hörbare Verarbeitungsfehler verändert.
- [ ] 5.5 Prüfen, dass Host-Reset und erneute Vorbereitung bei Sampleratenänderung veraltete Audiodaten löschen und mit dem dokumentierten Startverhalten fortfahren.
