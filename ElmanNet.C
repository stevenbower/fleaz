/**
 * Name:		Peter Vetere
 * Date:		Feb. 18, 1999
 * File:		ElmanNet.C
 * Description:	This file contains functionality to train and simulate a
 *				standard Elman Network.
 */

#include "ElmanNet.h"

/**
 * Create the Elman Network.
 * Arguments:
 * 		inCount - The number of input nodes for this net.
 * 		hiddenCount - The number of hidden nodes for this net.
 *		lRate - The learning rate.
 */
ElmanNet::ElmanNet( int inCount, int hiddenCount, int outCount, float lRate )
{
	inCt = inCount;
	hiddenCt = hiddenCount;
	outCt = outCount;

	inLayer = ( float * ) calloc( inCt, sizeof( float ) );
	hiddenLayer = ( float * ) calloc( hiddenCt, sizeof( float ) );
	outLayer = ( float * ) calloc( outCt, sizeof( float ) );

	weights_1 = ( float ** ) calloc( hiddenCt, sizeof( float * ) );
	for ( int i = 0; i < hiddenCt; i++ )
	{
		weights_1[i] = ( float * ) calloc( inCt, sizeof( float ) );
	}
	weights_2 = ( float ** ) calloc( outCt, sizeof( float * ) );
	for ( int i = 0; i < outCt; i++ )
	{
		weights_2[i] = ( float * ) calloc( hiddenCt, sizeof( float ) );
	}
	
	biases_1 = ( float * ) calloc( hiddenCt, sizeof( float ) );
	biases_2 = ( float * ) calloc( outCt, sizeof( float ) );

	// Reset the network.
	reset();

	alpha = lRate;
}

ElmanNet::ElmanNet( float _alpha, int _inCt, int _hiddenCt, int _outCt, 
    float *_inLayer, float *_hiddenLayer, float *_outLayer, 
    float **_weights_1, float **_weights_2, float *_biases_1, float *_biases_2 )
{
    inCt           = _inCt;
    hiddenCt       = _hiddenCt;
    outCt          = _outCt;
    alpha          = _alpha;

    inLayer        = ( float *  ) calloc( inCt, sizeof( float ) );
    for ( int i = 0; i < inCt; i++ )
    {
        inLayer[i] = _inLayer[i];
    }

    hiddenLayer    = ( float *  ) calloc( hiddenCt, sizeof( float   ) );
    biases_1       = ( float *  ) calloc( hiddenCt, sizeof( float   ) );
    weights_1      = ( float ** ) calloc( hiddenCt, sizeof( float * ) );
    for ( int j = 0; j < hiddenCt; j++ )
    {
        hiddenLayer[j] = _hiddenLayer[j];
        weights_1[j] = ( float * ) calloc( inCt, sizeof( float ) );
		for ( int i = 0; i < inCt; i++ )
		{
            weights_1[j][i] = _weights_1[j][i];
		}
		biases_1[j] = _biases_1[j];
    }

    outLayer       = ( float *  ) calloc( outCt, sizeof( float ) );
    biases_2       = ( float *  ) calloc( outCt, sizeof( float ) );
    weights_2      = ( float ** ) calloc( outCt, sizeof( float * ) );
    for ( int j = 0; j < outCt; j++ )
    {
        outLayer[j] = _outLayer[j];
        weights_2[j] = ( float * ) calloc( hiddenCt, sizeof( float ) );
		for ( int i = 0; i < hiddenCt; i++ )
		{
			weights_2[j][i] = _weights_2[j][i];
		}
		biases_2[j] = _biases_2[j];
    }
}

