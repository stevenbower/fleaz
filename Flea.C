/*
   $RCSfile: Flea.C,v $
   $Author: steve $
   $Date: 1999/05/10 01:52:35 $
   Rev Hist:
      $Log: Flea.C,v $
      Revision 1.21  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.20  1999/05/05 00:50:25  steve
      made destructors virtual and emptied the base class destructor

      Revision 1.19  1999/05/04 17:16:47  psykelus
      made fitness virtual, fixed mutate

      Revision 1.18  1999/05/04 17:12:19  psykelus
      made fitness virtual

      Revision 1.17  1999/05/04 07:01:48  steve
      change distMoved to delta value

      Revision 1.16  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.15  1999/05/04 02:43:57  psykelus
      fixed putData error

      Revision 1.14  1999/05/04 02:20:13  psykelus
      added putData, initialized nextMove in def. constructor

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

// name :	Flea.C
// desc :	implementation of flea parent class
// auth :	matthew j weaver
//

#include "Flea.h"

// name :	( constructor )
//
Flea::Flea(){

	soul.age = 0;
	soul.disposition = 0;
	soul.mutations = 0;
    /* make our memory location our ID, at least its unique */

	// the first dawn, we were heading east
	nextMove = 5;

    id = (UINT)this; 
    data = 0;
}

Flea::Flea( int _id, fleaInfo *info, int ndata, int *_data ){
    id            = (UINT)_id;
    dataLength    = ndata;
    data          = (int*)calloc( dataLength + 1, sizeof( int ) );
    memcpy( data, _data, dataLength );
    memcpy( &soul, info, sizeof( fleaInfo ));
}

Flea::~Flea(){
}

// name :	respirate
//
void Flea::respirate( ){

	remainingFood--;
	if( remainingFood < 0 ){
		soul.alive = 0;
	}
}


// name :	feed
//
void Flea::feed( char courses ){

	remainingFood += courses;

	// only allowed to have a fixed amount of food
	// ( see fleaz.h )
	//if( remainingFood > MAXFOOD ){
	//	remainingFood = MAXFOOD;
	//}
}

void Flea::fitness( ){
}

// name :	getStats
//
fleaInfo *Flea::getStats(){
    return &soul;
}


// name :   think
//
void Flea::think( infoBlock *info ){
    /* this is just a stub to keep the compiler happy */
}

// name :	mutate
//
void Flea::mutate( ){
}

// name :   crossover
//
void Flea::crossover( Flea *mate, Flea **child1, Flea **child2 )
{
    **child1 = *mate;
    **child2 = *this;
}

void Flea::display(){
   
}

// name :   getData
//
int Flea::getData( int **dataPtr )
{
    *dataPtr = data;
    return dataLength;
}

// name :   getFoodLeft
//
int Flea::getFoodLeft()
{
    return remainingFood;
}

