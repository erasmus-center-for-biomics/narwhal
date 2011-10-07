
'''

 A module to run the Tophat aligner 

'''


import subprocess, os.path, logging, tempfile, warnings

from ngs.alignment import Aligner


class Tophat(Aligner):

    '''
    A class to run the Tophat aligner 

    '''

    def __init__( self ):
        '''

        '''
        self._locs = {}

        logging.info( "Initializing Tophat object" )
             # set the nice loc        
        self._locs['nice']   = 'nice'
        self._locs['tophat'] = 'tophat'


    def is_available( self, name ):
        '''
        check whether the index is available

        At the moment this function is not implemented and will always return True

        Args:
        self
        name  -  path and basename of the index to check

        Returns:
        True or False        
        '''
        suffixes = [ '1.ebwt', '2.ebwt', '3.ebwt', '4.ebwt', 'rev.1.ebwt', 'rev.2.ebwt' ]
        present = True
	for s in suffixes:
	    fn = name + '.' + s
	    if not os.path.exists( fn ):
	        present = False

        return present 

    def align( self, index, infiles, outdir, opt={} ):
        '''
        Use Tophat to align the reads in the infiles
        and output outdir.

        Args:
        self     -  a reference to self
        infile   -  the input fastq file(s)
                    If this is comma-separated, multiple fastq files are regarded as 
                    technical replicates for the sample
                    If this is a list of length 2, it is regarded as to contain the 
                    inputfile(s) (possibly comma-separated for technical replicates)
                    for paired end reads.
        index    -  the index (reference genome) name to use
        outdir   -  the output directory where the bam file and other output is written
        opt      -  options to pass to the alignment
        
        Returns:
        
        '''

        # check whether the input files exist
	# the elements of infiles may be comma separated
        for fn in infiles:
            for fn1 in fn.split(','):
                if not os.path.exists( fn1 ):                
                    logging.warn( 'Input file %s does not exist' % fn1 )
                    return None

        # check if the output folder exists and is empty, or create if not
	if not os.path.exists( outdir ):
	    os.mkdir( outdir )
	else:
	    if not len( os.listdir( outdir )) == 0:
                logging.warn( 'Output folder %s is not empty' % outdir )
                return None
		
        retval = None

        # build the tophat command
        cmd = [ ]
        cmd.append( self._locs['nice'] )        
        cmd.append( self._locs['bowtie'] )

	# options from the profiles and/or samplesheet should include -G
	# check if the annotation specified with -G exists.
	if '-G' in opt:
            if not os.path.exists( opt['-G'] ):
	        logging.warn ('annotation file %s does not exist' % opt['-G'] )
		return None
	else:
	    logging.warn( 'annotation not specified with -G option' )
	    return None

	cmd = self._add_options( cmd, opt )

	# add the output location
	cmd.append( ['-o', outdir] ) 

	# add the reference genome (index)
        cmd.append( self._index )

	# add the input fastq file(s)
        if len( infiles ) == 1:
	    # single read
	    cmd.append( infiles[0] )
        elif len( infiles ) == 2:
	    # paired end
	    cmd.append( infiles[0] )
	    cmd.append( infiles[1] )
        else: 
            logging.warn( 'Incorrect number of input files' )
            return None
        
	# run bowtie
        (err,pout,perr) = self._subprocess( cmd, stdout=self._samfile )
        return retval
        
            
    def _SingleRead( self, opt={} ):
        '''

        '''
        cmd = [ ]
        cmd.append( self._locs['nice'] )        
        cmd.append( self._locs['bowtie'] )
        cmd = self._add_options( cmd, opt )
        cmd.append( '-S' )
        
        # add the index        
        cmd.append( self._index )
        
        # add the input fastq file
        cmd.append( os.path.abspath(self._infile) )

        # run bowtie
        (err,pout,perr) = self._subprocess( cmd, stdout=self._samfile )

        #
        return (err,pout,perr)

        
    def _PairedEnd( self, opt={} ):
        '''

        '''
        cmd = [ ]
        cmd.append( self._locs['nice'] )        
        cmd.append( self._locs['bowtie'] )
        cmd = self._add_options( cmd, opt )

       # add the index
        cmd.append( self._index )
        
        # add the paired end input files
        cmd.append( '-1' )
        cmd.append( self._infile[0] )        
        cmd.append( '-2' )
        cmd.append( self._infile[1] )

        # run bowtie
        (err,pout,perr) = self._subprocess( cmd, stdout=self._samfile ) 
    
        #
        return (err,pout,perr)
  
 
if __name__ == '__main__' :

    pass       
