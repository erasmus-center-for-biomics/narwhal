#/bin/env bash

#
#
#
#
#
usage(){
    echo "$1" 
    echo "
Usage: narwhal.sh -j <maxjobs> -m <mismatches> -s [samplesheet] [BASECALLS folder] [OUTPUT folder]

Options
-j	maximum number on concurrent processes
-m	maximum number of mismatches allowed during demultiplexing
-s	the samplesheet
 
"
    exit 2	
}



# get the installation directory
idir=$( readlink -f $0 ) 
idir=$( dirname $idir )
idir=${idir/%bin/} 

mismatches=0
MAXJOBS=8
samplesheet=""
#
#
# PARSE THE OPTIONS
#
# parse the options
while getopts 'j:m:s:' opt; do
    case $opt in
        'j')
            MAXJOBS=$OPTARG
            ;;
        'm')
            mismatches=$OPTARG
            ;;
	's')
	    samplesheet=$( readlink -f $OPTARG )
	    ;;
    esac
done

# parse the non flagged options
shift $(($OPTIND - 1))
basecalls=$( readlink -f $1 )
outdir=$( readlink -f $2 )

# check whether these are directories
[[ -d $outdir ]] || usage "The output directory is not defined"
[[ -d $basecalls ]] || usage "The basecalls folder is not defined" 
[[ -f $samplesheet ]] || usage "samplesheet ${samplesheet} does not exist"

#
#
# SETUP THE ENVIRONMENT
#
#
NARWHAL_TOOLS="${idir}tools/"
export PATH=$NARWHAL_TOOLS:$PATH
export PYTHONPATH=$PYTHONPATH:"${idir}/lib/python/"

#
#
# RUN NARWHAL
#
#

echo $idir 
# setup the runfolder
python ${idir}/bin/prepare_run.py -s $samplesheet $basecalls $outdir > tmp.log

# get the runfolder
runfolder=$( grep "Output will be written to " tmp.log )
runfolder=${runfolder/#*Output will be written to /} 

echo "`date` Performing analysis is folder ${runfolder}"

# move the log
mv tmp.log "${runfolder}/prepare.log"

# copy the samplesheet
cp "$samplesheet" "${runfolder}/samplesheet.txt"

# run the tools
${idir}/bin/run_tools.sh -j $MAXJOBS -m $mismatches $runfolder > ${runfolder}/run.log

echo "`date` Finished"
