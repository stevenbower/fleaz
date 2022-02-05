/*
   $RCSfile: FleaFSM.C,v $
   $Author: psykelus $
   $Date: 1999/05/12 21:52:35 $
   Rev Hist:
      $Log: FleaFSM.C,v $
      Revision 1.19  1999/05/12 21:52:35  psykelus
      final for presentation

      Revision 1.18  1999/05/11 03:41:11  psykelus
      working save/load & jiggler penalty

      Revision 1.17  1999/05/10 03:52:24  psykelus
      saves and loads, but load is screwy - shift
      of one base left in the genotype on load.
      dunno why.

      Revision 1.16  1999/05/10 01:42:11  steve
      added data file constructor

      Revision 1.15  1999/05/10 01:17:31  psykelus
      phattie new file i/o - puts 32 bits from genotype into single int

      Revision 1.14  1999/05/05 05:57:30  steve
      made lastBlock part of flea and moved update into RingMaster

      Revision 1.13  1999/05/05 05:47:49  psykelus
      ther're working!

      Revision 1.12  1999/05/05 01:33:22  psykelus
      brand new fleas!

      Revision 1.11  1999/05/04 22:58:30  psykelus
      fitness function now appears to 'do the right thing.'

      Revision 1.10  1999/05/04 22:28:59  psykelus
      BR4Nd 5P4NkiN' new fitness function. should award eaters
      and long walkers. also - more chance of two-point crossover

      Revision 1.9  1999/05/04 17:13:07  psykelus
      added fitness function, fixed 66tate

      Revision 1.8  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.7  1999/05/04 03:16:20  psykelus
      fixed all my stupidness in think, now calculating
      state correctly (for real)

      Revision 1.6  1999/05/04 02:44:19  psykelus
      fixed incorrect calculation of state, hopefully

      Revision 1.5  1999/05/04 02:20:26  psykelus
      implemented putData, fixed FSMLENGTH

      Revision 1.4  1999/04/27 03:37:09  pete
      Added new function "crossover" to handle reproduction instead of the
      constructor.  Reason:  Outside classes (like Midwife) need to be able to
      generically access reproduction functionality.

      Revision 1.3  1999/04/26 03:07:50  psykelus
      compiles!

      Revision 1.2  1999/04/25 16:56:10  psykelus
      initial implementation of think()

      Revision 1.1  1999/04/24 18:33:48  psykelus
      created breeding, incarnate, and random constructor



*/

#include "FleaFSM.h"

FleaFSM::FleaFSM(){

	int i;

    Flea();

	for( i = 0; i < FSMLENGTH; i++ ){

		if( !(i % 4) ){
			if ( ( i / 4 ) < 216 ){
				genotype[ i ] = !( !( rand() % 4 ) );
			} else {
				genotype[ i ] = !( rand() % 4 );
			}
		} else {
			genotype[ i ] = rand() %  2;
		}
	}

	lastFitness = 0;
    type = FSM;
	xOver = 99;
}

FleaFSM::FleaFSM( char gene[ FSMLENGTH ] ){

	int i;

    Flea();

	for( i = 0; i < FSMLENGTH; i++ ){

		genotype[ i ] = gene[ i ];
	}

	lastFitness = 0;
    type = FSM;
}

FleaFSM::FleaFSM( int _id, fleaInfo *info, int ndata, int *_data ){

	int i,j,geneIndex;
	unsigned int	token;

	if( !( ndata == ( FSMLENGTH / 32 ) ) ){
	lprintf( " FleaFSM got bad data length on load. Was %d, not %d\n",
				ndata, ( FSMLENGTH / 32 ) );
	exitError( " FleaFSM load constructor got bad data length\n" );
	}
	for( i = 0; i < ndata; i++ ){

		geneIndex = i * 32;
		token = _data[ i ];

	// printf( "geneIndex : %d  token : %x\n", geneIndex, token );
		// I'M 741kiNG 4B0U7 l337, B4Bi!
		//
		for( j = 0; j < 32; j++ ){
			genotype[ geneIndex + j ] = ( ( token & 0x80000000 ) >> 31 );
			token <<= 1;
		}
	}

	// when you run, and play... you wear out the soul.
	memcpy( &soul, info, sizeof( fleaInfo ) );
	lastFitness = soul.fitness;
	xOver=23;
}

int FleaFSM::getData( int **dataPtr ){

	int i,j,packedLength,geneIndex;
	unsigned int	token;

	packedLength = ( FSMLENGTH / 32 );

	*dataPtr = (int *) ( calloc( ( packedLength ), sizeof( int ) ) );

	for( i = 0; i < packedLength; i++ ){

		geneIndex = i * 32;
		token = 0;
	// printf( " geneIndex : %d \n ", geneIndex );

		for( j = 0; j < 31; j++ ){
			token |= genotype[ geneIndex + j ];
			token <<= 1;
		}
		// or in the last bit
		j++;
		token |= genotype[ geneIndex + j ];

		( *dataPtr )[ i ] = token;
	}
	return ( packedLength );
}

