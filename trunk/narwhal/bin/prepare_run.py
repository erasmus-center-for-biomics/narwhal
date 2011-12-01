#/bin/env python

import os
import os.path
import sys
import re

import logging 
import getopt


DRYRUN = False


'''
This script prepares the OUTPUT FOLDER for a NARWHAL run. 

Args:
	-s	samplesheet
	basecalls_directory
	output_directory

This script will perform the following operations in succession:

*) make a runfolder in the output directory to which all the results will be written

*) generate the required subfolders in the runfolder to form a scaffold in which the analyzes will
take place: The subdirectories are
	.) raw_data
	.) demultiplex
		# sampleids
	.) alignment
		# sampleids
	.) stats 
		# sampleids
	.) project

*) generate the qseq2fastq.lst file in the raw_data directory. This file will govern the neccessary
qseq2fastq conversions. 

*) generate the singleplex.lst file that will govern the single plex combination from the raw_data to
the demultiplex folders

*) generate the demultiplex_index.lst file that holds the options for the demultiplexing of the tiles in raw_data

*) generate the demultiplex_divide.lst 

*) add the alignment options files to the alignment directory

*) add the sam file conversion options

*)
 

'''



#
#
# INPUT FUNCTIONS
#
#


def _read_sample_sheet( fname ):
    '''
    Read a sample sheet and return the data

    Args:


    Returns:


    '''
    retval = [] 
    f      = open( fname, 'rU' )

    for l in f:
        l = l.rstrip()
        l = l.replace( '"', '' )
        # parse the header and fill the column mapping
        if l == '':
           pass
        elif l[0] == '#':
            pass
        # parse the file contents
        else:
            s = l.split( "\t" )  # split the sample-sheet line on whitespace characters
            if len(s) >= 9:
                d = {
                    'id'                    : s[0],
                    'lanes'                 : s[1].split(','),
                    'data_reads'            : s[2].split(','),
                    'multiplex.codes'       : s[3].split(','),
                    'multiplex.starts'      : s[4].split(','),
                    'multiplex.indexes'     : s[5].split(','),
                    'alignment.reference'   : s[6],
                    'alignment.paired_end'  : s[7],
                    'alignment.application' : s[8],
                    'alignment.options'     : [] #s[9].split(',')
                }
		if len(s) == 10: 
                    d['alignment.options'] = s[9].split(',')
                retval.append( (s[0],d) )
            else:
                 logging.fatal( "A line in the samplesheet could not be parsed:\n%s " % l )
    # 
    return retval 

def _read_fastq_sample_sheet( fname ):
    '''
    Read a sample sheet and return the data

    Args:


    Returns:


    '''
    retval = []
    f      = open( fname, 'rU' )

    for l in f:
        l = l.rstrip()
        # parse the header and fill the column mapping
        if l == '':
           pass
        elif l[0] == '#':
            pass
        # parse the file contents
        else:
            s = l.split( "\t" )  # split the sample-sheet line on whitespace characters
            if len(s) >= 5:
                d = {
                    'id'                    : s[0],
                    'files'                 : s[1].split(','),
                    'alignment.reference'   : s[2],
                    'alignment.paired_end'  : s[3],
                    'alignment.application' : s[4],
                    'alignment.options'     : []
                }
                if len(s) == 6:
                    d['alignment.options'] = s[5].split(',')
                retval.append( (s[0],d) )
            else:
                 logging.fatal( "A line in the samplesheet could not be parsed:\n%s " % l )
    #
    return retval



#
#
#
#
# DIRECTORY PROCESSING FUNCTIONS
#
#
#
#

def _get_flowcell_dirname( path ):
    '''
    Get the flowcell directory

    Args:
      path  -  the basecalls file path

    Returns:
      the flowcell dirname or none
    '''
    # prepare the return value
    retval = None 

    # get the absolute path and split it on '/'
    path = os.path.abspath( path )
    spa  = path.split( '/' )

    # go through the directories
    for s in spa:
        m = re.match( '^\d{6}_\w{5}_\d{4}_\w{10}', s )
	if m : retval = s
	

    # return the value
    return retval
	
	
