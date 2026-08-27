# Design

## Build

CMake lÃ¤dt JUCE reproduzierbar per `FetchContent` (kein JUCE-Binary im Repository), erzeugt ein VST3-Ziel Ã¼ber `juce_add_plugin` und ein separates JUCE-Unit-Testziel. `BUILD_TESTING` steuert die Tests.

## Prozessor

`AudioFragmenterAudioProcessor` besitzt eine `AudioProcessorValueTreeState` mit den IDs `fragmentLengthMs` und `dryWet`. Der Millisekundenwert liegt zwischen 20 und 2000 ms, Standard 222 ms. In `prepareToPlay` werden Ringpuffer und alle Cursors vorallokiert bzw. zurÃ¼ckgesetzt. Bei jedem Sample wird der Eingang geschrieben. An einer Fragmentgrenze wird das gerade beendete Fragment als einziges verfÃ¼gbares Recent-Fragment fÃ¼r die Wiedergabe ausgewÃ¤hlt; das Lesen erfolgt aus dem Ringpuffer. Bei einer LÃ¤ngenÃ¤nderung wird die neue Sampleanzahl nur an dieser Grenze Ã¼bernommen.

Die RingkapazitÃ¤t betrÃ¤gt mindestens `2 * maxFragmentSamples + maximumBlockSize`, sodass das gerade geschriebene und das zuletzt vollstÃ¤ndige Fragment sicher lesbar bleiben. `processBlock` enthÃ¤lt keine Allokation, Datei-/UI-Arbeit oder Locks.

## OberflÃ¤che und State

Der Editor verwendet zwei einfache JUCE-Slider mit Labels. `getStateInformation` und `setStateInformation` serialisieren den APVTS-State; Audiopuffer werden nicht serialisiert.

## Tests

Das Testprogramm prÃ¼ft Parameterdefinitionen und Defaults, State-Roundtrip, Dry/Wet-Grenzen sowie die verzÃ¶gerte Ãœbernahme einer neuen FragmentlÃ¤nge Ã¼ber nicht gleich groÃŸe AudioblÃ¶cke hinweg.
