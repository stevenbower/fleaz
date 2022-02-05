/*
   $RCSfile: FleaNN.C,v $
   $Author: steve $
   $Date: 1999/05/17 16:22:12 $
   Rev Hist:
      $Log: FleaNN.C,v $
      Revision 1.16  1999/05/17 16:22:12  steve
      added CVS headers

*/

#include "FleaNN.h"

extern int current_repo_size;
extern infoBlock infoRepository[];

FleaNN::FleaNN()
{
    Flea();

#ifdef BREED_STRUCTURE
    double alpha       = nn_random( 100 );
    int    hiddenNodes = ( int ) nn_random( 5000 );  // Between 1-50 hidden nodes.
#else
    double alpha       = NN_ALPHA;
    int    hiddenNodes = NN_HIDDEN_NODES;
#endif

    nn = new ElmanNet( 
        NN_INPUT_NODES, hiddenNodes, NN_OUTPUT_NODES, alpha );

    type = NN;
    correctPredictions = 0;
    prevFitness = 0;
}

FleaNN::FleaNN( ElmanNet *nnet )
{
    Flea();

    nn = nnet;
    
    type = NN;
    correctPredictions = 0;
    prevFitness = 0;
}

FleaNN::FleaNN( int _id, fleaInfo *info, int ndata, int *_data ){
    Flea();

    id            = (UINT)id;
    dataLength    = ndata;
    data          = _data;
    std::memcpy( &soul, info, sizeof( fleaInfo ));

    prevFitness = _data[0];
    correctPredictions = _data[1];
    nn = new ElmanNet( _data + 2 );
}

FleaNN::~FleaNN()
{
    delete nn;
}

double FleaNN::nn_random( int base )
{
    double d = ( ( double ) ( rand() % base ) ) / 100.0;
    return d;
}

float *FleaNN::info2input( infoBlock *info )
{
    float *input = ( float * ) malloc( NN_INPUT_NODES * sizeof( float ) );
    int    inctr = 0;
    int    i;
    int    j;

/*
    // Turn the info block into some valid neural net input.  Put a 1.0 where
    // food is, -1.0 for everything else.
    for ( j = 0; j < 5; j++ )
    {
        for ( i = 0; i < 5; i++ )
        {
            input[inctr++] = ( *info )[j][i] == INFO_FOOD ? 1.0 : -1.0;
        }
    }
 */
    for ( i = 0; i < NN_INPUT_NODES; i++ )
    {
        input[i] = i == ( *info )[2][2] ? 1.0 : -1.0;
    }

    return input;
}