void FleaFSM::putData( ){
}

void FleaFSM::mutate( ){

	int i;

	for( i = 0; i < FSMLENGTH; i++ ){

		if( ( FSM_MUTRATE > ( rand() % 1000 ) ) ){

		/*
		if( !( !( i % 4 ) && ( ( i / 4 ) < 216 ) && ( genotype[ i ] )) || 
			!( !( i % 4 ) && ( ( i / 4 ) > 216 ) && !(genotype[ i ] )) ){
				soul.mutations++;
				genotype[ i ] = !( genotype[ i ] );
			}
		*/

		soul.mutations++;
		genotype[ i ] = !( genotype[ i ] );

		}
	}

}

void FleaFSM::display(){

	int		i;

	/*
	for( i = 0; i < 70; i++ ){
		lprintf( "%d", genotype[ i ] );
	}
	lprintf( "\n" );
	for( i = FSMLENGTH - 70; i < FSMLENGTH; i++ ){
		lprintf( "%d", genotype[ i ] );
	}
	*/

lprintf( "f: %5d  e: %2d  l: %4d  m: %4d/%-3d  a: %2d  m: %4d  x: %1d\n",
				soul.fitness, soul.numEats, soul.lifespan, soul.numMoves,
				soul.distMoved, soul.age, soul.mutations, xOver );
}

void FleaFSM::fitness( ){

	int x,y;

	soul.fitness = ( ( soul.lifespan * 5 ) / 2 ) + ( lastFitness / 2 );
				 // ( ( soul.distMoved ) * ( soul.numEats + 1 ) );

	if( !( lastInfo[ 2 ][ 2 ] ) ){
		soul.fitness = soul.fitness + ( soul.lifespan );
	}
	if( !( lastInfo[ 2 ][ 1 ] ) || !( lastInfo[ 2 ][ 3 ] ) ||
		!( lastInfo[ 1 ][ 1 ] ) || !( lastInfo[ 1 ][ 2 ] ) ||
		!( lastInfo[ 1 ][ 3 ] ) || !( lastInfo[ 3 ][ 1 ] ) ||
		!( lastInfo[ 3 ][ 2 ] ) || !( lastInfo[ 3 ][ 3 ] ) ){
		soul.fitness = soul.fitness + ( soul.lifespan / 2 );
	}
	if( !( lastInfo[ 1 ][ 0 ] ) || !( lastInfo[ 1 ][ 1 ] ) ||
		!( lastInfo[ 1 ][ 2 ] ) || !( lastInfo[ 1 ][ 3 ] ) ||
		!( lastInfo[ 1 ][ 4 ] ) || !( lastInfo[ 2 ][ 0 ] ) ||
		!( lastInfo[ 2 ][ 4 ] ) || !( lastInfo[ 3 ][ 0 ] ) ||
		!( lastInfo[ 3 ][ 4 ] ) || !( lastInfo[ 4 ][ 0 ] ) ||
		!( lastInfo[ 4 ][ 1 ] ) || !( lastInfo[ 4 ][ 2 ] ) ||
		!( lastInfo[ 4 ][ 3 ] ) || !( lastInfo[ 4 ][ 4 ] ) ){
		soul.fitness = soul.fitness + ( soul.lifespan / 4 );
	}

	/*
	if( ( soul.numMoves / (soul.distMoved+1) ) > 4.0 ){
		soul.fitness = soul.fitness - ( soul.lifespan / 5 );
	}
	*/

	/*
	float	adjustment;
	soul.fitness = ( soul.lifespan ) + ( soul.numEats * 16 ) +
					( soul.numMoves );
	if( soul.distMoved && soul.numMoves ){
	if( ( soul.numMoves / soul.distMoved ) < 3.0 ){
		adjustment = ((float)soul.lifespan) * 3 *
		(1.0/(((float)soul.numMoves) / ((float)soul.distMoved)));
		soul.fitness = soul.fitness + (int)( rint( adjustment ) );
	}
	}
	*/
}

