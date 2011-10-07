
'''

 A module to index and demultiplex FastQ files

'''

import  os.path, logging, tempfile, warnings 
from ngs.fastq import FastQ

class Demultiplex:

    def __init__( self, mismatches=0 ):
        '''

        '''
        #
        logging.info( "Initializing Demultiplexer with %d mismatches" % mismatches )
        self._mismatches = mismatches
        self._codes      = []
        self._unknown    = 'UNKNOWN'
        self._indexpath  = None

        if self._mismatches > 0:
            self._matcher = Demultiplex.edit_distance
        else:
            self._matcher = Demultiplex.direct 

    @staticmethod
    def direct( a, b ):
        '''
        Direct string comparison method

        Args:
        a - string a
        b - string b

        Returns:
        0 (perfect match) or 1 (mismatch)
        '''
        if a == b:
            return 0
        else:
            return 1
        
    @staticmethod
    def edit_distance(a,b):
        '''
        the calculates the hamming edit distance between 2 string of equal length

        Args:
        a - string a
        b - string b

        Returns:
        the edit distance between them
        '''
        if len(a) != len(b):
            return 100
        else:
            return sum( c1 != c2 for c1,c2 in zip(a,b) )
    
    def register_barcode(self, sampleid, start, end, barcode):
        '''
        register barcodes

        self
        sampleid    --  the sample id to return
        start       --  the start position
        end         --  the end position
        barcode     --  the barcode sequence

        Returns:
        None
        '''

        start = int( start )
        end   = int( end )
        if (end - start) != len( barcode ):
            raise ValueError( 'Barcode of sample %s does not fit in the specified matching area' % sampleid )
        self._codes.append( {
            'id'      : sampleid,
            'start'   : start,
            'end'     : end,
            'barcode' : barcode
            } )
        logging.info( "Adding the barcode information for sample %s" % sampleid )
        return None        
        
    def fastq_index( self, file, indexpath=None ):
        '''
        Builds the index file

        Args:
        self      --  a reference to self
        file      --  the file to base the index on. If this path is not
                      defined, a temporary file will be created.
        indexpath -- the path to save the index to 

        Returns:
        true/false
        '''
        
        if indexpath == None:
            (h,indexpath) = tempfile.mkstemp( prefix='bowtie_wrapper_', suffix='.index' )

        #
        logging.info( "Indexing file %s to file %s" % (file, indexpath)  )
        
        # open the input fastq file
        fq = FastQ( file, 'r' )

        # open the index to write to it 
        self._indexpath = indexpath
        o    = open( indexpath, 'w')

        for (id,seq,qual) in fq:

            # get the sample name and reset the buffer
            name = self._which_sample( seq ) 
            o.write( "%s\t%s\n" % ( id, name) )        

        # close the file handles
        o.close()
        fq.close()
    
    def _which_sample( self, seq ):
        '''
        determine the sample name

        Args:
        self
        seq   --  the sequence to search in

        Returns:
        the sample name
        '''
        name = self._unknown
        
        # foreach barcode
        for cd in self._codes:

            # get the sequence and determine the distance to the barcode
            s = seq[ cd['start']:cd['end'] ]
            d = self._matcher( s, cd['barcode'] )

            # if the number of mismatches is smaller than distance 
            if d <= self._mismatches:
               name = cd['id']
               break
            
        # return the sample id
        return name

    def divide_fastq( self, path, outdir=None, indexpath=None ):
        '''
        divide the fastq file provided

        Args:
        self      - 
        fname     - the path of the fastq file to divide
        indexpath - (optional) 

        Returns:
        a dict with the sample ids and the output file names
        
        '''
        if not os.path.exists( path ):
            raise ValueError( 'FastQ file %s not found' % path )

        #
        if indexpath == None and self._indexpath == None:
            raise ValueError( 'No path to the index file provided' )
        elif indexpath == None:
            indexpath = self._indexpath

        # get the output directory
        if outdir == None:
            outdir = os.path.dirname( path )

        # 
        logging.info( "Dividing file %s using index %s" % (path, indexpath) )
        
        # get the output file names
        basen  = os.path.basename( path )
        basen  = basen.replace( '.fastq', '' )
        basen  = basen.replace( '.fq', '' )
        fnames = {
            self._unknown : os.path.join( outdir, basen + '.UNKNOWN.fastq' )
            }        
        for cd in self._codes:
            nm = basen + '_' + cd['id'] + '.fastq'
            fnames[ cd['id'] ] = os.path.join( outdir, nm )

        try:
            # open the input file handles
            fq = FastQ( path, 'r' )
            fi = open( indexpath, 'r' )
            
            # open the output file handles
            output = {}        
            for k in fnames.keys():
                output[k] = FastQ( fnames[k], 'w' )
                
                #
            for (id,seq,qual) in fq:        
                sl  = fi.readline().rstrip()
                if sl == '':
                    raise ValueError( 'The index file (%s) contains fewer reads than the input file (%s)' % (indexpath, path) )
                aid    = sl.split( '\t' )
                sample = aid[1]
                # if the sample was defined write to the output 
                if output.has_key( sample ):                
                    output[ sample ].write( id, seq, qual )
                else:
                    warn( 'sample id %s is not known' % sample )
                    


        except ValueError as (errno, strerr):
            pass
        finally:
            # close the handles
            fq.close()
            fi.close()
            for k in output.keys():
                output[k].close()

        # return the fnames dict
        return fnames

if __name__ == '__main__' :

    pass
