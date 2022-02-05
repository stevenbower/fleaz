/*
   $RCSfile: RingMaster.C,v $
   $Author: steve $
   $Date: 1999/05/12 02:50:44 $
   Rev Hist:
      $Log: RingMaster.C,v $
      Revision 1.8  1999/05/12 02:50:44  steve
      general futzing

      Revision 1.7  1999/05/10 08:01:44  steve
      added more command line args and generally neatend stuff up

      Revision 1.6  1999/05/05 05:57:30  steve
      made lastBlock part of flea and moved update into RingMaster

      Revision 1.5  1999/05/04 07:02:18  steve
      change distMoved to delta value

      Revision 1.4  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.3  1999/04/24 18:34:25  psykelus
      changed obnoxious things, fixed respirate call to happen occasionally

      Revision 1.2  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.1  1999/04/20 15:57:29  psykelus
      First import into CVS

*/

#include "RingMaster.h"

RingMaster::RingMaster( Population *_pop, renderer *_cRend=0 ){
    int         i;
    fleaInfo    *tInfo;

	pop = _pop;
    popBeg = (point*)calloc( pop->size(), sizeof(point) );
    popLoc = (point*)calloc( pop->size(), sizeof(point) );

    for( i = 0; i < pop->size(); i++ ){
        popLoc[i].x = random() % BOARDX;
        popLoc[i].y = random() % BOARDY;
        popBeg[i].x = popLoc[i].x;
        popBeg[i].y = popLoc[i].y;
        tInfo = pop->locate(i)->getStats();

        /* reset tournament specific information */
        tInfo->alive = 1;
        tInfo->lifespan = tInfo->numMoves = tInfo->numEats = 0;
        tInfo->conflicts = tInfo->wins = tInfo->fitness = 0;
        tInfo->foodCrossed = 0;
        pop->locate(i)->remainingFood = 0;
        pop->locate(i)->feed( MAXFOOD );
    }

    if( !_cRend ){
        iMadeRend = 1;
        switch( R_CURRENT ){
            case R_NULL:
                cRend = new rend_NULL(); 
                break;
            case R_TEXT:
                cRend = new rend_TEXT(); 
                break;
            case R_SVGA:
                cRend = new rend_SVGA(); 
                break;
            case R_SVGAFILE:
                cRend = new rend_SVGAFILE(); 
                break;
        }
    } else{
        iMadeRend = 0;
        cRend = _cRend;
    }

    curMap = new map( NFOOD, BOARDX, BOARDY, cRend, pop, popLoc );

    cRend->init( BOARDX, BOARDY );
}


RingMaster::~RingMaster(){

    cRend->shutdown();
    delete( curMap );
    if( iMadeRend )
        delete( cRend );
    free( popLoc );
    free( popBeg );
    
}

Population *RingMaster::run(){
    int i;

    popLeft = 9999;

    while( popLeft ){
        thinkAct(); 
        curMap->reCalc();
        curMap->draw();
        curCycle++;
    }

    for( i=0; i< pop->size(); i++ ){
        pop->locate(i)->getStats()->distMoved = (int)
                sqrt((double)( ((popLoc[i].x - popBeg[i].x)*
                                (popLoc[i].x - popBeg[i].x)) +
                                ((popLoc[i].y - popBeg[i].y)*
                                (popLoc[i].y - popBeg[i].y)) ) );
    }
    return pop;
}

void RingMaster::thinkAct(){
    int         s = pop->size();
    int         i;
    Flea        *cFlea;
    int         dx, dy;
    infoBlock   tInfo;
    int         respNow = 0;

    popLeft = 0;
    for( i=0; i < s; i++ ){
        cFlea = pop->locate( i );
        if( cFlea->getStats()->alive ){
            curMap->getInfo( &tInfo, popLoc[i].x, popLoc[i].y );
            cFlea->think( &tInfo );
            memcpy( cFlea->lastInfo, tInfo, sizeof( infoBlock ) );
            cFlea->getStats()->lifespan++;
            popLeft++;
        }
    }
    respNow = !(curCycle % BREATHFREQ );

    for( i=0; i < s; i++ ){
        cFlea = pop->locate( i );

        /* turn this on when we want to actually run things */ 
		if( respNow ){
			cFlea->respirate();
		}

        if( cFlea->getStats()->alive ){
            if( curMap->matrix[popLoc[i].y][popLoc[i].x]->type == MAP_FOOD )
                pop->locate(i)->getStats()->foodCrossed++;
            dx = dy = 0;
            switch( cFlea->nextMove ){
                case ACT_NOP:
                    break;
                case ACT_MNORTH:
                    dy--;
                    break;
                case ACT_MSOUTH:
                    dy++;
                    break;
                case ACT_MEAST:
                    dx++;
                    break;
                case ACT_MWEST:
                    dx--;
                    break;
                case ACT_MNEAST:
                    dx++;
                    dy--;
                    break;
                case ACT_MNWEST:
                    dx--;
                    dy--;
                    break;
                case ACT_MSEAST:
                    dx++;
                    dy++;
                    break;
                case ACT_MSWEST:
                    dx--;
                    dy++;
                    break;
                case ACT_ATTACK:
                    break;
                case ACT_EAT:
                    if( curMap->eatFood( popLoc[i].x, popLoc[i].y) ){
                        cFlea->feed( MAXFOOD );
                        cFlea->getStats()->numEats++;
                    }
                    break;
            }

            if( !dx && !dy ){
                /* NOP */
            } else if( dx + popLoc[i].x >= 0 &&
                       dx + popLoc[i].x < curMap->dimX &&
                       dy + popLoc[i].y >= 0 &&
                       dy + popLoc[i].y < curMap->dimY ){
                popLoc[i].x += dx;
                popLoc[i].y += dy;
                cFlea->getStats()->numMoves++;

            } else if( dx + popLoc[i].x >= 0 &&
                       dx + popLoc[i].x < curMap->dimX ){

                popLoc[i].x += dx;
                cFlea->getStats()->numMoves++;

            } else if( dy + popLoc[i].y >= 0 && 
                       dy + popLoc[i].y < curMap->dimY ){

                popLoc[i].y += dy;
                cFlea->getStats()->numMoves++;
            }

        }
    }

}
