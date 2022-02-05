/*
   $RCSfile: main.C,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $ 
   Rev Hist:
      $Log: main.C,v $
      Revision 1.14  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.13  1999/05/12 03:24:35  steve
      fixed curgen

      Revision 1.12  1999/05/12 02:50:44  steve
      general futzing

      Revision 1.11  1999/05/10 08:01:44  steve
      added more command line args and generally neatend stuff up

      Revision 1.10  1999/05/10 04:22:01  steve
      file io stuff

      Revision 1.9  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.8  1999/05/09 06:29:46  steve
      added signal handler for dumping fleaz to files

      Revision 1.7  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.6  1999/05/04 03:39:27  steve
      added main program loop without breeding

      Revision 1.5  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.4  1999/04/20 04:07:21  steve
      main gobbles food randomly to test map functions

      Revision 1.3  1999/04/14 05:53:45  steve
      Fixed CVS headers

      Revision 1.2  1999/04/14 05:50:13  steve
      Added CVS headers
 
*/ 

#define __NOSTATS
#include "fleaz.h"
#undef  __NOSTATS

#include "renderer.h"
#include "rend_TEXT.h"
#include "rend_SVGA.h"
#include "rend_SVGAFILE.h"
#include "rend_NULL.h"
#include "Population.h"
#include "RingMaster.h"
#include "DataFile.h"
#include "Midwife.h"
#include "Flea.h"

#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static int  writeNow = 0;

// stat stuff
fleaInfo        fsmStat[STATSIZE];
fleaInfo        gpStat[STATSIZE];
fleaInfo        nnStat[STATSIZE];
int             curGen;
int             statGood = 0;
char            fleaPic[300*107];

/* signal handler code */
void sigHandler( int signum ){
    if( signum == SIGHUP ){
        writeNow = 1;
        lprintf( "caught signal - preparing to write population to file\n" );
    }
}

