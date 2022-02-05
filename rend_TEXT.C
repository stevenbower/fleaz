/*
   $RCSfile: rend_TEXT.C,v $
   $Author: steve $
   $Date: 1999/05/12 02:50:44 $                      
   Rev Hist:
      $Log: rend_TEXT.C,v $
      Revision 1.5  1999/05/12 02:50:44  steve
      general futzing

      Revision 1.4  1999/05/04 03:38:54  steve
      fixed/checked memory leaks

      Revision 1.3  1999/05/04 01:14:21  steve
      fixed for fleaz display and added delay after render

      Revision 1.2  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.1  1999/04/20 04:03:59  steve
      First revision of rendering code

*/

#include "rend_TEXT.h"

rend_TEXT::rend_TEXT(){
    write( 0, "\033[2J", 4 );
    write( 0, "\033[1;1H", 6 );
}

void rend_TEXT::init( int _dimX, int _dimY ){

    dimX = _dimX;
    dimY = _dimY;

}

void rend_TEXT::render( mapNode ***matrix, Population *pop, point *loc ){
    int x, y;
    char    tStr[32];

    /* re-locate to topleft */
    write( 0, "\033[0;0H", 6 );

    for( y = 0; y < dimY; y++ ){
        for( x = 0; x < dimX; x++ ){
            if( matrix[y][x]->type == MAP_FOOD ){
                printf( "O" );
            } else if( matrix[y][x]->dirNearest == DIR_NORTH ){
                printf( "^" );
            } else if( matrix[y][x]->dirNearest == DIR_SOUTH ){
                printf( "v" );
            } else if( matrix[y][x]->dirNearest == DIR_EAST ){
                printf( ">" );
            } else if( matrix[y][x]->dirNearest == DIR_WEST ){
                printf( "<" );
            } else
                printf( "." );
        }
        printf( "\n" );
    }

    for( x = 0; x < pop->size(); x++ ){
        if( pop->locate(x)->getStats()->alive )
            printf( "\033[%d;%dH#", loc[x].y+1, loc[x].x+1 );
    }
    fflush( stdout );

    //usleep( 100 );
}

void rend_TEXT::shutdown(){
    printf( "\033[%d;%dH", dimY+1, 0 );
    fflush( stdout );
}
