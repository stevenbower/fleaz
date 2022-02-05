/*
   $RCSfile: mapNode.h,v $
   $Author: steve $
   $Date: 1999/05/04 03:38:54 $ 
   Rev Hist:
      $Log: mapNode.h,v $
      Revision 1.6  1999/05/04 03:38:54  steve
      fixed/checked memory leaks

      Revision 1.5  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.4  1999/04/20 04:06:42  steve
      fixed to work with new renderer and removed some bugs

      Revision 1.3  1999/04/14 05:53:45  steve
      Fixed CVS headers

      Revision 1.2  1999/04/14 05:49:56  steve
      Added CVS headers
*/ 

#ifndef __MAPNODE_H
#define __MAPNODE_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "fleaz.h"

#define     MAP_NONE        0 
#define     MAP_FOOD        1 

#define     DIR_NONE        0
#define     DIR_NORTH       INFO_NORTH
#define     DIR_SOUTH       INFO_SOUTH
#define     DIR_EAST        INFO_EAST
#define     DIR_WEST        INFO_WEST

class mapNode{

public: // constructor
    mapNode( int fPerc, int _xLoc, int _yLoc );
    ~mapNode();

    void setFoodList( int _foodCount, mapNode **_foodList );

    void reCalc();

public: // attributes

    UCHAR    type;    
    UCHAR    fleaCount;    
    UCHAR    xLoc;
    UCHAR    yLoc;

    int        idxNearest; // index of nearest food
    int        dirNearest; // direction of nearest food

    int        foodCount; 
    mapNode    **foodList;
    float      *distList;
};

#endif
