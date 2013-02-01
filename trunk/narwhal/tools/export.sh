#/bin/env bash

# get the parameters
indir="$1"
outdir="$2"

# get the samples and prepare the output dirs
samples=$( find demultiplexed/ -type d | sed 's/demultiplexed\///' )


# make the output dirs
for sample in ${samples[@]}; do
	mkdir -p "${outdir}/${sample}"
done

# find the BAM files and link them
for sample in ${samples[@]}; do
	input=$( ls ${indir}/alignment/${sample}/*srt.ba* )
	for file in ${input[@]}; do
		path=$(readlink -f $file)
		ln -s "$path" "${outdir}/${sample}"
	done
done


# find the BAM files and link them
for sample in ${samples[@]}; do
	input=$( ls ${indir}/demultiplexed/${sample}/*fastq.gz )
	for file in ${input[@]}; do
		path=$(readlink -f $file)
		ln -s "$path" "${outdir}/${sample}"
	done
done

# find the BAM files and link them
for sample in ${samples[@]}; do
	input=$( ls ${indir}/stats/${sample}/*pdf )
	for file in ${input[@]}; do
		path=$(readlink -f $file)
		ln -s "$path" "${outdir}/${sample}"
	done
done

