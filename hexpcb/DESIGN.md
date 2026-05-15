# HexPCB — Game Design & Technical Plan

---

## 1. Overview

An open-world circuit sandbox on an infinite hexagonal grid. Players route traces across 4
signal layers (+ 1 implicit ground plane) and place electronic components to satisfy input/output
conditions. The world has emergent electromagnetic field physics: magnetic fields leak from
inductors and current-carrying traces, electric fields radiate from charged nodes, and hidden
morse-code signals are embedded in ambient external fields — discoverable only with in-game sensors.

---

## 2. Coordinate System

### 2.1 Hex Coordinates

Cube coordinates **(q, r, s)** with the constraint `q + r + s = 0`. Store **(q, r)** only;
derive `s = -q - r`. This gives clean neighbor math and distance formulas.

The 6 neighbors of **(q, r)**:

| Direction | Δq | Δr |
|-----------|----|----|
| E         | +1 |  0 |
| NE        | +1 | -1 |
| NW        |  0 | -1 |
| W         | -1 |  0 |
| SW        | -1 | +1 |
| SE        |  0 | +1 |

Hex size `H` (meters, a game constant) converts to real-space for field physics:

```
real_distance(a, b) = H * hex_distance(a, b)
hex_distance(a, b)  = (|Δq| + |Δr| + |Δs|) / 2
```

Field magnitudes use real-space distance. Neighbor lookup uses cube coords directly.

### 2.2 Cell Address

Every addressable cell is `(q, r, layer)` where layer ∈ {1, 2, 3, 4}. Ground is not a
layer — it is the MNA reference node (V = 0), accessed only via Ground Via components.

---

## 3. World Representation

### 3.1 HashMap Storage

```js
const world = new Map();           // key: "q,r" → HexCell
const traceMap = new Map();        // key: "q,r,layer" → TraceNode
```

A `HexCell` holds:
- `component`: null | ComponentInstance
- `layers`: per-layer trace presence flags
- `dirty`: boolean (changed this tick)
- `sleepCounter`: ticks since last change

Empty cells with no component and no traces are **not stored** — absent from the map means empty.
If a lookup misses, the cell is empty. This is the GC strategy: no explicit deletion needed for
empty cells; they simply fall out of the map when cleared.

### 3.2 Sleeping Nodes

A node (trace hex on a given layer, or component terminal) is **active** if its voltage,
current, or field value changed by more than epsilon (`ε = 1e-6`) last tick.

- Active nodes: included in MNA solve.
- Inactive nodes: excluded from solve; retain last value.
- A sleeping node wakes if any neighbor becomes active.

Component terminals always stay in the MNA graph as long as they are connected to at least
one active node. Isolated trace segments with no voltage source path sleep immediately.

### 3.3 Chunking for Rendering

Chunks of **16×16 hexes**. Only chunks intersecting the viewport + 1-chunk border are rendered.
All placed components tick regardless of viewport (simulation is global). Rendering is culled.

---

## 4. Layers

| Layer | Color  | Purpose         |
|-------|--------|-----------------|
| 1     | Red    | Signal          |
| 2     | Green  | Signal          |
| 3     | Blue   | Signal          |
| 4     | Yellow | Signal          |
| GND   | —      | Implicit ground plane (reference node) |

Players route traces freely on layers 1–4. Ground vias connect a cell to the GND node.
The ground plane has no physical hexes — it is a virtual node in the MNA matrix.

### 4.1 Active Layer

A persistent **layer selector** (always visible, showing 1–4 with layer colors) sets the
**active layer**. It controls:

- **Draw mode**: traces are placed on the active layer only.
- **Place mode**: components are associated with the active layer.
- **View**: the active layer renders at full opacity. All other layers render as dim ghosts
  (e.g. 20% opacity). Components and vias that span multiple layers always render at full
  opacity regardless of active layer, so cross-layer connections remain legible.

---

## 5. Interaction Modes

A persistent toolbar selects the active mode. Spacebar held temporarily activates Pan.

