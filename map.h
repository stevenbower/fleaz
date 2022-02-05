/*
   $RCSfile: map.h,v $
   $Author: steve $
   $Date: 1999/04/23 20:17:09 $ 
   Rev Hist:
      $Log: map.h,v $
      Revision 1.5  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.4  1999/04/20 04:06:42  steve
      fixed to work with new renderer and removed some bugs

      Revision 1.3  1999/04/14 05:53:45  steve
      Fixed CVS headers

      Revision 1.2  1999/04/14 05:49:56  steve
      Added CVS headers
 
*/ 

#ifndef __MAP_H
#define __MAP_H

#include "fleaz.h"
#include "mapNode.h"
#include "renderer.h"
#include "Population.h"

class renderer;

class map{

public:

    /* constructors and destructors */
    map( int nFood, int _dimX, int _dimY, renderer *_rend,
         Population *_pop, point *_popLoc );
    ~map();
    

    void getInfo( infoBlock *info, int x, int y );
    int eatFood( int x, int y );

    void reCalc();
    void draw();

    /* display functions */
    void initSVGA();
    void displaySVGA();

public:
    int             dimX;
    int             dimY;
    int             foodCount;

    mapNode         ***matrix;
    mapNode         **foodList;
    Population      *pop;       // pointer to the population
    point           *popLoc;    // a vector of flea locations
    renderer        *rend;

};

#endif
