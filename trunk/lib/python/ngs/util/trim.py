#!/bin/env python

'''

A tool to perform sequence trimming from the 3\' end

'''

#
# a function to perform the seeded trimming
#
def seeded_trim( seq, query, seed, pre='' ):
    '''
    trims a sequence object from the 3\' end based on the
    sequences provided
    
    Args:
    self -- self
    seq  -- a string containing the sequence
    pre  -- a string to be prepended to the return string
    
    Return:
    the (trimmed) sequence
    '''
    # work per cycle:
    #        1             2
    #        |             |
    # .......SS............SSMMMMMMMMMM
    # ---------------------------------
    #
    #
    
    # set the full sequence as the return value
    ret = pre + seq
    
    # determine whether the seed is present in the sequence
    i = seq.find( seed )
    if i != -1 or seq != '':

        # we found the seed in the sequence, now we determine
        # whether the remaining string is present 
        mseq = seq[i:]
        if len( query ) > len(mseq) :
            squery = query[ 0:len(mseq) ]
        else :
            mseq   = seq[ i:len(query) ]
            squery = query
                
        # test whether the match is found
        if mseq == squery :
            # we found a match 
            ret = pre + seq[0:i]
        else :
            # recursively search for more matches
            # but first go over the seed and proceed
            # 
            co  = i + len(seed)
            if co > len(seq): co = len(seq)
            tch = seq[ co: ]
            ret = seeded_trim( tch, query, seed, pre=pre+seq[0:co] )
                    
    # return the sub sequence
    return ret 
 
class TrimRead:

    def __init__( self, prime5=0, prime3=0):
        '''

        '''
        self._prime5 = prime5
        self._prime3 = prime3        

    def process(self, id, seq, qual ):
        '''
        trims a sequence object from the 3\' end based on the
        registered sequences.

        Args:
        self  --  a reference to self
        id    --  the read id
        seq   --  the read sequence
        qual  --  the phred quality score

        Returns:
        a tuple with the id, sequence, and quality
        '''
        rseq  = seq
        rqual = qual
        
        sta = self._prime5
        end = len(seq) - self._prime3

        # sanity check
        if end > 0 and sta < len(seq):        
            rseq  = seq[sta:end]
            rqual = qual[sta:end]

        return (id, rseq, rqual)


class TrimSeq:
    
    def __init__( self, seedlength=2 ):
        '''

        '''
        self._seedlength = seedlength
        self._sequences  = []
        self._seed       = []

    def register( self, sequence ):
        '''


        '''
        seed = sequence[0:self._seedlength]

        # 
        self._sequences.append( sequence )
        self._seed.append( seed )

    def process( self, id, seq, qual ):
        '''
        trims a sequence object from the 3\' end based on the
        registered sequences.

        Args:
        self  --  a reference to self
        id    --  the read id 
        seq   --  the read sequence 
        qual  --  the phred quality score

        Returns:
        a tuple with the id, sequence, and quality
        '''
        rseq  = seq
        rqual = qual
        for i in range( 0, len(self._sequences) ):
            # perform the seeded trimming
            rseq  = seeded_trim( rseq, self._sequences[i], self._seed[i] )
            rqual = qual[ 0:len(rseq) ]
        #
        return (id, rseq, rqual)

if __name__ == '__main__':
    print "Tests not yet implemented"
    
    