float *FleaNN::info2target( infoBlock *info )
{
    float *target = ( float * ) malloc( NN_OUTPUT_NODES * sizeof( float ) );
    int    action;
    int    badaction;
    int    j;

    // Construct target block, based on surroundings. The target response
    // will be the direction that the flea is sitting on.
    switch ( ( *info )[2][2] )
    {
        case INFO_FOOD:            
            action = ACT_EAT;     
            badaction = ACT_NOP; 
            break;
        case INFO_FLEA:            
            action = ACT_EAT;     
            badaction = ACT_NOP;
            break;
        case INFO_EAST:    
            switch ( ( *info )[2][3] )
            {
                case INFO_NORTH:   
                    action = ACT_MNEAST;  
                    badaction = ACT_MSWEST;
                    break;
                case INFO_SOUTH:   
                    action = ACT_MSEAST;  
                    badaction = ACT_MNWEST;
                    break;
                default:           
                    action = ACT_MEAST;   
                    badaction = ACT_MWEST;
                    break;
            }
            break;
        case INFO_WEST:    
            switch ( ( *info )[2][1] )
            {
                case INFO_NORTH:   
                    action = ACT_MNWEST;  
                    badaction = ACT_MSEAST;
                    break;
                case INFO_SOUTH:   
                    action = ACT_MSWEST;  
                    badaction = ACT_MNEAST;
                    break;
                default:           
                    action = ACT_MWEST;   
                    badaction = ACT_MEAST;
                    break;
            }
            break;
        case INFO_NORTH:
            switch ( ( *info )[1][2] )
            {
                case INFO_EAST:    
                    action = ACT_MNEAST;  
                    badaction = ACT_MSWEST;
                    break;
                case INFO_WEST:    
                    action = ACT_MNWEST;  
                    badaction = ACT_MSEAST;
                    break;
                default:           
                    action = ACT_MNORTH;  
                    badaction = ACT_MSOUTH;
                    break;
            }
            break;
        case INFO_SOUTH:
            switch ( ( *info )[3][2] )
            {
                case INFO_EAST:    
                    action = ACT_MSEAST;  
                    badaction = ACT_MNWEST;
                    break;
                case INFO_WEST:    
                    action = ACT_MSWEST;  
                    badaction = ACT_MNEAST;
                    break;
                default:           
                    action = ACT_MSOUTH;  
                    badaction = ACT_MNORTH;
                    break;
            }
            break;
        default:                   
            action = ACT_EAT;     
            badaction = ACT_NOP;
            break;
    }

    for ( j = 0; j < 11; j++ )
    {
    /*
        if ( j == badaction )
        {
            target[j] = -1.0;
        }
        else if ( j == action )
        {
            target[j] = 1.0;
        }
        else
        {
            target[j] = 0.0;
        }
    */
        
        target[j] = j == action ? 1.0 : -1.0;
    }

    return target;
}

void FleaNN::think( infoBlock *info )
{
    int    i, j, k;
    float *input;
    float *target;
    float *output;
    int    correctNextMove;

    input  = info2input( info );
    output = nn->simulate( input );
    target = info2target( info );

    nextMove = 0;
    correctNextMove = 0;
    for ( i = 1; i < NN_OUTPUT_NODES; i++ )
    {
        if ( output[i] > output[nextMove] )
        {
            nextMove = i;
        }
        if ( target[i] > target[correctNextMove] )
        {
            correctNextMove = i;
        }
    }

    if ( nextMove == correctNextMove )
    {
        correctPredictions++;
    }

    // Add infoblock to repository.
    if ( current_repo_size < REPO_SIZE )
    {
        std::memcpy( infoRepository[current_repo_size], *info, sizeof( infoBlock ) );
        current_repo_size++;
    }
    else
    {
        int which = ( int ) nn_random( current_repo_size * 100 );
        std::memcpy( infoRepository[which], *info, sizeof( infoBlock ) );
    }

    free( target );
    free( input );
    free( output );
}

void FleaNN::crossover( Flea *mate, Flea **child1, Flea **child2 )
{
    // Duplicate the mother and father.

    FleaNN *father = this;
    FleaNN *mother = ( FleaNN * ) mate;

#ifdef BREED_STRUCTURE
    // Crossover the alpha and hidden-node count.
    FleaNN *_child1 = new FleaNN( 
        new ElmanNet( 
            NN_INPUT_NODES, 
            father->nn->hiddenCt,
            NN_OUTPUT_NODES,
            mother->nn->alpha
        )
    );
            
    FleaNN *_child2 = new FleaNN( 
        new ElmanNet( 
            NN_INPUT_NODES, 
            mother->nn->hiddenCt,
            NN_OUTPUT_NODES,
            father->nn->alpha
        )
    );
#else
    FleaNN *_child1 = new FleaNN( 
        new ElmanNet( father->nn, father->nn->alpha ) );
    FleaNN *_child2 = new FleaNN( 
        new ElmanNet( mother->nn, mother->nn->alpha ) );

/*
    FleaNN *_child1 = new FleaNN(
        new ElmanNet( father->nn->alpha, father->nn->inCt, father->nn->hiddenCt, 
            mother->nn->outCt, father->nn->inLayer, father->nn->contextLayer, 
            father->nn->hiddenLayer, mother->nn->outLayer, father->nn->weights_1, 
            mother->nn->weights_2, father->nn->biases_1, mother->nn->biases_2 
        )
    );
    FleaNN *_child2 = new FleaNN(
        new ElmanNet( mother->nn->alpha, mother->nn->inCt, mother->nn->hiddenCt, 
            father->nn->outCt, mother->nn->inLayer, mother->nn->contextLayer, 
            mother->nn->hiddenLayer, father->nn->outLayer, mother->nn->weights_1, 
            father->nn->weights_2, mother->nn->biases_1, father->nn->biases_2 
        )
    );
*/
#endif

    *child1 = _child1;
    *child2 = _child2;
    father->correctPredictions = 0;
    mother->correctPredictions = 0;
}

