
'''

A class to handle FastQ related functions.

The functions in this module follow the File
module in naming and function.

'''

import sys

class FastQ:
    '''
    A class describing a FastQ file
    '''
    

    def __init__( self, path=None, mode='r' ):
        '''
        Initializes the FastQ object

        Args:
        self  --  reference to self
        path  --  the filepath of the FastQ file to open or None when dealing with the standard in/out 
        mode  --  the file mode to use (r or w)

        Returns:
        self
        '''
        self.fname = path
        if path != None:
            # we have a path
            self._f = open( path, mode )
        else:
            # when we would like to have data from standard out or standard in
            if mode == 'w':
                self._f = sys.stdout
            else:
                self._f = sys.stdin

                
    def __iter__(self):
        '''
        Returns the iterator object
        
        '''
        return self 

    def close( self ):
        '''
        Closes the currently open file

        Args:
        self  -  reference to self

        Returns:
        None
        '''
        if self.fname != None:
            self._f.close()

        
    def write( self, id, seq, qual, doubleid=False ):
        '''
        Writes a read to a FastQ file

        Args:
        self  - 
        id    -   
        seq   -
        qual  -

        Returns:
        None
        '''
        tup = ( id, seq, '', qual )
        if doubleid:
            tup = ( id, seq, id, qual )
        self._f.write( '@%s\n%s\n+%s\n%s\n' %  tup )
        


    def next( self ):
        '''
        Obtain the next read from the file

        Args:
        self

        Returns:
        a tuple with the id, sequence and phred quality    
        '''
        id   = None
        seq  = None
        qual = None

        #
        id   = self._f.next().rstrip()[1:]
        seq  = self._f.next().rstrip()
        self._f.next()
        qual = self._f.next().rstrip()

        #
        return (id, seq, qual)
            
        
        

    def read( self, n=0 ):
        '''
        Obtains a single read from the currently open file

        Args:
        self  -  reference to self

        Returns:
        a list of tuples with the id, sequence and phred quality        
        '''

        retval = []
        cnt    = 0
        
        # take all reads if n == 0
        if n == 0: n = -1

        # loop over the reads
        try:
            while cnt != n:
                retval.append( self.next() )
                cnt += 1 
        except StopIteration:
            pass

        # return our values
        return retval 
            
        
if __name__ == '__main__':
    
    
    '''
    

    ''' 
    def usage():
        return '''Usage:
%s test < <fastq.file> > <tabular output>
''' % sys.argv[0]
        
    if sys.argv[1] != 'test' :
        print usage
        sys.exit( 2 )

    fq = FastQ()
    for (sid,seq,qual) in fq:
        print "%s\t%s\t%s" % (sid,seq,qual) 
    fq.close()
        
        
        
        
    
