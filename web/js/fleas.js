/* fleas.js -- the four species. Port of Flea.C, FleaFSM.C, FleaGP.C,
 * FleaNN.C and FleaRANDOM.C.
 *
 * Each species reads the same 5x5 sensor block and writes a single action
 * into `nextMove`; everything else about them differs. Where the 1999 code
 * had a bug that visibly shaped the results, the bug is preserved and
 * labelled QUIRK, so the simulation still behaves the way the paper's graphs
 * were drawn. `tweaks` (set by the UI) can switch a few of them off.
 */

const tweaks = { gpMutates: false };

let _nextId = 1;

class Flea {
  constructor() {
    this.id = _nextId++;
    this.type = null;
    this.nextMove = 5;              /* "the first dawn, we were heading east" */
    this.remainingFood = 0;
    this.lastInfo = new Int8Array(25);
    this.soul = newSoul();
  }

  think(_info) {}
  mutate() {}
  fitness() {}

  /* Flea::crossover is a no-op stub in the C; only FSM/GP/NN override it. */
  crossover(_mate) { return [null, null]; }

  respirate() {
    this.remainingFood--;
    if (this.remainingFood < 0) this.soul.alive = 0;
  }

  /* The MAXFOOD ceiling is commented out in Flea.C, so a flea that keeps
   * eating keeps banking food without limit. */
  feed(courses) { this.remainingFood += courses; }
}

/* ===================================================================== FSM
 * A lookup table with one 4-bit entry per state. The state is a base-6
 * number built from what the flea senses at the centre (C) and the weighted
 * averages of the block's left, right and "ahead" thirds -- 1296 states,
 * 5184 bits of genome.
 */

const FSMLENGTH = 5184;
const UNIFORM_CHANCE = 3;     /* percent chance of uniform crossover */
const ONEPOINT_CHANCE = 43;   /* cumulative percent for one-point crossover */
const FSM_MUTRATE = 9;        /* per-bit mutation rate, in tenths of a percent */

class FleaFSM extends Flea {
  constructor(genotype) {
    super();
    this.type = FSM;
    this.lastFitness = 0;
    this.xOver = 99;

    if (genotype) { this.genotype = genotype.slice(); return; }

    this.genotype = new Uint8Array(FSMLENGTH);
    for (let i = 0; i < FSMLENGTH; i++) {
      if (i % 4 === 0) {
        /* Bit 0 of each entry picks eat/attack over move. States below 216
         * are the ones where the flea is standing on food (or on nothing at
         * all), so those are born 75% likely to eat; the rest, 25%. */
        this.genotype[i] = (Math.trunc(i / 4) < 216) ? (rnd(4) !== 0 ? 1 : 0)
                                                     : (rnd(4) === 0 ? 1 : 0);
      } else {
        this.genotype[i] = rnd(2);
      }
    }
  }

  think(info) {
    const at = (r, c) => info[r * 5 + c];
    const C = at(2, 2);

    const avgL = (at(0, 0) + 2 * at(1, 0) + 2 * at(2, 0) + 2 * at(0, 1) +
                  3 * at(1, 1) + 2 * at(0, 2) + 3 * at(1, 2) + 2 * at(0, 3)) / 17.0;
    const avgR = (at(4, 0) + 2 * at(3, 0) + 2 * at(4, 1) + 3 * at(3, 1) +
                  3 * at(2, 1) + 2 * at(4, 2) + 3 * at(3, 2) + 2 * at(4, 3)) / 18.0;
    const avgA = (at(0, 4) + 2 * at(1, 4) + 2 * at(2, 4) + 2 * at(3, 4) +
                  at(4, 4) + 3 * at(1, 3) + 3 * at(2, 3) + 3 * at(3, 3)) / 17.0;

    const state = rint(avgA) + 6 * rint(avgL) + 36 * rint(avgR) + 216 * C;
    const i = state * 4;

    if (this.genotype[i]) {
      this.nextMove = (C === INFO_FLEA) ? ACT_ATTACK : ACT_EAT;
    } else {
      /* The remaining three bits are a move index, 1..8. */
      this.nextMove = this.genotype[i + 1] + 2 * this.genotype[i + 2] +
                      4 * this.genotype[i + 3] + 1;
    }
  }

  mutate() {
    for (let i = 0; i < FSMLENGTH; i++) {
      if (FSM_MUTRATE > rnd(1000)) {
        this.soul.mutations++;
        this.genotype[i] = this.genotype[i] ? 0 : 1;
      }
    }
  }