void FleaFSM::think( infoBlock *info ){

	double C,L,R,A;
	float avgL,avgR,avgA;
	int state,i,lastMoveType,x,y;

	C = (double)( (*info)[ 2 ][ 2 ] );	// set C to value of location

	avgL = ((float)( (*info)[0][0] + 2 * (*info)[1][0] + 2 * (*info)[2][0] +
					 2 * (*info)[0][1] + 3 * (*info)[1][1] + 2 * (*info)[0][2] +
					 3 * (*info)[1][2] + 2 * (*info)[0][3] )) / 17.0;

	avgR = ((float)( (*info)[4][0] + 2 * (*info)[3][0] + 2 * (*info)[4][1] +
					 3 * (*info)[3][1] + 3 * (*info)[2][1] + 2 * (*info)[4][2] +
					 3 * (*info)[3][2] + 2 * (*info)[4][3] )) / 18.0;

	avgA = ((float)( (*info)[0][4] + 2 * (*info)[1][4] + 2 * (*info)[2][4] +
					 2 * (*info)[3][4] + (*info)[4][4] + 3 * (*info)[1][3] +
					 3 * (*info)[2][3] + 3 * (*info)[3][3] )) / 17.0;

	L = rint( avgL );
	R = rint( avgR );
	A = rint( avgA );

//	if( nextMove == ACT_ATTACK || nextMove == ACT_EAT ){
//		lastMoveType = 0;
//	} else {
//		lastMoveType = 1;
//	}


//	state = (int)( A + ( 6.0 * L ) + ( 36.0 * R ) + ( 216.0 * C ) +
//					( 1296.0 * lastMoveType ) );
	state = (int)( A + ( 6.0 * L ) + ( 36.0 * R ) + ( 216.0 * C ) );
	i = state * 4;
	

	/*
	printf( " %d %d %d %d %d \n", (*info)[0][0], (*info)[1][0],
			(*info)[2][0], (*info)[3][0], (*info)[4][0] );
	printf( " s : %d  aL : %f  L : %f  C : %f \n", state, avgL, L, C);
	*/
            
	if( genotype[ state * 4 ] ){

		// eat or fight !
		if( C == INFO_FLEA ){
			nextMove = ACT_ATTACK;
		} else {
			nextMove = ACT_EAT;
		}

	} else {

		// move !
		nextMove = ( genotype[ i + 1 ] ) + ( 2 * genotype[ i + 2 ] ) + 
					( 4 * genotype[ i + 3 ] ) + 1;

	}

}

// name :  crossover
// desc :  Crosses over the mate with "this".  The arguments should all
//         be of the same class type as "this".
//
void FleaFSM::crossover( Flea *mate, Flea **child1, Flea **child2 )
{
	int dice, i, xOverOne, xOverTwo, xOverType;

    // We will use these var's to retain the names Matt originally used for
    // his variables.
    FleaFSM *mother = ( FleaFSM * ) mate;
    FleaFSM *father = ( FleaFSM * ) this;

    // Create the children.
    FleaFSM *_child1 = new FleaFSM();
    FleaFSM *_child2 = new FleaFSM();

	dice = rand() % 100;

	if( dice <= UNIFORM_CHANCE ){

		xOverType = 0;
		for( i = 0; i < FSMLENGTH; i++ ){
			if( ( rand() % 2 ) ){
				_child1->genotype[ i ] = mother->genotype[ i ];
				_child2->genotype[ i ] = father->genotype[ i ];
			} else {
				_child1->genotype[ i ] = father->genotype[ i ];
				_child2->genotype[ i ] = mother->genotype[ i ];
			}
		}
	} else
	if( dice > UNIFORM_CHANCE && dice <= ONEPOINT_CHANCE ){

		xOverType = 1;
		xOverOne = (int)( rand() % FSMLENGTH );

		for( i = 0; i < xOverOne; i++ ){
			_child1->genotype[ i ] = mother->genotype[ i ];
			_child2->genotype[ i ] = father->genotype[ i ];
		}
		for( i = xOverOne; i < FSMLENGTH; i++ ){
			_child1->genotype[ i ] = father->genotype[ i ];
			_child2->genotype[ i ] = mother->genotype[ i ];
		}
	} else {

		xOverType = 2;
		xOverOne = (int)( rand() % FSMLENGTH );
		xOverTwo = xOverOne +
					( rand() % ( FSMLENGTH - ( xOverOne ) ) + 1 );

		for( i = 0; i < xOverOne; i++ ){
			_child1->genotype[ i ] = mother->genotype[ i ];
			_child2->genotype[ i ] = father->genotype[ i ];
		}
		for( i = xOverOne; i < xOverTwo; i++ ){
			_child1->genotype[ i ] = father->genotype[ i ];
			_child2->genotype[ i ] = mother->genotype[ i ];
		}
		for( i = xOverTwo; i < FSMLENGTH; i++ ){
			_child1->genotype[ i ] = mother->genotype[ i ];
			_child2->genotype[ i ] = father->genotype[ i ];
		}
	}

	_child1->lastFitness = lastFitness;
	_child2->lastFitness = lastFitness;
	
	_child1->xOver = xOverType;
	_child2->xOver = xOverType;

    *child1 = _child1;
    *child2 = _child2;
}

