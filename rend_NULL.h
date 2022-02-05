/*
   $RCSfile: rend_NULL.h,v $
   $Author: steve $
   $Date: 1999/04/23 20:17:09 $
   Rev Hist:
      $Log: rend_NULL.h,v $
      Revision 1.1  1999/04/23 20:17:09  steve
      setup population handling and tournament loop


*/

#ifndef __REND_NULL_H
#define __REND_NULL_H

#include "renderer.h"

class rend_NULL: public renderer {

public:

    virtual void init( int _dimX, int _dimY );

    virtual void render( mapNode ***matrix, Population *pop, point *loc );

    virtual void shutdown();


};

#endif