ElmanNet::ElmanNet( int *data )
{
    int ptr = 0, i, j;

    alpha     = INT_2_FLOAT_REP( data[ptr++] );
    inCt      = data[ptr++];
    hiddenCt  = data[ptr++];
    outCt     = data[ptr++];

	inLayer = ( float * ) calloc( inCt, sizeof( float ) );
	hiddenLayer = ( float * ) calloc( hiddenCt, sizeof( float ) );
	outLayer = ( float * ) calloc( outCt, sizeof( float ) );

	weights_1 = ( float ** ) calloc( hiddenCt, sizeof( float * ) );
	for ( int i = 0; i < hiddenCt; i++ )
	{
		weights_1[i] = ( float * ) calloc( inCt, sizeof( float ) );
	}
	weights_2 = ( float ** ) calloc( outCt, sizeof( float * ) );
	for ( int i = 0; i < outCt; i++ )
	{
		weights_2[i] = ( float * ) calloc( hiddenCt, sizeof( float ) );
	}
	
	biases_1 = ( float * ) calloc( hiddenCt, sizeof( float ) );
	biases_2 = ( float * ) calloc( outCt, sizeof( float ) );

    /* Fill in the data structures. */

    /* Fill in inLayer. */
    for ( i = 0; i < inCt; i++ )
    {
        inLayer[i] = INT_2_FLOAT_REP( data[ptr] );
        ptr++;
    }
    /* Fill in hiddenLayer. */
    for ( i = 0; i < hiddenCt; i++ )
    {
        hiddenLayer[i] = INT_2_FLOAT_REP( data[ptr++] );
    }
    /* Fill in outLayer. */
    for ( i = 0; i < outCt; i++ )
    {
        outLayer[i] = INT_2_FLOAT_REP( data[ptr++] );
    }

    /* Fill in biases_1. */
    for ( i = 0; i < hiddenCt; i++ )
    {
        biases_1[i] = INT_2_FLOAT_REP( data[ptr++] );
    }
    /* Fill in biases_2. */
    for ( i = 0; i < outCt; i++ )
    {
        biases_2[i] = INT_2_FLOAT_REP( data[ptr++] );
    }

    /* Fill in weights_1. */
    for ( j = 0; j < hiddenCt; j++ )
    {
        for ( i = 0; i < inCt; i++ )
        {
            weights_1[j][i] = INT_2_FLOAT_REP( data[ptr++] );
        }
    }
    /* Fill in weights_2. */
    for ( j = 0; j < outCt; j++ )
    {
        for ( i = 0; i < hiddenCt; i++ )
        {
            weights_2[j][i] = INT_2_FLOAT_REP( data[ptr++] );
        }
    }
}

ElmanNet::ElmanNet( ElmanNet *parent )
{
    ElmanNet( parent, parent->alpha );
}

/**
 * Copy constructor.
 */
ElmanNet::ElmanNet( ElmanNet *parent, float alph )
{
    inCt           = parent->inCt;
    hiddenCt       = parent->hiddenCt;
    outCt          = parent->outCt;
    alpha          = alph;

    inLayer        = ( float *  ) calloc( inCt, sizeof( float ) );
    for ( int i = 0; i < inCt; i++ )
    {
        inLayer[i] = parent->inLayer[i];
    }

    hiddenLayer    = ( float *  ) calloc( hiddenCt, sizeof( float   ) );
    biases_1       = ( float *  ) calloc( hiddenCt, sizeof( float   ) );
    weights_1      = ( float ** ) calloc( hiddenCt, sizeof( float * ) );
    for ( int j = 0; j < hiddenCt; j++ )
    {
        hiddenLayer[j] = parent->hiddenLayer[j];
        weights_1[j] = ( float * ) calloc( inCt, sizeof( float ) );
		for ( int i = 0; i < inCt; i++ )
		{
            weights_1[j][i] = parent->weights_1[j][i];
		}
		biases_1[j] = parent->biases_1[j];
    }

    outLayer       = ( float *  ) calloc( outCt, sizeof( float ) );
    biases_2       = ( float *  ) calloc( outCt, sizeof( float ) );
    weights_2      = ( float ** ) calloc( outCt, sizeof( float * ) );
    for ( int j = 0; j < outCt; j++ )
    {
        outLayer[j] = parent->outLayer[j];
        weights_2[j] = ( float * ) calloc( hiddenCt, sizeof( float ) );
		for ( int i = 0; i < hiddenCt; i++ )
		{
			weights_2[j][i] = parent->weights_2[j][i];
		}
		biases_2[j] = parent->biases_2[j];
    }
}

/**
 * Destroy an ElmanNet object.
 */
ElmanNet::~ElmanNet()
{
	free( inLayer );
	free( hiddenLayer );
	free( outLayer );
	for ( int i = 0; i < hiddenCt; i++ )
	{
		free( weights_1[i] );
	}
	free( weights_1 );
	for ( int i = 0; i < outCt; i++ )
	{
		free( weights_2[i] );
	}
	free( weights_2 );
	free( biases_1 );
	free( biases_2 );
}

/**
 * Produce a random number.
 */
double ElmanNet::random( int base )
{
	double d = ( ( double ) ( rand() % base ) ) / 100.0;
	if ( ( ( double ) ( rand() % 100 ) ) / 100.0 < 0.50 )
	{
		d *= -1.0;
	}
	return d;
}

/**
 * Reset weights and activations within the network.
 */
void ElmanNet::reset()
{
	for ( int j = 0; j < hiddenCt; j++ )
	{
		for ( int i = 0; i < inCt; i++ )
		{
			// Init all other weights to small random values.
			weights_1[j][i] = ( float ) ( random( 20 ) );
            //weights_1[j][i] = ( float ) ( random( 200 ) - 1.0 );
		}
		biases_1[j] = 1.0;
	}

	for ( int j = 0; j < outCt; j++ )
	{
		for ( int i = 0; i < hiddenCt; i++ )
		{
			weights_2[j][i] = ( float ) ( random( 20 ) );
			//weights_2[j][i] = ( float ) ( random( 200 ) - 1.0 );
		}
		biases_2[j] = 1.0;
	}
}

