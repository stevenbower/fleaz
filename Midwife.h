/*
   $RCSfile: Midwife.h,v $
   $Author: steve $
   $Date: 1999/05/05 00:51:14 $ 
   Rev Hist:
      $Log: Midwife.h,v $
      Revision 1.2  1999/05/05 00:51:14  steve
      added CVS headers and modified for testing (fixed)

*/

#ifndef _MIDWIFE_H
#define _MIDWIFE_H

#include <stdio.h>
#include <stdlib.h>
#include "Population.h"

class Midwife
{
    public:
        
        Midwife();
        ~Midwife();

        Population *deliver( Population *pop );
};

#endif

