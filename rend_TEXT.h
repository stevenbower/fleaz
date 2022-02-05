/*
   $RCSfile: rend_TEXT.h,v $
   $Author: steve $
   $Date: 1999/05/12 02:50:44 $                      
   Rev Hist:
      $Log: rend_TEXT.h,v $
      Revision 1.3  1999/05/12 02:50:44  steve
      general futzing

      Revision 1.2  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.1  1999/04/20 04:03:59  steve
      First revision of rendering code

*/

#ifndef __REND_TEXT_H
#define __REND_TEXT_H

#include "renderer.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

class rend_TEXT: public renderer {

public:
    rend_TEXT();

    virtual void init( int _dimX, int _dimY );

    virtual void render( mapNode ***matrix, Population *pop, point *loc );

    virtual void shutdown();


};

#endif
