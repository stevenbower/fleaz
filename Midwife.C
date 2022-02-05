/*
   $RCSfile: Midwife.C,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $ 
   Rev Hist:
      $Log: Midwife.C,v $
      Revision 1.7  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.6  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.5  1999/05/05 01:33:50  steve
      fixed for random parent selection

      Revision 1.4  1999/05/05 00:51:14  steve
      added CVS headers and modified for testing (fixed)

*/

#include "Midwife.h"

Midwife::Midwife()
{
}

Midwife::~Midwife()
{
}

Population *Midwife::deliver( Population *pop )
{
    int         i; 
    int         sz;
    int         pivot;
    Population  *oPop = new Population();
    Flea        *tf1, *tf2, *tf3, *tf4;

    sz    = pop->size();
    pivot = pop->size() / 2; 

    for ( i = 0; i < pivot; i++ )
        delete( pop->getFlea( 0 ) );

    for ( i = 0; i < pivot; i+=2 ){
        tf1 = pop->getFlea(); 
        tf2 = pop->getFlea(); 
        tf1->crossover( tf2, &tf3, &tf4 );

        tf1->mutate();
        tf2->mutate();
        tf3->mutate();
        tf4->mutate();

        tf1->getStats()->age++;
        tf2->getStats()->age++;
        tf3->getStats()->age++;
        tf4->getStats()->age++;

        oPop->add( tf1 );
        oPop->add( tf2 );
        oPop->add( tf3 );
        oPop->add( tf4 );
    }

    delete(pop);
    return oPop;
}

