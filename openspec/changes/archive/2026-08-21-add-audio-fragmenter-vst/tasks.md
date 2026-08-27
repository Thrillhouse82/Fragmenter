## 1. Build und Plugin-Grundgerüst

- [x] 1.1 CMake mit reproduzierbarer JUCE-FetchContent-Einbindung, VST3-Ziel und CMake-Testziel einrichten.
- [x] 1.2 Stereo-Eingangs-/Ausgangsbus, VST3-Metadaten und einfacher Editor implementieren.

## 2. Parameter, State und Audioverarbeitung

- [x] 2.1 APVTS-Parameter für Fragment Length (222 ms Standard) und Dry/Wet definieren und automatisierbar machen.
- [x] 2.2 Vorallokierten Stereo-Ringpuffer sowie Reset-/Sampleratenwechsel-Verhalten implementieren.
- [x] 2.3 Random Recent mit festem N=1, Fragmentlatenz und stummem Startverhalten implementieren.
- [x] 2.4 Fragmentlängenänderung deterministisch an der nächsten Grenze übernehmen und Dry/Wet mischen.
- [x] 2.5 Parameter-State speichern und wiederherstellen.

## 3. Tests und Validierung

- [x] 3.1 JUCE-Unit-Tests für Parameter, Defaults, State, Dry/Wet und Fragmentgrenzen schreiben.
- [x] 3.2 VST3-Build und alle CMake-Unit-Tests ausführen; Einschränkungen dokumentieren.
