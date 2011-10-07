
'''



'''

import subprocess, os.path, logging, tempfile, warnings

from ngs.alignment import Aligner


class BWA(Aligner):

    '''
    A class to run the BWA aligner

    '''

    #
    # in order to perform a BWA alignment first an
    # sam index (sai) should be constructed. Seperately
    # for the forward and reverse read in the case of a 
    # paired end
    #
    #
    #
    #
    #
    #
    
    def __init__( self ):
        '''


        '''
        self._locs = {}
        logging.info( "Intializing the BWA object" )

        # set the nice loc        
        self._locs['nice'] = 'nice'
        self._locs['bwa']  = 'bwa'


    def is_available( self, fname ):
        '''
        Checks whether the specified index is available

        Args:
        self   -
        fname  - the file path to check for
        
        Returns:
        True or False        
        '''
        suffixes = [ 'amb', 'ann', 'bwt', 'fai', 'pac', 'sa', 'rbwt', 'rpac', 'rsa' ]
        present = True
        for s in suffixes:
            fn = fname + '.' + s
            if not os.path.exists( fn ):
                present = False
                
        # return whether the index is present or not
        return present

            
    def align( self, index, infiles, outfile, opt={}, saifiles=[] ):
        '''
        Use the BWA aligner to align the reads in infiles
        and output the data to the outfile

        Args:
        self     -  a reference to self
        infiles  -  the input FastQ file
        index    -  the index to which to align
        outfile  -  the output SAM file
        opt      -  options to pass to the alignment 

        Returns:
        A tuple with the error, samfile and error file
        '''
        
        retval = None
        opt    = self._add_default_options( opt )
        if len( infiles ) == 2:            
            self._index   = index
            self._samfile = outfile
            # saifiles = []

            # perform the alignment for each read
            if len(saifiles) != len(infiles):
                for f in infiles:
                    if not os.path.exists( f ):
                        logging.warn( "input file %f does not exist")
                        return None
                    self._infile  = f
                    self._saifile = "%s.sai" % f                
                    self._aln( opt=opt['aln'] )
                    saifiles.append( self._saifile )

                # gather the results
                self.infile  = infiles
                self.saifile = saifiles
                retval = self._sampe( opt=opt['sampe'] )
            
        elif len(infiles) == 1:
            self._index   = index
            self._infile  = infiles[0]
            if len(saifiles) == 1:
                self._saifile = saifiles[0]
            else:
                self._saifile = "%s.sai" % infiles[0]            
            self._samfile = outfile

            # perform the alignment and results gathering
            self._aln( opt=opt['aln'] )
            retval = self._samse( opt=opt['samse'] )
        else:
            logging.warn( 'Incorrect number of input files' )

        #
        return retval

        
    def _aln( self, opt={} ):
        '''
        Perform the alignment and make the sam index file

        Args:
        self
        opt 

        Returns:
        '''
        # 
        cmd = []
        cmd.append( self._locs['nice'] )
        cmd.append( self._locs['bwa'] )
        cmd.append( 'aln' )
        
        # add the options
        cmd = self._add_options( cmd, opt )

        # add the index
        cmd.append( self._index )
        cmd.append( self._infile )

        # run the process
        (err,pout,perr) = self._subprocess( cmd, stdout=self._saifile ) 

        #
        return (err,pout,perr)
        
    
    def _samse( self, opt={} ):
        '''
        Assemble the single read alignment file

        Args:

        Returns:
        
        '''
        #
        cmd = []
        cmd.append( self._locs['nice'] )
        cmd.append( self._locs['bwa'] )
        cmd.append( 'samse' )

        #
        cmd = self._add_options( cmd, opt )

        #
        cmd.append( self._index )
        cmd.append( self._saifile )
        cmd.append( self._infile )

        #
        (err,pout,perr) = self._subprocess( cmd, stdout=self._samfile )

        #
        return (err,pout,perr)

    def _sampe( self, opt={} ):
        '''
        Assemble a paired end alignment

        Args:

        Returns:
        
        '''
        #
        cmd = []
        cmd.append( self._locs['nice'] )
        cmd.append( self._locs['bwa'] )
        cmd.append( self._locs['sampe'] )

        #
        cmd = self._add_options( cmd, opt )
        cmd.append( self._index )

        #
        cmd.append( self._saifile[0] )
        cmd.append( self._saifile[1] )

        #
        cmd.append( self._infile[0] )
        cmd.append( self._infile[1] )

        #
        (err,pout,perr) = self._subprocess( cmd, stdout=self._samfile )
        return (err,pout,perr)

    def _add_default_options( self, opt ):
        '''
        This method will add 'sensible' default options. 
        So NARWHAL will not crash if these are absent. This 
        function will not fill fields that are defined.

        Args:
        opt   -    the options hash

        Returns:
          return the filled options
        '''
        if not opt.has_key( 'sampe' ):
            # add the sampe options
            opt['sampe'] = {}

        if not opt.has_key( 'samse' ):
            # the samse options
            opt['samse'] = {}

        if not opt.has_key( 'aln' ):
            # the alignment options
            opt['aln'] = {}

        return opt

if __name__ == '__main__':
    pass
