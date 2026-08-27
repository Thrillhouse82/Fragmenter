## Why

Die aktuelle Fragmentlänge ist ausschließlich in Millisekunden einstellbar und
damit unabhängig vom musikalischen Tempo des Hosts. Ein optionaler Tempo-Sync-
Modus soll rhythmische Slice-Längen ermöglichen, ohne bestehende Projekte und
Millisekunden-Automation zu brechen.

## What Changes

- Einen Modusparameter `sliceLengthMode` mit `Milliseconds` und `Tempo Sync`
  ergänzen; der Default bleibt `Milliseconds`.
- Einen hostautomatisierbaren Parameter `syncDivision` für gerade, punktierte
  und triolische Notenwerte von 1/1 bis 1/16 ergänzen, ohne 1/32.
- Zwei getrennte Längenregler anzeigen: `Fragment Length` in Millisekunden
  und `Tempo Division` für musikalische Werte.
- Im jeweils anderen Modus den nicht aktiven Regler sichtbar, aber disabled
  darstellen.
- Im Tempo-Sync-Modus die aktive Slice-Länge aus Host-BPM, Notenwert und
  Samplerate in Samples berechnen.
- Tempo- und Modusänderungen erst an der nächsten Fragmentgrenze übernehmen.
- Bei fehlendem Host-Tempo den letzten gültigen BPM-Wert verwenden und mit
  einem definierten Fallback starten.
- Fragmentauswahl, PRNG, Fade-Verhalten, Dry/Wet-Mischung und Audio-Thread-
  Einschränkungen unverändert beibehalten.

## Capabilities

### New Capabilities

Keine. Die Änderung erweitert die bestehende Fragmentierungsfähigkeit.

### Modified Capabilities

- `audio-fragment-recording`: Längenmodus, Tempo-Synchronisation, musikalische
  Divisionen, UI-Aktivierung und Grenzsemantik.

## Impact

Betroffen sind Parameterlayout und State-Migration in `PluginProcessor`, die
Fragmentlängenberechnung, `PluginEditor` und die Unit-Tests. Die bestehende
Parameter-ID `fragmentLengthMs` bleibt für alte Projekte und Automation
erhalten. Es werden keine externen Abhängigkeiten benötigt.
