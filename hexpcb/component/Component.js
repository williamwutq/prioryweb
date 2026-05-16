import { Hex } from '../hex/Hex.js';

// ── FootprintType ─────────────────────────────────────────────────────────────
//
// Letters match §6.1 exactly.  Used by rendering to pick the right art asset
// and by the placement validator to know the occupied hex pattern.

export const FootprintType = Object.freeze({ A: 'A', B: 'B', C: 'C', D: 'D', E: 'E' });

// ── Shared offset helpers ─────────────────────────────────────────────────────

// Type A: center body + W terminal (a) + E terminal (b).
function aTerms(nameA = 'a', nameB = 'b') {
    return [
        { name: nameA, offset: new Hex(-1,  0) },  // W
        { name: nameB, offset: new Hex( 1,  0) },  // E
    ];
}

// Type C: center body + E terminal (t0) + NW terminal (t1) + SW terminal (t2).
// Screen angles: 0°, 240°, 120° — three equally-spaced directions.
function cTerms(name0, name1, name2) {
    return [
        { name: name0, offset: new Hex( 1,  0) },  // E  (0°)
        { name: name1, offset: new Hex( 0, -1) },  // NW (240°)
        { name: name2, offset: new Hex(-1,  1) },  // SW (120°)
    ];
}

// Type E: center body + W + E + NE + SE terminals.
const E_TERMS = [
    { name: 'left',        offset: new Hex(-1,  0) },  // W
    { name: 'right',       offset: new Hex( 1,  0) },  // E
    { name: 'upper_right', offset: new Hex( 1, -1) },  // NE
    { name: 'lower_right', offset: new Hex( 0,  1) },  // SE
];

const BODY_ORIGIN = [new Hex(0, 0)];

// ── ComponentDef ──────────────────────────────────────────────────────────────
//
// Static description of a component type.  One instance exists per type; it is
// never mutated after construction.
//
// body      – hex offsets (at rotation 0) that are occupied but not electrical
//             terminals.  The center hex of A/C/E components lives here.
// terminals – electrical connection points: { name: string, offset: Hex }[].
//             Traces route into these hexes.  Absent for non-electrical types.
// poles     – Type D only: { name: string, offset: Hex }[] for the two magnet
//             poles.  Not electrical; used by field physics.
// params    – default parameter values copied into each new instance.

export class ComponentDef {
    constructor({ id, name, footprint, params = {}, body = [], terminals = [], poles = [] }) {
        this.id        = id;
        this.name      = name;
        this.footprint = footprint;
        this.params    = params;
        this.body      = body;
        this.terminals = terminals;
        this.poles     = poles;
    }

    // All hex offsets this component occupies (for placement validation).
    get allOffsets() {
        return [
            ...this.body,
            ...this.terminals.map(t => t.offset),
            ...this.poles.map(p => p.offset),
        ];
    }
}

// ── COMPONENTS registry ───────────────────────────────────────────────────────
//
// Keyed by component id.  Import COMPONENTS and look up by id string.