def _outdir( wdir, fdname, extra=[] ):
    '''
    Generate the output directory names

    '''
    retval = None

    #
    fdname = "NARWHAL_%s" % fdname
    retval = os.path.join( wdir, fdname )

    # add the 
    ename  = '_'.join( extra )
    if ename != '':
        retval += '_%s' % ename


    # make sure we do not get directory name collisions
    org = retval
    i   = 1 
    while os.path.exists( retval ) :
        i += 1
        retval = "%s.%d" % (org,i)
    #
    return retval	


def _get_qseq_paths( bdir ):
    '''
    Get the locations of all of the qseq files

    '''
    retval = []
    p      = re.compile( 's_\d_\d_\d{4}_qseq.txt' )
    for f in os.listdir(bdir) :
        if p.match( f ):
            retval.append( os.path.abspath( os.path.join(bdir,f) ) )


    # sort the files
    retval = sorted( retval )
    return retval


#
#
#
# CREATE THE DIRECTORY STRUCTURE FOR THE RUNDIR
#
#
#

def create_dir_structure( dirpath, sampleids=[] ) :
    '''
    Create the directory structure

    Args:
    dirpath    -   the location where to create the subdirectories
    sampleids  -   a list with all of the sample ids to be used

    Returns:
      A list with the created directories    

    '''
    retval = []
    # 
    d_raw = os.path.join( dirpath, 'raw_data' )
    d_dem = os.path.join( dirpath, 'demultiplexed' )
    d_aln = os.path.join( dirpath, 'alignment' )
    d_sta = os.path.join( dirpath, 'stats' )
    d_pro = os.path.join( dirpath, 'projects' )

    #
    retval.append( d_raw ) 
    retval.append( d_dem )
    retval.append( d_aln )
    retval.append( d_sta )
    retval.append( d_pro )
   
    #
    for dir in [d_dem,d_aln, d_sta]:       
        for sid in sampleids:
            dt = os.path.join( dir, sid )
            retval.append( dt )

    # 
    if not DRYRUN:
        if not os.path.exists(dirpath): os.mkdir( dirpath ) 
        [ os.mkdir(d) for d in retval ]

    return retval



#
#
# QSEQ2FASTQ LIST
#
#

def _qseq_fastq_lst( opath, qseq ):
    rt = []
    convlst = []
    for p in qseq:
        t = os.path.basename( p )
	t = t.replace( '_qseq.txt', '.fastq' )
        t = os.path.join( opath, t )
        
	# write the line to the file
        convlst.append( "%s;%s\n" % (p,t) )
        rt.append( t )

    if not DRYRUN: 
        fn = os.path.join( opath, "qseq2fastq.param" )
        f = open( fn, 'w' )
        [f.write(l) for l in convlst]
        f.close()
    #
    return rt

#
#
# DEMULTIPLEX THE DATA
#
#


def _process_singleplexes( opath, a_fastq, a_smpls ):
   '''
   This function will make the option lists that will 
   perform the sample based demultiplexing

   Args:
    opath 
    a_fastq 
    a_smpls

   '''
   retval = []
   conlst = []
   for (s,d) in a_smpls:

       if not d['multiplex.codes'] or d['multiplex.codes'][0] == "" :
           # no multiplex, so join the files in the correct folder
           data_reads = d['data_reads']
           for i in range( len(d['lanes']) ) :
               l = d['lanes'][i]
               for r in data_reads[i].split(';'):
                   pa = re.compile( '.*s_%d_%d_\d{4}.fastq' % (int(l), int(r) ) )
                   a_ifastq = [ fq for fq in a_fastq if pa.match(fq) ]
              
                   ofname   = "s_%d_%d_%s.fastq" % ( int(l), int(r), s )
                   ofpath   = os.path.join( opath, s )
                   ofpath   = os.path.join( ofpath, ofname )

                   # add the fastq file to the conversion list
                   for ifq in a_ifastq:
                       conlst.append( "%s;%s\n" % (ifq,ofpath) )

                   # add the output to the return array
                   retval.append( (s,ofpath) )
              

   #  write the files
   if not DRYRUN:
       fn_c = os.path.join( opath, 'concatenate.param' ) 
       f_c  = open( fn_c, 'w' )
       [f_c.write(l) for l in conlst ]
       f_c.close()
   #
   return retval 


