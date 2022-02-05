/*
   $RCSfile: mapNode.C,v $
   $Author: steve $
   $Date: 1999/05/09 18:16:19 $ 
   Rev Hist:
      $Log: mapNode.C,v $
      Revision 1.9  1999/05/09 18:16:19  steve
      fixed stupid bug

      Revision 1.8  1999/05/09 17:45:48  steve
      fixed something else

      Revision 1.7  1999/05/09 17:44:10  steve
      added check for empty map

      Revision 1.6  1999/05/05 00:51:36  steve
      changed a malloc to a calloc to make code clearer

      Revision 1.5  1999/05/04 03:38:54  steve
      fixed/checked memory leaks

      Revision 1.4  1999/04/20 04:06:42  steve
      fixed to work with new renderer and removed some bugs

      Revision 1.3  1999/04/14 05:53:45  steve
      Fixed CVS headers

      Revision 1.2  1999/04/14 05:50:13  steve
      Added CVS headers
 
*/ 

#include "mapNode.h"

mapNode::mapNode( int fPerc, int _xLoc, int _yLoc ){
    xLoc = _xLoc;
    yLoc = _yLoc;
    idxNearest = -1;
    dirNearest = DIR_NONE;
    foodList = 0;
    foodCount = 0;
    distList = 0;
    fleaCount = 0;

    if( !(random() % fPerc) )
        type = MAP_FOOD;
    else
        type = MAP_NONE;
}

mapNode::~mapNode(){
    if( distList )
        free( distList );
}
void mapNode::setFoodList( int _foodCount, mapNode **_foodList ){
    int i;
    int nearest = -1;
    float nDist = 99999.0;

    foodList = _foodList;
    foodCount = _foodCount;

    if( distList )
        free( distList );

    if( !(distList = (float*)calloc( foodCount, sizeof( float ) )) )
        printf( "OUT OF MEMORY\n");
    
    for( i=0; i < foodCount; i++ ){
        distList[i] = (float) sqrt( (double)(((xLoc-foodList[i]->xLoc)*
                                              (xLoc-foodList[i]->xLoc))+
                                             ((yLoc-foodList[i]->yLoc)*
                                              (yLoc-foodList[i]->yLoc))) );
        if( distList[i] < nDist ){
            nDist = distList[i];
            nearest = i;
        }
    }
    if( nearest >= 0 ){
        if( distList[nearest] == 0.0 ){
            dirNearest = DIR_NONE;
        } else if( abs( xLoc - foodList[nearest]->xLoc) >=
                   abs( yLoc - foodList[nearest]->yLoc) ){
            if( xLoc < foodList[nearest]->xLoc )
                dirNearest = DIR_EAST;
            else
                dirNearest = DIR_WEST;
        } else {
            if( yLoc < foodList[nearest]->yLoc )
                dirNearest = DIR_SOUTH;
            else
                dirNearest = DIR_NORTH;
        }
        idxNearest = nearest;
    } else 
        dirNearest = DIR_NONE;
}

void mapNode::reCalc(){
    int i;
    float nDist = 9999.0;
    int nearest = -1;

    /* if we are food then don't do any calculations */
    if( type == MAP_FOOD ){
        dirNearest = DIR_NONE;
        return;
    }

    for( i=0; i < foodCount; i++ ){
        if( distList[i] < nDist ){ 
            if( foodList[i]->type == MAP_FOOD ){
                nDist = distList[i];
                nearest = i;
            } else
                distList[i] = 99999.0;
        }
    }

    if( nearest < 0 )
        dirNearest = DIR_NONE; 
    else if( abs( xLoc - foodList[nearest]->xLoc) >=
               abs( yLoc - foodList[nearest]->yLoc) ){
        if( xLoc < foodList[nearest]->xLoc )
            dirNearest = DIR_EAST;
        else
            dirNearest = DIR_WEST;
    } else {
        if( yLoc < foodList[nearest]->yLoc )
            dirNearest = DIR_SOUTH;
        else
            dirNearest = DIR_NORTH;
    }
    idxNearest = nearest;
}