| Mode         | Action                                                                 |
|--------------|------------------------------------------------------------------------|
| **Pan**      | Click-drag to scroll. Scroll wheel to zoom.                           |
| **Draw**     | Click/drag to place trace hexes on selected layer. Draw over existing trace on same layer erases it. |
| **Erase**    | Click/drag to remove trace hexes.                                     |
| **Place**    | Opens component picker. Click empty hex to place. Respects component footprint. |
| **Delete**   | Click component body hex to remove entire component.                  |
| **Move**     | Click component to pick up, click destination to drop. Validates footprint. |
| **Rotate**   | Click component to cycle through 6 rotations (0°, 60°, 120°, 180°, 240°, 300°). |
| **Inspect**  | Click any hex to open parameter panel for that component or trace node. |

The active layer selector (1–4) is always visible. It filters the view and targets Draw and Place modes. See §4.1.

---

## 6. Components

### 6.1 Footprint Types

**Type A — 3-hex line** (body + 2 terminals at ends):

```
[T] — [B] — [T]
```

Rotates to 6 orientations (the 3 hex-axis directions × 2 for asymmetric components like diodes).
Terminals are the end hexes. Trace routes into them.

**Type B — 1-hex single**:

Just the body hex. Terminals are defined per-component (some use adjacency, some use layers).

**Type C — 4-hex star** (body + 3 terminals at 0°, 120°, 240°):

```
[T]
 \
  [B] — [T]
 /
[T]
```

Rotates to 6 orientations (3-fold symmetry × 2 for chirality where relevant).

**Type D — 2-hex pair**:

```
[A] — [B]
```

Rotates to 6 orientations.

**Type E — 5-hex cross** (body + 4 terminals: left, right, upper-right, lower-right at ±60°):

```
        [T]
       /
[T] - [B] - [T]
       \
        [T]
```

(Exact hex layout defined by component.)

### 6.2 Component List

| Component                   | Type | Params                              | Notes                                      |
|-----------------------------|------|-------------------------------------|--------------------------------------------|
| Ground Via                  | B    | —                                   | Connects all layers at cell to GND         |
| Power Via                   | B    | voltage (V)                         | Voltage source to GND                      |
| Input Via                   | B    | input number                        | Driven by test signal on Run               |
| Output Via                  | B    | output number                       | Sampled to verify condition                |
| Via                         | B    | connected layers (subset of 1–4)    | Shorts selected layers at this cell        |
| Junction                    | B    | —                                   | Explicit T/X connect on same layer         |
| Crossover                   | B    | —                                   | 6 directional channels, no internal connect|
| Simple Resistor             | A    | resistance (Ω)                      |                                            |
| Simple Inductor             | A    | loops (N)                           | Leaks B field; horizontal orientation      |
| Vertical Inductor           | A    | loops (N), core material            | Larger N range; leaks B field; vertical    |
| Simple Plate Capacitor      | A    | plate distance (d)                  |                                            |
| 3-Shell Spherical Capacitor | C    | —                                   | Center = dielectric, 3 terminals = shells  |
| Simple Diode                | A    | —                                   | Directional; piecewise linear model        |
| Zener Diode                 | A    | zener voltage (Vz)                  | Directional; piecewise linear with reverse breakdown |
| Switch                      | A    | open/closed state                   | Toggled in Inspect mode                    |
| Simple Magnet               | D    | field strength (T)                  | North and south hex                        |
| Vertical Magnet             | B    | field strength (T)                  | Out-of-plane field                         |
| Charged Ball                | B    | charge (C)                          | Static; contributes to E field             |
| NPN Transistor              | C    | —                                   | Base, collector, emitter at star terminals |
| PNP Transistor              | C    | —                                   | Base, collector, emitter at star terminals |
| Potentiometer               | C    | wiper position (0–1)                | Three terminals                            |
| Button                      | A    | —                                   | Acts as variable resistor; physically actuated by vertical magnet or charged ball field above threshold |
| Wave Display                | C    | mode (voltage/current/FFT)          | Power, GND, signal terminals               |
| Voltmeter                   | A    | —                                   | High-impedance; reads terminal voltage     |
| Ammeter                     | A    | —                                   | Near-zero resistance; reads branch current |
| Signal Display              | B    | sample period                       | Displays signal as morse code visually     |
| Signal Input                | B    | period                              | Outputs morse code as voltage signal       |
| Vertical E-Field Sensor     | C    | scale factor                        | Power, GND, output; reads out-of-plane E  |
| Planar E-Field Sensor       | E    | scale factor                        | 5-hex cross; distributes field onto 3 axes |