export const COMPONENTS = Object.freeze({

    // ── Type B — single hex ───────────────────────────────────────────────────

    ground_via: new ComponentDef({
        id: 'ground_via', name: 'Ground Via', footprint: FootprintType.B,
        terminals: [{ name: 'node', offset: new Hex(0, 0) }],
        // Simulation: shorts all 4 layers at this cell to GND (node 0).
    }),

    power_via: new ComponentDef({
        id: 'power_via', name: 'Power Via', footprint: FootprintType.B,
        params: { voltage: 5 },
        terminals: [{ name: 'node', offset: new Hex(0, 0) }],
    }),

    input_via: new ComponentDef({
        id: 'input_via', name: 'Input Via', footprint: FootprintType.B,
        params: { inputNumber: 1 },
        terminals: [{ name: 'node', offset: new Hex(0, 0) }],
    }),

    output_via: new ComponentDef({
        id: 'output_via', name: 'Output Via', footprint: FootprintType.B,
        params: { outputNumber: 1 },
        terminals: [{ name: 'node', offset: new Hex(0, 0) }],
    }),

    via: new ComponentDef({
        id: 'via', name: 'Via', footprint: FootprintType.B,
        // layers: subset of [1,2,3,4] to short together at this cell.
        params: { layers: [1, 2] },
        terminals: [{ name: 'node', offset: new Hex(0, 0) }],
    }),

    junction: new ComponentDef({
        id: 'junction', name: 'Junction', footprint: FootprintType.B,
        terminals: [{ name: 'node', offset: new Hex(0, 0) }],
        // Simulation: forces a T/X connection on the active layer.
    }),

    crossover: new ComponentDef({
        id: 'crossover', name: 'Crossover', footprint: FootprintType.B,
        body: BODY_ORIGIN,
        // No electrical terminals: 6 directional channels pass through
        // independently with no internal connection.
    }),

    magnet_v: new ComponentDef({
        id: 'magnet_v', name: 'Vertical Magnet', footprint: FootprintType.B,
        params: { fieldStrength: 1.0 },
        body: BODY_ORIGIN,
        // Not electrical; contributes B_vertical to field physics.
    }),

    charged_ball: new ComponentDef({
        id: 'charged_ball', name: 'Charged Ball', footprint: FootprintType.B,
        params: { charge: 1e-9 },
        body: BODY_ORIGIN,
        // Not electrical; contributes to E field.
    }),

    signal_display: new ComponentDef({
        id: 'signal_display', name: 'Signal Display', footprint: FootprintType.B,
        params: { samplePeriod: 1 },
        terminals: [{ name: 'node', offset: new Hex(0, 0) }],
    }),

    signal_input: new ComponentDef({
        id: 'signal_input', name: 'Signal Input', footprint: FootprintType.B,
        params: { period: 1 },
        terminals: [{ name: 'node', offset: new Hex(0, 0) }],
    }),

    // ── Type A — 3-hex line ───────────────────────────────────────────────────

    resistor: new ComponentDef({
        id: 'resistor', name: 'Simple Resistor', footprint: FootprintType.A,
        params: { resistance: 1000 },
        body: BODY_ORIGIN,
        terminals: aTerms('a', 'b'),
    }),

    inductor: new ComponentDef({
        id: 'inductor', name: 'Simple Inductor', footprint: FootprintType.A,
        params: { loops: 10 },
        body: BODY_ORIGIN,
        terminals: aTerms('a', 'b'),
        // Leaks B field proportional to current × loops.
    }),

    inductor_v: new ComponentDef({
        id: 'inductor_v', name: 'Vertical Inductor', footprint: FootprintType.A,
        params: { loops: 100, coreMaterial: 'air' },
        body: BODY_ORIGIN,
        terminals: aTerms('a', 'b'),
        // coreMaterial: 'air' | 'iron' | 'ferrite'
    }),

    capacitor_plate: new ComponentDef({
        id: 'capacitor_plate', name: 'Simple Plate Capacitor', footprint: FootprintType.A,
        params: { plateDistance: 0.001 },
        body: BODY_ORIGIN,
        terminals: aTerms('a', 'b'),
    }),

    diode: new ComponentDef({
        id: 'diode', name: 'Simple Diode', footprint: FootprintType.A,
        body: BODY_ORIGIN,
        terminals: aTerms('anode', 'cathode'),
        // Directional: current flows anode→cathode only.
        // Rotation distinguishes forward from reverse orientation.
    }),

    diode_zener: new ComponentDef({
        id: 'diode_zener', name: 'Zener Diode', footprint: FootprintType.A,
        params: { zenerVoltage: 5.1 },
        body: BODY_ORIGIN,
        terminals: aTerms('anode', 'cathode'),
    }),

    switch: new ComponentDef({
        id: 'switch', name: 'Switch', footprint: FootprintType.A,
        params: { closed: false },
        body: BODY_ORIGIN,
        terminals: aTerms('a', 'b'),
    }),

    button: new ComponentDef({
        id: 'button', name: 'Button', footprint: FootprintType.A,
        body: BODY_ORIGIN,
        terminals: aTerms('a', 'b'),
        // Acts as variable resistor; actuated when |B| or |E| at body exceeds
        // threshold.  Threshold is a world-level constant, not a param.
    }),

    voltmeter: new ComponentDef({
        id: 'voltmeter', name: 'Voltmeter', footprint: FootprintType.A,
        body: BODY_ORIGIN,
        terminals: aTerms('a', 'b'),
        // High-impedance; reads terminal voltage difference.
    }),

    ammeter: new ComponentDef({
        id: 'ammeter', name: 'Ammeter', footprint: FootprintType.A,
        body: BODY_ORIGIN,
        terminals: aTerms('a', 'b'),
        // Near-zero resistance; reads branch current.
    }),

    // ── Type C — 4-hex star ───────────────────────────────────────────────────

    capacitor_spherical: new ComponentDef({
        id: 'capacitor_spherical', name: '3-Shell Spherical Capacitor', footprint: FootprintType.C,
        body: BODY_ORIGIN,  // center = dielectric
        terminals: cTerms('shell0', 'shell1', 'shell2'),
    }),

    npn: new ComponentDef({
        id: 'npn', name: 'NPN Transistor', footprint: FootprintType.C,
        body: BODY_ORIGIN,
        terminals: cTerms('base', 'collector', 'emitter'),
    }),

    pnp: new ComponentDef({
        id: 'pnp', name: 'PNP Transistor', footprint: FootprintType.C,
        body: BODY_ORIGIN,
        terminals: cTerms('base', 'collector', 'emitter'),
    }),

    potentiometer: new ComponentDef({
        id: 'potentiometer', name: 'Potentiometer', footprint: FootprintType.C,
        params: { wiperPosition: 0.5 },
        body: BODY_ORIGIN,
        terminals: cTerms('high', 'wiper', 'low'),
    }),

    wave_display: new ComponentDef({
        id: 'wave_display', name: 'Wave Display', footprint: FootprintType.C,
        params: { mode: 'voltage' },  // 'voltage' | 'current' | 'fft'
        body: BODY_ORIGIN,
        terminals: cTerms('power', 'gnd', 'signal'),
    }),

    efield_sensor_v: new ComponentDef({
        id: 'efield_sensor_v', name: 'Vertical E-Field Sensor', footprint: FootprintType.C,
        params: { scaleFactor: 1.0 },
        body: BODY_ORIGIN,
        terminals: cTerms('power', 'gnd', 'out'),
        // out voltage ∝ |E_vertical| × scaleFactor × (power rail voltage)
    }),

    // ── Type D — 2-hex pair ───────────────────────────────────────────────────

    magnet: new ComponentDef({
        id: 'magnet', name: 'Simple Magnet', footprint: FootprintType.D,
        params: { fieldStrength: 1.0 },
        // Not electrical; poles are the field sources.
        poles: [
            { name: 'south', offset: new Hex(0, 0) },
            { name: 'north', offset: new Hex(1, 0) },  // E neighbor
        ],
    }),

    // ── Type E — 5-hex cross ──────────────────────────────────────────────────

    efield_sensor_planar: new ComponentDef({
        id: 'efield_sensor_planar', name: 'Planar E-Field Sensor', footprint: FootprintType.E,
        params: { scaleFactor: 1.0 },
        body: BODY_ORIGIN,
        terminals: E_TERMS,
        // Distributes |Eq|, |Er|, |Es| to left/right/upper_right outputs;
        // lower_right is the reference (GND-like) terminal.
    }),
});

