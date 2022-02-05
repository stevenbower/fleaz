/*
   $RCSfile: utils.C,v $
   $Author: steve $
   $Date: 1999/05/17 16:22:13 $
   Rev Hist:
      $Log: utils.C,v $
      Revision 1.3  1999/05/17 16:22:13  steve
      added CVS headers

*/

#include "fleaz.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h> 

static FILE *logFile;

void initLog( char *fName ){
    logFile = fopen( fName, "w" );
}

void exitError( char *str ){
    fprintf( stderr, "%s", str );
    exit(1);
}

void lprintf( char *format, ... ){
    va_list args;

    va_start( args, format );
    vfprintf( logFile, format, args );
    va_end(args);
    fflush( logFile );
}
