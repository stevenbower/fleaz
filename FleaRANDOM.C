/*
   $RCSfile: FleaRANDOM.C,v $
   $Author: steve $
   $Date: 1999/05/10 08:01:44 $ 
   Rev Hist:
      $Log: FleaRANDOM.C,v $
      Revision 1.4  1999/05/10 08:01:44  steve
      added more command line args and generally neatend stuff up

      Revision 1.3  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.2  1999/05/04 03:37:02  steve
      made fleaz set their type

      Revision 1.1  1999/05/03 18:19:40  steve
      Changed over to 'correct' FleaXXX naming convention

      Revision 1.1  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

*/

#include "FleaRANDOM.h"

FleaRANDOM::FleaRANDOM(){
    type = RANDOM;
}

FleaRANDOM::FleaRANDOM( int _id, fleaInfo *info, int ndata, int *_data ){
    Flea();

    type = RANDOM;
    id            = (UINT)id;
    dataLength    = 0;
    data          = 0;
    memcpy( &soul, info, sizeof( fleaInfo ));

}

void FleaRANDOM::fitness(){
    int x,y;

    soul.fitness = ( ( soul.lifespan * 7 )/2) ;

    if( !( lastInfo[ 2 ][ 2 ] ) ){
        soul.fitness = soul.fitness + ( soul.lifespan * 2 );
    }
    if( !( lastInfo[ 2 ][ 1 ] ) || !( lastInfo[ 2 ][ 3 ] ) ||
        !( lastInfo[ 1 ][ 1 ] ) || !( lastInfo[ 1 ][ 2 ] ) ||
        !( lastInfo[ 1 ][ 3 ] ) || !( lastInfo[ 3 ][ 1 ] ) ||
        !( lastInfo[ 3 ][ 2 ] ) || !( lastInfo[ 3 ][ 3 ] ) ){
        soul.fitness = soul.fitness + ( soul.lifespan );
    }
    if( !( lastInfo[ 1 ][ 0 ] ) || !( lastInfo[ 1 ][ 1 ] ) ||
        !( lastInfo[ 1 ][ 2 ] ) || !( lastInfo[ 1 ][ 3 ] ) ||
        !( lastInfo[ 1 ][ 4 ] ) || !( lastInfo[ 2 ][ 0 ] ) ||
        !( lastInfo[ 2 ][ 4 ] ) || !( lastInfo[ 3 ][ 0 ] ) ||
        !( lastInfo[ 3 ][ 4 ] ) || !( lastInfo[ 4 ][ 0 ] ) ||
        !( lastInfo[ 4 ][ 1 ] ) || !( lastInfo[ 4 ][ 2 ] ) ||
        !( lastInfo[ 4 ][ 3 ] ) || !( lastInfo[ 4 ][ 4 ] ) ){
        soul.fitness = soul.fitness + ( soul.lifespan / 2 );
    }
}

void FleaRANDOM::think( infoBlock *info ){
    nextMove = random() % 11;
}
