/* render.js -- two ways to draw the board.
 *
 * ModernBoardView is the default: the distance field drawn as a flow field,
 * with proximity as a tint and direction as an oriented stroke, everything
 * anti-aliased at device resolution.
 *
 * VgaBoardView is a literal port of rend_SVGA.C -- 6-pixel cells, direction
 * arrows hand-plotted with the same gl_line and gl_setpixel calls, composed
 * into a buffer and scaled up on whole pixels with smoothing off. It is the
 * 1999 output, magnified. The toggle above the board switches between them.
 */

const PSIZE = 6;          /* size of each square, in original pixels */
const G_LEN = 25;         /* length of the distance gradient */

/* rend_SVGA.h palette, as packed ABGR for a Uint32 framebuffer. */
const abgr = (hex) => (0xff000000 | ((hex & 0xff) << 16) | (hex & 0xff00) | ((hex >> 16) & 0xff)) >>> 0;

const C_BG    = abgr(0x000000);
const C_LINES = abgr(0x333333);
const C_FOOD  = abgr(0x999999);
const C_FLEA_COLOR = SPECIES.map((s) => abgr(parseInt(s.color.slice(1), 16)));

/* The gradient the arrows are shaded with: 0x66 down to nothing over 25
 * steps, using C's integer division (0x66/25 == 4). Anything further away
 * than that fell off the end of the palette into black. */
const GRADIENT = new Uint32Array(256);
for (let x = 0; x < 256; x++) {
  const v = x < G_LEN ? 0x66 - (Math.trunc(0x66 / G_LEN) * x) : 0;
  GRADIENT[x] = abgr((v << 16) | (v << 8) | v);
}

/* Shared: size the display canvas and hand back a device-pixel context. */
function fitCanvas(cvs, cell, dimX, dimY) {
  const cssW = cell * dimX, cssH = cell * dimY;
  cvs.style.width = cssW + 'px';
  cvs.style.height = cssH + 'px';
  const dpr = window.devicePixelRatio || 1;
  const bw = Math.round(cssW * dpr), bh = Math.round(cssH * dpr);
  if (cvs.width !== bw || cvs.height !== bh) { cvs.width = bw; cvs.height = bh; }
  return dpr;
}

function availWidth(cvs, fallback) {
  return (cvs.parentElement && cvs.parentElement.clientWidth) || fallback;
}

/* ---- trails -------------------------------------------------------------
 *
 * Where each flea has been, fading out behind it. Rather than decaying a
 * buffer every tick -- which would cost thousands of multiplies per tick and
 * bite hard at 3000 ticks a second -- each visited cell just records the tick
 * it was stamped on and which species did the stamping. Age, and so opacity,
 * is worked out at draw time. Stamping costs one write per living flea.
 *
 * Fading is measured in ticks, not frames, so a path looks the same whether
 * you are watching one tick a second or three thousand.
 */

const TRAIL_LIFE = 32;      /* ticks before a trail is gone */

class Trails {
  constructor() { this.owner = null; }

  /* Called once per simulation tick, after the tick has been applied. */
  step(t) {
    if (!t) { this.owner = null; return; }
    const cells = t.board.dimX * t.board.dimY;
    if (this.owner !== t || !this.tick || this.tick.length !== cells) {
      this.owner = t;
      this.dimX = t.board.dimX;
      this.tick = new Int32Array(cells).fill(-TRAIL_LIFE * 4);
      this.species = new Uint8Array(cells);
    }
    const pop = t.pop.v, loc = t.loc, now = t.curCycle;
    for (let i = 0; i < pop.length; i++) {
      const f = pop[i];
      if (!f.soul.alive) continue;
      const c = loc[i].y * this.dimX + loc[i].x;
      this.tick[c] = now;
      this.species[c] = f.type;
    }
  }

  matches(t) { return this.owner === t && this.tick; }
}

/* ====================================================== modern board view */

const MODERN = {
  ground:  '#080b10',
  tint:    [29, 58, 92],      /* cool blue, alpha does the work */
  flow:    [148, 178, 210],
  food:    '#f2d79b',
  foodGlow:'#c39433',
  reach:   10,                /* cells beyond this read as "far from food" */
  buckets: 8,
};

