/*
   $RCSfile: FleaGP.C,v $
   $Author: steve $
   $Date: 1999/05/12 21:38:46 $
   Rev Hist:
      $Log: FleaGP.C,v $
      Revision 1.11  1999/05/12 21:38:46  steve
      Fancy Display

      Revision 1.10  1999/05/12 02:50:43  steve
      general futzing

      Revision 1.9  1999/05/10 08:01:44  steve
      added more command line args and generally neatend stuff up

      Revision 1.8  1999/05/10 04:22:01  steve
      file io stuff

      Revision 1.7  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.6  1999/05/09 17:42:07  steve
      new stuff

      Revision 1.5  1999/05/05 00:50:25  steve
      made destructors virtual and emptied the base class destructor

      Revision 1.4  1999/05/04 06:23:31  steve
      TEAM CODING SESSION

      Revision 1.3  1999/05/04 03:36:31  steve
      made fleaz set their type

      Revision 1.2  1999/05/04 01:13:15  steve
      finished crossover function

      Revision 1.1  1999/05/03 18:18:42  steve
      import into CVS.


*/

#include "FleaGP.h"
#include <stdio.h>
#include <math.h>
char percs[PTOT]="";

int if_north( infoBlock *info, int idx ){
    return ((*info)[idx/5][idx%5]==INFO_NORTH);
}

int if_south( infoBlock *info, int idx ){
    return ((*info)[idx/5][idx%5]==INFO_SOUTH);
}

int if_east( infoBlock *info, int idx ){
    return ((*info)[idx/5][idx%5]==INFO_EAST);
}

int if_west( infoBlock *info, int idx ){
    return ((*info)[idx/5][idx%5]==INFO_WEST);
}

int if_food( infoBlock *info, int idx ){
    return ((*info)[idx/5][idx%5]==INFO_FOOD);
}

int if_flea( infoBlock *info, int idx ){
    return ((*info)[idx/5][idx%5]==INFO_FLEA);
}

FleaGP::FleaGP(){
    int     i=0;
    int     tc=0;
    
    Flea();

    nCount = 0;
    extraInfo = &nCount;

    if( percs[0] == 0 ){
        for( tc=0; tc<PIF_NORTH; tc++ ) percs[i++] = IF_NORTH;
        for( tc=0; tc<PIF_SOUTH; tc++ ) percs[i++] = IF_SOUTH;
        for( tc=0; tc<PIF_EAST; tc++ ) percs[i++] = IF_EAST;
        for( tc=0; tc<PIF_WEST; tc++ ) percs[i++] = IF_WEST;
        for( tc=0; tc<PIF_FOOD; tc++ ) percs[i++] = IF_FOOD;
        for( tc=0; tc<PIF_FLEA; tc++ ) percs[i++] = IF_FLEA;
        for( tc=0; tc<PMOVE_NORTH; tc++ ) percs[i++] = MOVE_NORTH;
        for( tc=0; tc<PMOVE_SOUTH; tc++ ) percs[i++] = MOVE_SOUTH;
        for( tc=0; tc<PMOVE_EAST; tc++ ) percs[i++] = MOVE_EAST;
        for( tc=0; tc<PMOVE_WEST; tc++ ) percs[i++] = MOVE_WEST;
        for( tc=0; tc<PATTACK; tc++ ) percs[i++] = ATTACK;
        for( tc=0; tc<PEAT; tc++ ) percs[i++] = EAT;
    }
    root = makeNode();

    type = GP;
    prevFitness = 0;
}

FleaGP::FleaGP( FleaGP *oldFlea ){
    int     i;

    Flea();

    nCount = oldFlea->nCount;
    extraInfo = &nCount;
    
    root = copyNode( oldFlea->root, 0 );

    type = GP;
    prevFitness = oldFlea->getStats()->fitness;
}

