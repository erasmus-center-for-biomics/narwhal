#!/bin/env python

'''

A tool to perform sequence trimming from the 3\' end

'''

import sys, warnings

from ngs import FastQ
from ngs.util import TrimSeq

if __name__ == '__main__' :

    import getopt

    def usage() :
        return '''
Usage: %s  [OPTIONS] -s <STRING> < [fastq file] > [output file]

Options:
-f/--fastq       - the fastq input file
-s/--sequence    - a comma delimited list with sequences
-l/--seedlength  - the seed length to take (default 6)
-o/--outfile     - the output file

        ''' % sys.argv[0]
   
    # option parsing
    try:
        argv       = sys.argv[1:]
        opts, args = getopt.getopt( argv, 'f:s:o:h:l:', ['fastq', 'sequences', 'outfile', 'help', 'seedlength']  )
    except getopt.GetoptError, err:        
        # in case of an error
        print str(err)
        print usage()
        sys.exit( 85 )
    
    #    
    fn_out   = None
    st_seq   = []
    fn_fastq = None
    seedl    = 2
    #
    for o, a in opts:
        if o in ('-f', '--fastq'):
            fn_fastq = a
        elif o in ('-s', '--sequences'):
            st_seq.append( a )
        elif o in ('-o', '--output'):
            fn_out = a
        elif o in ('-l', '--seedlength'):
            seedl = a
        elif o in ('-h', '--help'):
            # they want help, give it to them
            print usage()
            sys.exit( 0 )
                
    # are the necessary variables defined?
    if st_seq == None : 
        print usage()
        sys.exit( 85 )

    # setup the sequence trimming
    ts = TrimSeq( seedlength=int(seedl) )
    for s in st_seq:
        ts.register( s )

    # 
    fqi = FastQ( fn_fastq, 'r' )
    fqo = FastQ( fn_out, 'w' )

    for (id,seq,qual) in fqi:
        (oid, oseq, oqual) = ts.process( id, seq, qual )
        if len(oseq) > 0 :
            fqo.write( oid, oseq, oqual)
        