Every component hex contributes trace resistance (smallest resistance unit) per hex in its footprint.

### 6.3 Rotation Encoding

Store rotation as integer 0–5 (multiples of 60°). Apply rotation to terminal offset vectors
when resolving footprint positions during placement and simulation.

---

## 7. Circuit Simulation

### 7.1 Approach: Modified Nodal Analysis (MNA)

Each tick (real time: 1/16 s, simulation timestep: Δt = 1/16 s):

1. **Build node list**: every `(q, r, layer)` cell that has a trace or component terminal,
   plus virtual GND node (index 0).
2. **Stamp components** into the MNA matrix (G·x = b):
   - Resistors, trace resistance: conductance stamp.
   - Voltage sources (Power Via, Input Via): augmented matrix stamp.
   - Capacitors: backward Euler companion model → equivalent conductance + current source.
   - Inductors: backward Euler companion model → equivalent conductance + current source.
   - Nonlinear (diode, Zener, BJT): piecewise linear model based on previous tick voltage.
     Use 2–3 Newton-Raphson iterations per tick to refine.
3. **Solve** G·x = b via sparse LU decomposition (or conjugate gradient for large systems).
   Only active nodes are included in the solve. Sleeping nodes retain last-tick values.
4. **Update node voltages and branch currents.**
5. **Mark dirty**: nodes whose voltage changed > ε are marked active; others increment sleep counter.

### 7.2 Vias in MNA

A Via connecting layers {1, 3} at cell (q, r) inserts a near-zero resistance (or ideal short)
between nodes `(q,r,1)` and `(q,r,3)`. Ground Via shorts `(q,r,*)` to node 0.

### 7.3 Trace Connectivity

Two adjacent trace hexes on the same layer are connected if the player explicitly drew the
connecting hex. Connectivity is resolved by flood-fill from terminals to build nets before
MNA construction each tick (only re-run if topology changed, flagged by dirty).

---

## 8. Field Physics

### 8.1 Magnetic Field (B)

Sources: current-carrying trace segments and inductors.

Per tick, after MNA solve:

1. For each active trace hex with nonzero current I, treat it as a magnetic dipole segment.
2. For each field-sensitive hex within cutoff radius R_B (configurable, e.g. 20 hex units):
   - Compute real-space vector from source to target.
   - Add Biot-Savart contribution: `dB = (μ₀/4π) * I * dl × r̂ / r²`
   - `dl` is the hex edge length vector (derived from rotation + H).
   - Accumulate into (Bq, Br, Bs, B_vertical) at target hex — four field components matching
     the 4 easter egg directions.
3. Inductors: add dipole field contribution based on their stored flux.

Sleeping traces with I ≈ 0 skip contribution.

### 8.2 Electric Field (E)

Sources: charged nodes (voltage × local capacitance → charge Q).

Per tick:

1. For each node with nonzero Q, within cutoff R_E:
   - `dE = (1/4πε₀) * Q * r̂ / r²`
   - Accumulate into (Eq, Er, Es, E_vertical).
2. Charged Ball components contribute directly with their set charge.

### 8.3 Field Directions

Fields are stored per hex as 4-component vectors:
- **q-axis** (hex axis 0°)
- **r-axis** (hex axis 120°)
- **s-axis** (hex axis 240°)
- **vertical** (out-of-plane, ±z)

These are the 4 directions the field sensors measure. These are also the 4 directions of the
easter egg ambient fields.

### 8.4 Field → Component Interaction

- **Button**: actuated if total |B| or |E| at its hex exceeds threshold (set by button params).
- **Vertical E-Field Sensor**: outputs voltage proportional to |E_vertical| scaled by power rail.
- **Planar E-Field Sensor**: distributes |Eq|, |Er|, |Es| to its three output terminals (pi/3, 0, -pi/3 relative to orientation).
- **Inductors**: mutual inductive coupling computed implicitly — nearby inductors' fields induce
  EMF via flux linkage through their loop area. No explicit indicator shown; effect is emergent.

