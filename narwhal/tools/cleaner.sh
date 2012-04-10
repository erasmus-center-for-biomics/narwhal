#!/bin/env bash

# set the input directory to the current directory
INDIR="."


#
find $INDIR -name '*.sam' > files_sam.txt
find $INDIR -name '*.sam.gz' >> files_sam.txt

#
find $INDIR -name '*.index' -exec readlink -f {} \; > files_index.txt
find $INDIR -name '*.index.gz' -exec readlink -f {} \; >> files_index.txt

#
find $INDIR/raw_data -name '*.fastq' -exec readlink -f {} \; > files_raw_fastq.txt
find $INDIR/raw_data -name '*.fastq.gz' -exec readlink -f {} \; >> files_raw_fastq.txt

#
find $INDIR/demultiplexed -name '*.fastq' -exec readlink -f {} \; > files_sample_fastq.txt
find $INDIR/demultiplexed -name '*.fastq.gz' -exec readlink -f {} \; >> files_sample_fastq.txt

#
find $INDIR -name '*.srt.bam' -exec readlink -f {} \; > files_sortedbam.txt
find $INDIR -name '*.bam' -exec readlink -f {} \;| grep -v 'srt.bam$' > files_bam.txt

#
find $INDIR -name '*qseq.txt' -exec readlink -f {} \; > files_qseq.txt
find $INDIR -name '*qseq.txt.gz' -exec readlink -f {} \; >> files_qseq.txt







