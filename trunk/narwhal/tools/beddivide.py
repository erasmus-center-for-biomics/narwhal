#!/bin/env python

'''

A python script to divide the bedgraph files to seperate
tracks according to their chromosome(s)

'''

import os

class Divide :

    def __init__( self, fname, force=False ):
        '''

        '''
        self._outfname = fname
        self._track    = ''
        self._f        = None
        self._pchr     = None 
	self._generated = []
        self._force     = force

    def close( self ):
        self._f.close()
    

    def process( self, line ):
        '''
        process the file per line

        Args:
        self  --  reference to self
        line  --  the current line

        Return:
        nothing
        '''        
        
        if line[0:5] == 'track' :
            # assign a new track line
            self._track = line
        else :
            # 
            chr = line.split(' ')[0]
            
            if chr != self._pchr :
                # we are dealing with a different chromosome:
                # * close the file determine the new output file
                # * open a new file 
                # * add the track information

                if self._f != None: self._f.close()
                fname   = self._outfname.replace( '[chr]', chr ) 
		self._generated.append( fname )
                self._f = open( fname, 'a' )
                self._f.write( self._track )        
                
                if len( self._generated ) > 200 and not self._force :
                    self._f.close()
                    for f in self._generated:
                       os.remove( f )
                    raise ValueError("Too many output files generated: sanatizing directory\n(Too force file creation use the -f swith)") 
                    

            # write the line
            self._f.write( line )
                
            self._pchr = chr 
        # end of function
        
if __name__ == '__main__':
    

    import sys, getopt


    def usage():
        return '''
Usage: %s -n <basename> < <input bed file> 

Options:
-n/--name   -   then basename of the new file with [chr] where the chromosome name should be set (default [chr].bed)
-f/--force  -   force the creation of over 200 files.
''' % (sys.argv[0] )
    

    argv       = sys.argv[1:]
    opts, args = getopt.getopt( argv, 'n:hf', ['name', 'help', 'force'] )
    force      = False

    # option assignment
    name = '[chr].bed'
    for o, a in opts:
        if o in ('-h', '--help'):
            usage()
            sys.exit( 0 )
        elif o in ('-n', '--name'):
            name = a
        elif o in ('-f', '--force'):
            force = True

    # build the divider object 
    divide = Divide( name, force=force )
    # process each line
    for line in sys.stdin :
        val = divide.process( line )
    # close the divider
    divide.close()