int main( int argc, char **argv ){
    Population      *tPop;
    Population      *inPop, *outPop;
    RingMaster      *tour1;
    Midwife         *nellie = new Midwife();
    renderer        *curRend;
    int             i;
    char            fName[32];
    Flea            *tFlea;
    fleaInfo        *tInfo;

    /* load picture */
    {
        FILE    *pic;
        char    ch;

        pic = fopen( "fleaz.img", "r" );
   
        i = 0; 
        while( (ch = fgetc( pic )) != EOF ){
            if( ch == 8 ){
                fleaPic[i] = C_P0;
            } else {
                fleaPic[i] = (char)(ch + C_P1);
            }
            i++;
        }

        fclose( pic );

    }
    /* get command line args */
    getOptions( argc, argv );

    /* install the signal handler */
    signal( SIGHUP, sigHandler );

    /* init the log file */
    if( logFileName )
        initLog( logFileName );
    else
        initLog( "fleaz.log" );

    lprintf( "fleaz started\n" );

    /* prime the random generator with the current time */
    srandom( time(0) );

    /* select appropriate renderer */
    switch( R_CURRENT ){
        case R_NULL:
            curRend = new rend_NULL();
            break;
        case R_TEXT:
            curRend = new rend_TEXT();
            break;
        case R_SVGA:
            curRend = new rend_SVGA();
            break;
        case R_SVGAFILE:
            curRend = new rend_SVGAFILE();
            break;
    }

    /* adjust start generation */
    curGen = STARTGEN;

    outPop = new Population();

    /* check to see if we want to load from a file */
    if( loadFileName ){
        DataFile    *loadFile = new DataFile( loadFileName );

        inPop = loadFile->read();

        /* clean up for the arg handler */
        free( loadFileName );
        loadFileName = 0;
        delete(loadFile);
    } else {
        inPop = new Population();

        /* make random populations */
        if( NFSM ){
            tPop = new Population( NFSM, FSM );
            inPop->merge( tPop );
            delete(tPop);
        }
    
        if( NGP ){
            tPop = new Population( NGP, GP );
            inPop->merge( tPop );
            delete(tPop);
        }

        if( NNN ){
            tPop = new Population( NNN, NN );
            inPop->merge( tPop );
            delete(tPop);
        }

        if( NRANDOM ){
            tPop = new Population( NRANDOM, RANDOM );
            inPop->merge( tPop );
            delete(tPop);
        }
    }

    while( 1 ){
        while( inPop->size() ){ 
            /* select a random population */
            tPop = new Population( TOURNSIZE, inPop );

            tour1 = new RingMaster( tPop, curRend );
            tPop = tour1->run();

            outPop->merge( tPop );
            delete( tour1 );
            delete( tPop );
        }


        /* get fitness values */
        for( i=0; i < outPop->size(); i++ )
            outPop->locate(i)->fitness();

        outPop->sortByFitness(); 

        for( i=0; i < outPop->size(); i++ ){
            tFlea = outPop->locate(i);
            tInfo = tFlea->getStats();
            lprintf( "ID[%u] type: %d fit: %-4d eats: %-2d ",
                     tFlea->id,
                     tFlea->type,
                     tInfo->fitness,
                     tInfo->numEats );
            lprintf( "move(d/n): %3d/%-3d life: %-3d age: %-4d\n", 
                     tInfo->distMoved,
                     tInfo->numMoves,
                     tInfo->lifespan,
                     tInfo->age );
        }

        lprintf( "Generation [%d]\n", curGen );

        if( writeNow ){
            lprintf( "writing population to 'pop_%03d.flz' [", curGen );
            sprintf( fName, "pop_%03d.flz", curGen );

            DataFile    *saveFile = new DataFile( fName );
            saveFile->write( outPop );
            delete( saveFile );

            lprintf( "COMPLETE]\n" );

            writeNow = 0;
        }

        curGen++;
        /* Breed */
        outPop->sortByType(); 


/* build and breed FSM population -------------------------------------------*/
        tPop = new Population();
        for( i=0; i < NFSM; i++ )
            tPop->add( outPop->getFlea(0));    
        tPop->sortByFitness();

        // get stats
        if( NFSM ){
        for( i=0; i<STATSIZE; i++ )
            memcpy( &fsmStat[i], tPop->locate(tPop->size()-i-1)->getStats(), 
                    sizeof( fleaInfo ));
        }

        tPop = nellie->deliver( tPop );
        inPop->merge( tPop );
        delete( tPop );

/* build and breed GP population --------------------------------------------*/
        tPop = new Population();
        for( i=NFSM; i < NGP+NFSM; i++ )
            tPop->add( outPop->getFlea(0));    
        tPop->sortByFitness();

        // get stats
        if( NGP ){
        for( i=0; i<STATSIZE; i++ )
            memcpy( &gpStat[i], tPop->locate(tPop->size()-i-1)->getStats(), 
                    sizeof( fleaInfo ));
        }

        tPop = nellie->deliver( tPop );
        inPop->merge( tPop );
        delete( tPop );

/* build and breed NN population --------------------------------------------*/
        tPop = new Population();
        for( i=NGP+NFSM; i < NNN+NFSM+NGP; i++ )
            tPop->add( outPop->getFlea(0));    
        tPop->sortByFitness();

        // get stats
        if( NNN ){
        for( i=0; i<STATSIZE; i++ )
            memcpy( &nnStat[i], tPop->locate(tPop->size()-i-1)->getStats(), 
                    sizeof( fleaInfo ));
        }

        tPop = nellie->deliver( tPop );
        inPop->merge( tPop );
        delete( tPop );

/* build and breed RANDOM population ----------------------------------------*/
        for( i=NGP+NFSM+NNN; i < NRANDOM+NNN+NFSM+NGP; i++ )
            inPop->add( outPop->getFlea(0));    

        statGood = 1;
       
    }

    /* send her home */
    delete( curRend );
    delete( nellie );
}
