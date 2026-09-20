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

  let run = null;
  let playing = true;
  let speed = 6;
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

    lastGenSeen = -1;
    if (boardView.reset) boardView.reset();
    renderAll();
  }

  $('#play').addEventListener('click', () => {
    playing = !playing;
    $('#play').textContent = playing ? 'Pause' : 'Play';
    $('#play').setAttribute('aria-pressed', String(playing));
  });

  $('#step').addEventListener('click', () => {
    playing = false;
    $('#play').textContent = 'Play';
    run.tick();
    renderAll();
  });

  $('#skip').addEventListener('click', () => {
    const target = run.generation + 1;
    const budget = 200000;
    for (let i = 0; i < budget && run.generation < target; i++) run.tick();
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

  $('#speed').addEventListener('input', (e) => { speed = parseInt(e.target.value, 10); });
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
    boardView.draw(t ? t.board : null, t ? t.pop.v : null, t ? t.loc : null);
    renderReadout();
    if (run.generation !== lastGenSeen) {
      lastGenSeen = run.generation;
      renderLeaders();
      renderLog();
      chart.draw(run.history, chartStyle);
    }
  }

  /* ---- main loop ---- */

  function frame() {
    if (playing && run) {
      for (let i = 0; i < speed; i++) run.tick();
    }
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

  restart(DEFAULT_OPTS);
  requestAnimationFrame(frame);
})();
