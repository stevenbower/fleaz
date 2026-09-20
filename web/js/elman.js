/* elman.js -- port of ElmanNet.C (Peter Vetere, Feb 1999).
 *
 * Despite the name there is no context layer: the class as shipped is a plain
 * 6-9-11 feed-forward net with a bipolar sigmoid and vanilla backprop. The
 * recurrent wiring survives only as commented-out arguments in FleaNN.C.
 */

class ElmanNet {
  constructor(inCt, hiddenCt, outCt, lRate) {
    this.inCt = inCt;
    this.hiddenCt = hiddenCt;
    this.outCt = outCt;
    this.alpha = lRate;

    this.inLayer = new Float32Array(inCt);
    this.hiddenLayer = new Float32Array(hiddenCt);
    this.outLayer = new Float32Array(outCt);
    this.weights_1 = new Float32Array(hiddenCt * inCt);
    this.weights_2 = new Float32Array(outCt * hiddenCt);
    this.biases_1 = new Float32Array(hiddenCt);
    this.biases_2 = new Float32Array(outCt);

    this.reset();
  }

  static copy(parent, alpha) {
    const n = Object.create(ElmanNet.prototype);
    n.inCt = parent.inCt;
    n.hiddenCt = parent.hiddenCt;
    n.outCt = parent.outCt;
    n.alpha = alpha === undefined ? parent.alpha : alpha;
    n.inLayer = parent.inLayer.slice();
    n.hiddenLayer = parent.hiddenLayer.slice();
    n.outLayer = parent.outLayer.slice();
    n.weights_1 = parent.weights_1.slice();
    n.weights_2 = parent.weights_2.slice();
    n.biases_1 = parent.biases_1.slice();
    n.biases_2 = parent.biases_2.slice();
    return n;
  }

  /* ElmanNet::random -- a signed value in (-base/100, base/100). */
  random(base) {
    let d = rnd(base) / 100.0;
    if (rnd(100) / 100.0 < 0.50) d *= -1.0;
    return d;
  }

  reset() {
    for (let j = 0; j < this.hiddenCt; j++) {
      for (let i = 0; i < this.inCt; i++) {
        this.weights_1[j * this.inCt + i] = Math.fround(this.random(20));
      }
      this.biases_1[j] = 1.0;
    }
    for (let j = 0; j < this.outCt; j++) {
      for (let i = 0; i < this.hiddenCt; i++) {
        this.weights_2[j * this.hiddenCt + i] = Math.fround(this.random(20));
      }
      this.biases_2[j] = 1.0;
    }
  }

  /* Bipolar sigmoid, range (-1, 1). */
  activation(x) { return 2.0 / (1.0 + Math.exp(-x)) - 1.0; }

  activationPrime(x) {
    const act = this.activation(x);
    return 0.5 * (1.0 + act) * (1.0 - act);
  }

  simulate(input) {
    const { inCt, hiddenCt, outCt } = this;
    const z = new Float32Array(hiddenCt);
    const y = new Float32Array(outCt);

    for (let j = 0; j < hiddenCt; j++) {
      let sum = 0.0;
      for (let i = 0; i < inCt; i++) sum += input[i] * this.weights_1[j * inCt + i];
      z[j] = this.activation(this.biases_1[j] + sum);
    }
    for (let k = 0; k < outCt; k++) {
      let sum = 0.0;
      for (let j = 0; j < hiddenCt; j++) sum += z[j] * this.weights_2[k * hiddenCt + j];
      y[k] = this.activation(this.biases_2[k] + sum);
    }
    return y;
  }

  /* One online backprop step. The hidden and output activations are stored
   * back into the net, which is what FleaNN later serializes. */
  train(input, target) {
    const { inCt, hiddenCt, outCt, alpha } = this;
    const z_in = new Float32Array(hiddenCt);
    const y_in = new Float32Array(outCt);
    const delta2 = new Float32Array(outCt);
    const delta1 = new Float32Array(hiddenCt);

    for (let i = 0; i < inCt; i++) this.inLayer[i] = input[i];

    for (let j = 0; j < hiddenCt; j++) {
      let sum = 0.0;
      for (let i = 0; i < inCt; i++) sum += input[i] * this.weights_1[j * inCt + i];
      z_in[j] = this.biases_1[j] + sum;
      this.hiddenLayer[j] = this.activation(z_in[j]);
    }
    for (let k = 0; k < outCt; k++) {
      let sum = 0.0;
      for (let j = 0; j < hiddenCt; j++) {
        sum += this.hiddenLayer[j] * this.weights_2[k * hiddenCt + j];
      }
      y_in[k] = this.biases_2[k] + sum;
      this.outLayer[k] = this.activation(y_in[k]);
    }

    for (let k = 0; k < outCt; k++) {
      delta2[k] = (target[k] - this.outLayer[k]) * this.activationPrime(y_in[k]);
    }
    for (let j = 0; j < hiddenCt; j++) {
      let sum = 0.0;
      for (let k = 0; k < outCt; k++) sum += delta2[k] * this.weights_2[k * hiddenCt + j];
      delta1[j] = sum * this.activationPrime(z_in[j]);
    }

    for (let k = 0; k < outCt; k++) {
      for (let j = 0; j < hiddenCt; j++) {
        this.weights_2[k * hiddenCt + j] += alpha * delta2[k] * this.hiddenLayer[j];
      }
      this.biases_2[k] += alpha * delta2[k];
    }
    for (let j = 0; j < hiddenCt; j++) {
      for (let i = 0; i < inCt; i++) {
        this.weights_1[j * inCt + i] += alpha * delta1[j] * input[i];
      }
      this.biases_1[j] += alpha * delta1[j];
    }
  }
}
