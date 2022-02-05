/*
   $RCSfile: rend_SVGAFILE.C,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $                      
   Rev Hist:
      $Log: rend_SVGAFILE.C,v $
      Revision 1.4  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.3  1999/05/04 01:14:21  steve
      fixed for fleaz display and added delay after render

      Revision 1.2  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.1  1999/04/20 04:03:59  steve
      First revision of rendering code

*/

#include "rend_SVGAFILE.h"
#include "rend_SVGA.h"

void rend_SVGAFILE::init( int _dimX, int _dimY){
    int x, y;

    dimX = _dimX;
    dimY = _dimY;

    vga_init();
    vga_setmode(MODE);
    gl_setcontextvga(MODE);

    screen = gl_allocatecontext();
    work = gl_allocatecontext();
    pieces = gl_allocatecontext();
    gl_getcontext(screen);

    gl_setcontextvgavirtual(MODE);
    gl_getcontext(work);
    gl_setcontextvgavirtual(MODE);
    gl_getcontext(pieces);

    gl_setpalettecolor( C_LINES, 0x33, 0x33, 0x33 );
    gl_setpalettecolor( C_FOOD, 0x66, 0x00, 0x99 );

    for( x=0; x < G_LEN; x++ ){
        gl_setpalettecolor( G_START+x, 0xFF, ((0xFF/G_LEN)*x) , 0x00 );
    }

    for( x=G_START+G_LEN; x < G_START+100-G_LEN; x++ ){
        gl_setpalettecolor( x, 0xFF, 0xFF , 0x00 );
    }


    for( x=0; x <= PSIZE * dimX; x+=PSIZE){
        gl_line( x, 0, x, dimY * PSIZE, C_LINES);
    }
    for( y=0; y <= PSIZE * dimY; y+=PSIZE){
        gl_line( 0, y, dimY * PSIZE, y, C_LINES );

    }

    gl_setcontext(work);

    /* FILE */

    fCount = 0;
}

void rend_SVGAFILE::render( mapNode ***matrix, Population *pop, point *loc ){
    int x, y, dist;

    gl_copyboxfromcontext( pieces, 0, 0, dimX * PSIZE+1,
                           dimY * PSIZE+1, 0, 0 );

    for( y = 0; y < dimY; y++ ){
        for( x = 0; x < dimX; x++ ){

            if( matrix[y][x]->type == MAP_FOOD ){
                gl_fillbox( 1+(x*PSIZE), 1+(y*PSIZE),
                               PSIZE-1, PSIZE-1, C_FOOD);
            } else if( matrix[y][x]->dirNearest == DIR_NORTH ){

                dist=(int)(matrix[y][x]->distList[matrix[y][x]->idxNearest]);
                gl_line( 3+(x*PSIZE), 1+(y*PSIZE),
                         3+(x*PSIZE), 5+(y*PSIZE),
                         G_START+dist );
                gl_line( 2+(x*PSIZE), 3+(y*PSIZE),
                         2+(x*PSIZE), 5+(y*PSIZE),
                         G_START+dist );
                gl_line( 4+(x*PSIZE), 3+(y*PSIZE),
                         4+(x*PSIZE), 5+(y*PSIZE),
                         G_START+dist );
                gl_setpixel( 1+(x*PSIZE), 5+(y*PSIZE), G_START+dist );
                gl_setpixel( 5+(x*PSIZE), 5+(y*PSIZE), G_START+dist );

            } else if( matrix[y][x]->dirNearest == DIR_SOUTH ){

                dist=(int)(matrix[y][x]->distList[matrix[y][x]->idxNearest]);
                gl_line( 3+(x*PSIZE), 1+(y*PSIZE),
                         3+(x*PSIZE), 5+(y*PSIZE),
                         G_START+dist );
                gl_line( 2+(x*PSIZE), 1+(y*PSIZE),
                         2+(x*PSIZE), 3+(y*PSIZE),
                         G_START+dist );
                gl_line( 4+(x*PSIZE), 1+(y*PSIZE),
                         4+(x*PSIZE), 3+(y*PSIZE),
                         G_START+dist );
                gl_setpixel( 1+(x*PSIZE), 1+(y*PSIZE), G_START+dist );
                gl_setpixel( 5+(x*PSIZE), 1+(y*PSIZE), G_START+dist );

            } else if( matrix[y][x]->dirNearest == DIR_EAST ){

                dist=(int)(matrix[y][x]->distList[matrix[y][x]->idxNearest]);
                gl_line( 1+(x*PSIZE), 3+(y*PSIZE),
                         5+(x*PSIZE), 3+(y*PSIZE),
                         G_START+dist );
                gl_line( 1+(x*PSIZE), 2+(y*PSIZE),
                         3+(x*PSIZE), 2+(y*PSIZE),
                         G_START+dist );
                gl_line( 1+(x*PSIZE), 4+(y*PSIZE),
                         3+(x*PSIZE), 4+(y*PSIZE),
                         G_START+dist );
                gl_setpixel( 1+(x*PSIZE), 1+(y*PSIZE), G_START+dist );
                gl_setpixel( 1+(x*PSIZE), 5+(y*PSIZE), G_START+dist );

            } else if( matrix[y][x]->dirNearest == DIR_WEST ){

                dist=(int)(matrix[y][x]->distList[matrix[y][x]->idxNearest]);
                gl_line( 1+(x*PSIZE), 3+(y*PSIZE),
                         5+(x*PSIZE), 3+(y*PSIZE),
                         G_START+dist );
                gl_line( 3+(x*PSIZE), 2+(y*PSIZE),
                         5+(x*PSIZE), 2+(y*PSIZE),
                         G_START+dist );
                gl_line( 3+(x*PSIZE), 4+(y*PSIZE),
                         5+(x*PSIZE), 4+(y*PSIZE),
                         G_START+dist );
                gl_setpixel( 5+(x*PSIZE), 1+(y*PSIZE), G_START+dist );
                gl_setpixel( 5+(x*PSIZE), 5+(y*PSIZE), G_START+dist );

            } else
                printf( "0" );
        }
    }

    for( x = 0; x < pop->size(); x++ ){
        if( pop->locate(x)->getStats()->alive )
            gl_fillbox( PSIZE*loc[x].x+1, PSIZE*loc[x].y+1,PSIZE-1,
                        PSIZE-1, C_FLEA );
    }

    gl_copyscreen(screen);

    /* FILE */

    dumpScreen();
}

void rend_SVGAFILE::shutdown(){

}

void rend_SVGAFILE::dumpScreen(){
    int     x, y;
    FILE    *fil;
    char    tStr[20];
    int     r, g, b;

    sprintf( tStr, "%s%03d.dat", FPREF, fCount );
    fil = fopen( tStr, "w" );
    
    for( y=0; y<(dimY*PSIZE); y++ ){ 
        for( x=0; x<(dimX*PSIZE); x++ ){ 
            gl_getpalettecolor( work->vbuf[x+(y*640)], &r, &g, &b );
            fputc( r, fil );
            fputc( g, fil );
            fputc( b, fil );
        }
    }

    fclose( fil );
    fCount++;
}
