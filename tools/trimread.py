#!/bin/env python

import sys
import warnings
import getopt 

from ngs import FastQ
from ngs.util import TrimRead


def usage() :
    return '''
Usage: %s  [OPTIONS] -5 <int> -3 <int> < [fastq file] > [output file]

Options:
-5/--5prime       - the fastq input file
-3/--3prime       - a comma delimited list with sequences

    ''' % sys.argv[0]



if __name__ == '__main__':

    # option parsing
    try:
        argv       = sys.argv[1:]
        opts, args = getopt.getopt( argv, '5:3:h:', ['5prime', '3prime', 'outfile', 'help',] )
    except getopt.GetoptError, err:
        # in case of an error
        print str(err)
        print usage()
        sys.exit( 85 )

    #
    fn_out   = None
    fn_fastq = None
    p5       = 0
    p3       = 0

    #
    for o, a in opts:
        if o in ('-5', '--5prime'):
            p5 = int(a)
        elif o in ('-3', '--3prime'):
            p3 = int(a)
        elif o in ('-h', '--help'):
            # they want help, give it to them
            print usage()
            sys.exit( 0 )

    tr = TrimRead( prime5=p5, prime3=p3 )

    #
    fqi = FastQ( fn_fastq, 'r' )
    fqo = FastQ( fn_out, 'w' )

    for (id,seq,qual) in fqi:
        (oid, oseq, oqual) = tr.process( id, seq, qual )
        if len(oseq) > 0 :
            fqo.write( oid, oseq, oqual)


