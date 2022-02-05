/*
   $RCSfile: InfoRepository.C,v $
   $Author: steve $
   $Date: 1999/05/17 16:22:12 $
   Rev Hist:
      $Log: InfoRepository.C,v $
      Revision 1.2  1999/05/17 16:22:12  steve
      added CVS headers

*/

#include "InfoRepository.h"

infoBlock infoRepository[REPO_SIZE];
int       current_repo_size = 0;

char *examineRepository()
{
    char *retstring = ( char * ) calloc( 
        current_repo_size * sizeof( infoBlock ) + ( current_repo_size * 7 ), 
        sizeof( char ) );
    int i, cPtr = 0;

    for ( i = 0; i < current_repo_size; i++ )
    {
        int j, k;
        
        for ( j = 0; j < 5; j++ )
        {
            for ( k = 0; k < 5; k++ )
            {
                char c = '.';
                switch ( infoRepository[i][j][k] )
                {
                    case INFO_FOOD:  c = 'O'; break;
                    case INFO_FLEA:  c = '#'; break;
                    case INFO_EAST:  c = '>'; break;
                    case INFO_NORTH: c = '^'; break;
                    case INFO_SOUTH: c = 'v'; break;
                    case INFO_WEST:  c = '<'; break;
                }
                retstring[cPtr++] = c;
            }
            retstring[cPtr++] = '\n';
        }
        retstring[cPtr++] = '\n';
    }

    return retstring;
}