FleaGP::FleaGP( int _id, fleaInfo *info, int ndata, int *_data ){
    int     i=0;
    int     tc=0;

    Flea();

    id          = id;
    type        = GP;
    dataLength  = ndata;
    data        = _data;
    nCount      = 0;
    dPtr        = data;
    extraInfo   = &nCount;
    memcpy( &soul, info, sizeof( fleaInfo ));

    if( percs[0] == 0 ){
        for( tc=0; tc<PIF_NORTH; tc++ ) percs[i++] = IF_NORTH;
        for( tc=0; tc<PIF_SOUTH; tc++ ) percs[i++] = IF_SOUTH;
        for( tc=0; tc<PIF_EAST; tc++ ) percs[i++] = IF_EAST;
        for( tc=0; tc<PIF_WEST; tc++ ) percs[i++] = IF_WEST;
        for( tc=0; tc<PIF_FOOD; tc++ ) percs[i++] = IF_FOOD;
        for( tc=0; tc<PIF_FLEA; tc++ ) percs[i++] = IF_FLEA;
        for( tc=0; tc<PMOVE_NORTH; tc++ ) percs[i++] = MOVE_NORTH;
        for( tc=0; tc<PMOVE_SOUTH; tc++ ) percs[i++] = MOVE_SOUTH;
        for( tc=0; tc<PMOVE_EAST; tc++ ) percs[i++] = MOVE_EAST;
        for( tc=0; tc<PMOVE_WEST; tc++ ) percs[i++] = MOVE_WEST;
        for( tc=0; tc<PATTACK; tc++ ) percs[i++] = ATTACK;
        for( tc=0; tc<PEAT; tc++ ) percs[i++] = EAT;
    }

    root = readNode();

    prevFitness = soul.fitness;
}


FleaGP::~FleaGP(){
    eraseNode( root );
}

void FleaGP::think( infoBlock *info ){
    tInfo = info;
    nextMove = evalNode( root );
}

void FleaGP::mutate( int rate ){
    _mutate();
}


void FleaGP::crossover( Flea *mate, Flea **child1, Flea **child2 ){
    gpNode      *n1, *n2;
    gpNode      *p1, *p2;

    *child1 = new FleaGP( this );
    *child2 = new FleaGP( (FleaGP*)mate );
    n1 = ((FleaGP*)(*child1))->randNode();
    n2 = ((FleaGP*)(*child2))->randNode();

    p1 = n1->parent;
    p2 = n2->parent;

    /* swap nodes */
    if( p1->cTrue == n1 ){
        p1->cTrue = n2;
        n2->parent = p1;
    } else {
        p1->cFalse = n2;
        n2->parent = p1;
    }

    if( p2->cTrue == n2 ){
        p2->cTrue = n1;
        n1->parent = p2;
    } else {
        p2->cFalse = n1;
        n1->parent = p2;
    }

    ((FleaGP*)*child1)->reCalcSize();
    ((FleaGP*)*child2)->reCalcSize();
}

void FleaGP::display(){
    _display();
}