---

## 9. Easter Eggs

Four ambient external fields are always present in the world, each carrying a distinct morse code
signal modulated at game time:

| Field      | Direction  | Signal content     |
|------------|------------|--------------------|
| B_q        | q-axis     | Message A          |
| B_r        | r-axis     | Message B          |
| B_vertical | vertical   | Message C          |
| E_q        | q-axis     | Message D          |

(All four E/B directions carry signals; exact message content is a lore decision.)

Morse encoding: `-` = 1 period, `---` = 3 periods, gap = 1 period, letter gap = 3 periods,
word gap = 7 periods. Period = configurable game constant (e.g. 16 ticks = 1 second real time).

These fields add to the computed circuit fields. Sensors read the total. Players must build
appropriate sensor circuits + signal displays to decode.

---

## 10. Run Mechanic

- Simulation ticks always at 1/16 s real time. The world is always live.
- **Run** button (per input): reloads that Input Via's signal to the start of its test sequence.
  Does not pause or reset the rest of the circuit.
- An **Output Via** continuously samples its node voltage each tick.
- A condition checker compares the output signal against the expected pattern.
- When all output conditions are met for a given input, that input is marked complete and removed.
- Multiple inputs can be active simultaneously.

---

## 11. Rendering

### 11.1 Hex SVG

Each hex cell renders as a **hexagonal SVG clip region**. Component graphics are pre-authored
hexagonal SVGs (flat-top or pointy-top, consistent with coordinate system). This avoids
sub-pixel precision issues and keeps art assets naturally tile-able.

### 11.2 Layer Compositing

Layers render back-to-front (4 → 1). Each layer's traces are drawn with its color at partial
opacity so stacked layers are visible. Components on a layer render above that layer's traces.
Active (non-sleeping) traces render slightly brighter; current-carrying traces glow proportional
to |I|.

### 11.3 Camera

Pan: click-drag in Pan mode (or spacebar-hold in any mode).
Zoom: scroll wheel. Zoom range: ~4 hexes across screen to ~200 hexes across screen.
At low zoom, component details collapse to colored dots.

### 11.4 Field Visualization (optional toggle)

A debug/aesthetic toggle overlays field strength as a color heatmap on the hex grid.
Separate toggles for B and E. Useful for puzzle-solving and looks cool.

---

## 12. Technical Stack

| Concern          | Choice                                      |
|------------------|---------------------------------------------|
| Language         | Pure JS (no framework)                      |
| Rendering        | Canvas 2D (hex grid) + SVG (component art) |
| World storage    | `Map<string, HexCell>` keyed `"q,r"`       |
| Trace storage    | `Map<string, TraceNode>` keyed `"q,r,layer"` |
| MNA solver       | Sparse matrix, LU decomposition (hand-rolled or numeric.js) |
| Field compute    | Per-tick spatial accumulation with hex-distance cutoff |
| UI               | HTML/CSS overlay (toolbar, panels) on canvas |
| Save format      | JSON serialization of world Map (only non-empty cells) |

---

## 13. Open Questions / Future

- **Move mode**: validate that destination footprint is clear before drop; if not, show red
  highlight and disallow. Lifted component shown as ghost following cursor.
- **Undo/redo**: command stack on draw, erase, place, delete, parameter change.
- **Save/load**: JSON world export. Could support shareable puzzle links via URL-encoded world state.
- **Puzzle authoring**: a separate mode where a designer locks certain cells, sets victory
  conditions, and exports a puzzle definition.
- **Mutual inductance puzzles**: two inductors placed near each other with interesting coupling
  ratios could form the basis of transformer puzzles.
- **Trace antenna**: traces above a length threshold start radiating/receiving in the EM field
  simulation. Length threshold = (game constant) / frequency of signal. Could be a late-game mechanic.
- **Audio**: morse signals could also play as audio when a signal display is placed.