class ModernBoardView {
  constructor(canvas) {
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');
    /* One Path2D per opacity bucket, so the whole field strokes in eight
     * calls instead of twenty-five hundred. */
    this.paths = [];
    this.heads = [];
    this.cells = [];
    this.trailCells = [];
  }

  reset() {}

  draw(board, pop, loc, trails, nowTick) {
    if (!board) return;
    const { dimX, dimY, type, dirNearest, idxNearest, dist, foodCount } = board;

    const avail = availWidth(this.canvas, dimX * 10);
    const cell = Math.max(3, Math.min(22, Math.floor(avail / dimX)));
    const dpr = fitCanvas(this.canvas, cell, dimX, dimY);

    const ctx = this.ctx;
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    ctx.fillStyle = MODERN.ground;
    ctx.fillRect(0, 0, cell * dimX, cell * dimY);

    const N = MODERN.buckets;
    for (let b = 0; b < N; b++) {
      this.paths[b] = new Path2D();
      this.heads[b] = new Path2D();
      this.cells[b] = (this.cells[b] || []);
      this.cells[b].length = 0;
    }

    /* Pass one: bucket every cell by how close it is to food. */
    const half = cell / 2;
    const reach = MODERN.reach;
    for (let y = 0; y < dimY; y++) {
      for (let x = 0; x < dimX; x++) {
        const c = y * dimX + x;
        if (type[c] === MAP_FOOD) continue;
        const dir = dirNearest[c];
        if (dir === DIR_NONE) continue;

        const d = dist[c * foodCount + idxNearest[c]];
        const t = 1 - Math.min(d, reach) / reach;
        const b = Math.min(N - 1, Math.floor(t * N));
        this.cells[b].push(x, y);

        /* A short tail with a dot at its head. Full-width strokes would
         * line up with their neighbours and read as a grid rather than as a
         * field, since the map only ever points four ways. */
        if (cell >= 6) {
          const cx = x * cell + half, cy = y * cell + half;
          const len = cell * (0.12 + 0.16 * t);
          let dx = 0, dy = 0;
          if (dir === DIR_NORTH) dy = -1;
          else if (dir === DIR_SOUTH) dy = 1;
          else if (dir === DIR_EAST) dx = 1;
          else dx = -1;
          const hx = cx + dx * len, hy = cy + dy * len;
          const p = this.paths[b];
          p.moveTo(cx - dx * len, cy - dy * len);
          p.lineTo(hx, hy);
          const h = this.heads[b];
          h.moveTo(hx + cell * 0.075, hy);
          h.arc(hx, hy, cell * 0.075, 0, Math.PI * 2);
        }
      }
    }

    /* Pass two: one fill and one stroke per bucket. */
    const [tr, tg, tb] = MODERN.tint;
    const [fr, fg, fb] = MODERN.flow;
    ctx.lineCap = 'round';
    ctx.lineWidth = Math.max(1, cell * 0.075);
    for (let b = 0; b < N; b++) {
      const t = (b + 0.5) / N;
      const list = this.cells[b];
      if (list.length) {
        ctx.fillStyle = `rgba(${tr},${tg},${tb},${(0.03 + 0.62 * t * t).toFixed(3)})`;
        for (let i = 0; i < list.length; i += 2) {
          ctx.fillRect(list[i] * cell, list[i + 1] * cell, cell, cell);
        }
      }
      if (cell >= 6) {
        ctx.strokeStyle = `rgba(${fr},${fg},${fb},${(0.05 + 0.30 * t).toFixed(3)})`;
        ctx.stroke(this.paths[b]);
        ctx.fillStyle = `rgba(${fr},${fg},${fb},${(0.10 + 0.55 * t).toFixed(3)})`;
        ctx.fill(this.heads[b]);
      }
    }

    /* Trails, under everything else: the recent past of each flea. */
    if (trails && trails.tick && trails.tick.length === dimX * dimY) {
      const now = nowTick;
      const TB = 5;
      for (let b = 0; b < TB; b++) this.trailCells[b] = (this.trailCells[b] || []), this.trailCells[b].length = 0;
      for (let c = 0; c < trails.tick.length; c++) {
        const age = now - trails.tick[c];
        if (age < 0 || age >= TRAIL_LIFE) continue;
        const t = 1 - age / TRAIL_LIFE;
        const b = Math.min(TB - 1, Math.floor(t * TB));
        this.trailCells[b].push(c, trails.species[c]);
      }
      const inset = cell * 0.31, sz = cell - inset * 2;
      for (let b = 0; b < TB; b++) {
        const list = this.trailCells[b];
        if (!list.length) continue;
        const t = (b + 0.5) / TB;
        ctx.globalAlpha = 0.07 + 0.38 * t * t;
        for (let i = 0; i < list.length; i += 2) {
          const c = list[i];
          ctx.fillStyle = SPECIES[list[i + 1]].color;
          ctx.fillRect((c % dimX) * cell + inset, Math.floor(c / dimX) * cell + inset, sz, sz);
        }
      }
      ctx.globalAlpha = 1;
    }

    /* Food, then fleaz, each with a soft bloom so they sit above the field. */
    const r = Math.max(1, cell * 0.30);
    ctx.save();
    ctx.shadowColor = MODERN.foodGlow;
    ctx.shadowBlur = cell * 1.1;
    ctx.fillStyle = MODERN.food;
    for (let i = 0; i < board.foodCount; i++) {
      if (type[board.foodCell[i]] !== MAP_FOOD) continue;
      this.blob(board.foodX[i] * cell, board.foodY[i] * cell, cell, r, 0.16);
    }
    ctx.restore();

    if (pop) {
      for (const s of SPECIES) {
        let any = false;
        ctx.save();
        ctx.shadowColor = s.color;
        ctx.shadowBlur = cell * 0.7;
        ctx.fillStyle = s.color;
        for (let i = 0; i < pop.length; i++) {
          const f = pop[i];
          if (f.type !== s.id || !f.soul.alive) continue;
          this.blob(loc[i].x * cell, loc[i].y * cell, cell, r, 0.13);
          any = true;
        }
        ctx.restore();
        if (!any) continue;
      }
    }

    ctx.setTransform(1, 0, 0, 1, 0, 0);
  }

