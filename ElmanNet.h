/**
 * Name:		Peter Vetere
 * Date:		Feb. 18, 1999
 * File:		ElmanNet.h
 * Description:	This file contains functionality to train and simulate a 
 *				standard Elman Network.
 */

#ifndef ELMANNET_H
#define ELMANNET_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "fleaz.h"

#define FLOAT_REP_2_INT( x ) ( ( int ) *( ( int * ) &( x ) ) )
#define INT_2_FLOAT_REP( x ) ( ( float ) *( ( float * ) &( x ) ) )

class ElmanNet
{
	public:

		/**
		 * Create the Elman Network.
		 * Arguments:
		 *		inCount - The number of input nodes for this net.
		 *		hiddenCount - The number of hidden nodes for this net.
		 * 		lRate - The learning rate.
		 */
		ElmanNet( int inCount, int hiddenCount, int outCount, float lRate );
        ElmanNet( int *data );
        ElmanNet( ElmanNet *parent, float alph );
        ElmanNet( ElmanNet *parent );
        ElmanNet( float _alpha, int _inCt, int _hiddenCt, int _outCt,
            float *_inLayer, float *_hiddenLayer,
            float *_outLayer, float **_weights_1, float **_weights_2,
            float *_biases_1, float *_biases_2 );

		/**
		 * Destroy an ElmanNet object.
		 */
		~ElmanNet();

		/**
		 * Reset weights and activations within the network.
		 */
		void reset();

        /**
         * Train the network with an input vector and an expected output
         * vector.  The input vector size should be NN_INPUT_NODES and the 
         * output vector should be NN_OUTPUT_NODES long.
         */
        void train( float *input, float *output );

		/**
		 * Simulate the network.
		 */
		float *simulate( float *input );

		/**
		 * The activation function.
		 */
		float activation( float x );

		/**
		 * The derivation of the activation function (for backprop).
		 */
		float activationPrime( float x );

        /**
         * Serialize this network into an int array.
         */
        int serialize( int **dataPtr );

        void examineStructure();

		float alpha;
		int hiddenCt;

		/**
		 * Produce a random number.
		 */
		double random( int base );

		int inCt;
		int outCt;

		float *inLayer;
		float **weights_1;
		float *biases_1;
		float *hiddenLayer;
		float **weights_2;
		float *biases_2;
		float *outLayer;
};

#endif

