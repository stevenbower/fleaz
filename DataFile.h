/*
   $RCSfile: DataFile.h,v $
   $Author: steve $
   $Date: 1999/05/05 00:49:36 $ 
   Rev Hist:
      $Log: DataFile.h,v $
      Revision 1.2  1999/05/05 00:49:36  steve
      Added CVS headers

*/

#ifndef _DATAFILE_H
#define _DATAFILE_H

#include <stdio.h>
#include <stdlib.h>
#include "Population.h"

class DataFile
{
    public:

                     DataFile  ( char * );
                    ~DataFile  ( void );

        void         write     ( Population *);
        Population  *read      ( void );

    private:

        char        *filename;
};

#endif

