/*
   $RCSfile: Population.h,v $
   $Author: steve $
   $Date: 1999/05/04 06:23:31 $ 
   Rev Hist:
      $Log: Population.h,v $
      Revision 1.5  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.4  1999/05/04 03:37:30  steve
      fixed constructor to take type and create a population of that type

      Revision 1.3  1999/05/04 01:15:03  steve
      dunno

      Revision 1.2  1999/04/27 00:54:18  pete
      Added a sort function, along with a fitness comparison.

      Revision 1.1  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

*/ 
#ifndef __POPULATION_H
#define __POPULATION_H

#include "fleaz.h"
#include "Flea.h"
#include <math.h>
#include <stdio.h>

/* STL includes */
#include <vector>
#include <algorithm>

#include "fleaz.h"

class Population{

public: // Contructors

    /* create empty population */
    Population();

    /* create a population of 10 random fleaz */
    Population( int popSize, fleaType type );


    /* desc:    create a population by randomly grabbing X fleaz */
    /*          from a given pop */
    Population( int popSize, Population *pop );

    ~Population();

public: // Utility

    /* desc:    sort fleas by fitness. */
    void sortByFitness();
    void sortByType();

public: // addition

    /* desc:    add a flea to pop */
    /* returns: non-zero on error */
    int add( Flea *newFlea );

    /* desc:    empty _pop in to this population */
    void merge( Population *pop );

public: // getting fleaz from the population
   
    /* desc:    give up a random flea */ 
    /* returns: NULL on empty */
    Flea *getFlea();

    /* desc:    give up flea at idx */ 
    /* returns: NULL on empty or invalid index */
    Flea *getFlea( int idx );

public: // finding fleaz in population

    /* desc:    find base on index */
    Flea *locate( int idx );

    /* desc:    check to see if a given flea is in a population */
    int contains( Flea *aFlea );

public: // informational

    int size();

    void display();

private: //

    vector<Flea*>       *v;

};

bool fitnessLessThan( Flea *f1, Flea *f2 );
bool typeLessThan( Flea *f1, Flea *f2 );

#endif