  /* A rounded square inset from its cell. */
  blob(px, py, cell, r, inset) {
    const ctx = this.ctx;
    const i = cell * inset, sz = cell - i * 2;
    ctx.beginPath();
    if (ctx.roundRect) ctx.roundRect(px + i, py + i, sz, sz, Math.min(r, sz / 2));
    else ctx.rect(px + i, py + i, sz, sz);
    ctx.fill();
  }
}

/* ========================================================= classic VGA view */

class VgaBoardView {
  constructor(canvas) {
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d', { alpha: false });
    this.ctx.imageSmoothingEnabled = false;
    this.off = document.createElement('canvas');
    this.offCtx = this.off.getContext('2d', { alpha: false });
  }

  reset() { this.grid = null; }

  resize(dimX, dimY) {
    this.dimX = dimX;
    this.dimY = dimY;
    this.w = dimX * PSIZE + 1;
    this.h = dimY * PSIZE + 1;
    this.off.width = this.w;
    this.off.height = this.h;
    this.image = this.offCtx.createImageData(this.w, this.h);
    this.buf = new Uint32Array(this.image.data.buffer);
    this.grid = null;
  }

  px(x, y, c) {
    if (x < 0 || y < 0 || x >= this.w || y >= this.h) return;
    this.buf[y * this.w + x] = c;
  }

  hline(x0, x1, y, c) { for (let x = x0; x <= x1; x++) this.px(x, y, c); }
  vline(x, y0, y1, c) { for (let y = y0; y <= y1; y++) this.px(x, y, c); }

  box(x, y, w, h, c) {
    for (let j = 0; j < h; j++) {
      const row = (y + j) * this.w;
      for (let i = 0; i < w; i++) this.buf[row + x + i] = c;
    }
  }

