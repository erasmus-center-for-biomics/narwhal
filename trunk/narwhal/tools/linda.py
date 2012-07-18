#!/bin/env python 

from ngs.fastq import FastQ



def removeTrailing( seq, qual, char='A' ):
    '''
    Removes trailing characters from a sequence read

    Args:


    Returns 

    '''
    # examine the 5' end of the read
    bf = 0
    for b in seq:
        if b == char: 
            bf += 1
        else:
            break
    # examine the 3' end of the read
    af = 0
    for b in reversed(seq):
        if b == char: 
            af += 1
        else:
            break
    #    
    nseq   = seq[ bf : (len(seq) - af)]
    nqual  = qual[ bf : (len(seq) - af)]

    # return the new sequence and quality strings
    return ( nseq, nqual )


if __name__ == '__main__' :

    # open the input and the output FastQ files
    inp  = FastQ( None, mode='r' )
    outp = FastQ( None, mode='w' )

    #
    for (id, seq, qual) in inp :

        # remove the trailing A's
        nseq, nqual   = removeTrailing( seq, qual , char='A' )

        # if no A's were removed
        if len( nseq ) == len( seq ):
            # remove the trailing T's
            nseq, nqual = removeTrailing( seq, qual , char='T' )
        
        if len( seq ) > 0 :
            outp.write( id, nseq, nqual )



