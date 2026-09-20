# Fleaz on the web

A port of the 1999 C++/svgalib program in this repository to a static web page:
no build step, no dependencies, no server. Open `index.html`.

```
web/
  index.html      the console
  fleaz.css       the 640x480x256 palette, as CSS
  assets/flea.png the original fleaz.img sprite, converted
  js/core.js      constants from fleaz.h, seeded RNG, C's rint()
  js/world.js     map.C + mapNode.C   -- the board and its distance field
  js/elman.js     ElmanNet.C          -- the neural net
  js/fleas.js     Flea*.C             -- the four species
  js/sim.js       RingMaster.C, Population.C, Midwife.C, main.C's loop
  js/render.js    rend_SVGA.C         -- the board view, plus a fitness chart
  js/app.js       new: controls, panels, the frame loop
```

## What was kept

The simulation is a literal port. The genome layouts, the state arithmetic,
the fitness functions, the crossover dice, the breeding schedule and the
tournament loop all match the C line for line, down to `rint()` rounding half
to even and `mapNode::distList` being `float` rather than `double`.

There are two board views, switched by the button above the board. The default
is modern: the map's distance field drawn as a flow field, proximity to food as
a tint and direction as an oriented stroke, anti-aliased at device resolution.
**Classic VGA** is a literal port of `rend_SVGA.C` — 6-pixel cells, each
direction arrow hand-plotted with the same `gl_line` and `gl_setpixel` calls,
composed into a buffer and scaled up on whole pixels with smoothing off. Both
draw the same numbers; only Classic VGA is what 1999 actually looked like.

Scaling the VGA view on a whole number matters. At a fractional scale the
6-pixel arrow strokes land on uneven pixel boundaries and the distance field
turns to grey mush.

Bugs that shaped the results were kept and labelled `QUIRK` in the source:

- **GP fleaz never mutate.** `FleaGP::mutate(int rate)` takes an argument, so it
  never overrode `Flea::mutate()`; the Midwife always called the empty base
  version. The GP line evolved by recombination alone. There is a switch in the
  Options panel to turn real mutation on.
- **GP has east and west crossed.** `makeNode()` assigns `&if_west` to the
  `IF_EAST` case and `&if_east` to `IF_WEST`, in `_mutate()` as well.
- **Every GP conditional looks at the same cell.** The random `argIdx` was
  commented out in favour of a hardcoded `12`, the flea's own square. `IF_EAST`
  is the only case that still rolls for one.
- **The neural net only sees one cell.** The loop feeding it all 25 sensor
  values is commented out in `FleaNN.C`; what remains is a one-hot encoding of
  the centre square.
- **NN crossover exchanges nothing.** Each child is a clone of one parent, so
  that line evolves by selection plus what it learned while alive.
- **`INFO_FOOD` and `DIR_NONE` are both `0`.** Every fitness function's
  `!lastInfo[...]` test therefore reads as "food here, or no food anywhere in
  range", which is load-bearing and almost certainly not deliberate.

## What was changed

- **One seeded RNG** replaces libc's `random()`/`rand()`, so a run is
  reproducible from its seed. Sequences differ from a 1999 run; behaviour does
  not.
- **`curCycle` starts at zero.** `RingMaster` never initialised it, so the first
  respiration tick landed on whatever was on the stack.
- **The grid draws to the right width** in the Classic VGA view.
  `rend_SVGA::init` passed `dimY` as the horizontal extent of the horizontal
  rules, which drew short lines on any non-square board.
- **Tournaments are capped at 5000 ticks** so a pathological run cannot hang the
  page. The original had no cap and did not need one.
- **File I/O and signals are gone.** `DataFile.C`'s population serialisation and
  the `SIGHUP` handler that triggered it have no counterpart here; the browser
  keeps a run only for as long as the tab is open.
- **Unported:** the `rend_TEXT`, `rend_NULL` and `rend_SVGAFILE` renderers, and
  the `attack` action, which was declared in every species and implemented in
  none.

## Reading the board

Gold dots are food; the field behind them is the distance field, each empty
square pointing at its nearest surviving pellet and brightening as that pellet
gets closer; coloured dots are fleaz, in the palette from `rend_SVGA.h` — FSM
violet, GP pink, NN green, RANDOM slate.

Fitness is not comparable across species. FSM and RANDOM are linear in lifespan;
GP and NN are quadratic in meals and carry half the previous generation's score
forward, so they climb geometrically. The chart is log scale for that reason.
RANDOM is the control: it never breeds, and it is the line the other three have
to beat.
