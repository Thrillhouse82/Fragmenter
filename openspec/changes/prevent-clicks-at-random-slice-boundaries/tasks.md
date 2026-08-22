## 1. Hüllkurvenmodell

- [ ] 1.1 Sampleratenabhängige nominale 5-ms-Fade-Länge und Begrenzung auf höchstens ein Achtel der jeweiligen Fragmentlänge definieren.
- [ ] 1.2 Vorallokierte Hüllkurven- und Wiedergabefelder für Fade-in, Fade-out und vorzeitig beendete Fragmente ergänzen.
- [ ] 1.3 Half-Cosine-Fade ohne Allokationen, Locks, Latenz oder UI-Zugriffe in den Wet-Pfad integrieren.

## 2. Echtzeitverarbeitung und Kompatibilität

- [ ] 2.1 Hüllkurvenfortschritt samplecursorbasiert und unabhängig von Host-Blockgrenzen implementieren.
- [ ] 2.2 Sicherstellen, dass Fragmentgrenzen, Kandidatenfenster, Random-Recent-Auswahl und PRNG-Folge unverändert bleiben.
- [ ] 2.3 Sicherstellen, dass N=1, Dry/Wet, Host-Automation und State-Persistenz unverändert bleiben.
- [ ] 2.4 Reset und `prepareToPlay` für Hüllkurvenzustände deterministisch zurücksetzen, ohne Parameter zu verändern.

## 3. Tests und Verifikation

- [ ] 3.1 Stark unterschiedliche Werte an Slice-Grenzen sowie Fade-in und Fade-out testen.
- [ ] 3.2 Sehr kurze Fragmentlängen und kürzere ausgewählte Fragmente bis zum kontrollierten Wet-Fade-out testen.
- [ ] 3.3 Identische Ausgaben bei unterschiedlichen Host-Blockgrößen prüfen.
- [ ] 3.4 Unveränderte Auswahlfolge und PRNG-Deterministik mit bestehenden Random-Recent-Tests prüfen.
- [ ] 3.5 Bestehende Parameter-, State-, Dry/Wet- und N=1-Regressionsfälle ausführen.
- [ ] 3.6 CMake-Build, Unit-Tests und OpenSpec-Validierung ausführen.
