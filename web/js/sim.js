/* sim.js -- the run. Port of RingMaster.C, Population.C, Midwife.C and the
 * generation loop at the bottom of main.C.
 *
 * The C program ran a blocking while(1): drain the population into
 * tournaments, score, sort, breed, repeat. Here the same loop is turned
 * inside out so the page can drive it one tick at a time.
 */

/* ---- Population (Population.C) ----------------------------------------- */

class Population {
  constructor(items) { this.v = items || []; }

  get length() { return this.v.length; }
  size() { return this.v.length; }
  add(flea) { if (flea) this.v.push(flea); }
  locate(idx) { return this.v[idx]; }

  /* Both getFlea() forms *remove* the flea -- populations hand ownership
   * over rather than sharing it. */
  getFlea(idx) {
    if (idx === undefined) {
      if (!this.v.length) return null;
      idx = rnd(this.v.length);
    }
    if (idx >= this.v.length) return null;
    return this.v.splice(idx, 1)[0];
  }

  /* Drain another population into this one, in random order. */
  merge(pop) {
    let i = pop.size();
    while (i--) this.add(pop.getFlea());
  }

  /** Take `n` fleaz at random out of `pop`. (Population(int, Population*)) */
  static sample(n, pop) {
    const out = new Population();
    while (n--) {
      const f = pop.getFlea();
      if (!f) break;
      out.add(f);
    }
    return out;
  }

  /** `n` freshly rolled fleaz of one species. */
  static spawn(n, type) {
    const out = new Population();
    for (let i = 0; i < n; i++) out.add(makeFlea(type));
    return out;
  }

  sortByFitness() { this.v.sort((a, b) => a.soul.fitness - b.soul.fitness); }
  sortByType() { this.v.sort((a, b) => a.type - b.type); }
}

/* ---- Midwife (Midwife.C) ----------------------------------------------- */

/* Cull the bottom half by fitness, then pair the survivors off at random.
 * Every pair yields four fleaz -- both parents, aged a year, plus two
 * children -- so the species holds its headcount. This is why the option
 * parser insists each species count is divisible by four.
 */
function deliver(pop) {
  const out = new Population();
  const pivot = Math.trunc(pop.size() / 2);

  for (let i = 0; i < pivot; i++) pop.getFlea(0);   /* least fit first */

  for (let i = 0; i < pivot; i += 2) {
    const f1 = pop.getFlea();
    const f2 = pop.getFlea();
    if (!f1 || !f2) break;

    const [f3, f4] = f1.crossover(f2);
    for (const f of [f1, f2, f3, f4]) {
      if (!f) continue;
      f.mutate();
      f.soul.age++;
      out.add(f);
    }
  }
  return out;
}

/* A frozen description of one flea, for the specimen readout. Stands in for
 * the display() each species implemented in the C. */
function snapshotFlea(f) {
  switch (f.type) {
    case FSM: {
      let eatStates = 0;
      for (let i = 0; i < FSMLENGTH; i += 4) if (f.genotype[i]) eatStates++;
      return { kind: 'fsm', bits: f.genotype.slice(0, 480),
               eatStates, mutations: f.soul.mutations };
    }
    case GP:
      return { kind: 'gp', nodes: f.nCount, source: f.toSource() };
    case NN:
      return { kind: 'nn', alpha: f.nn.alpha, correct: f.correctPredictions,
               inCt: f.nn.inCt, hiddenCt: f.nn.hiddenCt,
               weights: f.nn.weights_1.slice() };
    default:
      return { kind: 'random' };
  }
}

/* ---- Tournament (RingMaster.C) ----------------------------------------- */

const MAX_TOURNAMENT_TICKS = 5000;   /* not in the C; a browser safety net */

class Tournament {
  constructor(pop, opts) {
    this.pop = pop;
    this.opts = opts;
    this.curCycle = 0;
    this.popLeft = pop.size();
    this.info = new Int8Array(25);

    this.loc = [];
    this.beg = [];
    for (let i = 0; i < pop.size(); i++) {
      const p = { x: rnd(opts.boardX), y: rnd(opts.boardY) };
      this.loc.push(p);
      this.beg.push({ x: p.x, y: p.y });

      const f = pop.locate(i);
      const s = f.soul;
      s.alive = 1;
      s.lifespan = s.numMoves = s.numEats = 0;
      s.conflicts = s.wins = s.fitness = 0;
      s.foodCrossed = 0;
      f.remainingFood = 0;
      f.feed(opts.maxFood);
    }

    this.board = new Board(opts.nFood, opts.boardX, opts.boardY);
  }

