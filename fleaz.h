/*
   $RCSfile: fleaz.h,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $ 
   Rev Hist:
      $Log: fleaz.h,v $
      Revision 1.17  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.16  1999/05/12 02:50:44  steve
      general futzing

      Revision 1.15  1999/05/10 08:01:44  steve
      added more command line args and generally neatend stuff up

      Revision 1.14  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.13  1999/05/09 06:31:09  steve
      hrmmm...

      Revision 1.12  1999/05/05 00:55:45  steve
      added mutations to soul

      Revision 1.11  1999/05/04 07:02:18  steve
      change distMoved to delta value

      Revision 1.10  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.9  1999/05/04 03:38:12  steve
      changed the tourney len and added tourney size

      Revision 1.8  1999/04/24 18:36:14  psykelus
      dded breathfreq define for ringmaster

      Revision 1.7  1999/04/23 20:17:09  steve
      setup population handling and tournament loop

      Revision 1.6  1999/04/21 02:11:35  psykelus
      fixed a typo (love me, baby!)

      Revision 1.5  1999/04/21 02:08:45  psykelus
      added MAXFOOD define for the Fleah class

      Revision 1.4  1999/04/21 02:06:11  psykelus
      added fleaType enum and fleaInfo struct

      Revision 1.3  1999/04/14 05:53:45  steve
      Fixed CVS headers

      Revision 1.2  1999/04/14 05:49:56  steve
      Added CVS headers
 
*/ 

#ifndef __FLEAZ_H
#define __FLEAZ_H

/* internal defines -- don't change */
#define     UCHAR           unsigned char
#define     UINT            unsigned int

/* current game rules */
#define     _NFSM        100
#define     _NGP         100
#define     _NNN         100
#define     _NRANDOM     100

#define     _BOARDX      50              /* board X dimension */
#define     _BOARDY      50              /* board Y dimension */
#define     _NFOOD       25              /* around how much food is placed */

#define     _MAXFOOD     5               /* the max a flea can hold */
#define     _BREATHFREQ  15              // how many clocks tick per
                                         // respirate

#define     _TOURNSIZE   100

/* externs for command line changeable options */
#ifndef __NOEXTERN
extern UINT BOARDX;
extern UINT BOARDY;
extern UINT NFOOD;
extern UINT MAXFOOD;
extern UINT BREATHFREQ;
extern UINT TOURNSIZE;
extern char *loadFileName;
extern char *logFileName;

extern UINT NFSM;
extern UINT NGP;
extern UINT NNN;
extern UINT NRANDOM;

extern UINT R_CURRENT;
extern UINT STARTGEN;
#endif

#define     R_NULL      0       /* don't render anything */
#define     R_TEXT      1       /* render TEXT */
#define     R_SVGA      2       /* render SVGA */
#define     R_SVGAFILE  3       /* render SVGA and make file dumps */

#define     _R_CURRENT  R_TEXT          /* default renderer */

#define     INFO_FOOD   0
#define     INFO_FLEA   1
#define     INFO_EAST   2
#define     INFO_NORTH  3
#define     INFO_SOUTH  4
#define     INFO_WEST   5

#define     ACT_NOP     0
#define     ACT_MNORTH  1
#define     ACT_MSOUTH  2
#define     ACT_MEAST   3
#define     ACT_MWEST   4
#define     ACT_MNEAST  5
#define     ACT_MNWEST  6
#define     ACT_MSEAST  7
#define     ACT_MSWEST  8
#define     ACT_ATTACK  9
#define     ACT_EAT     10


typedef char    infoBlock[5][5];

#define     NSPECIES    3;
typedef enum {
    FSM, 
    GP,
    NN,
    RANDOM,
    GENERIC     /* leave at end of enum */
} fleaType;
    
typedef struct {
    int         age;
    int         disposition;

    /* tournament info - reset after each tourney */
    int         alive;
    int         lifespan;
    int         numMoves;
    int         distMoved;
    int         numEats;
    int         foodCrossed;
    int         conflicts;
    int         wins;
    UINT        fitness;
    int         mutations;
    
} fleaInfo;

// statistics for renderer
#define     STATSIZE    15

#ifndef __NOSTATS
extern fleaInfo fsmStat[STATSIZE];
extern fleaInfo gpStat[STATSIZE];
extern fleaInfo nnStat[STATSIZE];
extern int      statGood;
extern int      curGen;
extern char     fleaPic[300*107];
#endif

typedef struct {
    int     x;
    int     y;
} point;

/* UTIL FUNCTIONS */
void initLog( char *fName );
void exitError( char *str );
void lprintf( char *format, ... );

/* OPTION FUNCTIONS */
void getOptions( int argc, char **argv );

#endif