  crossover(mate) {
    const mother = mate, father = this;
    const c1 = new FleaFSM(), c2 = new FleaFSM();
    const dice = rnd(100);
    let xOverType;

    if (dice <= UNIFORM_CHANCE) {
      xOverType = 0;
      for (let i = 0; i < FSMLENGTH; i++) {
        if (rnd(2)) {
          c1.genotype[i] = mother.genotype[i]; c2.genotype[i] = father.genotype[i];
        } else {
          c1.genotype[i] = father.genotype[i]; c2.genotype[i] = mother.genotype[i];
        }
      }
    } else if (dice <= ONEPOINT_CHANCE) {
      xOverType = 1;
      const cut = rnd(FSMLENGTH);
      for (let i = 0; i < cut; i++) {
        c1.genotype[i] = mother.genotype[i]; c2.genotype[i] = father.genotype[i];
      }
      for (let i = cut; i < FSMLENGTH; i++) {
        c1.genotype[i] = father.genotype[i]; c2.genotype[i] = mother.genotype[i];
      }
    } else {
      xOverType = 2;
      const a = rnd(FSMLENGTH);
      const b = a + rnd(FSMLENGTH - a) + 1;
      for (let i = 0; i < FSMLENGTH; i++) {
        const fromMother = (i < a || i >= b);
        c1.genotype[i] = fromMother ? mother.genotype[i] : father.genotype[i];
        c2.genotype[i] = fromMother ? father.genotype[i] : mother.genotype[i];
      }
    }

    /* QUIRK: both children inherit the father's carried-over fitness; the
     * mother's is dropped on the floor. */
    c1.lastFitness = c2.lastFitness = this.lastFitness;
    c1.xOver = c2.xOver = xOverType;
    return [c1, c2];
  }

  /* Rewards staying alive, with bonuses for ending the run near food. The
   * `!lastInfo[...]` tests read as "this cell is food, or has no food in
   * range at all" -- INFO_FOOD and DIR_NONE are both 0. */
  fitness() {
    const L = this.lastInfo, s = this.soul;
    let f = Math.trunc(s.lifespan * 5 / 2) + Math.trunc(this.lastFitness / 2);
    if (!L[12]) f += s.lifespan;
    if (!L[11] || !L[13] || !L[6] || !L[7] || !L[8] || !L[16] || !L[17] || !L[18]) {
      f += Math.trunc(s.lifespan / 2);
    }
    if (!L[5] || !L[6] || !L[7] || !L[8] || !L[9] || !L[10] || !L[14] ||
        !L[15] || !L[19] || !L[20] || !L[21] || !L[22] || !L[23] || !L[24]) {
      f += Math.trunc(s.lifespan / 4);
    }
    s.fitness = toUint(f);
  }
}

/* ====================================================================== GP
 * An evolved decision tree. Interior nodes test one cell of the sensor block
 * against a direction/food/flea value; leaves are actions.
 */

const PTOT = 100;
const PIF_NORTH = 9, PIF_SOUTH = 9, PIF_EAST = 9, PIF_WEST = 9;
const PIF_FOOD = 10, PIF_FLEA = 8;
const PMOVE_NORTH = 9, PMOVE_SOUTH = 9, PMOVE_EAST = 9, PMOVE_WEST = 9;
const PATTACK = 0, PEAT = 10;
const PCOND = PIF_NORTH + PIF_SOUTH + PIF_EAST + PIF_WEST + PIF_FOOD + PIF_FLEA;

const IF_NORTH = 0x11, IF_SOUTH = 0x12, IF_EAST = 0x13;
const IF_WEST = 0x14, IF_FOOD = 0x15, IF_FLEA = 0x16;

const MAXARGS = 25, MINLEVEL = 5, MAXLEVEL = 10, MRATE = 2;

/* The C built this table once into a static char[100] and drew from it. */
const percs = [];
{
  const push = (n, v) => { for (let i = 0; i < n; i++) percs.push(v); };
  push(PIF_NORTH, IF_NORTH); push(PIF_SOUTH, IF_SOUTH);
  push(PIF_EAST, IF_EAST);   push(PIF_WEST, IF_WEST);
  push(PIF_FOOD, IF_FOOD);   push(PIF_FLEA, IF_FLEA);
  push(PMOVE_NORTH, ACT_MNORTH); push(PMOVE_SOUTH, ACT_MSOUTH);
  push(PMOVE_EAST, ACT_MEAST);   push(PMOVE_WEST, ACT_MWEST);
  push(PATTACK, ACT_ATTACK);     push(PEAT, ACT_EAT);
}

