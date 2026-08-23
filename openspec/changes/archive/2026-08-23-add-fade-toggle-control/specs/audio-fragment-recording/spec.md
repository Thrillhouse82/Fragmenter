## MODIFIED Requirements

### Requirement: Parameter und State
Das Plugin SHALL `Fragment Length` in Millisekunden (20-2000 ms, Standard 222 ms), `Dry/Wet` (0-100 %, Standard 100 %), `Recent Slices N` (Ganzzahl 1-8, Standard 4) und einen booleschen Parameter `Fade` (Standard On) als hostautomatisierbare Parameter anbieten. Alle vier Werte SHALL im Plugin-State gespeichert und wiederhergestellt werden. Eine Änderung von `Recent Slices N` oder `Fade` SHALL erst an der nächsten Fragmentgrenze wirksam werden.

#### Scenario: Parameter-State
- **WHEN** der Host alle vier Parameter setzt, speichert und restauriert den State
- **THEN** entsprechen die restaurierten Parameter den gespeicherten Werten

#### Scenario: N-Grenzen und Default
- **WHEN** das Plugin neu erzeugt wird oder der Host Werte außerhalb des Bereichs anfordert
- **THEN** ist N standardmäßig 4 und wird auf den Bereich 1-8 begrenzt

#### Scenario: Alter State ohne Fade-Parameter
- **WHEN** ein State aus einer älteren Plugin-Version ohne `Fade` geladen wird
- **THEN** wird `Fade` auf On gesetzt und die übrigen Werte werden unverändert restauriert

#### Scenario: Verzögerte Fade-Übernahme
- **WHEN** der Host `Fade` mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentwiedergabe unverändert und der neue Wert gilt ab der nächsten Fragmentgrenze

#### Scenario: Verzögerte N-Übernahme
- **WHEN** der Host N mitten in einem Fragment ändert
- **THEN** bleibt die laufende Fragmentwiedergabe unverändert und der neue Wert gilt ab der nächsten Fragmentgrenze

### Requirement: Kompakte Regleroberfläche
Die Plugin-Oberfläche SHALL Fragment Length, Dry/Wet und Recent Slices N als drei beschriftete Drehregler mit sichtbarer Wertanzeige sowie einen beschrifteten `Fade`-Toggle darstellen. Fragment Length SHALL in Millisekunden, Dry/Wet in Prozent und Recent Slices N als Ganzzahl angezeigt werden. Der Toggle SHALL den Zustand On oder Off eindeutig anzeigen. Jedes Bedienelement SHALL mit seinem hostautomatisierbaren Parameter verbunden sein.

#### Scenario: Parameterdarstellung
- **WHEN** die Plugin-Oberfläche geöffnet wird
- **THEN** sind alle drei Drehregler, der Fade-Toggle, Beschriftungen und formatierte Werte sichtbar

#### Scenario: UI-Parameterbindung
- **WHEN** der Host oder die Oberfläche einen Parameterwert ändert
- **THEN** zeigt das zugehörige Bedienelement denselben Parameterwert und Zustand an

## ADDED Requirements

### Requirement: Steuerbare Wet-Signal-Hüllkurve
Bei `Fade On` SHALL jedes wiedergegebene Wet-Fragment an Anfang und Ende mit einer Half-Cosine-Hüllkurve versehen werden. Die Fade-Dauer je Rand SHALL `min(20 ms, Fragmentlänge / 8)` betragen, in Samples sampleratenabhängig berechnet werden und innerhalb der tatsächlichen Fragmentlänge bleiben. Bei `Fade Off` SHALL das Wet-Fragment ohne Hüllkurvenmultiplikation ausgegeben werden.

#### Scenario: Fade-in und Fade-out
- **WHEN** ein Fragment bei aktiviertem Fade wiedergegeben wird
- **THEN** beginnt und endet sein Wet-Anteil deterministisch mit dem vorgesehenen Half-Cosine-Verlauf bei null und erreicht zwischen den Rändern den vollständigen Pegel

#### Scenario: Begrenzung der Fade-Dauer
- **WHEN** Fragmente 20 ms, 100 ms oder mindestens 160 ms lang sind
- **THEN** beträgt die nominale Randdauer entsprechend 2,5 ms, 12,5 ms beziehungsweise höchstens 20 ms, abzüglich unvermeidbarer Sample-Rundung

#### Scenario: Sehr kurze Fragmente und Sampleraten
- **WHEN** ein Fragment sehr kurz ist oder mit verschiedenen Sampleraten verarbeitet wird
- **THEN** wird die Fade-Länge ausschließlich in Samples begrenzt, es entstehen keine ungültigen Indizes oder Divisionen durch null, und die Verarbeitung bleibt deterministisch

#### Scenario: Harte Glitch-Grenzen
- **WHEN** Fade deaktiviert ist
- **THEN** werden Anfang und Ende des Wet-Fragments nicht durch die De-Click-Hüllkurve abgeschwächt

#### Scenario: Unveränderte Fragmentlogik
- **WHEN** identisches Audio mit Fade On oder Fade Off verarbeitet wird
- **THEN** bleiben Fragmentgrenzen, Recent-Kandidaten, Auswahlfolge, PRNG-Folge und Dry/Wet-Mischung unverändert

#### Scenario: Blockgrößenunabhängigkeit
- **WHEN** identisches Audio mit unterschiedlichen Host-Blockgrößen verarbeitet wird
- **THEN** bleiben Wet-Ausgabe, Hüllkurve und Fade-Übernahme an Fragmentgrenzen identisch