/**
 * The activation function.
 */
float ElmanNet::activation( float x )
{
	float ex = exp( x * -1.0 );
	return ( 2.0 / ( 1.0 + ex ) ) - 1.0;
}

/**
 * The derivative of the activation function (for backprop).
 */
float ElmanNet::activationPrime( float x )
{
	float act = activation( x );
	return ( 0.5 * ( 1.0 + act ) * ( 1.0 - act ) );
}

/**
 * Train the network with just the next known value; no resetting of 
 * network each time.
 */
void ElmanNet::train( float *input, float *target )
{
	float *z_in        = ( float * )  calloc( hiddenCt, sizeof( float   ) );
	float *y_in        = ( float * )  calloc( outCt,    sizeof( float   ) );
	float *delta2      = ( float * )  calloc( outCt,    sizeof( float   ) );
	float *delta1      = ( float * )  calloc( hiddenCt, sizeof( float   ) );
	float **dweights_2 = ( float ** ) calloc( outCt,    sizeof( float * ) );
	float **dweights_1 = ( float ** ) calloc( hiddenCt, sizeof( float * ) );
	float *dbiases_2   = ( float * )  calloc( outCt,    sizeof( float   ) );
	float *dbiases_1   = ( float * )  calloc( hiddenCt, sizeof( float   ) );
	float *delta_in    = ( float * )  calloc( hiddenCt, sizeof( float   ) );

    for ( int i = 0; i < inCt; i++ )
    {
        inLayer[i] = input[i];
    }

	for ( int i = 0; i < hiddenCt; i++ )
	{
		dweights_1[i]  = ( float * ) calloc( inCt, sizeof( float ) );
	}
	for ( int i = 0; i < outCt; i++ )
	{
		dweights_2[i]  = ( float * ) calloc( hiddenCt, sizeof( float ) );
	}

	// Feed-forward.
	for ( int j = 0; j < hiddenCt; j++ )
	{
		float sum = 0.0;
		for ( int i = 0; i < inCt; i++ )
		{
			sum += input[i] * weights_1[j][i];
		}
		z_in[j] = biases_1[j] + sum;
		hiddenLayer[j] = activation( z_in[j] );
	}
	for ( int k = 0; k < outCt; k++ )
	{
		float sum = 0.0;
		for ( int j = 0; j < hiddenCt; j++ )
		{
			sum += hiddenLayer[j] * weights_2[k][j];
		}
		y_in[k] = biases_2[k] + sum;
		outLayer[k] = activation( y_in[k] );
	}

	// Back-propagate.
	for ( int k = 0; k < outCt; k++ )
	{
		delta2[k] = ( target[k] - outLayer[k] ) * activationPrime( y_in[k] );
		for ( int j = 0; j < hiddenCt; j++ )
		{
			dweights_2[k][j] = alpha * delta2[k] * hiddenLayer[j];
		}
		dbiases_2[k] = alpha * delta2[k];
	}
	for ( int j = 0; j < hiddenCt; j++ )
	{
		float sum = 0.0;
		for ( int k = 0; k < outCt; k++ )
		{
			sum += delta2[k] * weights_2[k][j];
		}
		delta_in[j] = sum;
		delta1[j] = delta_in[j] * activationPrime( z_in[j] );
	}
	for ( int j = 0; j < hiddenCt; j++ )
	{
		for ( int i = 0; i < inCt; i++ )
		{
			dweights_1[j][i] = alpha * delta1[j] * input[i];
		}
		dbiases_1[j] = alpha * delta1[j];
	}

	// Update weights and biases...
	for ( int k = 0; k < outCt; k++ )
	{
		for ( int j = 0; j < hiddenCt; j++ )
		{
			weights_2[k][j] = weights_2[k][j] + dweights_2[k][j];
		}
		biases_2[k] = biases_2[k] + dbiases_2[k];
	}
	for ( int j = 0; j < hiddenCt; j++ )
	{
		for ( int i = 0; i < inCt; i++ )
		{
			weights_1[j][i] = weights_1[j][i] + dweights_1[j][i];
		}
		biases_1[j] = biases_1[j] + dbiases_1[j];
	}

	free( z_in );
	free( y_in );
	free( delta2 );
	free( delta1 );
	free( dbiases_2 );
	free( dbiases_1 );
	free( delta_in );

	for ( int i = 0; i < hiddenCt; i++ )
	{
		free( dweights_1[i] );
	}
	for ( int i = 0; i < outCt; i++ )
	{
		free( dweights_2[i] );
	}
	free( dweights_2 );
	free( dweights_1 );
}