/* The value each conditional tests for.
 * QUIRK: IF_EAST tests for INFO_WEST and IF_WEST tests for INFO_EAST. The C
 * assigned &if_west to the IF_EAST case and vice versa, in makeNode() and
 * again in _mutate(), so every GP flea ever bred has had east and west
 * crossed. Left in place -- it is load-bearing for these results. */
const COND_TEST = {
  [IF_NORTH]: INFO_NORTH, [IF_SOUTH]: INFO_SOUTH,
  [IF_EAST]:  INFO_WEST,  [IF_WEST]:  INFO_EAST,
  [IF_FOOD]:  INFO_FOOD,  [IF_FLEA]:  INFO_FLEA,
};

const COND_LABEL = {
  [IF_NORTH]: 'NORTH', [IF_SOUTH]: 'SOUTH', [IF_EAST]: 'EAST',
  [IF_WEST]: 'WEST', [IF_FOOD]: 'FOOD', [IF_FLEA]: 'FLEA',
};

const ACT_LABEL = {
  [ACT_MNORTH]: 'MOVE_NORTH', [ACT_MSOUTH]: 'MOVE_SOUTH',
  [ACT_MEAST]: 'MOVE_EAST', [ACT_MWEST]: 'MOVE_WEST',
  [ACT_ATTACK]: 'ATTACK', [ACT_EAT]: 'EAT',
};

const isCond = (action) => action >= IF_NORTH && action <= IF_FLEA;

class FleaGP extends Flea {
  constructor(parent) {
    super();
    this.type = GP;
    this.nCount = 0;
    this.nLeft = 0;

    if (parent) {
      this.nCount = parent.nCount;
      this.root = this.copyNode(parent.root, null);
      this.prevFitness = parent.soul.fitness;
    } else {
      this.root = this.makeNode(0, null);
      this.prevFitness = 0;
    }
  }

  makeNode(level, parent) {
    const node = { command: 0, argIdx: 0, action: 0, parent, cTrue: null, cFalse: null };
    this.nCount++;

    /* Force conditionals near the root and actions past MAXLEVEL, so every
     * tree is between 5 and 10 levels deep. */
    let w;
    if (level < MINLEVEL) w = rnd(PCOND);
    else if (level > MAXLEVEL) w = PCOND + rnd(PTOT - PCOND);
    else w = rnd(PTOT);

    node.action = percs[w];
    if (isCond(node.action)) {
      node.command = 1;
      /* QUIRK: every conditional looks at cell 12 -- the flea's own square --
       * because the random argIdx was commented out. IF_EAST is the one case
       * that kept it, so only east-tests ever look anywhere else. */
      node.argIdx = (node.action === IF_EAST) ? rnd(MAXARGS) : 12;
      node.cTrue = this.makeNode(level + 1, node);
      node.cFalse = this.makeNode(level + 1, node);
    }
    return node;
  }

  copyNode(src, parent) {
    const node = {
      command: src.command, argIdx: src.argIdx, action: src.action,
      parent, cTrue: null, cFalse: null,
    };
    if (node.command) {
      node.cTrue = this.copyNode(src.cTrue, node);
      node.cFalse = this.copyNode(src.cFalse, node);
    }
    return node;
  }

  /* Pre-order walk that stops on the nLeft'th node. Never returns the root,
   * so crossover always has a parent to splice against. */
  randNode(node) {
    if (!node) {
      if (this.nCount === 1) return this.root;
      this.nLeft = rnd(this.nCount - 1) + 1;
      return this.randNode(this.root);
    }
    if (!this.nLeft) return node;
    this.nLeft--;
    let found = null;
    if (node.command) {
      found = this.randNode(node.cTrue);
      if (!found) found = this.randNode(node.cFalse);
    }
    return found;
  }

  evalNode(n, info) {
    while (n.command) {
      n = (info[n.argIdx] === COND_TEST[n.action]) ? n.cTrue : n.cFalse;
    }
    return n.action;
  }

  think(info) { this.nextMove = this.evalNode(this.root, info); }

  reCalcSize(node) {
    if (!node) { this.nCount = 0; this.reCalcSize(this.root); return; }
    this.nCount++;
    if (node.command) { this.reCalcSize(node.cTrue); this.reCalcSize(node.cFalse); }
  }

  /* Subtree swap: copy both parents, pick a node in each, trade them. */
  crossover(mate) {
    const c1 = new FleaGP(this);
    const c2 = new FleaGP(mate);
    const n1 = c1.randNode(null);
    const n2 = c2.randNode(null);
    const p1 = n1.parent, p2 = n2.parent;

    if (p1.cTrue === n1) p1.cTrue = n2; else p1.cFalse = n2;
    n2.parent = p1;
    if (p2.cTrue === n2) p2.cTrue = n1; else p2.cFalse = n1;
    n1.parent = p2;

    c1.reCalcSize(null);
    c2.reCalcSize(null);
    return [c1, c2];
  }