  get done() { return this.popLeft === 0 || this.curCycle >= MAX_TOURNAMENT_TICKS; }

  /** One pass of RingMaster::run's inner loop. */
  tick() {
    this.thinkAct();
    this.board.reCalc();
    this.curCycle++;
    if (this.done) this.finish();
  }

  thinkAct() {
    const pop = this.pop, n = pop.size(), board = this.board, loc = this.loc;

    /* Everyone perceives and decides before anyone moves. */
    this.popLeft = 0;
    for (let i = 0; i < n; i++) {
      const f = pop.locate(i);
      if (!f.soul.alive) continue;
      board.getInfo(this.info, loc[i].x, loc[i].y, pop.v, loc);
      f.think(this.info);
      f.lastInfo.set(this.info);
      f.soul.lifespan++;
      this.popLeft++;
    }

    const respNow = (this.curCycle % this.opts.breathFreq) === 0;

    for (let i = 0; i < n; i++) {
      const f = pop.locate(i);

      /* Respiration is charged to the dead as well; harmless, and it is what
       * the C did. */
      if (respNow) f.respirate();
      if (!f.soul.alive) continue;

      const cell = loc[i].y * board.dimX + loc[i].x;
      if (board.type[cell] === MAP_FOOD) f.soul.foodCrossed++;

      let dx = 0, dy = 0;
      switch (f.nextMove) {
        case ACT_MNORTH: dy--; break;
        case ACT_MSOUTH: dy++; break;
        case ACT_MEAST:  dx++; break;
        case ACT_MWEST:  dx--; break;
        case ACT_MNEAST: dx++; dy--; break;
        case ACT_MNWEST: dx--; dy--; break;
        case ACT_MSEAST: dx++; dy++; break;
        case ACT_MSWEST: dx--; dy++; break;
        case ACT_ATTACK: break;   /* declared, never implemented */
        case ACT_EAT:
          if (board.eatFood(loc[i].x, loc[i].y)) {
            f.feed(this.opts.maxFood);
            f.soul.numEats++;
          }
          break;
      }

      if (!dx && !dy) continue;

      /* A move that would leave the board is not refused outright: a
       * diagonal is allowed to slide along whichever edge still fits. */
      const nx = loc[i].x + dx, ny = loc[i].y + dy;
      const okX = nx >= 0 && nx < board.dimX;
      const okY = ny >= 0 && ny < board.dimY;
      if (okX && okY) { loc[i].x = nx; loc[i].y = ny; f.soul.numMoves++; }
      else if (okX)   { loc[i].x = nx; f.soul.numMoves++; }
      else if (okY)   { loc[i].y = ny; f.soul.numMoves++; }
    }
  }

  /* distMoved is the straight-line displacement from where the flea started,
   * not the length of the path it walked -- pacing in circles earns nothing. */
  finish() {
    for (let i = 0; i < this.pop.size(); i++) {
      const dx = this.loc[i].x - this.beg[i].x;
      const dy = this.loc[i].y - this.beg[i].y;
      this.pop.locate(i).soul.distMoved = Math.trunc(Math.sqrt(dx * dx + dy * dy));
    }
  }
}

/* ---- The run (main.C) --------------------------------------------------- */

const DEFAULT_OPTS = {
  boardX: 50, boardY: 50, nFood: 25, maxFood: 5, breathFreq: 15,
  tournSize: 100, nFsm: 100, nGp: 100, nNn: 100, nRandom: 100,
  seed: 1999,
};

/* Options.C rounds each species count up until it is divisible by four, so
 * that the Midwife's pair-and-quadruple arithmetic comes out even. */
function roundSpecies(n) {
  n = Math.max(0, Math.trunc(n));
  while (n && (n <= 2 || (n % 2) || ((n / 2) % 2))) n++;
  return n;
}

