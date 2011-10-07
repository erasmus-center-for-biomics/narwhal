
import tempfile, subprocess, shlex, logging

class Aligner():

    '''
    A base class for aligners

    '''

    
    def __init__(self):
        pass

    def is_available( self, index ):
        pass

    def align( self, index=None, infiles=[], outfile=None ,opt={} ):
        pass

      
    def _add_options( self, lst, opt={} ):
        '''


        '''
        # add the options
        for k in opt.keys():
            if k != "" and k != None:
                lst.append( str(k) )
                if opt[k] != None and opt[k] != '':
                    lst.append( str(opt[k]) )
        #
        return lst

    def _subprocess(self, cmd, stdout=None, stderr=None, stdin=None ):
        '''
        Run sub-process and save the output

        Args:
        self   -  reference to self
        cmd    -  the command line
        stdout -  a filepath to capture the standard out messages
        stderr -  a filepath to capture the standard error messages

        Returns:
        a tuple with the error code, stdout file
        path and the stderr path
        '''

        fin = None
        if stdin != None:
            fin = open( stdin, 'r' )
            
        # prepare the output file
        fout = None
        if stdout == None or stdout == '':
            (f,p)  = tempfile.mkstemp( prefix='alignment', suffix='.mess')
            fout   = open( p, 'w' )
            stdout = p
        else:
            fout = open( stdout, 'w' )

        # prepare the error file
        ferr = None
        if stderr == None or stderr == '':
            (f,p)  = tempfile.mkstemp( prefix='alignment', suffix='.err' )
            ferr   = open( p, 'w' )
            stderr = p
        else:
            ferr = open( stderr, 'w' )

        #
        logging.info( "Calling subprocess with the %s as the stdout path and %s as the stderr path" % (stdout, stderr) )
        logging.info( cmd )
      
        # call the subprocess
        pop = subprocess.Popen( cmd, stderr=ferr, stdout=fout, stdin=fin  )
        
        logging.info( "subprocess running with pid %d" % pop.pid )
        logging.info( "Waiting for it to finish" )
        pop.wait()

        #
        if fin != None: fin.close()
        ferr.close()
        fout.close()

        # return the error-code and file paths
        return (pop.returncode, stdout, stderr ) 

    
