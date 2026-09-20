/* world.js -- the board. Port of map.C / mapNode.C.
 *
 * The original allocated a mapNode object per cell, each holding a distance
 * to every food pellet on the board. That is kept, but flattened into typed
 * arrays: `dist` is (cells x foodCount) floats, indexed [cell * nFood + i].
 * Float32Array matters -- mapNode::distList was float, and reCalc() compares
 * against a float sentinel.
 */

class Board {
  constructor(nFood, dimX, dimY) {
    this.dimX = dimX;
    this.dimY = dimY;
    const cells = dimX * dimY;

    this.type = new Uint8Array(cells);
    this.dirNearest = new Uint8Array(cells);
    this.idxNearest = new Int32Array(cells).fill(-1);

    /* map::map -- one in every fPerc cells is seeded with food. With the
     * defaults that is 2500/25, i.e. a 1-in-100 chance per cell, so `nFood`
     * is an average rather than an exact count. */
    const fPerc = Math.trunc(dimX * dimY / nFood) || 1;
    const fx = [], fy = [];
    for (let y = 0; y < dimY; y++) {
      for (let x = 0; x < dimX; x++) {
        if (rnd(fPerc) === 0) {
          this.type[y * dimX + x] = MAP_FOOD;
          fx.push(x); fy.push(y);
        }
      }
    }

    this.foodCount = fx.length;
    this.foodX = Int32Array.from(fx);
    this.foodY = Int32Array.from(fy);
    this.foodCell = new Int32Array(this.foodCount);
    for (let i = 0; i < this.foodCount; i++) {
      this.foodCell[i] = fy[i] * dimX + fx[i];
    }

    /* mapNode::setFoodList -- cache the distance from every cell to every
     * pellet once, then never recompute it. Eaten pellets are retired by
     * poisoning their cached distance in reCalc(). */
    this.dist = new Float32Array(cells * this.foodCount);
    for (let y = 0; y < dimY; y++) {
      for (let x = 0; x < dimX; x++) {
        const cell = y * dimX + x;
        const base = cell * this.foodCount;
        let nDist = 99999.0, nearest = -1;
        for (let i = 0; i < this.foodCount; i++) {
          const dx = x - this.foodX[i], dy = y - this.foodY[i];
          const d = Math.fround(Math.sqrt(dx * dx + dy * dy));
          this.dist[base + i] = d;
          if (d < nDist) { nDist = d; nearest = i; }
        }
        this.setDir(cell, x, y, nearest);
      }
    }
  }

  /* Shared tail of setFoodList()/reCalc(): point the cell at `nearest`. */
  setDir(cell, x, y, nearest) {
    if (nearest < 0) { this.dirNearest[cell] = DIR_NONE; return; }
    if (this.dist[cell * this.foodCount + nearest] === 0.0) {
      this.dirNearest[cell] = DIR_NONE;
      this.idxNearest[cell] = nearest;
      return;
    }
    const fx = this.foodX[nearest], fy = this.foodY[nearest];
    if (Math.abs(x - fx) >= Math.abs(y - fy)) {
      this.dirNearest[cell] = x < fx ? DIR_EAST : DIR_WEST;
    } else {
      this.dirNearest[cell] = y < fy ? DIR_SOUTH : DIR_NORTH;
    }
    this.idxNearest[cell] = nearest;
  }

  /* map::reCalc -- re-point every cell at the nearest *surviving* pellet.
   *
   * Two quirks of the original are kept because they shape what you see: the
   * search only retires a pellet it would otherwise have chosen (so a distant
   * eaten pellet stays in the table until something closer is gone), and the
   * initial best distance is 9999 here versus 99999 at setup, which quietly
   * caps the useful radius of a very large board.
   */
  reCalc() {
    const { dimX, dimY, foodCount, type, dist } = this;
    for (let y = 0; y < dimY; y++) {
      for (let x = 0; x < dimX; x++) {
        const cell = y * dimX + x;
        if (type[cell] === MAP_FOOD) { this.dirNearest[cell] = DIR_NONE; continue; }

        const base = cell * foodCount;
        let nDist = 9999.0, nearest = -1;
        for (let i = 0; i < foodCount; i++) {
          const d = dist[base + i];
          if (d < nDist) {
            if (type[this.foodCell[i]] === MAP_FOOD) { nDist = d; nearest = i; }
            else dist[base + i] = 99999.0;
          }
        }
        this.setDir(cell, x, y, nearest);
      }
    }
  }

  eatFood(x, y) {
    const cell = y * this.dimX + x;
    if (this.type[cell] === MAP_FOOD) { this.type[cell] = MAP_NONE; return 1; }
    return 0;
  }

  foodLeft() {
    let n = 0;
    for (let i = 0; i < this.foodCount; i++) {
      if (this.type[this.foodCell[i]] === MAP_FOOD) n++;
    }
    return n;
  }

  /* map::getInfo -- fill a flea's 5x5 sensor block.
   *
   * Row/column order matches the C: info[row][col] == info[2 + dy][2 + dx].
   * Other fleaz are painted first and food overwrites them; empty cells fall
   * back to the direction arrow of the nearest pellet. The flea does not see
   * itself, but it does see the corpses of fleaz that have starved, since the
   * original never checked `alive` here.
   */
  getInfo(info, x, y, fleas, loc) {
    info.fill(-1);

    let foundSelf = 0;
    for (let i = 0; i < fleas.length; i++) {
      const px = loc[i].x, py = loc[i].y;
      if (px > x - 3 && px < x + 3 && py > y - 3 && py < y + 3) {
        if (!foundSelf && px === x && py === y) foundSelf++;
        else info[(2 + py - y) * 5 + (2 + px - x)] = INFO_FLEA;
      }
    }

    let iy = 0;
    for (let j = y - 2; j < y + 3; j++) {
      const cy = j < 0 ? 0 : (j >= this.dimY ? this.dimY - 1 : j);
      let ix = 0;
      for (let i = x - 2; i < x + 3; i++) {
        const cx = i < 0 ? 0 : (i >= this.dimX ? this.dimX - 1 : i);
        const cell = cy * this.dimX + cx;
        const slot = iy * 5 + ix;
        if (this.type[cell] === MAP_FOOD) info[slot] = INFO_FOOD;
        else if (info[slot] < 0) info[slot] = this.dirNearest[cell];
        ix++;
      }
      iy++;
    }
  }
}
