/*
   $RCSfile: FleaRANDOM.h,v $
   $Author: steve $
   $Date: 1999/05/10 08:01:44 $ 
   Rev Hist:
      $Log: FleaRANDOM.h,v $
      Revision 1.3  1999/05/10 08:01:44  steve
      added more command line args and generally neatend stuff up

      Revision 1.2  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.1  1999/05/03 18:19:40  steve
      Changed over to 'correct' FleaXXX naming convention

      Revision 1.1  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

*/

#ifndef __FLEA_RANDOM_H
#define __FLEA_RANDOM_H

#include <stdlib.h>
#include "Flea.h"

class FleaRANDOM: public Flea{

public: // yes 

    FleaRANDOM();
    FleaRANDOM( int id, fleaInfo *info, int ndata, int *_data );

    virtual void think( infoBlock *info );
    virtual void fitness();

};

#endif
