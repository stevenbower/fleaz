/*
   $RCSfile: FleaNN.h,v $
   $Author: pete $
   $Date: 1999/05/11 21:36:06 $
   Rev Hist:
      $Log: FleaNN.h,v $
      Revision 1.9  1999/05/11 21:36:06  pete
      Made some kick-ass fleaz now.

      Revision 1.8  1999/05/10 03:09:32  steve
      fixed for file I/O

      Revision 1.7  1999/05/10 02:14:42  pete
      Added serialization routines.

      Revision 1.6  1999/05/06 18:22:25  pete
      Add virtual fitness functions.

      Revision 1.5  1999/05/05 04:01:13  pete
      More tweaking.

      Revision 1.4  1999/05/05 03:45:12  pete
      Modified some param's.

      Revision 1.3  1999/05/05 02:48:56  pete
      These fleas use an Elman Network now.
      They perform much better.

      Revision 1.2  1999/05/03 01:18:52  pete
      Added destructor and fixed constructor that was segfaulting.

      Revision 1.1  1999/05/02 23:26:01  pete
      Initial version - Neural Network fleaz.

      Revision 1.4  1999/04/27 03:37:10  pete
      Added new function "crossover" to handle reproduction instead of the
      constructor.  Reason:  Outside classes (like Midwife) need to be able to
      generically access reproduction functionality.

      Revision 1.3  1999/04/26 03:07:50  psykelus
      compiles!

      Revision 1.2  1999/04/25 16:56:11  psykelus
      initial implementation of think()

      Revision 1.1  1999/04/24 18:33:48  psykelus
      created breeding, incarnate, and random constructor



*/

#ifndef __FLEANN_H
#define __FLEANN_H

#include <cstring>
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "fleaz.h"
#include "Flea.h"
#include "ElmanNet.h"
#include "InfoRepository.h"

//#define BREED_STRUCTURE 

#define NN_INPUT_NODES      6 
#define NN_HIDDEN_NODES     9 
#define NN_OUTPUT_NODES     11

#define NN_INIT_BIAS        1
#define NN_THRESHOLD        0.0

#define SINGLE_POINT        0
#define UNIFORM             1

#define NN_MUT_RATE         100 
#define NN_ALPHA            .23

class FleaNN: public Flea 
{
    public:

        FleaNN();
        FleaNN( ElmanNet *nnet );
        FleaNN( int _id, fleaInfo *info, int ndata, int *_data );
        ~FleaNN();

        virtual void think     ( infoBlock *info );
        virtual void mutate    (); 
        virtual void crossover ( Flea *mate, Flea **child1, Flea **child2 );
        virtual void fitness   ();
        virtual int  getData   ( int **dataPtr );
        virtual void display   ();

    protected:

        float *info2input( infoBlock *info );
        float *info2target( infoBlock *info );
        double nn_random( int base );
        int prevFitness;

        int correctPredictions;

        ElmanNet *nn;
};

#endif