  /* QUIRK: in the C this was declared `mutate(int rate)`, which does not
   * override the base class's `mutate()`. Midwife called the base version, so
   * GP fleaz never mutated at all -- they only ever recombined. The "GP
   * mutation" switch in the UI restores the intent. */
  mutate() { if (tweaks.gpMutates) this._mutate(null); }

  _mutate(node) {
    if (!node) { this._mutate(this.root); return; }
    if (rnd(100) < MRATE) {
      if (node.command) {
        node.action = percs[rnd(PCOND)];
        node.argIdx = (node.action === IF_EAST) ? rnd(MAXARGS) : 12;
      } else {
        node.action = percs[PCOND + rnd(PTOT - PCOND)];
      }
    }
    if (node.command) { this._mutate(node.cTrue); this._mutate(node.cFalse); }
  }

  /* Eating is everything here: fitness is quadratic in meals, and the three
   * proximity tests overwrite rather than accumulate, so the widest ring that
   * matches is the one that counts. */
  fitness() {
    const L = this.lastInfo, s = this.soul;
    let partial = 0;
    if (!L[12]) partial = (s.numEats + 1) * 20;
    if (!L[11] || !L[13] || !L[6] || !L[7] || !L[8] || !L[16] || !L[17] || !L[18]) {
      partial = (s.numEats + 1) * 10;
    }
    if (!L[5] || !L[6] || !L[7] || !L[8] || !L[9] || !L[10] || !L[14] ||
        !L[15] || !L[19] || !L[20] || !L[21] || !L[22] || !L[23] || !L[24]) {
      partial = (s.numEats + 1) * 5;
    }

    let f = Math.pow(s.numEats + 1, 2) + partial + s.numEats * 50;
    if (f + f / 2 > 500000000) f = 500000000;
    else f += Math.trunc(this.prevFitness / 2);
    s.fitness = toUint(f);
    this.prevFitness = s.fitness;
  }

  /* FleaGP::_display -- the evolved program as source. */
  toSource(n, out) {
    n = n || this.root;
    out = out || [];
    if (n.command) {
      const c = n.argIdx % 5, r = Math.trunc(n.argIdx / 5);
      out.push(`if(<${c},${r}>==${COND_LABEL[n.action]}){`);
      this.toSource(n.cTrue, out);
      out.push('}else{');
      this.toSource(n.cFalse, out);
      out.push('}');
    } else {
      out.push(`act(${ACT_LABEL[n.action] || n.action})`);
    }
    return out.join('');
  }
}

/* ====================================================================== NN
 * A net that learns during its lifetime. It is graded not on what it does
 * but on how often its chosen action matches a hand-written "ideal" move,
 * and it trains against that same oracle on samples drawn from a shared
 * repository of recently-seen sensor blocks.
 */

const NN_INPUT_NODES = 6, NN_HIDDEN_NODES = 9, NN_OUTPUT_NODES = 11;
const NN_MUT_RATE = 100, NN_ALPHA = 0.23;
const REPO_SIZE = 50;

/* InfoRepository.C -- one global pool, shared by every NN flea alive. */
const infoRepository = [];
let currentRepoSize = 0;
function resetRepository() { infoRepository.length = 0; currentRepoSize = 0; }

class FleaNN extends Flea {
  constructor(net) {
    super();
    this.type = NN;
    this.nn = net || new ElmanNet(NN_INPUT_NODES, NN_HIDDEN_NODES,
                                  NN_OUTPUT_NODES, NN_ALPHA);
    this.correctPredictions = 0;
    this.prevFitness = 0;
  }

  nnRandom(base) { return rnd(base) / 100.0; }

  /* QUIRK: the net is given a one-hot encoding of the centre cell only. The
   * loop that fed it all 25 cells is commented out in FleaNN.C, so 24/25ths
   * of the sensor block never reaches the network. */
  info2input(info) {
    const input = new Float32Array(NN_INPUT_NODES);
    for (let i = 0; i < NN_INPUT_NODES; i++) input[i] = (i === info[12]) ? 1.0 : -1.0;
    return input;
  }