/**
 * Simulate the network.
 */
float *ElmanNet::simulate( float *input )
{
	float *z_in = ( float * ) calloc( hiddenCt, sizeof( float ) );
	float *z = ( float * ) calloc( hiddenCt, sizeof( float ) );
	float *y_in = ( float * ) calloc( outCt, sizeof( float ) );
	float *y = ( float * ) calloc( outCt, sizeof( float ) );

	// Feed-forward.
	for ( int j = 0; j < hiddenCt; j++ )
	{
		float sum = 0.0;
		for ( int i = 0; i < inCt; i++ )
		{
			sum += input[i] * weights_1[j][i];
		}
		z_in[j] = biases_1[j] + sum;
		z[j] = activation( z_in[j] );
	}

	for ( int k = 0; k < outCt; k++ )
	{
		float sum = 0.0;
		for ( int j = 0; j < hiddenCt; j++ )
		{
			sum += z[j] * weights_2[k][j];
		}
		y_in[k] = biases_2[k] + sum;
		y[k] = activation( y_in[k] );
	}

	free( z_in );
	free( z );
	free( y_in );

    return y;
}

void ElmanNet::examineStructure()
{
    int i;

    lprintf( "------------------------------------------------------------------\n" );
    lprintf( "inLayer:\n" );
    for ( i = 0; i < inCt; i++ )
    {
        lprintf( "\t%.4f", inLayer[i] );
    }
    lprintf( "\n" );
    lprintf( "hiddenLayer:\n" );
    for ( i = 0; i < hiddenCt; i++ )
    {
        lprintf( "\t%.4f", hiddenLayer[i] );
    }
    lprintf( "\n" );
    lprintf( "outLayer:\n" );
    for ( i = 0; i < outCt; i++ )
    {
        lprintf( "\t%.4f", outLayer[i] );
    }
}

int ElmanNet::serialize( int **datPtr )
{
    /* NOTE:  This serialization will not work if int's are not the same size 
     *        as float's. 
     */

    /* Storage will be determined as:
     *      4 +                                  [For alpha, inCt, hiddenCt, 
     *                                            outCt]
     *      inCt +                               [For inLayer]
     *      hiddenCt + outCt +                   [For hiddenLayer, outLayer]
     *      ( ( inCt * hiddenCt ) +              [For weights1]
     *      ( hiddenCt * outCt ) +               [For weights2]
     *      hiddenCt + outCt                     [For biases_1, biases_2]
     */

    int allocSize = 4 + inCt + ( hiddenCt * 2 ) + ( outCt * 2 ) + 
        ( inCt * hiddenCt ) + ( hiddenCt * outCt );
    int *dat;
    int ptr = 0;
    int i, j;

    *datPtr = ( int * ) malloc( allocSize * sizeof( int ) );
    dat = *datPtr;

    dat[ptr++] = FLOAT_REP_2_INT( alpha );
    dat[ptr++] = inCt;
    dat[ptr++] = hiddenCt;
    dat[ptr++] = outCt;

    /* Write inLayer. */
    for ( i = 0; i < inCt; i++ )
    {
        dat[ptr++] = FLOAT_REP_2_INT( inLayer[i] );
    }
    /* Write hiddenLayer. */
    for ( i = 0; i < hiddenCt; i++ )
    {
        dat[ptr++] = FLOAT_REP_2_INT( hiddenLayer[i] );
    }
    /* Write outLayer. */
    for ( i = 0; i < outCt; i++ )
    {
        dat[ptr++] = FLOAT_REP_2_INT( outLayer[i] );
    }

    /* Write biases_1. */
    for ( i = 0; i < hiddenCt; i++ )
    {
        dat[ptr++] = FLOAT_REP_2_INT( biases_1[i] );
    }
    /* Write biases_2. */
    for ( i = 0; i < outCt; i++ )
    {
        dat[ptr++] = FLOAT_REP_2_INT( biases_2[i] );
    }

    /* Write weights_1. */
    for ( j = 0; j < hiddenCt; j++ )
    {
        for ( i = 0; i < inCt; i++ )
        {
            dat[ptr++] = FLOAT_REP_2_INT( weights_1[j][i] );
        }
    }
    /* Write weights_2. */
    for ( j = 0; j < outCt; j++ )
    {
        for ( i = 0; i < hiddenCt; i++ )
        {
            dat[ptr++] = FLOAT_REP_2_INT( weights_2[j][i] );
        }
    }

    return allocSize;
}