void FleaNN::mutate()
{
    
#ifdef BREED_STRUCTURE
    int r = ( int ) nn_random( 10000 );
    if ( r < NN_MUT_RATE )
    {
        double new_alpha;
        int    new_hidden_ct;
        float  t;

        // Mutate alpha:     33.3%
        //        hiddenCt:  33.3%
        //        both:      33.3%

        t = nn_random( 300 );
        if ( t > 2.0 )
        {
            new_alpha     = nn_random( 100 );
            new_hidden_ct = nn->hiddenCt;
        }
        else if ( t > 1.0 )
        {
            new_alpha     = nn->alpha;
            new_hidden_ct = ( int ) nn_random( 5000 );
        }
        else
        {
            new_alpha     = nn_random( 100 );
            new_hidden_ct = ( int ) nn_random( 5000 );
        }

        if ( nn )
        {
            delete nn;
        }
        
        nn = new ElmanNet( NN_INPUT_NODES, new_hidden_ct, NN_OUTPUT_NODES,
            new_alpha );
    }
#else
    // Mutate by training.
    int r = ( int ) nn_random( 10000 );
    if ( r < NN_MUT_RATE && current_repo_size > 0 )
    {
        // Get a sample set of input infoblocks.
        int    start  = ( int ) nn_random( current_repo_size * 100 );
        int    stop   = ( int ) nn_random( current_repo_size * 100 );
        int    which;

        if ( stop < start )
        {
            start = stop;
        }

        stop = start; 

        for ( which = start; which <= stop; which++ )
        {
            // Convert the infoblock to NN data.
            float *input  = info2input( &infoRepository[which] );

            // Create ideal target data for the NN.
            float *target = info2target( &infoRepository[which] );
        
            // Train the network with the newly-gotten example.
            // 3 times.
            for ( int p = 0; p < 3; p++ )
            {
                nn->train( input, target );
            }

            free( input );
            free( target );
        }
    }
#endif
}

void FleaNN::fitness()
{
    int x,y;

    if ( soul.numMoves == 0 )
    {
        soul.fitness = 0;
    }
    else
    {
        soul.fitness = ( ( int ) pow( correctPredictions, 2 ) ) * 
                   ( ( int ) pow( 1 + soul.numEats,   2 ) );
    }

    soul.fitness += prevFitness / 2;
    prevFitness = soul.fitness;
}

int FleaNN::getData( int **dataPtr )
{
    int i;
    int *nnser = NULL;
    int len;

    len = nn->serialize( &nnser ); 
    *dataPtr = ( int * ) malloc( ( 2 + len ) * sizeof( int ) );

    ( *dataPtr )[0] = prevFitness;
    ( *dataPtr )[1] = correctPredictions;

    for ( i = 0; i < len; i++ )
    {
        ( *dataPtr )[i + 2] = nnser[i];
    }

    if ( nnser )
    {
        free( nnser );
    }
    return i;
}

void FleaNN::display()
{
#ifdef BREED_STRUCTURE
    lprintf( "\n alpha: %f  hiddenCt: %d  fitness: %d\n", nn->alpha, 
        nn->hiddenCt, soul.fitness );
#else
    nn->examineStructure();
#endif
}