// ── ComponentInstance ─────────────────────────────────────────────────────────
//
// A placed component in the world.  Holds a reference to its def plus the
// mutable per-instance state: origin hex, rotation, and parameter values.

export class ComponentInstance {
    constructor(def, origin, rotation = 0, params = null) {
        this.def      = def;
        this.origin   = origin;    // Hex — body reference point
        this.rotation = rotation;  // int 0–5 (§6.3)
        this.params   = params !== null ? { ...params } : { ...def.params };
    }

    // ── Footprint resolution ──────────────────────────────────────────────────

    // All world-space hexes this instance occupies.
    hexes() {
        return this.def.allOffsets.map(h => this.origin.add(h.rotate(this.rotation)));
    }

    // True if this instance occupies the given hex.
    occupies(hex) {
        return this.hexes().some(h => h.equals(hex));
    }

    // ── Terminal resolution ───────────────────────────────────────────────────

    // Resolved electrical terminals: [{ name, hex }].
    terminals() {
        return this.def.terminals.map(({ name, offset }) => ({
            name,
            hex: this.origin.add(offset.rotate(this.rotation)),
        }));
    }

    // Terminal at a specific hex, or null.
    terminalAt(hex) {
        return this.terminals().find(t => t.hex.equals(hex)) ?? null;
    }

    // ── Pole resolution (Type D) ──────────────────────────────────────────────

    // Resolved magnet poles: [{ name, hex }].
    poles() {
        return this.def.poles.map(({ name, offset }) => ({
            name,
            hex: this.origin.add(offset.rotate(this.rotation)),
        }));
    }

    // ── Serialization ─────────────────────────────────────────────────────────

    toJSON() {
        return {
            id:       this.def.id,
            q:        this.origin.q,
            r:        this.origin.r,
            rotation: this.rotation,
            params:   { ...this.params },
        };
    }

    static fromJSON(data) {
        const def = COMPONENTS[data.id];
        if (!def) throw new Error(`Unknown component id: "${data.id}"`);
        return new ComponentInstance(def, new Hex(data.q, data.r), data.rotation, data.params);
    }
}
