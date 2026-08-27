## Why

Der MVP verwendet fest `N = 1` und spielt dadurch immer nur das zuletzt aufgenommene Fragment ab. Ein hostautomatisierbares Recent-Fenster von 1–8 Fragmenten soll echte zufällige Auswahl ermöglichen, während eine kompakte Oberfläche die drei zentralen Parameter direkt verständlich bedienbar macht.

## What Changes

- `Recent Slices N` als gespeicherten und hostautomatisierbaren Integer-Parameter mit Bereich 1–8 und Standardwert 4 hinzufügen.
- An jeder Fragmentgrenze zufällig aus den letzten N vollständig aufgenommenen Fragmenten wählen.
- Unmittelbare Wiederholungen möglichst vermeiden, wenn mehrere Kandidaten verfügbar sind.
- Änderungen von N und Fragment Length erst an der nächsten Fragmentgrenze aktivieren.
- Vorallokierte Fragmenthistorie und Ringpuffer für maximal acht Kandidaten auslegen.
- Fragment Length, Dry/Wet und Recent Slices N als beschriftete Drehregler mit sichtbaren Werten darstellen.
- Bestehendes Dry/Wet-Mischverhalten und stummen Wet-Start erhalten.

## Capabilities

### New Capabilities

Keine.

### Modified Capabilities

- `audio-fragment-recording`: Recent-Auswahl wird von festem N=1 auf konfigurierbares Random Recent erweitert; Parameter-, State-, Echtzeit- und UI-Anforderungen werden ergänzt.

## Impact

- `src/PluginProcessor.*`: APVTS-Parameter, Fragmentmetadaten, Zufallsauswahl und Ringpufferverwaltung.
- `src/PluginEditor.*`: drei kompakte Rotary-Slider mit Wertformatierung und Attachments.
- `tests/AudioFragmenterTests.cpp`: Parameter-, State-, Grenz-, Auswahl-, Blockgrößen- und Echtzeitsicherheits-nahe Verhaltenstests.
- Keine neuen externen Abhängigkeiten; JUCE/APVTS und VST3-Parameterautomation bleiben die Integrationsbasis.
