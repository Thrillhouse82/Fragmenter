# AudioFragmenter: kleiner JUCE/VST3-MVP

## Ziel

Ein lauffähiger Stereo-VST3-Audioeffekt fragmentiert das Eingangssignal fortlaufend und gibt immer genau eines der zuletzt aufgenommenen Fragmente wieder. Die einzige Playback-Strategie im MVP ist **Random Recent** mit festem `N = 1`; damit wird das zuletzt vollständig aufgenommene Fragment deterministisch gewählt.

## Umfang

- CMake-Projekt mit JUCE-Einbindung und VST3-Ziel.
- Stereo-Audio-Insert ohne Instrument- oder MIDI-Funktion.
- `Fragment Length` als hostautomatisierbarer Millisekunden-Parameter, Standard 222 ms (entspricht 1/8 bei 135 BPM), inklusive einfacher Plugin-Oberfläche.
- `Dry/Wet` als hostautomatisierbarer Parameter von 0 bis 100 %.
- Fragmentgrenzen über einen monotonen Sample-Cursor; Längenänderungen werden erst an der nächsten Fragmentgrenze aktiv.
- Vorallokierter, begrenzter Stereo-Ringpuffer ohne Audio-Thread-Allokationen oder Locks.
- Plugin-State speichert und restauriert die Parameter.
- JUCE-Unit-Tests für Parameter, State, Mischung und Fragmentgrenzen; Build und Tests laufen über CMake.

## Nicht-Ziele

Sequential, Last Slice, Repeat, einstellbares `N`, Wiederholungsanzahl, Host-Tempo-Sync, aufwendiges UI, Audio-Persistenz und Ableton-Smoke-/DAW-Integrationstests.

## Akzeptanz

Der VST3-Build und alle CMake-Unit-Tests sind erfolgreich. Die Audioverarbeitung arbeitet nach `output = (1 - wet) * input + wet * fragment` und bleibt während der Startbefüllung im Wet-Anteil stumm.