def _aggregate_multipexes( a_samples ):
    ''' 
    Aggregate the multiplexes over all the samples
    
    Args:
     a_samples   --   a list with samples including the multiplexes

    Returns:
     get the multiplex options per (lane, index read) pair
    '''
    retval = {}
    multi  = [ t for t in a_samples if len(t[1]['multiplex.codes']) > 0 and t[1]['multiplex.codes'][0] != "" ]
    for (id,opt) in multi:
        for i in range( len(opt['lanes']) ):           
            key = ( opt['lanes'][i], opt['multiplex.indexes'][i] )
            if not retval.has_key( key ):
                retval[key] = {
                               'barcodes'    : [],
                               'sample_ids'  : [],
                               'start_positions' : [],
                               'data_reads'   : []
                              }
            retval[key]['barcodes'].append( opt['multiplex.codes'][i] )
            retval[key]['sample_ids'].append( id )
            retval[key]['start_positions'].append( opt['multiplex.starts'][i] )
            [ retval[key]['data_reads'].append( read ) for read in opt['data_reads'][i].split(';') ]
            retval[key]['data_reads'] = unique( retval[key]['data_reads'] )
    #
    return retval




def _process_multiplexes( opath, a_fastq, a_samples ):
    '''
    Process the multiplexed samples and generate file lists with the
    the 

    Args: 
     opath      --  the output path where to write the data
     a_fastq    --  a list with fastq files
     a_samples  --  a list with samples
    
    Returns:
      a tuple with a list of index fastq files and a list of demultiplexed fastq files

    Note:
      this method assumes that each index read is used 
      for a single set of target reads to demultiplex. 

    '''
    toindex   = _aggregate_multipexes( a_samples )
    indexlst  = []
    dividelst = []

    # determine the indexes to build
    a_index  = []
    a_divide = []
    for key in toindex.keys():
        lane = key[0]
        val  = toindex[key]
        cds = ','.join( val['barcodes'] ) 
        sta = ','.join( val['start_positions'] )
        sid = ','.join( val['sample_ids'] )

        # determine the index options
        pa  = re.compile( '.+s_%s_%s_\d{4}.fastq' % ( str(key[0]), str(key[1]) ) )
        ifq = [ f for f in a_fastq if pa.match(f) ]

        # for each matching fastq
        for fq in ifq:

            # determine the tile number
            tile = "" 
            obj  = re.match( '.+s_\d_\d_(\d{4}).fastq', fq )
            if not obj:
                continue
            else:
                tile = obj.group( 1 )


            # determine the index basename
            bo = os.path.basename( fq )
            bi = bo.replace('.fastq', '.index')
            fi = os.path.join( opath, bi )

            # write the option lists to a file
            indexlst.append( "%s;%s;%s;%s;%s\n" % ( fq, fi, cds, sta, sid ) )
            a_index.append( fi )
             
            # for each lane read combination that should be divided
            for read in val['data_reads']:
                # get the input files from the raw_data array
                pa  = re.compile( '.+s_%s_%s_%s.fastq' % ( str(lane), str(read), tile ) )
                ofq = [ f for f in a_fastq if pa.match(f) ]
  
                # for each of the fastq files to divide
                for f in ofq:

                    # determine the basename of the files and 
                    # the final names by replacing the fastq extension 
                    # with the sampleid.fastq string
                    bo = os.path.basename( f )
                    bi = bo.replace('.fastq', '')
                    so = [ (s, re.sub('\d{4}.fastq', '%s.fastq' % s, bo )) for s in val['sample_ids'] ]

                    # determine the correct output locations including directories
                    fs = []               
                    for x in so:
                        cpath = os.path.join( opath, x[0] ) 
                        p     = os.path.join( cpath, x[1] ) 
                        fs.append( (x[0],p) )      
                    # make the array comma delimited
                    fo = ','.join( [x[1] for x in fs] ) 
                    ns = ','.join( [x[0] for x in fs] )

                    # write the demultiplex options to a file 
                    # and add the output files to the a_divide list
                    dividelst.append( "%s;%s;%s;%s\n" % (f, fi, fo, sid) )
                    [a_divide.append( x ) for x in fs ]

    if not DRYRUN: 
        f_index  = open( os.path.join( opath,"demultiplex_index.param"), "w" )
        [f_index.write(l) for l in indexlst]
        f_index.close()

        f_divide = open( os.path.join( opath,"demultiplex_divide.param"), "w" )
        [f_divide.write(l) for l in dividelst]
        f_divide.close()
    #           
    a_divide = unique(a_divide)   

    # return the output files from the index and division
    return( a_index, a_divide )
    

#
#
# ALIGNMENT HELPER FUNCTIONS
#
#