void FleaGP::fitness(){
    int partial = 0;
    int x,y;

    if( !( lastInfo[ 2 ][ 2 ] ) ){
        partial = (soul.numEats + 1) * 20;
    }
    if( !( lastInfo[ 2 ][ 1 ] ) || !( lastInfo[ 2 ][ 3 ] ) ||
        !( lastInfo[ 1 ][ 1 ] ) || !( lastInfo[ 1 ][ 2 ] ) ||
        !( lastInfo[ 1 ][ 3 ] ) || !( lastInfo[ 3 ][ 1 ] ) ||
        !( lastInfo[ 3 ][ 2 ] ) || !( lastInfo[ 3 ][ 3 ] ) ){
        partial = (soul.numEats + 1) * 10;
    }
    if( !( lastInfo[ 1 ][ 0 ] ) || !( lastInfo[ 1 ][ 1 ] ) ||
        !( lastInfo[ 1 ][ 2 ] ) || !( lastInfo[ 1 ][ 3 ] ) ||
        !( lastInfo[ 1 ][ 4 ] ) || !( lastInfo[ 2 ][ 0 ] ) ||
        !( lastInfo[ 2 ][ 4 ] ) || !( lastInfo[ 3 ][ 0 ] ) ||
        !( lastInfo[ 3 ][ 4 ] ) || !( lastInfo[ 4 ][ 0 ] ) ||
        !( lastInfo[ 4 ][ 1 ] ) || !( lastInfo[ 4 ][ 2 ] ) ||
        !( lastInfo[ 4 ][ 3 ] ) || !( lastInfo[ 4 ][ 4 ] ) ){
        partial = (soul.numEats + 1) * 5;
    }
    
    soul.fitness = (int)pow( soul.numEats + 1, 2 ) + partial + (soul.numEats*50);

    if( soul.fitness + (soul.fitness/2) > 500000000 )
        soul.fitness = 500000000;
    else
        soul.fitness += (prevFitness/2);
    prevFitness = soul.fitness;
}

int FleaGP::getData( int **dataPtr ){
    data = (int*)malloc( nCount * GPBLOCKSIZE * sizeof( int ));
    *dataPtr = data;

    dPtr = data;

    writeNode();

    return nCount * GPBLOCKSIZE;
}

/* ######### INTERNAL FUNCTIONS ####################*/

void FleaGP::writeNode( gpNode *node=0 ){
    int     dta=0;
    if( !node ){
        writeNode( root );
        return;
    }

    dta = (int)node->action;
    dta <<= ((sizeof(int) * 8)/2);
    dta |= (int)node->argIdx; 
    *dPtr++ = dta;

    if( node->command ){
        writeNode( node->cTrue );
        writeNode( node->cFalse );
    }
}

gpNode *FleaGP::readNode( gpNode *parent=0 ){
    gpNode      *tNode = (gpNode*)malloc( sizeof(gpNode) );

    tNode->parent = parent;
    tNode->action = (*dPtr & 0xFFFF0000) >> ((sizeof(int) * 8)/2);
    tNode->argIdx = *dPtr & 0x0000FFFF;
    dPtr++;
    nCount++;

    switch( tNode->action ){
        case IF_NORTH:
            tNode->command = &if_north;
            tNode->cTrue = readNode( tNode );
            tNode->cFalse = readNode( tNode );
            break;
        case IF_SOUTH:
            tNode->command = &if_south;
            tNode->cTrue = readNode( tNode );
            tNode->cFalse = readNode( tNode );
            break;
        case IF_EAST:
            tNode->command = &if_east;
            tNode->cTrue = readNode( tNode );
            tNode->cFalse = readNode( tNode );
            break;
        case IF_WEST:
            tNode->command = &if_west;
            tNode->cTrue = readNode( tNode );
            tNode->cFalse = readNode( tNode );
            break;
        case IF_FOOD:
            tNode->command = &if_food;
            tNode->cTrue = readNode( tNode );
            tNode->cFalse = readNode( tNode );
            break;
        case IF_FLEA:
            tNode->command = &if_flea;
            tNode->cTrue = readNode( tNode );
            tNode->cFalse = readNode( tNode );
            break;
        case MOVE_NORTH:
        case MOVE_SOUTH:
        case MOVE_EAST:
        case MOVE_WEST:
        case ATTACK:
        case EAT:
            tNode->command = 0;
            break;
    }

    return tNode;
}

