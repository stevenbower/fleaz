#!/usr/bin/python

from sys import stdin, stdout
from string import *

ln = stdin.readline()
fitTot = 0.0
lTot = 0.0
eTot = 0.0
mTot = 0.0
gCount = 0.0
while( ln != '' ):
    s = split( ln )
    if ln[:2] == 'ID':
        fitTot = fitTot + atof( s[4] )
        lTot = lTot + atof( s[10] )
        eTot = eTot + atof( s[6] )
        mTot = mTot + atof( split( s[8], '/' )[0] )
        gCount = gCount + 1.0
    elif ln[:2] == 'Ge':
        stdout.write( '%s %f %f %f %f\n' % ( s[1][1:-1], fitTot / gCount, \
                      lTot / gCount, eTot / gCount, mTot / gCount ) )
        fitTot = 0.0
        lTot = 0.0
        eTot = 0.0
        mTot = 0.0
        gCount = 0.0
    ln = stdin.readline()

