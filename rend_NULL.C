/*
   $RCSfile: rend_NULL.C,v $
   $Author: steve $
   $Date: 1999/04/23 20:17:09 $
   Rev Hist:
      $Log: rend_NULL.C,v $
      Revision 1.1  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

*/

#include "rend_NULL.h"

void rend_NULL::init( int _dimX, int _dimY ){
    dimX = _dimX;
    dimY = _dimY;
}
void rend_NULL::render( mapNode ***matrix, Population *pop, point *loc ){
}

void rend_NULL::shutdown(){
}