gpNode *FleaGP::makeNode( int level, gpNode *parent ){
    gpNode      *tNode = (gpNode*)malloc( sizeof(gpNode) );
    int         w;

    tNode->parent = parent; 
    nCount++;

    if( level < MINLEVEL ){
        w = random() % (PIF_NORTH+PIF_SOUTH+PIF_EAST+PIF_WEST+
                        PIF_FOOD+PIF_FLEA);
    } else if( level > MAXLEVEL ){
        w = random() % (PTOT - (PIF_NORTH+PIF_SOUTH+PIF_EAST+PIF_WEST+
                        PIF_FOOD+PIF_FLEA));
        w += (PIF_NORTH+PIF_SOUTH+PIF_EAST+PIF_WEST+ PIF_FOOD+PIF_FLEA);
    } else
        w = random() % PTOT;
    
    tNode->action = percs[w];

    switch( percs[w] ){
        case IF_NORTH:
            tNode->command = &if_north;
            tNode->argIdx = 12; //random() % MAXARGS;
            tNode->cTrue = makeNode(level+1, tNode );
            tNode->cFalse = makeNode(level+1, tNode );
            break;
        case IF_SOUTH:
            tNode->command = &if_south;
            tNode->argIdx = 12; //random() % MAXARGS;
            tNode->cTrue = makeNode(level+1, tNode );
            tNode->cFalse = makeNode(level+1, tNode );
            break;
        case IF_EAST:
            tNode->command = &if_west;
            tNode->argIdx = 12; //random() % MAXARGS;
            tNode->argIdx = random() % MAXARGS;
            tNode->cTrue = makeNode(level+1, tNode );
            tNode->cFalse = makeNode(level+1, tNode );
            break;
        case IF_WEST:
            tNode->command = &if_east;
            tNode->argIdx = 12; //random() % MAXARGS;
            tNode->cTrue = makeNode(level+1, tNode );
            tNode->cFalse = makeNode(level+1, tNode );
            break;
        case IF_FOOD:
            tNode->command = &if_food;
            tNode->argIdx = 12; //random() % MAXARGS;
            tNode->cTrue = makeNode(level+1, tNode );
            tNode->cFalse = makeNode(level+1, tNode );
            break;
        case IF_FLEA:
            tNode->command = &if_flea;
            tNode->argIdx = 12; //random() % MAXARGS;
            tNode->cTrue = makeNode(level+1, tNode );
            tNode->cFalse = makeNode(level+1, tNode );
            break;
        case MOVE_NORTH:
        case MOVE_SOUTH:
        case MOVE_EAST:
        case MOVE_WEST:
        case ATTACK:
        case EAT:
            tNode->command = 0;
            break;
    } 

    return tNode;
}

gpNode *FleaGP::copyNode( gpNode *node, gpNode *parent ){
    gpNode      *tNode = (gpNode*)malloc( sizeof(gpNode) );

    tNode->command = node->command;
    tNode->argIdx = node->argIdx;
    tNode->action = node->action;
    
    tNode->parent = parent;
  
    if( tNode->command ){ 
        tNode->cTrue = copyNode( node->cTrue, tNode );
        tNode->cFalse = copyNode( node->cFalse, tNode );
    }

    return tNode;
}

gpNode *FleaGP::randNode( gpNode *node=0 ){
    gpNode      *tNode=0;

    if( !node ){
        if( nCount == 1 )
            return root;
        nLeft = (random() % (nCount-1))+ 1;
        return randNode( root );
    }

    if( !nLeft ){
        return node;
    }

    nLeft--;
    if( node->command ){
        tNode = randNode( node->cTrue );
        if( !tNode )
            tNode = randNode( node->cFalse );
    }
    return tNode;
}
    
int FleaGP::evalNode( gpNode *n ){
    if( n->command ){
        if( n->command( tInfo, n->argIdx ) )
            return evalNode( n->cTrue );
        else
            return evalNode( n->cFalse );
    } else
        return n->action;
}

void FleaGP::eraseNode( gpNode *node ){
    if( node->command ){
        eraseNode( node->cTrue );
        eraseNode( node->cFalse );
    }
    free( node );
}

