/* core.js -- constants, RNG and small numeric helpers shared by the port.
 *
 * Ported from the 1999 C++ "fleaz" sources (fleaz.h, mapNode.h, utils.C).
 * The C code mixed random()/rand() from libc; here a single seeded generator
 * stands in for both so that a run is reproducible from its seed.
 */

/* ---- game constants, verbatim from fleaz.h ---------------------------- */

const INFO_FOOD  = 0;
const INFO_FLEA  = 1;
const INFO_EAST  = 2;
const INFO_NORTH = 3;
const INFO_SOUTH = 4;
const INFO_WEST  = 5;

const ACT_NOP    = 0;
const ACT_MNORTH = 1;
const ACT_MSOUTH = 2;
const ACT_MEAST  = 3;
const ACT_MWEST  = 4;
const ACT_MNEAST = 5;
const ACT_MNWEST = 6;
const ACT_MSEAST = 7;
const ACT_MSWEST = 8;
const ACT_ATTACK = 9;
const ACT_EAT    = 10;

/* mapNode.h. Note DIR_NONE shares the value 0 with INFO_FOOD -- the original
 * relies on that collision in every fitness function, so it is preserved. */
const MAP_NONE  = 0;
const MAP_FOOD  = 1;
const DIR_NONE  = 0;
const DIR_NORTH = INFO_NORTH;
const DIR_SOUTH = INFO_SOUTH;
const DIR_EAST  = INFO_EAST;
const DIR_WEST  = INFO_WEST;

/* fleaType enum */
const FSM = 0, GP = 1, NN = 2, RANDOM = 3;

const SPECIES = [
  { id: FSM,    key: 'fsm',    name: 'FSM',    color: '#4000e0',
    blurb: 'Finite state machine. A 5184-bit lookup table indexed by what it sees.' },
  { id: GP,     key: 'gp',     name: 'GP',     color: '#cc66cc',
    blurb: 'Genetic program. An evolved if/else tree over the 5x5 sensor block.' },
  { id: NN,     key: 'nn',     name: 'NN',     color: '#009966',
    blurb: 'Neural net. A 6-9-11 backprop network that learns while it lives.' },
  { id: RANDOM, key: 'random', name: 'RANDOM', color: '#336699',
    blurb: 'Control group. Picks an action uniformly at random, never breeds.' },
];

const STATSIZE = 15;   /* how many leaders per species the panel shows */

/* ---- seeded RNG -------------------------------------------------------- */

/* mulberry32: small, fast, good enough for a simulation toy, and seedable so
 * an interesting run can be shared by its seed. */
function makeRng(seed) {
  let a = seed >>> 0;
  return function () {
    a = (a + 0x6d2b79f5) >>> 0;
    let t = a;
    t = Math.imul(t ^ (t >>> 15), t | 1);
    t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

let _next = makeRng(1);

/** Reseed the global generator. */
function seedRng(seed) { _next = makeRng(seed); }

/** Stand-in for C random()/rand(): a non-negative int in [0, n). */
function rnd(n) { return (_next() * n) | 0; }

/** Uniform float in [0, 1). */
function rndf() { return _next(); }

/* ---- numeric helpers --------------------------------------------------- */

/* C's rint() rounds half to even under the default rounding mode; Math.round
 * rounds half up. The FSM's state index is sensitive to this, so do it right. */
function rint(x) {
  const f = Math.floor(x);
  const d = x - f;
  if (d > 0.5) return f + 1;
  if (d < 0.5) return f;
  return (f % 2 === 0) ? f : f + 1;
}

/* soul.fitness is a UINT in the original. Keep values in that range so the
 * runaway GP/NN fitness functions saturate the way they did in 1999. */
function toUint(x) {
  if (!isFinite(x) || x < 0) return 0;
  return Math.min(Math.trunc(x), 4294967295);
}

/** A fresh fleaInfo struct (fleaz.h). */
function newSoul() {
  return {
    age: 0, disposition: 0,
    alive: 0, lifespan: 0, numMoves: 0, distMoved: 0,
    numEats: 0, foodCrossed: 0, conflicts: 0, wins: 0,
    fitness: 0, mutations: 0,
  };
}
