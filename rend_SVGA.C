/*
   $RCSfile: rend_SVGA.C,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $                      
   Rev Hist:
      $Log: rend_SVGA.C,v $
      Revision 1.5  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.4  1999/05/12 02:50:44  steve
      general futzing

      Revision 1.3  1999/05/04 01:14:21  steve
      fixed for fleaz display and added delay after render

      Revision 1.2  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.1  1999/04/20 04:03:59  steve
      First revision of rendering code

*/

#include "rend_SVGA.h"


rend_SVGA::rend_SVGA(){
    int x, y;


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


    gl_setpalettecolor( C_TEXT, 0xCC, 0xCC, 0xCC );
    gl_setpalettecolor( C_LINES, 0x33, 0x33, 0x33 );
    gl_setpalettecolor( C_FOOD, 0x99, 0x99, 0x99 );
    gl_setpalettecolor( C_FLEAFSM, 0x40, 0x00, 0xE0 );
    gl_setpalettecolor( C_FLEAGP, 0xCC, 0x66, 0xCC );
    gl_setpalettecolor( C_FLEANN, 0x00, 0x99, 0x66 );
    gl_setpalettecolor( C_FLEARAND, 0x33, 0x66, 0x99 );

    gl_setpalettecolor( C_P1, 0x00, 0x33, 0x66 );
    gl_setpalettecolor( C_P2, 0x33, 0x99, 0xCC );
    gl_setpalettecolor( C_P3, 195, 148, 51 );
    gl_setpalettecolor( C_P4, 255, 255, 255 );
    gl_setpalettecolor( C_P5, 245, 245, 245 );
    gl_setpalettecolor( C_P6, 239, 239, 239 );
    gl_setpalettecolor( C_P7, 179, 179, 179 );
    gl_setpalettecolor( C_P8, 76, 76, 76 );

    for( x=0; x < G_LEN; x++ ){
        gl_setpalettecolor( G_START+x, 0x66-((0x66/G_LEN)*x),
                                       0x66-((0x66/G_LEN)*x),
                                       0x66-((0x66/G_LEN)*x) );
    }

    for( x=G_START+G_LEN; x < G_START+100-G_LEN; x++ ){
        gl_setpalettecolor( x, 0x00, 0x00 , 0x00 );
    }

    // setup font
    gl_expandfont( 8, 8, C_FLEAFSM, gl_font8x8, fsmFont );
    gl_expandfont( 8, 8, C_FLEAGP, gl_font8x8, gpFont );
    gl_expandfont( 8, 8, C_FLEANN, gl_font8x8, nnFont );
    gl_expandfont( 8, 8, C_TEXT, gl_font8x8, baseFont );
    gl_setwritemode( WRITEMODE_MASKED );

    gl_setcontext(work);
}

void rend_SVGA::init( int _dimX, int _dimY){
    int x, y;

    dimX = _dimX;
    dimY = _dimY;

    gl_setcontext(pieces);
    gl_fillbox( 0, 0, XRES, YRES, 0 );

    gl_putbox( 0, 350, 300, 107, fleaPic );

    for( x=0; x <= PSIZE * dimX; x+=PSIZE){
        gl_line( x, 0, x, dimY * PSIZE, C_LINES);
    }
    for( y=0; y <= PSIZE * dimY; y+=PSIZE){
        gl_line( 0, y, dimY * PSIZE, y, C_LINES );

    }

    gl_setcontext(work);

}

void rend_SVGA::render( mapNode ***matrix, Population *pop, point *loc ){
    int x, y, dist;

    gl_copyboxfromcontext( pieces, 0, 0, XRES,
                           YRES, 0, 0 );

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

            }
	     // else
             //   printf( "0" );
        }

        for( x = 0; x < pop->size(); x++ ){
            if( pop->locate(x)->getStats()->alive )
                switch( pop->locate(x)->type ){
                    case FSM:
                        gl_fillbox( PSIZE*loc[x].x+1, 
                                    PSIZE*loc[x].y+1,
                                    PSIZE-1, PSIZE-1, C_FLEAFSM );
                        break;
                    case GP:
                        gl_fillbox( PSIZE*loc[x].x+1, 
                                    PSIZE*loc[x].y+1,
                                    PSIZE-1, PSIZE-1, C_FLEAGP );
                        break;
                    case NN:
                        gl_fillbox( PSIZE*loc[x].x+1, 
                                    PSIZE*loc[x].y+1,
                                    PSIZE-1, PSIZE-1, C_FLEANN );
                        break;
                    case RANDOM:
                        gl_fillbox( PSIZE*loc[x].x+1, 
                                    PSIZE*loc[x].y+1,
                                    PSIZE-1, PSIZE-1, C_FLEARAND );
                        break;
                }
        }

    }

    int i;

    x = 0; 
   
    if( NFSM ){ 
    for( i=0; i < STATSIZE; i++ ){
        gl_setfont( 8, 8, fsmFont );
        gl_printf( 320, x, "FSM: " );
        gl_setfont( 8, 8, baseFont );
        if( statGood ){
            gl_printf( 360, x, "eat=%-2d dis=%-2d age=%-2d fit=%d",
                       fsmStat[i].numEats, 
                       fsmStat[i].distMoved,
                       fsmStat[i].age, 
                       fsmStat[i].fitness );
        } else {
            gl_printf( 360, x, "(no data)" );
        }
        x+=9;
    }
    x+=9;
    }
    if( NGP ){ 
    for( i=0; i < STATSIZE; i++ ){
        gl_setfont( 8, 8, gpFont );
        gl_printf( 320, x, "GP: " );
        gl_setfont( 8, 8, baseFont );
        if( statGood ){
            gl_printf( 360, x, "eat=%-2d dis=%-2d age=%-2d fit=%d",
                       gpStat[i].numEats, 
                       gpStat[i].distMoved,
                       gpStat[i].age, 
                       gpStat[i].fitness );
        } else {
            gl_printf( 360, x, "(no data)" );
        }
        x+=9;
    }
    x+=9;
    }
    if( NNN ){ 
    gl_setfont( 8, 8, nnFont );
    for( i=0; i < STATSIZE; i++ ){
        gl_setfont( 8, 8, nnFont );
        gl_printf( 320, x, "NN: " );
        gl_setfont( 8, 8, baseFont );
        if( statGood ){
            gl_printf( 360, x, "eat=%-2d dis=%-2d age=%-2d fit=%d",
                       nnStat[i].numEats, 
                       nnStat[i].distMoved,
                       nnStat[i].age, 
                       nnStat[i].fitness );
        } else {
            gl_printf( 360, x, "(no data)" );
        }
        x+=9;
    }
    x+=9;
    }
    gl_printf( 320, x, "generation [%d]", curGen );
    gl_setfont( 8, 8, baseFont );

    gl_copyscreen(screen);

}

void rend_SVGA::shutdown(){

}
