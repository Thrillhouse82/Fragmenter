## ADDED Requirements

### Requirement: Wet-Signal De-Click-Hüllkurve
Das Plugin SHALL jedes wiedergegebene Fragment im Wet-Signal mit einer sampleratenabhängigen Half-Cosine-Hüllkurve versehen. Der Fade-in und Fade-out SHALL nominal 5 ms dauern, jedoch jeweils höchstens ein Achtel der Länge des wiedergegebenen Fragments. Die Hüllkurve SHALL ohne zusätzliche Latenz angewendet werden und darf Fragmentgrenzen, Kandidatenfenster, Auswahlfolge oder PRNG-Zustand nicht verändern.

#### Scenario: Fade-in an einer Fragmentgrenze
- **WHEN** ein ausgewähltes Fragment die Wiedergabe beginnt
- **THEN** steigt sein Wet-Anteil mit einer kurzen Half-Cosine-Kurve von null auf den vollständigen Wet-Anteil an

#### Scenario: Fade-out an einer Fragmentgrenze
- **WHEN** ein ausgewähltes Fragment seine Wiedergabelänge erreicht
- **THEN** fällt sein Wet-Anteil vor dem Ende des Fragments mit einer kurzen Half-Cosine-Kurve auf null ab, bevor das nächste Fragment beginnt

#### Scenario: Sehr kurze Fragmente
- **WHEN** die Fragmentlänge so kurz ist, dass 5 ms mehr als ein Achtel ihrer Länge wären
- **THEN** werden Fade-in und Fade-out jeweils auf höchstens ein Achtel der Fragmentlänge begrenzt und bleiben deterministisch

#### Scenario: Kürzeres ausgewähltes Fragment
- **WHEN** das ausgewählte Fragment kürzer als das aktuell laufende Aufnahmefragment ist
- **THEN** wird der Wet-Pfad am tatsächlichen Ende des ausgewählten Fragments kontrolliert ausgeblendet und nicht abrupt auf null gesetzt

#### Scenario: Keine zusätzliche Latenz
- **WHEN** ein Fragment verarbeitet wird
- **THEN** bleibt der erste ausgegebene Wet-Sample an derselben samplecursorbasierten Fragmentgrenze wie zuvor

#### Scenario: Unveränderte Random-Recent-Auswahl
- **WHEN** identisches Audio mit identischem Startzustand verarbeitet wird
- **THEN** bleiben Kandidatenfenster, Auswahlfolge, Fragmentgrenzen und PRNG-Folge gegenüber der Verarbeitung ohne Hüllkurve unverändert

#### Scenario: Blockgrößenunabhängigkeit
- **WHEN** identisches Audio mit unterschiedlichen Host-Blockgrößen verarbeitet wird
- **THEN** sind Wet-Ausgabe einschließlich Hüllkurve, Fragmentgrenzen und Auswahlfolge identisch
