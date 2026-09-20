/* app.js -- UI wiring. Nothing here is a port; this is the part main.C
 * didn't need, because it owned the whole screen and never had to stop.
 */

(function () {
  'use strict';

  const $ = (sel) => document.querySelector(sel);
  const el = (tag, cls, text) => {
    const n = document.createElement(tag);
    if (cls) n.className = cls;
    if (text !== undefined) n.textContent = text;
    return n;
  };

  const views = { modern: new ModernBoardView($('#board')), vga: new VgaBoardView($('#board')) };
  let boardView = views.modern;
  const chart = new FitnessChart($('#chart'));
  const chartStyle = { dim: '#79838f', grid: '#1a2029', mono: '"IBM Plex Mono", monospace' };

  const trails = new Trails();

  let run = null;
  let playing = true;

  /* Speed is a rate in ticks per second, mapped logarithmically off the
   * slider so the slow end is usable. At the bottom it is one tick every two
   * seconds -- slow enough to watch a single flea decide -- and at the top it
   * is fast enough to burn through generations. Ticks are driven off elapsed
   * time rather than frames, so the number on the slider is honest. */
  const TPS_MIN = 0.5, TPS_MAX = 3000;
  const MAX_BURST = 4000;          /* ticks per frame, so a stall can't freeze the tab */
  const tpsFor = (v) => TPS_MIN * Math.pow(TPS_MAX / TPS_MIN, v / 100);
  let tps = tpsFor(62);
  let tickAcc = 0, lastTs = 0;
  let selected = 'fsm';
  let lastGenSeen = -1;

  /* ---- controls ---- */

  const fields = {
    boardX: $('#f-boardx'), boardY: $('#f-boardy'), nFood: $('#f-food'),
    maxFood: $('#f-feed'), breathFreq: $('#f-breath'), tournSize: $('#f-tourn'),
    nFsm: $('#f-fsm'), nGp: $('#f-gp'), nNn: $('#f-nn'), nRandom: $('#f-random'),
    seed: $('#f-seed'),
  };

  function readFields() {
    const opts = {};
    for (const [k, input] of Object.entries(fields)) {
      const v = parseInt(input.value, 10);
      opts[k] = isNaN(v) ? DEFAULT_OPTS[k] : Math.max(0, v);
    }
    opts.boardX = Math.min(Math.max(opts.boardX, 8), 160);
    opts.boardY = Math.min(Math.max(opts.boardY, 8), 160);
    opts.nFood = Math.max(1, opts.nFood);
    opts.tournSize = Math.max(4, opts.tournSize);
    opts.breathFreq = Math.max(1, opts.breathFreq);
    return opts;
  }

  function writeFields(opts) {
    for (const [k, input] of Object.entries(fields)) input.value = opts[k];
  }

  /* Open on a run that is already underway. Two generations cost about a
   * tenth of a second and mean the leader board, the log and the chart all
   * have something in them on the first frame instead of three empty boxes. */
  const WARM_GENERATIONS = 2;

  function restart(opts) {
    tweaks.gpMutates = $('#f-gpmut').checked;
    run = new Run(opts || readFields());
    writeFields(run.opts);

    const budget = 400000;
    for (let i = 0; i < budget && run.generation < WARM_GENERATIONS; i++) run.tick();
    trails.step(run.tournament);

    lastGenSeen = -1;
    tickAcc = 0;
    if (boardView.reset) boardView.reset();
    renderAll();
  }

  $('#play').addEventListener('click', () => {
    playing = !playing;
    $('#play').textContent = playing ? 'Pause' : 'Play';
    $('#play').setAttribute('aria-pressed', String(playing));
  });

  function stepOnce() {
    run.tick();
    trails.step(run.tournament);
  }

  $('#step').addEventListener('click', () => {
    playing = false;
    $('#play').textContent = 'Play';
    $('#play').setAttribute('aria-pressed', 'false');
    stepOnce();
    renderAll();
  });

  $('#skip').addEventListener('click', () => {
    const target = run.generation + 1;
    const budget = 200000;
    for (let i = 0; i < budget && run.generation < target; i++) stepOnce();
    renderAll();
  });

  $('#restart').addEventListener('click', () => restart());

  $('#reseed').addEventListener('click', () => {
    const opts = readFields();
    opts.seed = (Math.random() * 100000) | 0;
    restart(opts);
  });

  $('#view').addEventListener('click', () => {
    const toVga = boardView === views.modern;
    boardView = toVga ? views.vga : views.modern;
    boardView.reset();
    $('#view').textContent = toVga ? 'Modern' : 'Classic VGA';
    $('#view').setAttribute('aria-pressed', String(toVga));
    $('#board').removeAttribute('style');
    renderAll();
  });

  function showSpeed() {
    $('#speed-val').textContent = tps >= 10 ? `${Math.round(tps)}/s`
      : tps >= 1 ? `${tps.toFixed(1)}/s`
      : `1 per ${(1 / tps).toFixed(1)}s`;
  }

  $('#speed').addEventListener('input', (e) => {
    tps = tpsFor(parseInt(e.target.value, 10));
    showSpeed();
  });
  $('#f-gpmut').addEventListener('change', () => { tweaks.gpMutates = $('#f-gpmut').checked; });

  /* ---- species tabs ---- */

  const tabBar = $('#tabs');
  for (const s of SPECIES) {
    const b = el('button', null, s.name);
    b.style.setProperty('--tab', s.color);
    b.setAttribute('role', 'tab');
    b.setAttribute('aria-selected', String(s.key === selected));
    b.addEventListener('click', () => {
      selected = s.key;
      for (const other of tabBar.children) {
        other.setAttribute('aria-selected', String(other === b));
      }
      renderLeaders();
    });
    tabBar.appendChild(b);
  }

  const legend = $('#legend');
  for (const s of SPECIES) {
    const span = el('span');
    const swatch = el('i');
    swatch.style.background = s.color;
    span.append(swatch, document.createTextNode(s.name.toLowerCase()));
    legend.appendChild(span);
  }

  /* ---- panels ---- */

  function speciesOf(key) { return SPECIES.find((s) => s.key === key); }

  function renderReadout() {
    const t = run.tournament;
    let alive = 0;
    if (t) {
      for (const f of t.pop.v) if (f.soul.alive) alive++;
    }
    $('#r-gen').textContent = run.generation;
    $('#r-tourn').textContent = `${run.tournamentNo}/${run.tournamentsPerGen}`;
    $('#r-tick').textContent = t ? t.curCycle : 0;
    $('#r-alive').textContent = t ? `${alive}/${t.pop.size()}` : '0';
    $('#r-food').textContent = t ? t.board.foodLeft() : 0;
  }

  function renderLeaders() {
    const s = speciesOf(selected);
    const rows = run.stats[selected] || [];
    const host = $('#leaders');
    host.innerHTML = '';
    const count = run.opts['n' + s.key.charAt(0).toUpperCase() + s.key.slice(1)];
    $('#leader-count').textContent = rows.length
      ? `top ${Math.min(STATSIZE, rows.length)} of ${count}`
      : `${count} fleaz`;

    if (!rows.length) {
      host.appendChild(el('p', 'empty', count === 0
        ? 'none of this species in the run'
        : 'no scores yet — finish generation 1'));
      $('#specimen').textContent = '';
      return;
    }

    const table = el('table', 'leaders');
    table.style.setProperty('--rank', s.color);
    const head = el('tr');
    for (const h of ['id', 'eat', 'dist', 'life', 'age', 'fit']) head.appendChild(el('th', null, h));
    table.appendChild(head);

    for (const r of rows) {
      const tr = el('tr');
      tr.appendChild(el('td', null, '#' + r.id));
      tr.appendChild(el('td', null, r.numEats));
      tr.appendChild(el('td', null, r.distMoved));
      tr.appendChild(el('td', null, r.lifespan));
      tr.appendChild(el('td', null, r.age));
      tr.appendChild(el('td', 'fit', r.fitness.toLocaleString('en-US')));
      table.appendChild(tr);
    }
    host.appendChild(table);
    renderSpecimen(rows[0].specimen, s);
  }

  /* The display() each species implemented in the C, transcribed. */
  function renderSpecimen(spec, s) {
    const pre = $('#specimen');
    pre.innerHTML = '';
    pre.style.setProperty('--spec', s.color);
    if (!spec) return;

    pre.appendChild(el('b', null, `${s.name} leader  `));
    let body;

    if (spec.kind === 'gp') {
      body = `${spec.nodes} nodes\n` +
        (spec.source.length > 1400 ? spec.source.slice(0, 1400) + ' \u2026' : spec.source);
    } else if (spec.kind === 'fsm') {
      let strip = '';
      for (let i = 0; i < spec.bits.length; i++) strip += spec.bits[i] ? '\u2588' : '\u00b7';
      body = `${FSMLENGTH}-bit genotype, ${spec.mutations} mutations, ` +
             `${spec.eatStates}/1296 states say eat\n${strip}`;
    } else if (spec.kind === 'nn') {
      const ramp = ' \u00b7\u2591\u2592\u2593\u2588';
      let rows = '';
      for (let j = 0; j < spec.hiddenCt; j++) {
        for (let i = 0; i < spec.inCt; i++) {
          const v = Math.min(0.999, Math.abs(spec.weights[j * spec.inCt + i]) / 2);
          rows += ramp[Math.floor(v * ramp.length)];
        }
        rows += j < spec.hiddenCt - 1 ? '  ' : '';
      }
      body = `alpha ${spec.alpha}, ${spec.correct} correct calls\n` +
             `input\u2192hidden weights, 6 per hidden node:\n${rows}`;
    } else {
      body = 'no genome \u2014 RANDOM fleaz are the control group';
    }
    pre.appendChild(document.createTextNode(body));
  }

  function renderLog() { $('#log').textContent = run.log.join('\n'); }

  function renderAll() {
    const t = run.tournament;
    boardView.draw(t ? t.board : null, t ? t.pop.v : null, t ? t.loc : null,
                   trails, t ? t.curCycle : 0);
    renderReadout();
    if (run.generation !== lastGenSeen) {
      lastGenSeen = run.generation;
      renderLeaders();
      renderLog();
      chart.draw(run.history, chartStyle);
    }
  }

  /* ---- main loop ---- */

  function frame(ts) {
    if (playing && run) {
      const dt = lastTs ? Math.min((ts - lastTs) / 1000, 0.25) : 0;
      tickAcc = Math.min(tickAcc + tps * dt, MAX_BURST);
      let n = 0;
      while (tickAcc >= 1 && n < MAX_BURST) { stepOnce(); tickAcc -= 1; n++; }
    } else {
      tickAcc = 0;
    }
    lastTs = ts;
    if (run) renderAll();
    requestAnimationFrame(frame);
  }

  let resizeTimer;
  window.addEventListener('resize', () => {
    clearTimeout(resizeTimer);
    resizeTimer = setTimeout(() => {
      if (run) { renderAll(); chart.draw(run.history, chartStyle); }
    }, 120);
  });

  showSpeed();
  restart(DEFAULT_OPTS);
  requestAnimationFrame(frame);
})();
