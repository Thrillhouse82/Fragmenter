## Why

Bei `Recent Slices N > 1` können nicht zusammenhängende Fragmente an einer Wiedergabegrenze hart aufeinanderfolgen und dadurch hörbare Knackser erzeugen. Zusätzlich kann ein kürzeres ausgewähltes Fragment den Wet-Pfad vor der nächsten Fragmentgrenze abrupt auf null setzen.

## What Changes

- Eine interne, sampleratenabhängige De-Click-Hüllkurve für jedes wiedergegebene Wet-Fragment ergänzen.
- Einen kurzen Half-Cosine-Fade-in und Fade-out mit nominal 5 ms verwenden, begrenzt auf höchstens ein Achtel der jeweiligen Fragmentlänge.
- Kürzere ausgewählte Fragmente kontrolliert ausblenden, ohne zusätzliche Latenz oder Änderung der Fragmentgrenzen.
- Fragmenthistorie, Kandidatenauswahl, PRNG-Folge, Dry/Wet-Mischung, Automation und State-Persistenz unverändert lassen.
- Unit-Tests für Hüllkurvenverlauf, kurze Fragmente, Blockgrößenunabhängigkeit und unveränderte Random-Recent-Auswahl ergänzen.

## Capabilities

### New Capabilities

Keine.

### Modified Capabilities

- `audio-fragment-recording`: Wet-Fragmente werden an ihren Wiedergabegrenzen de-clicked, ohne die bestehende Random-Recent-Semantik zu verändern.

## Impact

- `src/PluginProcessor.*`: vorallokierte, samplebasierte Hüllkurvenzustände im Audiopfad.
- `tests/AudioFragmenterTests.cpp`: Hüllkurven-, Kurzfragment-, Determinismus- und Regressionstests.
- Keine neuen Parameter, Abhängigkeiten oder State-Felder.
