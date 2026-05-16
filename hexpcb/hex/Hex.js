const SQRT3 = Math.sqrt(3);

// ── Direction ─────────────────────────────────────────────────────────────────
//
// Six neighbor directions in clockwise order starting from East.
// Stored as index 0–5; the integer encodes component rotation (§6.3).

export const Direction = Object.freeze({
    E:  0,
    NE: 1,
    NW: 2,
    W:  3,
    SW: 4,
    SE: 5,
});

// Δ(q, r) for each direction index, matching the table in §2.1.
const DVEC = [
    [ 1,  0],   // 0  E
    [ 1, -1],   // 1  NE
    [ 0, -1],   // 2  NW
    [-1,  0],   // 3  W
    [-1,  1],   // 4  SW
    [ 0,  1],   // 5  SE
];

// ── Hex ───────────────────────────────────────────────────────────────────────
//
// Immutable cube-coordinate hex.  Only (q, r) are stored; s = −q − r.
// All operations return new instances.

export class Hex {
    constructor(q, r) {
        this.q = q;
        this.r = r;
    }

    get s() { return -this.q - this.r; }

    // ── Arithmetic ──────────────────────────────────────────────────────────────

    add(b)   { return new Hex(this.q + b.q, this.r + b.r); }
    sub(b)   { return new Hex(this.q - b.q, this.r - b.r); }
    scale(k) { return new Hex(this.q * k,   this.r * k  ); }
    negate() { return new Hex(-this.q,      -this.r      ); }

    // ── Neighbors ───────────────────────────────────────────────────────────────

    /** Single neighbor in direction dir (0–5). */
    neighbor(dir) {
        const [dq, dr] = DVEC[dir];
        return new Hex(this.q + dq, this.r + dr);
    }

    /** All 6 neighbors in order E, NE, NW, W, SW, SE. */
    neighbors() {
        return DVEC.map(([dq, dr]) => new Hex(this.q + dq, this.r + dr));
    }

    // ── Distance ────────────────────────────────────────────────────────────────

    /** Hex-space distance (§2.1). Always an integer. */
    distanceTo(b) {
        return (Math.abs(this.q - b.q) + Math.abs(this.r - b.r) + Math.abs(this.s - b.s)) / 2;
    }

    /** Real-space distance in meters. H is the hex size constant (§2.1). */
    realDistanceTo(b, H) {
        return H * this.distanceTo(b);
    }

    // ── Rotation ─────────────────────────────────────────────────────────────────
    //
    // Each step rotates 60° counter-clockwise in hex space.
    // Negative steps rotate clockwise.
    // Under y-down screen coordinates CCW hex space = CW on screen, so
    // component rotation index 1 (§6.3) corresponds to rotate(-1) here.
    //
    // CCW step formula: (q, r, s) → (−s, −q, −r)

    rotate(steps) {
        const n = ((steps % 6) + 6) % 6;
        let q = this.q, r = this.r, s = this.s;
        for (let i = 0; i < n; i++) [q, r, s] = [-s, -q, -r];
        return new Hex(q, r);
    }

    // ── Screen coordinates (pointy-top orientation) ──────────────────────────────
    //
    // x = size * √3 * (q + r/2)
    // y = size * 3/2 * r
    //
    // Derived from the neighbor vectors: E has Δx = size·√3, Δy = 0; SE has
    // Δx = size·√3/2, Δy = size·3/2 — consistent with a pointy-top layout.

    /** Pixel position of this hex's center. `size` = circumradius in pixels. */
    toPixel(size) {
        return {
            x: size * SQRT3 * (this.q + this.r * 0.5),
            y: size * 1.5   *  this.r,
        };
    }

    // ── Map key ─────────────────────────────────────────────────────────────────

    /** Compact string key for use in world Map (§3.1). */
    key() { return `${this.q},${this.r}`; }

    // ── Equality ────────────────────────────────────────────────────────────────

    equals(b) { return this.q === b.q && this.r === b.r; }

    toString() { return `Hex(${this.q}, ${this.r})`; }

    // ── Static constructors ──────────────────────────────────────────────────────

    /** Unit hex in the given direction. */
    static direction(dir) {
        const [dq, dr] = DVEC[dir];
        return new Hex(dq, dr);
    }

    /** Parse a key produced by Hex#key(). */
    static fromKey(key) {
        const i = key.indexOf(',');
        return new Hex(+key.slice(0, i), +key.slice(i + 1));
    }

    /**
     * Nearest hex to pixel coordinate (px, py).
     * `size` = circumradius in pixels.
     * Inverse of toPixel; uses cube rounding (Hex.round).
     */
    static fromPixel(px, py, size) {
        const q = (SQRT3 / 3 * px - py / 3) / size;
        const r = (2 / 3 * py) / size;
        return Hex.round(q, r);
    }

    /**
     * Cube rounding: snap fractional (q, r) to the nearest integer hex.
     * Recomputes whichever of q/r/s had the largest rounding error so that
     * the cube constraint q + r + s = 0 is preserved exactly.
     */
    static round(q, r) {
        const s = -q - r;
        let qi = Math.round(q), ri = Math.round(r), si = Math.round(s);
        const dq = Math.abs(qi - q), dr = Math.abs(ri - r), ds = Math.abs(si - s);
        if      (dq > dr && dq > ds) qi = -ri - si;
        else if (dr > ds)             ri = -qi - si;
        return new Hex(qi, ri);
    }

    /**
     * All hexes within manhattan distance `radius` (inclusive).
     * Count = 3·radius²  + 3·radius + 1.
     */
    range(radius) {
        const out = [];
        for (let dq = -radius; dq <= radius; dq++) {
            const rMin = Math.max(-radius, -dq - radius);
            const rMax = Math.min( radius, -dq + radius);
            for (let dr = rMin; dr <= rMax; dr++) {
                out.push(new Hex(this.q + dq, this.r + dr));
            }
        }
        return out;
    }

    /**
     * Hexes on the ring at exactly `radius` from this hex.
     * Starts at the SW corner and walks clockwise (on screen).
     * Count = 6·radius for radius ≥ 1.
     */
    ring(radius) {
        if (radius === 0) return [this];
        const out = [];
        // SW corner of the ring
        let h = this.add(Hex.direction(Direction.SW).scale(radius));
        // Walk each of the 6 edges: direction d takes you along edge d of the ring
        for (let d = 0; d < 6; d++) {
            for (let i = 0; i < radius; i++) {
                out.push(h);
                h = h.neighbor(d);
            }
        }
        return out;
    }
}

// ── CellAddress ───────────────────────────────────────────────────────────────
//
// A (q, r, layer) triple addressing a single trace node (§2.2, §3.1).
// layer ∈ {1, 2, 3, 4}; ground is not a layer.

export class CellAddress {
    constructor(q, r, layer) {
        this.q     = q;
        this.r     = r;
        this.layer = layer;
    }

    /** The hex at this cell's grid position. */
    get hex() { return new Hex(this.q, this.r); }

    /** Compact string key for use in traceMap (§3.1). */
    key() { return `${this.q},${this.r},${this.layer}`; }

    equals(b) { return this.q === b.q && this.r === b.r && this.layer === b.layer; }

    toString() { return `Cell(${this.q}, ${this.r}, layer ${this.layer})`; }

    static fromKey(key) {
        const [q, r, layer] = key.split(',');
        return new CellAddress(+q, +r, +layer);
    }

    static fromHex(hex, layer) {
        return new CellAddress(hex.q, hex.r, layer);
    }
}
