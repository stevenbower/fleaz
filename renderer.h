/*
   $RCSfile: renderer.h,v $
   $Author: steve $
   $Date: 1999/04/23 20:17:09 $                      
   Rev Hist:
      $Log: renderer.h,v $
      Revision 1.2  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.1  1999/04/20 04:03:59  steve
      First revision of rendering code

*/

#ifndef __RENDERER_H
#define __RENDERER_H

#include "fleaz.h"
#include "map.h"
#include "Population.h"

class renderer{

public:
    virtual void init( int _dimX, int _dimY );

    virtual void render( mapNode ***matrix, Population *pop, point *loc );

    virtual void shutdown();

protected:
    int     dimX, dimY;

};

#endif
