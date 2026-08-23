## 1. Parameter und Prozessorzustand

- [x] 1.1 Den booleschen APVTS-Parameter `fadeEnabled` mit stabiler ID, Anzeige `Fade` und Default `true` ergänzen.
- [x] 1.2 Einen vorinitialisierten, primitiven Fade-Latch für `prepareToPlay` und Fragmentgrenzen ergänzen, ohne Auswahl- oder PRNG-Zustand zu verändern.
- [x] 1.3 Die Hüllkurvenberechnung auf nominal 20 ms umstellen, mit sampleratenabhängiger Sampleberechnung und Begrenzung auf `floor(fragmentLengthSamples / 8)`.
- [x] 1.4 Fade On/Off ausschließlich für das Wet-Fragment anwenden; Fade Off muss harte Grenzen und unveränderte Dry/Wet-Mischung erlauben.
- [x] 1.5 State-Migration für ältere States ohne `fadeEnabled` auf Default On ergänzen und bestehende Fallbacks erhalten.

## 2. Plugin-Oberfläche

- [x] 2.1 Einen Toggle mit Beschriftung `Fade` und eindeutigem On/Off-Zustand neben den drei vorhandenen Drehreglern ergänzen.
- [x] 2.2 Den Toggle über die APVTS-Button-Bindung mit `fadeEnabled` verbinden und die Vier-Spalten-Anordnung responsiv ausrichten.

## 3. Reproduzierbare Tests

- [x] 3.1 Parameter-Default, State-Roundtrip und Laden eines alten States ohne Fade-Parameter testen.
- [x] 3.2 Fade On/Off sowie verzögerte Übernahme erst an der nächsten Fragmentgrenze testen.
- [x] 3.3 Fade-Samplegrenzen für 20-, 100- und mindestens 160-ms-Fragmente bei mehreren Sampleraten testen.
- [x] 3.4 Sehr kurze Fragmente, Endpunktbehandlung und Schutz vor ungültigen Indizes oder Division durch null testen.
- [x] 3.5 Blockgrößenunabhängigkeit und unveränderte Fragmentgrenzen, Recent-Auswahl, PRNG-Folge sowie Dry/Wet-Mischung testen.
- [x] 3.6 Bestehende Regressionstests für Random-Recent, Fragmentlängenänderung und Wet/Dry-Endpunkte ausführen und bei Bedarf um Fade-Ausnahmen ergänzen.

## 4. Validierung

- [x] 4.1 Debug- und Release-Build sowie die vollständige Test-Suite ausführen.
- [x] 4.2 Die UI-Darstellung und Host-Automation des Fade-Toggles in einer VST3-Hostumgebung prüfen.
