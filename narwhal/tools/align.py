#!/bin/env python 

import getopt
import sys
import json
import re
import logging
import subprocess
import os

from ngs.alignment import Bowtie, BWA, Tophat



#
# Option parsing
#
#
#
def _add_option( s, d ):
    '''
    Adds an option to the dict
    '''
    spl = re.split( '[\=\ ]', s )
    if len(spl) == 1:
        d[spl[0]] = ""
    elif len(spl) == 2:
        d[spl[0]] = spl[1]
    # return the dict
    return d


def parse_option( opt, d={} ):
    '''
    Parse the options and return these in a dict

    '''   
    if opt :
        for o in opt:
            s = o.split( ':' ) 
            if len(s) == 1:
                d = _add_option( s[0], d )
            elif len(s) == 2:
                if not d.has_key( s[0] ): d[s[0]] = {}
                d[s[0]] = _add_option( s[1], d[s[0]] )
    # return the dictionary
    return d

#
#
# 
#
#
#

def usage( mess="", error=None ):
    '''
    Prints the usage information and exits if an unrecoverable error has occured

    '''
    print """
%s

Usage: 
%s -p <profiles.json> 

Options:
-p/--profiles	the file path of a file containing the various alignment profiles that can be selected in this tool
-f/--fastq	the input fastq files
-s/--sam	the output sam file
-r/--reference	the reference sequence to use
-t/--type	the type of analysis to perform. This word should be a key in the alignment profiles
-P/--paired_end	Perform a paired-end alignment 
-/--options	Additional alignment options. These should be in the following format(comma delimited): [field]:[flag]=[value]. If there are
no additional options use NA, None 



Description:    


    """ % (mess, sys.argv[0])
    if error != None: sys.exit( error )


if __name__ == '__main__' :
    
    logging.basicConfig( format="%(asctime)s:%(module)s\t%(message)s", level=logging.DEBUG, stream=sys.stdout )

    # prepare the input variables
    fn_fastq = []
    fn_sam   = None
    fn_ref   = None
    profstr  = None
    prof     = None
    pe       = False
    aopt     = []
    fn_prof  = None
    allprof  = {}
    debug    = False

    # parse the command line options
    opts, args = getopt.getopt( sys.argv[1:], 'p:f:s:r:o:t:P:D', ['profiles', 'fastq','sam','reference', 'options', 'type',
'paired_end', 'debug'] )
    for (k,v) in opts:
        v = unicode(v)
        k = unicode(k)
        if k in ('-p', '--profiles'): 
            f = open( v, 'r' )
            allprof = json.load( f )
            f.close() 
        if k in ('-f', '--fastq'):
            a = v.split(',')
            for x in a: fn_fastq.append(x)
        if k in ('-s', '--sam'):
            fn_sam = v
        if k in ('-r', '--reference'):
            fn_ref = v
        if k in ('-t', '--type'):
            profstr = v
        if k in ('-o', '--options'):
            if v.lower() != 'na' and  v.lower() != 'none':
                aopt = v.split(',')
        if k in ('-P', '--paired_end'):
            if v.lower() in ('true','yes','1'): pe = True 
        if k in ('-D', '--debug'):
            debug = True
    if debug:
        print "Command line options:" 
        for (k,v) in opts:
            print "\t%s\t%s" % (str(k), str(v))
        print "\nprofiles:"
        for k in allprof.keys():
            print "\t%s" % str(k)

        print "profile: %s" % profstr
        print "FastQ: %s" % ','.join( fn_fastq )
        print "reference: %s" % fn_ref
        print "SAM file: %s" % fn_sam
        

    # check the command line options
    if len(fn_fastq) == 0: usage( "No FastQ files provided" , error=2 )
    if fn_sam == None:  usage( "No output SAM file name provided" , error=2 )
    if fn_ref == None:  usage( "No reference file provided" , error=2 )   
    if len(allprof.keys()) == 0: usage( "No application profiles were loaded" , error=2 )
    if profstr not in allprof.keys(): usage( "Profile '%s' not present in profiles" % profstr, error=2)

    # prepare the profiles
    prof = allprof[ profstr ]
    tool = prof['tool']
    dopt = parse_option( aopt, d=prof['options'] )
    
    # prepare the alignment objects
    aln = None 
    if tool.lower() == 'bwa':
        aln = BWA( )
        sai = []       
        # prepare the SAI files
        for i in  range(len(fn_fastq)): 
            sai.append( "%s.%d.sai" % (fn_sam, i) )
        #
        aln.align( fn_ref, fn_fastq, fn_sam, saifiles=sai, opt=dopt )
    elif tool.lower() == 'bowtie':
        aln = Bowtie( )
        #
        aln.align( fn_ref, fn_fastq, fn_sam, opt=dopt )
    
    elif tool.lower() == 'tophat':
        aln = Tophat( )
        #
        # construct an extra foldername for Tophat to write its output
        sample_dir = os.path.dirname( fn_sam )
        sample_output_samname = os.path.basename( fn_sam )
        sample_regexp = re.search("s_(\d|NA)_(\d|NA)_(.*)\.%s" % profstr, sample_output_samname)
        sample_id     = sample_regexp.group(3)
        sample_lane   = sample_regexp.group(1)
        sample_read   = sample_regexp.group(2)
        d_sam = '/'.join([sample_dir, "%s_%s_%s" % (sample_id, sample_lane, sample_read)])
        aln.align( fn_ref, fn_fastq, d_sam, opt=dopt )
        fn_bam = os.path.join( d_sam, 'accepted_hits.bam' )
        if os.path.isfile( fn_bam ):
             cmd = [ 'samtools', 'view', '-h', '-o', fn_sam, fn_bam ]
             ret = subprocess.call( cmd )
             if ret != 0:
                 logging.warn( "SAMtools conversion returned an error: %s" % ' '.join(cmd) )
	     else:
		 logging.warn( "SAMtools conversion from bam to sam OK" )
        else:
            logging.warn( "Tophat output file (%s) does not exist" % fn_bam )
