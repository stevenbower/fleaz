/*
   $RCSfile: DataFile.C,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $ 
   Rev Hist:
      $Log: DataFile.C,v $
      Revision 1.10  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.9  1999/05/10 18:24:07  pete
      Fixed memory leak in write routine.

      Revision 1.8  1999/05/10 08:01:44  steve
      added more command line args and generally neatend stuff up

      Revision 1.7  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.6  1999/05/09 06:30:07  steve
      add/removed some fields

      Revision 1.5  1999/05/05 00:49:36  steve
      Added CVS headers

*/

#include "DataFile.h"

/* so we can create appropriate flea types */
#include "FleaFSM.h"
#include "FleaGP.h"
#include "FleaNN.h"
#include "FleaRANDOM.h"

DataFile::DataFile( char *_filename )
{
    filename = strdup(_filename );
}

DataFile::~DataFile()
{
    if ( filename )
    {
        free( filename );
    }
}

void DataFile::write( Population *pop )
{
    int        i;
    int        datalen;
    int        nfleas;
    int        foodleft;
    Flea      *flea;
    fleaInfo  *info;
    int       *data = NULL;
    FILE      *file;

    if ( ( file = fopen( filename, "wb" ) ) == 0 )
    {
        fprintf( stderr, "Could not open %s for writing!\n", filename );
        exit( 1 );
    }

    // Write the total number of fleas.
    nfleas = pop->size();
    fwrite( &nfleas,                      sizeof( int ), 1,       file );

    // Write each flea consecutively to the file. 
    for ( i = 0; i < pop->size(); i++ )
    {
        // Get flea info.
        flea     = pop->locate( i );
        info     = flea->getStats();
        if ( data )
        {
            free( data );
            data = NULL;
        }
        datalen  = flea->getData( &data );
        foodleft = flea->getFoodLeft();

        // Write out flea header.
        fwrite( &flea->id,                sizeof( int ), 1,       file );
        fwrite( &flea->type,              sizeof( int ), 1,       file );
        fwrite( &info->age,               sizeof( int ), 1,       file );
        fwrite( &info->disposition,       sizeof( int ), 1,       file );
        fwrite( &info->lifespan,          sizeof( int ), 1,       file );
        fwrite( &info->numMoves,          sizeof( int ), 1,       file );
        fwrite( &info->distMoved,         sizeof( int ), 1,       file );
        fwrite( &info->numEats,           sizeof( int ), 1,       file );
        fwrite( &info->foodCrossed,       sizeof( int ), 1,       file );
        fwrite( &info->conflicts,         sizeof( int ), 1,       file );
        fwrite( &info->wins,              sizeof( int ), 1,       file );
        fwrite( &info->fitness,           sizeof( int ), 1,       file );
        fwrite( &info->mutations,         sizeof( int ), 1,       file );

        // Write out flea unique data.
        fwrite( &datalen,                 sizeof( int ), 1,       file );
        fwrite( data,                     sizeof( int ), datalen, file );
    }

    fclose( file );

	// clean up the data memory allocated by the flea itself
    if ( data )
    {
	    free( data );
    }
}

Population *DataFile::read()
{
    int         i;
    int         nfleas;
    int         datalen;
    int         id;
    int         type;
    Flea       *flea;
    fleaInfo    info;
    int        *data = NULL;
    FILE       *file;
    Population *pop;

    if ( ( file = fopen( filename, "rb" ) ) == 0 )
    {
        fprintf( stderr, "Could not open %s for reading!\n", filename );
        exit( 1 );
    }

    // Read the total number of fleas.
    fread( &nfleas,               sizeof( int ), 1,       file );

    // Construct the population.
    pop = new Population();

    NFSM = 0;
    NGP = 0;
    NNN = 0;
    NRANDOM = 0;
    // Read each flea into the population. 
    for ( i = 0; i < nfleas; i++ )
    {
        // Read in flea header.
        fread( &id,               sizeof( int ), 1,       file );
        fread( &type,             sizeof( int ), 1,       file );
        fread( &info.age,         sizeof( int ), 1,       file );
        fread( &info.disposition, sizeof( int ), 1,       file );
        fread( &info.lifespan,    sizeof( int ), 1,       file );
        fread( &info.numMoves,    sizeof( int ), 1,       file );
        fread( &info.distMoved,   sizeof( int ), 1,       file );
        fread( &info.numEats,     sizeof( int ), 1,       file );
        fread( &info.foodCrossed, sizeof( int ), 1,       file );
        fread( &info.conflicts,   sizeof( int ), 1,       file );
        fread( &info.wins,        sizeof( int ), 1,       file );
        fread( &info.fitness,     sizeof( int ), 1,       file );
        fread( &info.mutations,   sizeof( int ), 1,       file );

        // Read in flea unique data.
        fread( &datalen,          sizeof( int ), 1,       file );
        if ( datalen )
        {
            data = ( int * ) calloc( datalen + 1, sizeof( int ) );
            fread( data,          sizeof( int ), datalen, file );
        }

        switch( type ){
            case FSM:
                flea = new FleaFSM( id, &info, datalen, data );
                NFSM++;
                break;
            case GP:
                flea = new FleaGP( id, &info, datalen, data );
                NGP++;
                break;
            case NN:
                NNN++;
                flea = new FleaNN( id, &info, datalen, data );
                break;
            case RANDOM:
                flea = new FleaRANDOM( id, &info, datalen, data );
                break;
        }
        // Create a new flea.

        // Add it to the population.
        pop->add( flea );

        if ( data )
        {
            free( data );
            data = NULL;
        }
    }

    fclose( file );

    return pop;
}

