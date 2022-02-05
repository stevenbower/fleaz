/*
   $RCSfile: RingMaster.h,v $
   $Author: steve $
   $Date: 1999/05/12 02:50:44 $
   Rev Hist:
      $Log: RingMaster.h,v $
      Revision 1.6  1999/05/12 02:50:44  steve
      general futzing

      Revision 1.5  1999/05/04 07:02:18  steve
      change distMoved to delta value

      Revision 1.4  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.3  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.2  1999/04/20 16:53:39  steve
      fixed some syntax errors

      Revision 1.1  1999/04/20 15:57:29  psykelus
      First import into CVS

*/

#ifndef __RINGMASTER_H
#define __RINGMASTER_H

#include "fleaz.h"
#include "Flea.h"
#include "Population.h"

/* our renderers */
#include "rend_NULL.h"
#include "rend_TEXT.h"
#include "rend_SVGA.h"
#include "rend_SVGAFILE.h"

class RingMaster{

public:

    RingMaster(  Population *_pop, renderer *_cRend=0 );

    ~RingMaster();

    /* runs the  tourney, returns fit individuals and delete unfit */
    Population *run();

private:
    void thinkAct();
    
private: 	// data members
    
    int             curCycle;   // our current cycle
    int             popLeft;    // Yeah

    Population		*pop;	    // pointer to the population
    point           *popLoc;    // a vector of flea locations    
    point           *popBeg;    // a vector of flea locations    
    map             *curMap;    // map pointer
    renderer        *cRend;     // current tourney renderer
    int             iMadeRend;
};

#endif
