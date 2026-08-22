## 1. Parameter und Audiostatus

- [ ] 1.1 APVTS-Parameter `recentSlices` als Integer 1–8 mit Default 4 ergänzen und State-Kompatibilität für fehlende Altwerte sicherstellen.
- [ ] 1.2 Vorallokierte Historienmetadaten für maximal acht Fragmente sowie latched N- und Längenwerte ergänzen.
- [ ] 1.3 Ringpuffer für `(8 + 1) * maxFragmentSamples + maximumBlockSize` auslegen und Reset-/`prepareToPlay`-Semantik aktualisieren.

## 2. Random-Recent-Verarbeitung

- [ ] 2.1 Reproduzierbaren, allocation-freien PRNG mit Reset in `prepareToPlay` integrieren.
- [ ] 2.2 An Fragmentgrenzen vollständige Fragmente in die Historie aufnehmen und Kandidaten auf die letzten latched N Einträge begrenzen.
- [ ] 2.3 Zufallsauswahl mit Vermeidung unmittelbarer Wiederholung implementieren; Einzelkandidat und N=1 korrekt behandeln.
- [ ] 2.4 Unterschiedliche Fragmentlängen, stummen Start und unverändertes Dry/Wet-Mischverhalten erhalten.
- [ ] 2.5 Sicherstellen, dass N und Fragment Length mitten im Fragment erst an der nächsten Grenze wirksam werden.

## 3. Kompakte Oberfläche

- [ ] 3.1 Fragment Length, Dry/Wet und Recent Slices N als beschriftete Rotary-Slider in einem kompakten Layout anlegen.
- [ ] 3.2 APVTS-Attachments und Wertformatierung für Millisekunden, Prozent und Ganzzahl ergänzen.
- [ ] 3.3 Sichtbare Wertanzeigen und Synchronisation bei UI- sowie Host-Automation prüfen.

## 4. Unit-Tests und Verifikation

- [ ] 4.1 Parameterbereiche, Defaults, diskrete N-Schritte und State-Roundtrip einschließlich Alt-State ohne N testen.
- [ ] 4.2 Grenzsemantik für N und Fragment Length über unterschiedlich große Host-Blöcke testen.
- [ ] 4.3 Auswahlfenster, stummen Start, N=1, unzureichende Historie und unmittelbare Wiederholungsvermeidung mit unterscheidbaren Fragmentwerten testen.
- [ ] 4.4 Unterschiedliche Fragmentlängen, N=8 und Ringpuffer-Lebensdauer bis zum Wiedergabeende testen.
- [ ] 4.5 Feste PRNG-Folge, Blockgrößenunabhängigkeit sowie Dry/Wet-Endpunkte und lineare Mischung testen.
- [ ] 4.6 CMake-Build, Unit-Tests und OpenSpec-Validierung ausführen.
