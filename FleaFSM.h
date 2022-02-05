/*
   $RCSfile: FleaFSM.h,v $
   $Author: psykelus $
   $Date: 1999/05/12 21:52:35 $
   Rev Hist:
      $Log: FleaFSM.h,v $
      Revision 1.13  1999/05/12 21:52:35  psykelus
      final for presentation

      Revision 1.12  1999/05/10 01:42:11  steve
      added data file constructor

      Revision 1.11  1999/05/10 01:17:31  psykelus
      phattie new file i/o - puts 32 bits from genotype into single int

      Revision 1.10  1999/05/05 05:57:30  steve
      made lastBlock part of flea and moved update into RingMaster

      Revision 1.9  1999/05/05 05:47:49  psykelus
      ther're working!

      Revision 1.8  1999/05/05 01:33:22  psykelus
      brand new fleas!

      Revision 1.7  1999/05/04 22:28:59  psykelus
      BR4Nd 5P4NkiN' new fitness function. should award eaters
      and long walkers. also - more chance of two-point crossover

      Revision 1.6  1999/05/04 17:13:07  psykelus
      added fitness function, fixed mutate

      Revision 1.5  1999/05/04 02:20:26  psykelus
      implemented putData, fixed FSMLENGTH

      Revision 1.4  1999/04/27 03:37:10  pete
      Added new function "crossover" to handle reproduction instead of the
      constructor.  Reason:  Outside classes (like Midwife) need to be able to
      generically access reproduction functionality.

      Revision 1.3  1999/04/26 03:07:50  psykelus
      compiles!

      Revision 1.2  1999/04/25 16:56:11  psykelus
      initial implementation of think()

      Revision 1.1  1999/04/24 18:33:48  psykelus
      created breeding, incarnate, and random constructor



*/

#ifndef __FLEAFSM_H
#define __FLEAFSM_H

#define FSMLENGTH		5184	// length of FSM
#define UNIFORM_CHANCE	3		// percent chance of uniform crossover
#define ONEPOINT_CHANCE 43		// percent chance of one point crossover
#define FSM_MUTRATE		9		// mutation rate (tenths of a percent)

#include "math.h"
#include "stdlib.h"
#include "fleaz.h"
#include "Flea.h"


class FleaFSM: public Flea {

public:

	// name :	( random genotype constructor )
	// args :	states - number of possible states for each index
	// desc :	creates random genotype
	//
	FleaFSM();

	// name :	( incarnation constructor )
	// args :	gene - pointer to a genotype
	// desc :	sets genotype to gene
	FleaFSM( char gene[ FSMLENGTH ] );

	// name :	( file constructor )
	// args :	id, info block, data size, data 
	// desc :	file loading const
    FleaFSM( int _id, fleaInfo *info, int ndata, int *_data );

	// name : think
	// args : char array of surrounding location
	// desc : sets next move based on lookup into genotype
	//
	virtual void think( infoBlock *info );

	// name :	getData
	// args :	dataPtr - pointer to char array
	// desc :	see Flea.h
	//
	virtual int getData( int **dataPtr );

	// name :	putData
	// args :	none
	// desc :	see Flea.h
	virtual void putData( );

	// name :	display
	// args :	none
	// desc :	see Flea.h
	//
	virtual void display();

	// name :	fitness
	// args :	none
	// desc :	calculates fitness according to current scheme
	//
	virtual void fitness();

	// name :	mutate
	//
	virtual void mutate( );

    // name : crossover
    //
    virtual void crossover( Flea *mate, Flea **child1, Flea **child2 );

protected:

	char	lastMove;				// last move made
	int		lastFitness;			// fitness in previous generation
	char	genotype[ FSMLENGTH ];	// the FSM lookup table
	char	xOver;					// which crossover type made me
};

#endif