def _get_pairs( fnames ):
    '''
    Divide the fnames list in a list with read pairs
    Args:
      fnames   a list with fastq filenames

    Returns:
      a list with read pair information
    '''
    p  = re.compile( ".*s_\d_1_.*" )
    fr = [f for f in fnames if p.match(f) ]
    rr = [None for f in fr]

    # get the matching read for a read pair
    for k in range(0,len(fr)):
        f = fr[k]
        obj = re.match( "(.*s_\d_)\d(_.*)", f )
        if obj:
            for i in range(2,4):
                ss = obj.group(1) + str(i) + obj.group(2)
                if ss in fnames:
                    rr[k] = ss
    #
    retval = [ [fr[i],rr[i]] for i in range(0, len(fr)) ] 
    return retval 
    

def _get_info( fname ):
    '''
    Get the lane and read information from a sequence file

    Args:
      fname   a file name

    Returns:
      the lane and read information. If these can not be determined 
      return NA for both
    '''
    lane = 'NA'
    read = 'NA'
    obj = re.match( '.*s_(\d)_(\d)_.*', fname )
    if obj:
        lane = obj.group(1)
        read = obj.group(2) 

    # return the lane and read information
    return ( lane, read )



#
#
#
# ALIGNMENT OPTIONS
#
#
#

    


def _prepare_alignment_options( opath, a_sfastq, a_smpls ) : 
    ''' 
    Prepare the alignment options

    Args:
       opath     --  the location where to write the files
       a_sfastq  --  a list with sample id and fastq filepath tuples
       a_smpls   --  a list with sample id and option dict tuples

    Returns:
       a list with sample id and sam file path tuples

    '''
    #
    inf = {}
    for (s,fn) in a_sfastq:
        # prepare the input files
        if not inf.has_key(s): inf[s] = []
        inf[s].append( fn )

    retfiles = []
    optlines = []
    # for each sample to analyze get the options and the input files
    for (s,d) in a_smpls:

        # 
        if inf.has_key(s):
            
            # get the general alignment options
            appl    = d['alignment.application']
            refseq  = d['alignment.reference']
            options = d['alignment.options']

            # determine the reference sequence name
            refnm   = os.path.basename( refseq )
            refnm   = re.sub( '\..*', '', refnm )

            # parse the paired end options
            paired  = False
            if d['alignment.paired_end'].lower() in ('yes', 'true', '1'): paired = True
             
            # determine whether we require pairs of files or single reads
            # represent all of these data as tuples
            fq = []
            if paired:
                fq = _get_pairs( inf[s] )
            else:
                fq = [[f] for f in inf[s]]

            # for each cluster of input files determine 
            # the SAM output file
            samfiles = []
            infiles  = []
            for flst in fq:
                 lns = []
                 rds = []
                 # determine the read and lane information
                 for f in flst:
                     fname = os.path.basename( f )
                     ret   = _get_info( fname )
                     if ret: 
                         ln, rd = ret 
                         lns.append( ln )
                         rds.append( rd )

                 # add this information to the sam output file name
                 samfname = "s_" + ''.join(lns) + "_" + ''.join(rds) + "_" + s + "." + appl + "." + refnm + ".sam" 
                 cpath    = os.path.join( opath, s )

                 # add the input fastqs and the output sam to a list
                 samfiles.append( os.path.join(cpath, samfname) )
                 infiles.append( ','.join(flst) )

            # write these options to an array
            for i in range( len(samfiles) ):
                spe = 'TRUE'
                if not paired: spe = 'FALSE'

                opt  = ','.join( options )
                if opt == "": opt = 'None' 
                line = "%s;%s;%s;%s;%s;%s\n" %( infiles[i], samfiles[i], refseq, appl, opt, spe )
                optlines.append( line )
            [retfiles.append( (s,f) ) for f in samfiles ]
                        
        else:
            pass 

    if not DRYRUN:
        f = open( os.path.join(opath, "alignment.param") , "w" )
        [f.write(l) for l in optlines]
        f.close()

    # return the files
    return retfiles

#
#
#
# SAM FILE converions
#
#
#

