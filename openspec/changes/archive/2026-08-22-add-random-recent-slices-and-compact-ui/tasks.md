## 1. Parameter und Audiostatus

- [x] 1.1 APVTS-Parameter `recentSlices` als Integer 1–8 mit Default 4 ergänzen und State-Kompatibilität für fehlende Altwerte sicherstellen.
- [x] 1.2 Vorallokierte Historienmetadaten für maximal acht Fragmente sowie latched N- und Längenwerte ergänzen.
- [x] 1.3 Ringpuffer für `(8 + 1) * maxFragmentSamples + maximumBlockSize` auslegen und Reset-/`prepareToPlay`-Semantik aktualisieren.

## 2. Random-Recent-Verarbeitung

- [x] 2.1 Reproduzierbaren, allocation-freien PRNG mit Reset in `prepareToPlay` integrieren.
- [x] 2.2 An Fragmentgrenzen vollständige Fragmente in die Historie aufnehmen und Kandidaten auf die letzten latched N Einträge begrenzen.
- [x] 2.3 Zufallsauswahl mit Vermeidung unmittelbarer Wiederholung implementieren; Einzelkandidat und N=1 korrekt behandeln.
- [x] 2.4 Unterschiedliche Fragmentlängen, stummen Start und unverändertes Dry/Wet-Mischverhalten erhalten.
- [x] 2.5 Sicherstellen, dass N und Fragment Length mitten im Fragment erst an der nächsten Grenze wirksam werden.

## 3. Kompakte Oberfläche

- [x] 3.1 Fragment Length, Dry/Wet und Recent Slices N als beschriftete Rotary-Slider in einem kompakten Layout anlegen.
- [x] 3.2 APVTS-Attachments und Wertformatierung für Millisekunden, Prozent und Ganzzahl ergänzen.
- [x] 3.3 Sichtbare Wertanzeigen und Synchronisation bei UI- sowie Host-Automation prüfen.

## 4. Unit-Tests und Verifikation

- [x] 4.1 Parameterbereiche, Defaults, diskrete N-Schritte und State-Roundtrip einschließlich Alt-State ohne N testen.
- [x] 4.2 Grenzsemantik für N und Fragment Length über unterschiedlich große Host-Blöcke testen.
- [x] 4.3 Auswahlfenster, stummen Start, N=1, unzureichende Historie und unmittelbare Wiederholungsvermeidung mit unterscheidbaren Fragmentwerten testen.
- [x] 4.4 Unterschiedliche Fragmentlängen, N=8 und Ringpuffer-Lebensdauer bis zum Wiedergabeende testen.
- [x] 4.5 Feste PRNG-Folge, Blockgrößenunabhängigkeit sowie Dry/Wet-Endpunkte und lineare Mischung testen.
- [x] 4.6 CMake-Build, Unit-Tests und OpenSpec-Validierung ausführen.