  /* rend_SVGA::init built the grid once into a scratch context and copied it
   * back each frame. Same idea: cache it as a Uint32Array. */
  buildGrid() {
    this.buf.fill(C_BG);
    for (let x = 0; x <= PSIZE * this.dimX; x += PSIZE) {
      this.vline(x, 0, this.dimY * PSIZE, C_LINES);
    }
    /* The C used dimY for the horizontal extent here, which drew short lines
     * on any non-square board. Corrected. */
    for (let y = 0; y <= PSIZE * this.dimY; y += PSIZE) {
      this.hline(0, this.dimX * PSIZE, y, C_LINES);
    }
    this.grid = this.buf.slice();
  }

  /* The four arrow glyphs, plotted exactly as rend_SVGA::render plotted
   * them: a 5-pixel shaft with a two-stroke head, shaded by distance. */
  arrow(dir, x, y, c) {
    const bx = x * PSIZE, by = y * PSIZE;
    switch (dir) {
      case DIR_NORTH:
        this.vline(bx + 3, by + 1, by + 5, c);
        this.vline(bx + 2, by + 3, by + 5, c);
        this.vline(bx + 4, by + 3, by + 5, c);
        this.px(bx + 1, by + 5, c); this.px(bx + 5, by + 5, c);
        break;
      case DIR_SOUTH:
        this.vline(bx + 3, by + 1, by + 5, c);
        this.vline(bx + 2, by + 1, by + 3, c);
        this.vline(bx + 4, by + 1, by + 3, c);
        this.px(bx + 1, by + 1, c); this.px(bx + 5, by + 1, c);
        break;
      case DIR_EAST:
        this.hline(bx + 1, bx + 5, by + 3, c);
        this.hline(bx + 1, bx + 3, by + 2, c);
        this.hline(bx + 1, bx + 3, by + 4, c);
        this.px(bx + 1, by + 1, c); this.px(bx + 1, by + 5, c);
        break;
      case DIR_WEST:
        this.hline(bx + 1, bx + 5, by + 3, c);
        this.hline(bx + 3, bx + 5, by + 2, c);
        this.hline(bx + 3, bx + 5, by + 4, c);
        this.px(bx + 5, by + 1, c); this.px(bx + 5, by + 5, c);
        break;
    }
  }

  draw(board, pop, loc) {
    if (!board) return;
    if (this.dimX !== board.dimX || this.dimY !== board.dimY) {
      this.resize(board.dimX, board.dimY);
    }
    if (!this.grid) this.buildGrid();
    this.buf.set(this.grid);

    const { dimX, dimY, type, dirNearest, idxNearest, dist, foodCount } = board;
    for (let y = 0; y < dimY; y++) {
      for (let x = 0; x < dimX; x++) {
        const cell = y * dimX + x;
        if (type[cell] === MAP_FOOD) {
          this.box(x * PSIZE + 1, y * PSIZE + 1, PSIZE - 1, PSIZE - 1, C_FOOD);
          continue;
        }
        const dir = dirNearest[cell];
        if (dir === DIR_NONE) continue;
        const d = Math.trunc(dist[cell * foodCount + idxNearest[cell]]);
        this.arrow(dir, x, y, GRADIENT[Math.min(d, 255)]);
      }
    }

    if (pop) {
      for (let i = 0; i < pop.length; i++) {
        const f = pop[i];
        if (!f.soul.alive) continue;
        this.box(PSIZE * loc[i].x + 1, PSIZE * loc[i].y + 1,
                 PSIZE - 1, PSIZE - 1, C_FLEA_COLOR[f.type]);
      }
    }

    this.offCtx.putImageData(this.image, 0, 0);
    this.blit();
  }