def _prepare_SAMBAM( opath, a_sam ):
    ''' 
    Prepare the sam bam file conversions

    Args:
       opath   --   the output path to write the files
       s_sam   --   a list with sample id and sam file path tuples

    Returns:
       a list with sample_ids and sorted BAM filepaths
    '''
    convlst = []   
    retval  = []
    for (s,f) in a_sam:

        # prepare the data
        bname = os.path.basename( f )
        cpath = os.path.join( opath, s )

        # prepare the file paths
        fn_bam  = bname.replace( '.sam', '.bam' )
        st_srt  = bname.replace( '.sam', '.srt' )
        fn_sbam = bname.replace( '.sam', '.srt.bam' )

        # 
        pa_bam  = os.path.join( cpath, fn_bam ) 
        pa_srt  = os.path.join( cpath, st_srt )
        pa_sbam = os.path.join( cpath, fn_sbam )

        # add a line to the file content list and the return list
        line = "%s;%s;%s;%s\n" % ( f, pa_bam, pa_srt, pa_sbam )
        convlst.append( line )
        retval.append( (s, pa_sbam) )

    # write the output file if we are not performing a dryrun
    if not DRYRUN:
        fo = open( os.path.join( opath, "sambam.param" ), "w" )
        [ fo.write(l) for l in convlst ]
        fo.close()

    # return the results
    return retval

#
#
#
# STATS file generation
#
#
#

def _prepare_stats( opath, a_bam ):
    '''


    '''
    convlst = []
    retval  = []
    for (s,f) in a_bam:

        # prepare the data
        bname = os.path.basename( f )
        cpath = os.path.join( opath, s )

        #
        fn_stu  = bname.replace( '.bam', '' )
        fn_stat = bname.replace( '.bam', '.stats' )
        fn_pdf  = bname.replace( '.bam', '.pdf' )

        #
        pa_stat = os.path.join( cpath, fn_stat )
        pa_stu  = os.path.join( cpath, fn_stu )
        pa_pdf  = os.path.join( cpath, fn_pdf )


        line = "%s;%s;%s;%s;%s\n" % ( f, s, pa_stu, pa_stat, pa_pdf )
        convlst.append( line )
        retval.append( (s, pa_pdf) )

    # write the output file if we are not performing a dryrun
    if not DRYRUN:
        fo = open( os.path.join( opath, "bamstats.param" ), "w" )
        [ fo.write(l) for l in convlst ]
        fo.close()

    # return the results
    return retval


#
#
# HELPER FUNCTIONS
#
#
#
def unique( lst, f=None ):
    ''' 
    Return the unique entries in a list
    '''
    if f == None: 
        def f(x): return x
    #
    seen = {}
    result = []
    for v in lst:
        m = f(v)
        if m in seen: continue
        seen[m] = 1
        result.append( m )
    return result

#
#
#
# MAIN
#
#
#

def usage( message, error=None ): 
    '''

    '''
    print """
%s


Usage:

There are two use cases for this software. In the first use-case reads are obtained directly from the BaseCalls directory of the run folder. When reads are directly obtained from the BaseCalls folder,Narwhal is able to make its own unique run-folder. In this case a
working directory is required.

1A) python %s -s <sample-sheet> <Basecalls directory> <Working directory>

The output folder can also be explicitely set by using the -o option 

1B) python %s -s <sample-sheet> -o <output directory> <Basecalls directory> 

Narwhal can also process previously constructed FastQ files. In this case, a differently formatted sample-sheet is required. To run
Narwhal in this mode, the "-f flag" should be provided.

2) python %s -fs <sample-sheet> -o <output directory>

Options:
-s/--samplesheet	the samplesheet
-o/--outdir		the output directory
-f/--fastq-input	use Narwhal in FastQ mode
-D/--DRYRUN		perform a DRYRUN only
-h/--help		display the help message

<Basecalls directory> 	the directory containing the Illumina QseQ files
<Working directory>	the directory in which Narwhal should create a new run folder


""" % (message, sys.argv[0],sys.argv[0],sys.argv[0])
    if error: sys.exit( error )