class Run {
  constructor(opts) {
    this.opts = Object.assign({}, DEFAULT_OPTS, opts);
    for (const k of ['nFsm', 'nGp', 'nNn', 'nRandom']) {
      this.opts[k] = roundSpecies(this.opts[k]);
    }

    seedRng(this.opts.seed);
    resetRepository();

    this.generation = 0;
    this.tournamentNo = 0;
    this.stats = { fsm: [], gp: [], nn: [], random: [] };
    this.history = [];            /* one entry per completed generation */
    this.log = [];

    this.inPop = new Population();
    this.outPop = new Population();
    for (const [n, type] of [[this.opts.nFsm, FSM], [this.opts.nGp, GP],
                             [this.opts.nNn, NN], [this.opts.nRandom, RANDOM]]) {
      if (n) this.inPop.merge(Population.spawn(n, type));
    }

    this.tournamentsPerGen = Math.max(1,
      Math.ceil(this.inPop.size() / this.opts.tournSize));
    this.startTournament();
  }

  get population() { return this.opts.nFsm + this.opts.nGp + this.opts.nNn + this.opts.nRandom; }

  startTournament() {
    if (!this.inPop.size()) { this.tournament = null; return; }
    this.tournamentNo++;
    this.tournament = new Tournament(
      Population.sample(this.opts.tournSize, this.inPop), this.opts);
  }

  /** Advance the whole run by one simulation tick. */
  tick() {
    if (!this.tournament) { this.breed(); return; }
    this.tournament.tick();
    if (this.tournament.done) {
      this.outPop.merge(this.tournament.pop);
      this.startTournament();
      if (!this.tournament) this.breed();
    }
  }

  /* End of generation: score everyone, take the leader boards, then hand
   * each species to the Midwife. */
  breed() {
    const out = this.outPop;
    for (let i = 0; i < out.size(); i++) out.locate(i).fitness();
    out.sortByFitness();

    this.generation++;
    this.tournamentNo = 0;
    out.sortByType();

    const gen = { gen: this.generation };
    const order = [
      ['fsm', this.opts.nFsm], ['gp', this.opts.nGp],
      ['nn', this.opts.nNn], ['random', this.opts.nRandom],
    ];

    for (const [key, count] of order) {
      if (!count) { this.stats[key] = []; continue; }

      const tPop = new Population();
      for (let i = 0; i < count; i++) tPop.add(out.getFlea(0));
      tPop.sortByFitness();

      /* The SVGA panel showed the top STATSIZE of each species, best first.
       * Snapshot rather than keep references: the Midwife is about to mutate
       * these same fleaz and reset the NN counters, and the panel is supposed
       * to show the generation that just ran. */
      const leaders = [];
      for (let i = 0; i < STATSIZE && i < tPop.size(); i++) {
        const f = tPop.locate(tPop.size() - i - 1);
        leaders.push({
          id: f.id, numEats: f.soul.numEats, distMoved: f.soul.distMoved,
          age: f.soul.age, lifespan: f.soul.lifespan, fitness: f.soul.fitness,
          mutations: f.soul.mutations,
          specimen: i === 0 ? snapshotFlea(f) : null,
        });
      }
      this.stats[key] = leaders;

      let best = 0, total = 0;
      for (let i = 0; i < tPop.size(); i++) {
        const fit = tPop.locate(i).soul.fitness;
        if (fit > best) best = fit;
        total += fit;
      }
      gen[key] = { best, mean: tPop.size() ? total / tPop.size() : 0 };

      /* RANDOM is the control: it is copied forward untouched. Everyone else
       * goes through crossover and mutation. */
      if (key === 'random') this.inPop.merge(tPop);
      else this.inPop.merge(deliver(tPop));
    }

    this.history.push(gen);
    if (this.history.length > 400) this.history.shift();

    this.log.unshift(`gen ${String(this.generation).padStart(3, ' ')}  ` +
      order.filter(([k, c]) => c && gen[k])
           .map(([k]) => `${k}=${gen[k].best}`).join('  '));
    if (this.log.length > 60) this.log.pop();

    this.startTournament();
  }
}
