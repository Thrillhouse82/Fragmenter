## 1. Parameter und State

- [x] 1.1 `sliceLengthMode` als hostautomatisierbaren Choice-Parameter mit `Milliseconds`, `Tempo Sync` und Default `Milliseconds` ergänzen.
- [x] 1.2 `syncDivision` als hostautomatisierbaren Choice-Parameter mit geraden, punktierten und triolischen Werten von 1/1 bis 1/16 ohne 1/32 ergänzen.
- [x] 1.3 State-Migration für fehlende Mode- und Division-Properties ergänzen, ohne `fragmentLengthMs` oder bestehende Fade-/Recent-Werte zu verändern.

## 2. Tempo-DSP und Fragmentgrenzen

- [x] 2.1 Letzten gültigen Host-BPM-Wert mit initialem Fallback 120 BPM erfassen, ohne Audio-Thread-Allokationen, Locks oder UI-Zugriffe.
- [x] 2.2 Gerade, punktierte und triolische Beat-Faktoren in Sample-Längen umrechnen und bei verschiedenen Sampleraten korrekt runden.
- [x] 2.3 Modus, Division und Tempo ausschließlich an Fragmentgrenzen übernehmen; laufende Fragmente, Fade, Random-Recent, PRNG und Dry/Wet unverändert lassen.
- [x] 2.4 Fehlendes oder ungültiges Host-Tempo deterministisch über den letzten gültigen BPM-Wert behandeln.

## 3. Benutzeroberfläche

- [x] 3.1 Einen Moduswähler mit den Anzeigen `Milliseconds` und `Tempo Sync` ergänzen.
- [x] 3.2 Einen separaten `Tempo Division`-Regler mit eindeutigen Labels für gerade, punktierte und triolische Werte ergänzen.
- [x] 3.3 `Fragment Length` im `Tempo Sync`-Modus und `Tempo Division` im `Milliseconds`-Modus sichtbar, aber disabled schalten.
- [x] 3.4 Beide Längenregler sowie den Moduswähler mit den korrekten hostautomatisierbaren Parametern verbinden.

## 4. Tests und Validierung

- [x] 4.1 Defaults, State-Roundtrip und Legacy-States ohne Sync-Parameter testen.
- [x] 4.2 BPM-Berechnung für 120 BPM, mehrere Notenwerte, punktierte und triolische Varianten testen.
- [x] 4.3 Fehlendes Host-Tempo, Tempoänderungen und Übernahme erst an Fragmentgrenzen testen.
- [x] 4.4 Sampleraten-, Host-Blockgrößen-, Fragmentgrenzen-, Random-Recent-, Fade- und Dry/Wet-Regressionstests ausführen.
- [x] 4.5 UI-Disabled-Zustände, Mode-Wechsel ohne Wertverlust und Host-Automation der Choice-Parameter prüfen.
- [x] 4.6 Release-Build, VST3-Erzeugung und vollständige Testsuite ausführen.
