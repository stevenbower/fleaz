/*
   $RCSfile: Population.C,v $
   $Author: steve $
   $Date: 1999/05/10 08:01:44 $ 
   Rev Hist:
      $Log: Population.C,v $
      Revision 1.7  1999/05/10 08:01:44  steve
      added more command line args and generally neatend stuff up

      Revision 1.6  1999/05/10 02:56:41  steve
      fixed

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

#include "Population.h"

/* our different flea types */
#include "FleaFSM.h"
#include "FleaGP.h"
#include "FleaNN.h"
#include "FleaRANDOM.h"

Population::Population(){
    v = new vector<Flea*>();
}

Population::Population( int popSize, fleaType type ){
    Flea        *tmpFlea;

    v = new vector<Flea*>();

    while( popSize ){
        switch( type ){
            case FSM:
                tmpFlea = new FleaFSM();
                break;
            case GP:
                tmpFlea = new FleaGP();
                break;
            case NN:
                tmpFlea = new FleaNN();
                break;
            case RANDOM:
                tmpFlea = new FleaRANDOM();
                break;
        } 
        v->insert( v->end(), tmpFlea );
        popSize--;
    }
}

Population::Population( int popSize, Population *pop ){
    Flea    *tmpFlea;

    v = new vector<Flea*>();

    while( popSize ){
        if( tmpFlea = pop->getFlea() ){
            v->insert( v->end(), tmpFlea );
            popSize--;
        } else
            popSize = 0;
    }
}

Population::~Population(){
    int     i = v->size();

    vector<Flea*>::iterator current = v->begin();

    while( i ){
        delete( *current );
        current++;
        i--;
    }
    delete( v );
}

int Population::add( Flea *newFlea ){
    if( newFlea ){
        v->insert( v->end(), newFlea );
        return 0;
    } else
        return -1;
}

void Population::merge( Population *pop ){
    int     i = pop->size();
   
    while( i ){
        v->insert( v->end(), pop->getFlea() );
        i--;
    } 
}

Flea *Population::getFlea(){
    if( v->size() )
        return getFlea( random() % v->size());
    else
        return NULL;
}

Flea *Population::getFlea( int idx ){
    Flea    *tmpFlea;

    if( idx >= v->size() )
        return NULL; 

    vector<Flea*>::iterator current = v->begin();

    /* get the node */
    tmpFlea = *(current += idx);
    
    /* remove from vector */
    v->erase( current ); 

    return tmpFlea;
}

void Population::sortByFitness()
{
    sort( v->begin(), v->end(), fitnessLessThan );
}

void Population::sortByType()
{
    sort( v->begin(), v->end(), typeLessThan );
}

Flea *Population::locate( int idx ){
    if( idx >= v->size() )
        return NULL; 

    vector<Flea*>::iterator current = v->begin();

    return *(current += idx);
}

int Population::contains( Flea *aFlea ){
    return binary_search( v->begin(), v->end(), aFlea );
}

int Population::size(){
    return v->size();
}

void Population::display(){
    int     i = 0;

    vector<Flea*>::iterator current = v->begin();

    printf( "v---------------------- population -----------------------v\n" );
    while( i < v->size() ){
        printf( "%3d: id[%u]  ", i, (*current)->id );
        i++;
        current++;
        if( !(i % 3 ) && i )
            printf( "\n" );
    }
    if( (i % 3 ) )
        printf( "\n" );
    printf( "^---------------------- population -----------------------^\n" );
    

}

bool fitnessLessThan( Flea *f1, Flea *f2 )
{
    return f1->getStats()->fitness < f2->getStats()->fitness;
}

bool typeLessThan( Flea *f1, Flea *f2 )
{
    return f1->type < f2->type;
}

