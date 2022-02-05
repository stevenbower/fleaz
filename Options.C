/*
   $RCSfile: Options.C,v $
   $Author: steve $
   $Date: 1999/05/17 16:22:13 $
   Rev Hist:
      $Log: Options.C,v $
      Revision 1.4  1999/05/17 16:22:13  steve
      added CVS headers

*/

#define __NOEXTERN
#include "fleaz.h"
#undef __NOEXTERN

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UINT BOARDX = _BOARDX;
UINT BOARDY = _BOARDY;
UINT NFOOD = _NFOOD;
UINT MAXFOOD = _MAXFOOD;
UINT BREATHFREQ = _BREATHFREQ;
UINT TOURNSIZE = _TOURNSIZE;
char *loadFileName = 0;
char *logFileName = 0;

UINT NFSM = 0;
UINT NGP = 0;
UINT NNN = 0;
UINT NRANDOM = 0;

UINT R_CURRENT = _R_CURRENT;
UINT STARTGEN = 0;

void showUsage(){
    fprintf( stderr, "\n" );
    fprintf( stderr, "Usage: fleaz [OPTION]\n" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "-x", "--boardx=<size>", _BOARDX, "board x dimension" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "-y", "--boardy=<size>", _BOARDY, "board y dimension" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "-n", "--nfood=<amount>",_NFOOD,  "amount of food on the board" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "-m", "--feed=<amount>", _MAXFOOD, "flea eat amount" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "-b", "--bfreq=<cycles>", _BREATHFREQ, "how often fleaz breathe" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "-t", "--tsize=<size>", _TOURNSIZE, "tournament size" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "-g", "--gen=<start>", 0, "start generation number" );
    fprintf( stderr, "%4s %-20s \t\t%s\n", "-f", "--file=<filename>", "file to load" );
    fprintf( stderr, "%4s %-20s <%s>\t%s\n", "-l", "--log=<filename>", "fleaz.log", "set the log file name" );
    fprintf( stderr, "%4s %-20s <%s>\t%s\n", "-r", "--rend=<renderer>", "TEXT", "renderer (NULL,TEXT,SVGA,SVGAFILE)" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "", "--fsm=[amount]", _NFSM, "amount of FSM fleaz in pop" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "", "--gp=[amount]", _NGP, "amount of GP fleaz in pop" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "", "--nn=[amount]", _NNN, "amount of NN fleaz in pop" );
    fprintf( stderr, "%4s %-20s <%d>\t\t%s\n", "", "--random=[amount]", _NRANDOM, "amount of RANDOM fleaz in pop" );
    fprintf( stderr, "%4s %-20s \t\t%s\n", "-h", "--help", "show usage information" );
    fprintf( stderr, "\n" );
}
void getOptions( int argc, char **argv ){
    char    opt;
    int     index = 0;

    struct option long_options[] = { 
                                     {"boardx", required_argument, 0, 'x' },
                                     {"boardy", required_argument, 0, 'y' },
                                     {"nfood", required_argument, 0, 'n' },
                                     {"feed", required_argument, 0, 'm' },
                                     {"bfreq", required_argument, 0, 'b' },
                                     {"tsize", required_argument, 0, 't' },
                                     {"gen", required_argument, 0, 'g' },
                                     {"file", required_argument, 0, 'f' },
                                     {"log", required_argument, 0, 'l' },
                                     {"rend", required_argument, 0, 'r' },
                                     {"fsm", optional_argument, 0, FSM },
                                     {"gp", optional_argument, 0, GP},
                                     {"nn", optional_argument, 0, NN},
                                     {"random", optional_argument, 0, RANDOM },
                                     {"help", 0, 0, 'h' },
                                     {0, 0, 0, 0} };

    while( (opt = getopt_long( argc, argv, "x:y:n:m:b:t:f:hl:r:g:",
                               long_options, &index )) > -1 ){
        switch( opt ){
            case 'x':
                BOARDX = atoi( optarg );
                break;
            case 'y':
                BOARDY = atoi( optarg );
                break;
            case 'n':
                NFOOD = atoi( optarg );
                break;
            case 'm':
                MAXFOOD = atoi( optarg );
                break;
            case 'b':
                BREATHFREQ = atoi( optarg );
                break;
            case 't':
                TOURNSIZE = atoi( optarg );
                break;
            case 'g':
                STARTGEN = atoi( optarg );
                break;
            case 'f':
                loadFileName = strdup( optarg );
                break;
            case 'l':
                logFileName = strdup( optarg );
                break;
            case 'r':
                if( !strncmp( optarg, "NULL", 4 ) )
                    R_CURRENT = R_NULL;
                else if( !strncmp( optarg, "TEXT", 4 ) )
                    R_CURRENT = R_TEXT;
                else if( !strncmp( optarg, "SVGA", 4 ) )
                    R_CURRENT = R_SVGA;
                else if( !strncmp( optarg, "SVGAFILE", 8 ) )
                    R_CURRENT = R_SVGAFILE;
                break;
            case 'h':
                showUsage();
                exitError( "" );
                break;
            case FSM:
                if( optarg )
                    NFSM = atoi( optarg );
                else
                    NFSM = _NFSM;
                break;
            case GP:
                if( optarg )
                    NGP = atoi( optarg );
                else
                    NGP = _NGP;
                break;
            case NN:
                if( optarg )
                    NNN = atoi( optarg );
                else
                    NNN = _NNN;
                break;
            case RANDOM:
                if( optarg )
                    NRANDOM = atoi( optarg );
                else
                    NRANDOM = _NRANDOM;
                break;
            case '?':
                showUsage();
                exitError( "Invalid Argument(s)\n\n" );
                break;
        }
    }

    /* make sure to give'em a good mix if they don't specify flea types */
    if( !(NFSM || NGP || NNN || NRANDOM) ){
        NFSM = _NFSM;
        NGP = _NGP;
        NNN = _NNN;
        NRANDOM = _NRANDOM;
    }

    while( NFSM && (NFSM <= 2 || (NFSM % 2) || ((NFSM/2)%2)) )
        NFSM++;
    while( NGP && (NGP <= 2 || (NGP % 2) || ((NGP/2)%2)) )
        NGP++;
    while( NNN && (NNN <= 2 || (NNN % 2) || ((NNN/2)%2)) )
        NNN++;
    while( NRANDOM && (NRANDOM <= 2 || (NRANDOM % 2) || ((NRANDOM/2)%2)) )
        NRANDOM++;
}