  /* Blit at a whole-number scale. This matters: a 6-pixel cell magnified by,
   * say, 3.34 puts the arrow strokes on uneven pixel boundaries and the
   * distance field turns to mush. Only a board too wide for the column falls
   * back to a fractional scale. */
  blit() {
    const cvs = this.canvas;
    const avail = availWidth(cvs, this.w);
    const scale = this.w > avail ? avail / this.w : Math.max(1, Math.floor(avail / this.w));
    const cssW = Math.round(this.w * scale), cssH = Math.round(this.h * scale);

    cvs.style.width = cssW + 'px';
    cvs.style.height = cssH + 'px';
    const dpr = window.devicePixelRatio || 1;
    const bw = Math.round(cssW * dpr), bh = Math.round(cssH * dpr);
    if (cvs.width !== bw || cvs.height !== bh) { cvs.width = bw; cvs.height = bh; }

    const ctx = this.ctx;
    ctx.imageSmoothingEnabled = false;
    ctx.fillStyle = '#000';
    ctx.fillRect(0, 0, bw, bh);
    ctx.drawImage(this.off, 0, 0, bw, bh);
  }
}

/* ---- fitness chart ------------------------------------------------------ */

/* Best fitness per species per generation. The four functions are on wildly
 * different scales -- GP and NN are quadratic in meals and carry half of the
 * previous generation forward, FSM and RANDOM are linear in lifespan -- so
 * the vertical axis is log10 and labelled with the values it actually hits.
 */
class FitnessChart {
  constructor(canvas) {
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');
  }

  draw(history, style) {
    const cvs = this.canvas;
    const rect = cvs.getBoundingClientRect();
    const dpr = window.devicePixelRatio || 1;
    cvs.width = Math.max(1, Math.round(rect.width * dpr));
    cvs.height = Math.max(1, Math.round(rect.height * dpr));

    const ctx = this.ctx;
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
    const W = rect.width, H = rect.height;
    ctx.clearRect(0, 0, W, H);

    const padL = 44, padR = 8, padT = 10, padB = 20;
    const plotW = W - padL - padR, plotH = H - padT - padB;

    if (history.length < 2) {
      ctx.fillStyle = style.dim;
      ctx.font = `11px ${style.mono}`;
      ctx.fillText('waiting for generation 2', padL, padT + plotH / 2);
      return;
    }

    let max = 1;
    for (const g of history) {
      for (const s of SPECIES) {
        const e = g[s.key];
        if (e && e.best > max) max = e.best;
      }
    }
    const top = Math.ceil(Math.log10(max + 1));
    const yOf = (v) => padT + plotH - (Math.log10(v + 1) / top) * plotH;
    const xOf = (i) => padL + (history.length === 1 ? 0 : (i / (history.length - 1)) * plotW);

    ctx.font = `10px ${style.mono}`;
    ctx.textBaseline = 'middle';
    for (let d = 0; d <= top; d++) {
      const v = Math.pow(10, d) - 1;
      const y = yOf(v);
      ctx.strokeStyle = style.grid;
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.moveTo(padL, Math.round(y) + 0.5);
      ctx.lineTo(W - padR, Math.round(y) + 0.5);
      ctx.stroke();
      ctx.fillStyle = style.dim;
      ctx.textAlign = 'right';
      ctx.fillText(d === 0 ? '0' : `1e${d}`, padL - 6, y);
    }

    ctx.textAlign = 'center';
    ctx.textBaseline = 'top';
    ctx.fillStyle = style.dim;
    ctx.fillText(`gen ${history[0].gen}`, padL + 16, padT + plotH + 5);
    ctx.fillText(`gen ${history[history.length - 1].gen}`, W - padR - 20, padT + plotH + 5);

    for (const s of SPECIES) {
      let started = false;
      ctx.strokeStyle = s.color;
      ctx.lineWidth = 1.75;
      ctx.lineJoin = 'round';
      ctx.beginPath();
      history.forEach((g, i) => {
        const e = g[s.key];
        if (!e) return;
        const x = xOf(i), y = yOf(e.best);
        if (!started) { ctx.moveTo(x, y); started = true; } else ctx.lineTo(x, y);
      });
      if (started) ctx.stroke();

      const last = history[history.length - 1][s.key];
      if (last) {
        ctx.fillStyle = s.color;
        ctx.beginPath();
        ctx.arc(xOf(history.length - 1), yOf(last.best), 2.5, 0, Math.PI * 2);
        ctx.fill();
      }
    }
  }
}
