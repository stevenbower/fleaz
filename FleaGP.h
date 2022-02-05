/*
   $RCSfile: FleaGP.h,v $
   $Author: steve $
   $Date: 1999/05/12 02:50:43 $
   Rev Hist:
      $Log: FleaGP.h,v $
      Revision 1.5  1999/05/12 02:50:43  steve
      general futzing

      Revision 1.4  1999/05/10 01:52:35  steve
      added options handling and file/IO

      Revision 1.3  1999/05/09 17:42:07  steve
      new stuff

      Revision 1.2  1999/05/04 01:13:15  steve
      finished crossover function

      Revision 1.1  1999/05/03 18:18:42  steve
      import into CVS.


*/

#ifndef __FLEAGP_H
#define __FLEAGP_H

#include "fleaz.h"
#include "Flea.h"
#include "stdlib.h"

/* proportions of each */
#define     PTOT        100
#define     PIF_NORTH   9
#define     PIF_SOUTH   9  
#define     PIF_EAST    9  
#define     PIF_WEST    9  
#define     PIF_FOOD    10
#define     PIF_FLEA    8 
#define     PMOVE_NORTH 9
#define     PMOVE_SOUTH 9
#define     PMOVE_EAST  9
#define     PMOVE_WEST  9
#define     PATTACK     0 
#define     PEAT        10 

#define     IF_NORTH    0x11
#define     IF_SOUTH    0x12
#define     IF_EAST     0x13
#define     IF_WEST     0x14
#define     IF_FOOD     0x15
#define     IF_FLEA     0x16
#define     MOVE_NORTH  ACT_MNORTH
#define     MOVE_SOUTH  ACT_MSOUTH
#define     MOVE_EAST   ACT_MEAST
#define     MOVE_WEST   ACT_MWEST
#define     ATTACK      ACT_ATTACK
#define     EAT         ACT_EAT

#define     MAXARGS     25
#define     MINLEVEL    5
#define     MAXLEVEL    10

#define     MRATE       2   /* percent mutation rate */

#define     GPBLOCKSIZE 1

typedef struct _gpNode {
    int         (*command)( infoBlock*, int);
    UCHAR       argIdx;
    UCHAR       action;

    _gpNode     *parent;
    _gpNode     *cTrue;
    _gpNode     *cFalse;

} gpNode;

int if_north( infoBlock *info, int idx );
int if_south( infoBlock *info, int idx );
int if_east( infoBlock *info, int idx );
int if_west( infoBlock *info, int idx );
int if_food( infoBlock *info, int idx );
int if_flea( infoBlock *info, int idx );



class FleaGP: public Flea{

public: // constructors and destructors

    /* make a random GP flea */
    FleaGP();

    /* copy constructor */
    FleaGP( FleaGP *oldFlea );

    /* data constructor */
    FleaGP( int _id, fleaInfo *info, int ndata, int *_data );

    ~FleaGP();
    
public: // virtual functions
    virtual void think( infoBlock *info );
    virtual void mutate( int rate );
    virtual void crossover( Flea *mate, Flea **child1, Flea **child2 );
    virtual void display();
    virtual void fitness();
    virtual int getData( int **dataPtr );

private: // internal functions

    /* serialize and de-serialize tree to data */
    void    writeNode( gpNode *node=0 );
    gpNode *readNode( gpNode *parent=0 );

    /* generate a random node */
    gpNode *makeNode( int level=0, gpNode *parent=0 );

    /* copy a node */
    gpNode *copyNode( gpNode *node, gpNode *parent );

    /* return a random node in the tree */
    gpNode *randNode( gpNode *node=0 );

    /* evaluate the tree */
    int     evalNode( gpNode *n );

    /* erase a tree */
    void    eraseNode( gpNode *node );

    /* recalculate the size of the tree */
    void    reCalcSize( gpNode *node=0 );

    /* mutate all nodes in a tree */
    void    _mutate( gpNode *node=0 );

    /* display a tree */
    void    _display( gpNode *n=0, int level=0 );

private: // data members
    gpNode *root;               /* our root gpNode */
    int     nCount;             /* the number of nodes in our tree */

    /* global data members for recursive functions */
    int        *dPtr;           /* data pointer to for read-write */
    infoBlock  *tInfo;          /* temporary pointer to an info block */
    int         nLeft;          /* number of elements till we hit a 
                                       random node */
    UINT        prevFitness;

};

#endif
