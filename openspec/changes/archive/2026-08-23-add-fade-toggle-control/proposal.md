## Why

Die bestehende De-Click-Hüllkurve ist fest verdrahtet und ihre Dauer nicht als
kreative Klangentscheidung steuerbar. Ein einfacher Fade-Schalter soll zwischen
geglätteten Fragmentübergängen und bewusst harten Glitch-Grenzen wählen, ohne
die deterministische Fragmentauswahl oder das bisherige Standardverhalten zu
verändern.

## What Changes

- Einen hostautomatisierbaren booleschen Parameter `fadeEnabled` mit dem
  Anzeigenamen `Fade` und Standardwert On ergänzen.
- Einen direkt bedienbaren Toggle neben den vorhandenen Drehreglern ergänzen.
- Bei Fade On pro wiedergegebenem Fragment einen Half-Cosine-Fade-in und
  Fade-out anwenden, jeweils mit `min(20 ms, Fragment Length / 8)`.
- Bei Fade Off das Wet-Fragment ohne Hüllkurve wiedergeben und harte Grenzen
  zulassen.
- Parameteränderungen erst an der nächsten Fragmentgrenze übernehmen.
- Ältere States ohne Fade-Parameter kompatibel mit dem Default On laden.
- Reproduzierbare Tests für Sampleraten, kurze Fragmente, State, UI-/Host-
  Automation, Grenzübernahme und unveränderte Zufallsauswahl ergänzen.

## Capabilities

### New Capabilities

Keine. Die Änderung erweitert die bestehende Fragmentierungsfähigkeit.

### Modified Capabilities

- `audio-fragment-recording`: Steuerbare Wet-Signal-Hüllkurve, Fade-Parameter,
  State-Kompatibilität und verzögerte Übernahme an Fragmentgrenzen.

## Impact

Betroffen sind `PluginProcessor`, `PluginEditor`, der APVTS-Parameter-State und
die bestehenden AudioFragmenter-Unit-Tests. Es werden keine externen
Abhängigkeiten oder bestehenden Parameter-IDs geändert.
