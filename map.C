/*
   $RCSfile: map.C,v $
   $Author: steve $
   $Date: 1999/05/10 01:52:36 $ 
   Rev Hist:
      $Log: map.C,v $
      Revision 1.7  1999/05/10 01:52:36  steve
      added options handling and file/IO

      Revision 1.6  1999/05/02 19:53:09  steve
      fixed map to update flea locations -- should work now

      Revision 1.5  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.4  1999/04/20 04:06:42  steve
      fixed to work with new renderer and removed some bugs

      Revision 1.3  1999/04/14 05:53:45  steve
      Fixed CVS headers

      Revision 1.2  1999/04/14 05:50:13  steve
      Added CVS headers
 
*/ 

#include "map.h"

map::map( int nFood, int _dimX, int _dimY, renderer *_rend,
          Population *_pop, point *_popLoc ){
    int x, y;
    int fPerc;

    dimX = _dimX;
    dimY = _dimY;
    pop = _pop;
    popLoc = _popLoc;
    foodCount = 0;

    /* save current renderer */  
    rend = _rend;

    /* calculate the percentage of nodes that should be food */
    fPerc = (int)( ((float)(dimX*dimY))/((float)nFood) );

    /* make my X-matrix and food lists */
    if( !(matrix = (mapNode***)malloc( sizeof(int) * dimY )) )
        printf( "OUT OF MEMORY\n");

    if( !(foodList = (mapNode**)malloc( sizeof(int) * dimX * dimY )) )
        printf( "OUT OF MEMORY\n");
    
    for( y = 0; y < dimY; y++ ){

        /* make each Y list as needed */
        matrix[y] = (mapNode**)malloc( sizeof(int)*dimX );

        for( x = 0; x < dimX; x++ ){

            /* make a new map node */
            matrix[y][x] = new mapNode( fPerc, x, y );

            /* if its food, add it to the food list */
            if( matrix[y][x]->type == MAP_FOOD )
               foodList[foodCount++] = matrix[y][x];
        }
    }

    /* set up food distance references in all nodes */
    for( y = 0; y < dimY; y++ ){
        for( x = 0; x < dimX; x++ ){
            matrix[y][x]->setFoodList( foodCount, foodList );
        }
    }

}

map::~map(){
    int x, y;

    for( y=0; y < dimY; y++ ){
        for( x=0; x < dimX; x++ )
            delete( matrix[y][x] );
        free(matrix[y]);
    }
    free(matrix);
    free(foodList);

}

void map::getInfo( infoBlock *info, int x, int y ){
    int     i, j;
    int     cx, cy, ix, iy;
    int     foundSelf=0;
    Flea    *tFlea;

    for( i=0; i<5; i++ ){
        for( j=0; j<5; j++ ){
            (*info)[i][j] = -1;
        }
    }

    /* this is silly I should be doing something in the beginning to set up
       all the fleaz in a grid so I don't have to taverse the list every time
       but I don't think that I matters too much if we have 50 or so fleaz on
       the board at a time */

    for( i=0; i < pop->size(); i++ ){
        
        if( popLoc[i].x > x-3 && popLoc[i].x < x+3 &&
            popLoc[i].y > y-3 && popLoc[i].y < y+3 ){
            if( !foundSelf && popLoc[i].x == x && popLoc[i].y == y )
                foundSelf++;
            else
                (*info)[2+popLoc[i].y-y][2+popLoc[i].x-x] = INFO_FLEA;            
        }
    }

    /*
       this could be optimized by making lists of ofsets so you
       don't have to bounds check every time but I doubt its worth
       it.
    */

    iy = 0;

    for( j = y-2; j < y+3; j++ ){

        /* check bounds on y axis */
        if( j < 0 )
            cy = 0;
        else if( j >= dimY)
            cy = dimY - 1;
        else
            cy = j;

        ix = 0;
        for( i = x-2; i < x+3; i++ ){
            
            /* check bounds on x axis */
            if( i < 0 )
                cx = 0;
            else if( i >= dimX)
                cx = dimX - 1;
            else
                cx = i;

            if( matrix[cy][cx]->type == MAP_FOOD )
                (*info)[iy][ix] = INFO_FOOD;
            else {
                if( (*info)[iy][ix] < 0 )
                    (*info)[iy][ix] = matrix[cy][cx]->dirNearest;
            }
                
            ix++;
        }
        iy++;
    }

}

int map::eatFood( int x, int y ){
    /*
       no checking for dimensions is done here because it
       is assumed that users will do this.
    */
    if( matrix[y][x]->type == MAP_FOOD ){
        matrix[y][x]->type = MAP_NONE;
        return 1;
    } else 
        return 0;

}

void map::reCalc(){
    int x, y;

    for( y = 0; y < dimY; y++ ){
        for( x = 0; x < dimX; x++ ){
            matrix[y][x]->reCalc();
        }
    } 
}

void map::draw(){
    rend->render( matrix, pop, popLoc );
}