void FleaGP::reCalcSize( gpNode *node=0 ){
    if( !node ){
        nCount = 0;
        reCalcSize( root );
    } else {
        nCount++;
        if( node->command ){
            reCalcSize( node->cTrue );
            reCalcSize( node->cFalse );
        }
    }
}

void FleaGP::_mutate( gpNode *node ){
    int     w;

    if( !node ){
        nCount = 0;
        _mutate( root );
    } else {
        if( (random() % 100) < MRATE ){
            if( node->command ){
                w = random() % (PIF_NORTH+PIF_SOUTH+PIF_EAST+PIF_WEST+
                                PIF_FOOD+PIF_FLEA);
                switch( percs[w] ){
                    case IF_NORTH:
                        node->command = &if_north;
                        node->argIdx = 12; //random() % MAXARGS;
                        break;
                    case IF_SOUTH:
                        node->command = &if_south;
                        node->argIdx = 12; //random() % MAXARGS;
                        break;
                    case IF_EAST:
                        node->command = &if_west;
                        node->argIdx = 12; //random() % MAXARGS;
                        break;
                    case IF_WEST:
                        node->command = &if_east;
                        node->argIdx = 12; //random() % MAXARGS;
                        break;
                    case IF_FOOD:
                        node->command = &if_food;
                        node->argIdx = 12; //random() % MAXARGS;
                        break;
                    case IF_FLEA:
                        node->command = &if_flea;
                        node->argIdx = 12; //random() % MAXARGS;
                        break;
                } 
            } else {
                w = random() % (PTOT - (PIF_NORTH+PIF_SOUTH+PIF_EAST+PIF_WEST+
                                        PIF_FOOD+PIF_FLEA));
                w += (PIF_NORTH+PIF_SOUTH+PIF_EAST+PIF_WEST+PIF_FOOD+PIF_FLEA);
                node->action = percs[w];
            }

        }
        if( node->command ){
            _mutate( node->cTrue );
            _mutate( node->cFalse );
        }
    }
}

void FleaGP::_display( gpNode *n=0, int level=0 ){
    int     x;
    
    n = (!n)?root:n;
    if( n->command ){
        //for( x=0; x < level; x++ )
        //    printf( " " );
        switch( n->action ){
            case IF_NORTH:
                printf( "if(<%d,%d>==NORTH){", n->argIdx%5, n->argIdx/5 );
                break;
            case IF_SOUTH:
                printf( "if(<%d,%d>==SOUTH){", n->argIdx%5, n->argIdx/5 );
                break;
            case IF_EAST:
                printf( "if(<%d,%d>==EAST){", n->argIdx%5, n->argIdx/5 );
                break;
            case IF_WEST:
                printf( "if(<%d,%d>==WEST){", n->argIdx%5, n->argIdx/5 );
                break;
            case IF_FLEA:
                printf( "if(<%d,%d>==FLEA){", n->argIdx%5, n->argIdx/5 );
                break;
            case IF_FOOD:
                printf( "if(<%d,%d>==FOOD){", n->argIdx%5, n->argIdx/5 );
                break;
        }
        _display( n->cTrue, level+1 );
        //for( x=0; x < level; x++ )
        //    printf( " " );
        printf( "}else{" );
        _display( n->cFalse, level+1 );
        //for( x=0; x < level; x++ )
        //    printf( " " );
        printf( "}" );
    } else {
        //for( x=0; x < level; x++ )
        //    printf( " " );
        switch( n->action ){
            case MOVE_NORTH:
                printf( "act(MOVE_NORTH)" );
                break;
            case MOVE_SOUTH:
                printf( "act(MOVE_SOUTH)" );
                break;
            case MOVE_WEST:
                printf( "act(MOVE_WEST)" );
                break;
            case MOVE_EAST:
                printf( "act(MOVE_EAST)" );
                break;
            case EAT:
                printf( "act(EAT)" );
                break;
        }
        //printf( "action: %d\n", n->action );
    }

}

