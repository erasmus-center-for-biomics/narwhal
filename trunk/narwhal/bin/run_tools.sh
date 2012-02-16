#!/bin/env bash 

#
# 
#

#
#
# FUNCTIONS
#
#

usage() {
    echo $1
    echo "
Usage: run_tools.sh -j [maxjobs] RUNFOLDER



"
    exit 2
}


# VARIABLES
#
idir=$( readlink -f $0 )
idir=$( dirname $idir )
idir=${idir/%bin/}

align="${idir}tools/align.py"			# the location of the alignment script
profile="${idir}conf/profiles.json"		# the default location for the profiles
MAXJOBS=8					# run at most 8 parallel processes
RUNFOLDER=""					# the run folder to write the results in 
mismatches=0

echo "`date` Started"

#
# PARSE THE OPTIONS
#
while getopts 'j:m:' opt; do
    case $opt in
        'j')
            MAXJOBS=$OPTARG
            ;; 
        'm')
            mismatches=$OPTARG
            ;;
    esac
done

#
shift $(($OPTIND - 1))
RUNFOLDER=$( readlink -f $1 ) 
echo "`date` Processing folder ${RUNFOLDER}"

# ADD ENVIRONMENTAL VARIABLES
#
#

# Modify the path as to include the narwhal tools directory 

NARWHAL_TOOLS="${idir}tools/"
export PATH=$NARWHAL_TOOLS:$PATH
export PYTHONPATH=$PYTHONPATH:"${idir}/lib/python/"


# PREFLIGHT CHECKS
#
# determine whether the neccessary tools are in the path
tools=( nice cat sync qseq2fastq samtools demultiplex bamstats parallel)
for t in ${tools[@]} ; do
    which $t > /dev/null 2> /dev/null
    (( $? != 0 )) && usage "Tool ${t} is not present in the PATH" 
done

# DATA PREPROCESSING
#
#
#

# Convert the qseq to fastq files
optfile="$( find $RUNFOLDER -name 'qseq2fastq.param' )"

if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then 
	echo "`date` Processing qseq conversions from ${optfile}"
	parallel -a $optfile --colsep ';' -j $MAXJOBS qseq2fastq {1} {2}

fi

# Concatenate the tile FastQ files to he correct sample files. Parallelisation of this step 
# should be on the output file level.
optfile="$( find $RUNFOLDER -name 'concatenate.param')"
if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then
	echo "`date` Processing non multiplexed reads from ${optfile}"
        parallel -a $optfile --colsep ';' -j 1 "cat {1} >> {2}; sync;"
fi


# Index the tiled FastQ reads. For each tile continaing a barcode an independent index will be created.  
optfile="$( find $RUNFOLDER -name 'demultiplex_index.param' )"
if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then
	echo "`date` Indexing multiplexed FastQ files from ${optfile}"
        parallel -a $optfile --colsep ';' -j $MAXJOBS demultiplex index -f {1}  -i {2} -b {3} -s {4} -c {5} -m $mismatches
fi

# Divide the reads 
optfile="$( find $RUNFOLDER -name 'demultiplex_divide.param' )"
if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then
	echo "`date` Dividing multiplexed FastQ files from ${optfile}"
        parallel -a $optfile --colsep ';' -j 1 demultiplex divide -f {1}  -i {2} -o {3} -c {4}
fi

# ALIGNMENT
#
#
#

# Perform the alignment by running the python alignment script
optfile="$( find $RUNFOLDER -name 'alignment.param' )"
if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then
	echo "`date` Aligning samples from ${optfile}"
	parallel -a $optfile --colsep ';' -j 1 $align -p $profile -f "{1}" -s "{2}" -r "{3}" -t "{4}" -o "{5}" -P "{6}"
fi

# DATA POSTPROCESSING
#
#
#

# Convert the SAM files to sorted BAM files etc.
optfile="$( find $RUNFOLDER -name 'sambam.param' )"
if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then
	echo "`date` Performing file format conversions from ${optfile}"
        parallel -a $optfile --colsep ';' -j $MAXJOBS "samtools view -Sbo {2} {1}; samtools sort {2} {3}; samtools index {4};"  
fi

# Calculate statistics over the BAM files
optfile="$( find $RUNFOLDER -name 'bamstats.param' )"
if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then
	echo "`date` Generating statistics from ${optfile}"
        parallel -a $optfile --colsep ';' -j $MAXJOBS "bamstats {1} {3} >> {4} ; plotter.R --prefix {3} ; quality_info.sh -p {3} -n {2} -s {4} -o {5}"
fi

# Generate overview tables of demultiplexing and alignment statistics
optfile="$( find $RUNFOLDER -name 'demultiplex_index.param' )"
if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then
	echo "`date` Generating demultiplexing statistics overview"
	gawk -f ${idir}tools/demultiplex_stats.awk ${RUNFOLDER}/demultiplexed/*.index > ${RUNFOLDER}/stats/demultiplex_stats_overview.txt
fi
optfile="$( find $RUNFOLDER -name 'alignment.param' )"
if [[ -f $optfile ]] && [[ $(wc -l $optfile) > 0 ]] ; then
	echo "`date` Generating alignment statistics overview"
	gawk -f ${idir}tools/alignment_stats.awk ${RUNFOLDER}/stats/*/*.stats > ${RUNFOLDER}/stats/alignment_stats_overview.txt
fi


# DATA PACKAGING
#
#
#

# pack the largest data
find $RUNFOLDER -name '*.sam' >> $RUNFOLDER/pack.param
find $RUNFOLDER -name '*.fastq' >> $RUNFOLDER/pack.param
find $RUNFOLDER -name '*.index' >> $RUNFOLDER/pack.param

echo "`date` Packing output data"
parallel -a $RUNFOLDER/pack.param -j $MAXJOBS gzip --best {}
echo "`date` Finished"