  /* The oracle: what the flea "should" do, given what it is standing on and
   * the cell beyond it in that direction. */
  info2target(info) {
    const at = (r, c) => info[r * 5 + c];
    let action;
    switch (at(2, 2)) {
      case INFO_FOOD: case INFO_FLEA: action = ACT_EAT; break;
      case INFO_EAST:
        action = at(2, 3) === INFO_NORTH ? ACT_MNEAST
               : at(2, 3) === INFO_SOUTH ? ACT_MSEAST : ACT_MEAST;
        break;
      case INFO_WEST:
        action = at(2, 1) === INFO_NORTH ? ACT_MNWEST
               : at(2, 1) === INFO_SOUTH ? ACT_MSWEST : ACT_MWEST;
        break;
      case INFO_NORTH:
        action = at(1, 2) === INFO_EAST ? ACT_MNEAST
               : at(1, 2) === INFO_WEST ? ACT_MNWEST : ACT_MNORTH;
        break;
      case INFO_SOUTH:
        action = at(3, 2) === INFO_EAST ? ACT_MSEAST
               : at(3, 2) === INFO_WEST ? ACT_MSWEST : ACT_MSOUTH;
        break;
      default: action = ACT_EAT; break;
    }
    const target = new Float32Array(NN_OUTPUT_NODES);
    for (let j = 0; j < NN_OUTPUT_NODES; j++) target[j] = (j === action) ? 1.0 : -1.0;
    return target;
  }

  think(info) {
    const output = this.nn.simulate(this.info2input(info));
    const target = this.info2target(info);

    this.nextMove = 0;
    let correct = 0;
    for (let i = 1; i < NN_OUTPUT_NODES; i++) {
      if (output[i] > output[this.nextMove]) this.nextMove = i;
      if (target[i] > target[correct]) correct = i;
    }
    if (this.nextMove === correct) this.correctPredictions++;

    /* Stash this sensor block for everyone to train on later; once the pool
     * is full, new blocks displace random old ones. */
    const copy = Int8Array.from(info);
    if (currentRepoSize < REPO_SIZE) infoRepository[currentRepoSize++] = copy;
    else infoRepository[Math.trunc(this.nnRandom(currentRepoSize * 100))] = copy;
  }

  /* "Mutation" is really lifetime learning: three backprop steps on one
   * remembered sensor block. The rate test always passes -- nnRandom(10000)
   * tops out at 99.99, which is below NN_MUT_RATE. */
  mutate() {
    const r = Math.trunc(this.nnRandom(10000));
    if (r < NN_MUT_RATE && currentRepoSize > 0) {
      const which = Math.min(Math.trunc(this.nnRandom(currentRepoSize * 100)),
                             currentRepoSize - 1);
      const info = infoRepository[which];
      const input = this.info2input(info);
      const target = this.info2target(info);
      for (let p = 0; p < 3; p++) this.nn.train(input, target);
    }
  }

  /* QUIRK: no genetic material is exchanged. Each child is a straight clone
   * of one parent, so the NN line evolves only by selection plus whatever it
   * learned while alive -- Lamarck, not Mendel. */
  crossover(mate) {
    const c1 = new FleaNN(ElmanNet.copy(this.nn, this.nn.alpha));
    const c2 = new FleaNN(ElmanNet.copy(mate.nn, mate.nn.alpha));
    this.correctPredictions = 0;
    mate.correctPredictions = 0;
    return [c1, c2];
  }

  fitness() {
    const s = this.soul;
    let f = (s.numMoves === 0) ? 0
          : Math.pow(this.correctPredictions, 2) * Math.pow(1 + s.numEats, 2);
    f += Math.trunc(this.prevFitness / 2);
    s.fitness = toUint(f);
    this.prevFitness = s.fitness;
  }
}

/* ================================================================== RANDOM
 * The control group. Never breeds, never mutates -- main.C copies RANDOM
 * fleaz straight into the next generation without going near the Midwife.
 */

class FleaRANDOM extends Flea {
  constructor() { super(); this.type = RANDOM; }

  think(_info) { this.nextMove = rnd(11); }

  fitness() {
    const L = this.lastInfo, s = this.soul;
    let f = Math.trunc(s.lifespan * 7 / 2);
    if (!L[12]) f += s.lifespan * 2;
    if (!L[11] || !L[13] || !L[6] || !L[7] || !L[8] || !L[16] || !L[17] || !L[18]) {
      f += s.lifespan;
    }
    if (!L[5] || !L[6] || !L[7] || !L[8] || !L[9] || !L[10] || !L[14] ||
        !L[15] || !L[19] || !L[20] || !L[21] || !L[22] || !L[23] || !L[24]) {
      f += Math.trunc(s.lifespan / 2);
    }
    s.fitness = toUint(f);
  }
}

function makeFlea(type) {
  switch (type) {
    case FSM: return new FleaFSM();
    case GP: return new FleaGP();
    case NN: return new FleaNN();
    case RANDOM: return new FleaRANDOM();
  }
}
