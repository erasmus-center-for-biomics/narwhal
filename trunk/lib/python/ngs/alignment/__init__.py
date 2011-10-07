
import os

#if not os.environ.has_key( 'NARWHAL_TOOLS' ):
#    raise ImportError( "NARWHAL tool path not found" )

# declare this variable first as it may create circular references
#flocs = {
 #   'nice'   : '%s/nice' % os.environ['NARWHAL_TOOLS'],
 #   'bwa'    : '%s/bwa' % os.environ['NARWHAL_TOOLS'],
 #   'bowtie' : '%s/bowtie' % os.environ['NARWHAL_TOOLS'] 
 #   }


# import the classes 
from ngs.alignment.base import Aligner
from ngs.alignment.bwa import BWA
from ngs.alignment.bowtie import Bowtie
from ngs.alignment.tophat import Tophat
