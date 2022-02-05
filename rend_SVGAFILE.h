/*
   $RCSfile: rend_SVGAFILE.h,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $                      
   Rev Hist:
      $Log: rend_SVGAFILE.h,v $
      Revision 1.3  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.2  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.1  1999/04/20 04:03:59  steve
      First revision of rendering code

*/

#ifndef __REND_SVGAFILE_H
#define __REND_SVGAFILE_H

#include "renderer.h"
#include <vga.h>
#include <vgagl.h>
#include <stdio.h> /* FILE */

#define     FPREF       "flea_out"      /* FILE */

class rend_SVGAFILE: public renderer {

public:

    ~rend_SVGAFILE();

    virtual void init( int _dimX, int _dimY );

    virtual void render( mapNode ***matrix, Population *pop, point *loc );

    virtual void shutdown();

/* FILE */
private:
    void dumpScreen();

private:

    /* FILE */
    int     fCount;

    /* drawing spaces */
    GraphicsContext *screen;    /* handle to the screen */
    GraphicsContext *work;      /* temporary canvas */
    GraphicsContext *pieces;    /* canvas with predefined pieces on it */


};

#endif
