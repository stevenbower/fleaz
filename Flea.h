/*
   $RCSfile: Flea.h,v $
   $Author: steve $
   $Date: 1999/05/10 01:52:35 $
   Rev Hist:
      $Log: Flea.h,v $
      Revision 1.20  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.19  1999/05/05 05:57:30  steve
      made lastBlock part of flea and moved update into RingMaster

      Revision 1.18  1999/05/05 00:50:25  steve
      made destructors virtual and emptied the base class destructor

      Revision 1.17  1999/05/04 17:12:19  psykelus
      made fitness virtual

      Revision 1.16  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.15  1999/05/04 02:43:57  psykelus
      fixed putData error

      Revision 1.14  1999/05/04 02:09:59  psykelus
      added putData

      Revision 1.13  1999/05/04 01:12:24  steve
      made fitness and getData virtual

      Revision 1.12  1999/05/02 20:07:24  steve
      added display function

      Revision 1.11  1999/05/01 19:12:24  pete
      Changed flea-dependent data from char* to int*.

      Revision 1.10  1999/04/27 03:02:14  pete
      Added virtual function "crossover" which will handle reproduction.

      Revision 1.9  1999/04/26 04:18:36  pete
      Added new constructor, and new getters -- getData and getFoodLeft.

      Revision 1.8  1999/04/26 03:41:15  psykelus
      removed the incarnate() function that we needed yesterday

      Revision 1.7  1999/04/26 03:24:09  psykelus
      added fitness calculator

      Revision 1.6  1999/04/26 03:07:32  psykelus
      bloooop!

      Revision 1.5  1999/04/24 18:34:48  psykelus
      added incarante function to reset tourney-pertinint vars

      Revision 1.4  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.3  1999/04/21 02:11:19  psykelus
      Updated to reflect design changes, including adding the struct and
      removing unneccesary routines.

      Revision 1.2  1999/04/20 16:53:38  steve
      fixed some syntax errors

      Revision 1.1  1999/04/20 15:57:29  psykelus
      First import into CVS

*/

// name : Flea.h
// desc : header file for flea class (parental abstract)
// auth : matthew j weaver
//

#ifndef __FLEA_H
#define __FLEA_H

#include <stdio.h>
#include <stdlib.h>
#include "fleaz.h"

class Flea{

public:

    // name :	( constructor )
    // args :	none
    //
    Flea( );

    // name :   ( constructor )
    // args :   id        - The flea's id #.
    //          type      - The type of flea.
    //          nmove     - The next move of the flea.
    //          food      - How much food left?
    //          info      - Info about the flea.
    //          ndata     - Number of species-specific data.
    //          data      - Unique, species-specific data.
    Flea( int _id, fleaInfo *info, int ndata, int *_data );

    // name :	( destructor )
    // args :	none
    //
    virtual ~Flea( );

    // name :	think
    // args :	state - char array, each item representing the state of
    //			a perception segment (C,L,R,A)
    // desc : sets nextMove to the action desired, based on stateOfMind
    //
    virtual void think( infoBlock *info );

	// name :	mutate
	// args :	rate - mutation rate in tenths of a percent
	// desc :	mutates genome of flea
	//
	virtual void mutate( );

    // name :   crossover
    // args :   mate   - The flea to breed with.
    //          child1 - Pointer to the first child.
    //          child2 - Pointer to the second child.
    // desc :   This function mates this flea with the first argument, and
    //          places the two resultant children into child1 and child2,
    //          respectively.
    virtual void crossover( Flea *mate, Flea **child1, Flea **child2 );

    // name :   display
    // args :   (none)
    // desc :   This function displays a flea
    virtual void display();

    // name :   getData
    // args :   pointer to pointer to data
    // desc :   function should be overridden to return species-specific info.
    //          The return type will give the size of the int array, and the 
    //          argument will be pointed to the integer array.
    virtual int getData( int **dataPtr );

	// name :	fitness
	// args :	none
	// desc :	calculates fitness according to current scheme
	//
	virtual void fitness();

    // name :	respirate
    // args :	none
    // desc :	decrements remainingFood by one
    //
    void respirate();

    // name :	feed
    // args :	courses - amount of food gathered
    // desc :	increments remainingFood by courses
    //
    void feed( char courses );

	// name :	getStats
	// args :	none
	// desc :	returns pointer to soul
	//
    fleaInfo *getStats();

    // name :   getFoodLeft
    // args :   none
    // desc :   returns how much food is left.
    int getFoodLeft();

public: 
    UINT        id;             // the fleaz id
    char		nextMove;		// action the flea will take - set by think()
    fleaType    type;
    char		remainingFood;	// food remaining ( 5 through 0 )
    infoBlock   lastInfo;       // last info block given

    int         *extraInfo;     // int pointer to extra information
protected: // data members

	fleaInfo	soul;			// struct of all information we care about
    int        *data;           // Species-specific data.
    int         dataLength;     // Length of species-specific data.
    
};

#endif