if __name__ == '__main__':

    FROM_FASTQ  = False 

    args = []
    d_basecalls = None 
    d_workdir   = None
    d_outdir    = None
    f_samplesh  = None

    # setup the logger so we can log what happens
    logging.basicConfig( format="%(asctime)s:%(module)s\t%(message)s", level=logging.DEBUG, stream=sys.stdout )
    logging.info( "Started %s" % sys.argv[0] )
    logging.info( "Parsing commandline options from '%s'" % ' '.join(sys.argv[1:]))

    #
    # parse the options
    #
    try:
        opts, args = getopt.getopt( sys.argv[1:], "o:fhs:D", ["outdir", "fastq-input", "help", "samplesheet", "DRYRUN"] )

        for o,a in opts:
            if o in ('-s', '--samplesheet'):
                # get the sample sheet
                f_samplesh = a 
            if o in ('-D', '--DRYRUN'):
                # determine whether we are doing a dry run
                DRYRUN = True
            if o in ('-f','--fastq-input'):
                FROM_FASTQ = True
            if o in ('-o','--outdir'):
                d_outdir = os.path.abspath(a)

    except getopt.GetoptError, err:
        usage( str(err), error=2 )

    if not f_samplesh or not os.path.exists( f_samplesh ):
        usage( "The sample sheet (%s) was not found." %str(f_samplesh), error=2 ) 

    # parse the non getop options
    if len(args) == 2:
        d_basecalls = os.path.abspath( args[0] )
        d_workdir   = os.path.abspath( args[1] )
   
        if not os.path.exists( d_basecalls ):
            usage( 'basecall directory (%s) does not exist' % d_basecalls, error=2 )
    elif FROM_FASTQ :
        if not os.path.exists( d_outdir ):
            usage( 'Output directory (%s) does not exist' % d_outdir, error=2 )
    elif d_outdir != None and len(args) == 1:
        d_basecalls = os.path.abspath( args[0] )
        if not os.path.exists( d_basecalls ):
            usage( 'basecall directory (%s) does not exist' % d_basecalls, error=2 )
    else:
        usage( 'The BaseCalls and WorkDir arguments are not defined', error=2 )


    #
    # start the workflow
    # 
    if not FROM_FASTQ:
        # get the new working directory
        if d_outdir == None:
            dn_flowc = _get_flowcell_dirname( d_basecalls )
            d_outdir = _outdir( d_workdir, dn_flowc )

        logging.info( "Output will be written to %s" % d_outdir )


        # read the sample sheet
        a_samples = _read_sample_sheet( f_samplesh )     
        logging.info( "%d Samples parsed from samplesheet %s" %(len(a_samples), f_samplesh) )    

        # create the directory structure
        odirs = create_dir_structure( d_outdir, sampleids=[ s[0] for s in a_samples] ) 
        logging.info( "Created %d output directories" % len(odirs) )

        # make the list file that will govern the qseq to fastq conversion 
        a_qseq  = _get_qseq_paths( d_basecalls )
        a_fastq = _qseq_fastq_lst( odirs[0], a_qseq )
        logging.info( "Prepared %d QseQ to FastQ conversions" % len(a_fastq) )
    
        # make the list file that will govern the sample demultiplexing
        a_sfastq = _process_singleplexes( odirs[1], a_fastq, a_samples )
        a_dindex, a_dfastq = _process_multiplexes( odirs[1], a_fastq, a_samples )
        logging.info( "Prepared singleplex mergere to %d FastQ files" % len(a_sfastq) )
        logging.info( "Prepared demultiplexing and mergere to %d FastQ files from %d index files" % (len(a_dfastq), len(a_dindex)) )    
     
        # 
        a_fastq = []
        [a_fastq.append( t )  for t in a_sfastq ]
        [a_fastq.append( t )  for t in a_dfastq ]
    else :
        # 
        logging.info( "Running NARWHAL in FastQ mode" )
        logging.info( "Output will be written to %s" % d_outdir )
        a_samples = _read_fastq_sample_sheet( f_samplesh ) 
        odirs     = create_dir_structure( d_outdir, sampleids=[ s[0] for s in a_samples] ) 

        # prepare the FastQ files
        a_fastq = []
        for s in a_samples:
            [ a_fastq.append( (s[0], f) ) for f in s[1]['files'] ]
   
    # make the options files for the alignments
    a_samf = _prepare_alignment_options( odirs[2], a_fastq, a_samples )  
    logging.info( "Prepared alignments to %d SAM files" % len(a_samf) )

    # make the options files for the SAM to BAM conversions
    a_bamf = _prepare_SAMBAM( odirs[2], a_samf )
    a_pdf  = _prepare_stats( odirs[3], a_bamf )
    logging.info( "Prepared file conversions and statistics for %d and %d BAM files" % (len(a_bamf),len(a_pdf) ))
    logging.info( "Finished %s" % sys.argv[0] )


    # 





