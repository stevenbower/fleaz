/*
   $RCSfile: rend_SVGA.h,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $                      
   Rev Hist:
      $Log: rend_SVGA.h,v $
      Revision 1.3  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.2  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.1  1999/04/20 04:03:59  steve
      First revision of rendering code

*/

#ifndef __REND_SVGA_H
#define __REND_SVGA_H

#include "renderer.h"
#include <vga.h>
#include <vgagl.h>

#define     MODE        G640x480x256
#define     XRES        640
#define     YRES        480

#define     PSIZE       6               /* size of each square */

#define     C_LINES     1               /* line color index */
#define     C_FOOD      2               /* food color index */
#define     C_FLEA      3               /* flea color index */
#define     C_FLEAFSM   3               /* flea color index */
#define     C_FLEAGP    4               /* flea color index */
#define     C_FLEANN    5               /* flea color index */
#define     C_FLEARAND  6               /* flea color index */

#define     C_TEXT      7

#define     C_P0        0               /* */
#define     C_P1        9               /* 0 yellow */
#define     C_P2        10              /* 1 lighter yellow */
#define     C_P3        11              /* 2 yellow-brown */
#define     C_P4        12              /* 3 white */
#define     C_P5        13              /* 4 white */
#define     C_P6        14              /* 5 white */
#define     C_P7        15              /* 6 medium grey */
#define     C_P8        16              /* 7 black */

#define     G_START     20              /* gradient start index */
#define     G_LEN       25              /* gradient length */

class rend_SVGA: public renderer {

public:

    rend_SVGA();

    ~rend_SVGA();

    virtual void init( int _dimX, int _dimY );

    virtual void render( mapNode ***matrix, Population *pop, point *loc );

    virtual void shutdown();

private:

    /* drawing spaces */
    GraphicsContext *screen;    /* handle to the screen */
    GraphicsContext *work;      /* temporary canvas */
    GraphicsContext *pieces;    /* canvas with predefined pieces on it */
    
    char    fsmFont[256*8*8*1];
    char    gpFont[256*8*8*1];
    char    nnFont[256*8*8*1];
    char    baseFont[256*8*8*1];

};

#endif
