
'''

 A module to run the Bowtie aligner 

'''


import subprocess, os.path, logging, tempfile, warnings

from ngs.alignment import Aligner


class Bowtie(Aligner):

    '''
    A class to run the bowtie 

    '''

    def __init__( self ):
        '''

        '''
        self._locs = {}

        logging.info( "Initializing Bowtie object" )
             # set the nice loc        
        self._locs['nice']   = 'nice'
        self._locs['bowtie'] = 'bowtie'


    def is_available( self, name ):
        '''
        check whether the index is available

        At the moment this function is not implemented and will always return True

        Args:
        self
        name  -  the name of the index file

        Returns:
        True or False        
        '''
        return True 

    def align( self, index, infiles, outfile, opt={} ):
        '''
        Use Bowtie to align the reads in the infile
        and output the data to outfile.

        Args:
        self     -  a reference to self
        infile   -  the input fastq file
        index    -  the index name to use
        outfile  -  the output sam file
        opt      -  options to pass to the alignment
        
        Returns:
        
        '''

        # check whether they exist
        for fn in infiles:
            if not os.path.exists( fn ):                
                logging.warn( 'Input file %s does not exist' % fn )
                return None

        retval = None
        if len( infiles ) == 2:
            self._index   = index
            self._infile  = infiles
            self._samfile = outfile            
            retval = self._PairedEnd( opt )
        elif len(infiles) == 1:
            self._index   = index
            self._infile  = infiles[0]
            self._samfile = outfile
            retval = self._SingleRead( opt )
        else:
            logging.warn( 'Incorrect number of input files' )
            return None
        #
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
