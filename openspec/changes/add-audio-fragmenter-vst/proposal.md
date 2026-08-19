## Warum

AudioFragmenter ist als Echtzeit-Kreativeffekt für Ableton Live 12 unter Windows gedacht, nicht als Aufnahmeprogramm. Der Effekt wandelt eingehendes Stereoaudio kontinuierlich in deterministische, zeitbasierte Fragmente um, damit zukünftige Wiedergabefunktionen ergänzt werden können, ohne den grundlegenden Audiopfad zu ändern.

## Änderungen

- Einen Windows-VST3-Audioeffekt mit Stereo-Eingang und -Ausgang bereitstellen, der als Insert-Effekt auf einer Audiospur in Ableton Live verwendet werden kann.
- Eingehende Stereosamples während der Host-Verarbeitung kontinuierlich in einen begrenzten, vorallokierten Ringpuffer schreiben.
- Den gepufferten Datenstrom anhand einer konfigurierbaren Fragmentlänge in Millisekunden in aufeinanderfolgende Fragmente teilen und intern in Samples umrechnen.
- Das zuletzt vollständig empfangene Fragment im bearbeiteten Signal mit einer deterministischen Pufferlatenz von einem Fragment wiedergeben.
- Das bearbeitete Fragmentsignal mit dem ursprünglichen Eingangssignal über einen Dry/Wet-Parameter mischen: 0 % ist vollständig trocken, 100 % vollständig bearbeitet, Zwischenwerte sind lineare Mischungen.
- Flüchtigen Puffer- und Wiedergabestatus bei Host-Reset und Sampleratenänderungen zurücksetzen, während Parameterwerte erhalten bleiben, sofern der Host dies unterstützt.
- Den Audiocallback begrenzt und echtzeitfähig halten. Der MVP enthält keinen manuellen Aufnahmeablauf, keine nachträgliche Analyse, keine Silence Detection, keinen Export, keine Fragmentbibliothek, keine Persistenz aufgezeichneter Audiodaten und keine manuelle Fragmentverwaltung.

## Fähigkeiten

### Neue Fähigkeiten

- `audio-fragment-recording`: Eingehendes Audio im Echtzeit-Insert-Effekt kontinuierlich puffern, zeitbasiert teilen, wiedergeben und mischen.

### Geänderte Fähigkeiten

- Keine.

## Auswirkungen

- Neue Plugin-, Echtzeit-Audio-, Ringpuffer-, Slicer-, Parameter- und Test-Harness-Komponenten werden benötigt.
- JUCE/CMake sowie das VST3 SDK bzw. Framework werden für einen Windows-Build mit Ziel Ableton Live 12 eingeführt.
- Das Plugin stellt Stereo-Audio-I/O sowie Dry/Wet- und Fragmentlängenparameter bereit; aufgezeichnete Audiodaten sind flüchtig und werden nicht persistiert.
- Tests benötigen deterministische Audiopuffer-Fixtures für Fragmenttiming, Startbefüllung, Mischung, Reset-/Sampleratenänderungen und Echtzeitsicherheit.
