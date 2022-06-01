# User guide

## Samplesheet

NARWHAL will obtain sample information from a tab-delimited sample-sheet. The sample-sheet is a tab-delimited format with a fixed column order.

| Column | Description | Comment |
| ------ | ----------- | ------- |
| 1 | Sample ID | This value should not contain spaces. The LaTEX document processor used to create the PDF QC report is not able to process samples with spaces |
| 2 | Lane number | The lane number in which the sample is present. |
| 3 | Data reads* | If multiple data reads are present these should be comma delimited. |
| 4 | multiplex sequence (barcode)* | The barcode sequence. |
| 5 | multiplex start sites* | In NARWHAL, barcodes are not required to start at the first base. In this field, the 0-offset barcode start should be present. |
| 7 | Index read* | The read that contains the barcode sequence. |
| 8 | Reference sequence | The path to the reference sequence. |
| 9 | Paired end alignment | Should a paired-end alignment be performed? |
| 10 | Application profile | The alignment settings and tool that will be used to align the data files for this sample are specified by the application profile. The application profiles are specified in the json configuration file. |
| 11 | Additional options | Additional options can be specified in this field. The options should be comma delimited in a specific option format. This option format should be [sub-tool]:key=value. Key and value are mandatory and the sub-tool is optional. For an example see example 1. |

It is possible to have a sample in multiple lanes on the flow-cell. The lanes,
data reads and barcodes should be comma delimited. The data reads themselves
should be semicolon delimited. For example, when the same sample is present in
lanes 1 and 2; the lane column should contain 1, 2. The barcode column should
be something like ACTG, ACTG. If the sample was sequenced in a paired-end
manner with reads 1 and 3, the data reads column should be 1;3,1;3. For more
examples see the examples files attached to this page.

NARWHAL can also be run in FastQ mode (see below under Detailed run), in which
the input is one fastq file per sample. It then requires a simpler samplesheet,
which is still tab-delimited:

| Column | Description | Comment |
| ------ | ----------- | ------- |
| 1 | Sample ID | This value should not contain spaces, as above |
| 2 | FastQ File | Path to the FastQ input file for the sample |
| 3 | Reference sequence | The path to the reference sequence (as above) |
| 4 | Paired end alignment | Should a paired-end alignment be performed? (as above) |
| 5 | Application profile | as above. |
| 6 | Additional options | as above. |

## Quick start

After creating the sample-sheet, NARWHAL can be run using a single command:

```
# simple run
> $narwhal_path/bin/narwhal.sh -s path_to_sample_sheet path_to_BASECALLS path_to_OUTPUT
``` 

The `narwhal.sh` script runs the `prepare_run.py` and the `run_tools.sh` scripts in sequence.

## Detailed run: dry-run for error checking of samplesheet

In most cases, users may want to perform some error checking prior to running a long-running analysis.

```
### Perform a dry run to test the configuration
> python $narwhal_path/bin/prepare_run.py -Ds path_to_sample_sheet path_to_BASECALLS path_to_OUTPUT
#
# Output describing the various processes that will be performed
#

### setup the run 
# make the working directory
> python $narwhal_path/bin/prepare_run.py -s path_to_sample_sheet path_to_BASECALLS path_to_OUTPUT
# Output describing the various processes that will be performed
```

The `prepare_run.py` script will setup the analysis output folder and all of the parameter files required to run the Narwhal tools. This runfolder has the following fixed folder structure:

```
output_folder/NARWHAL_illumina_runfolder/
    raw_data/
    demultiplexed/
        sample_name_1/
        ..
        sample_name_N
    alignment/
        sample_name_1/
        ..
        sample_name_N/
    stats/
        sample_name_1/
        ..
        sample_name_N/
    projects
```

The raw_data folder will contain the FastQ obtained from the Qseq conversions. These FastQ files are obtained per tile. These will be used in the demultiplex procedure that will write its ouput in the demultiplexed folder. The alignment uses those files and writes the aligned reads in SAM format to the alignemnt folder. In that folder, the SAM to BAM format conversions will be performed. The output from the QC analysis will be written to the stats folder. The projects folder is reserved for sample specific analyses.

## setup the run in FastQ mode
To run NARWHAL in FastQ mode, use the -f flag. In this mode, you need to specify the output folder with -o, 
as there is probably no `flowcell_ID/runfolder` name to parse and create a standard output folder.

```
> python $narwhal_path/bin/prepare_run.py -f -s path_to_sample_sheet -o path_to_OUTPUT
```

## start the run
To start the analyses, simply run the `run_tools.sh` script on the runfolder.

```
# run the tools
> bash $narwhal_path/bin/run_tools.sh $runfolder
```

## Output
After the `run_tools.sh` script has finished, numerous files will have been created in the various sub-folders of the runfolder. Depending on which files need to be exported to the researchers, the operator will need to go to the demultiplexed of alignment folders. Of particular importance is the stats folder. This folder holds the alignment QC reports generated from the BAM files.

In the QC reports, information is present on the general alignment statistics and figures showing the edit rates, alignment percentage per chromosome, read-length distribution and replication distributions. Examples of these graphs are shown below:

### Table 1. Alignment statistics

| Total number of reads | 49922743 |
| Number of aligned reads | 48165331 |
| Alignment percentage | 96.4 |
| Forward reads | 21304254 |
| Reverse reads | 26861077 |
| Edit-rate | 0.0033234 |

### Fig. 1. edit distance

Count of reads that align at the reference genome versus the number of mismatches (edit distance) in this alignment.
[edit_distance.PNG](doc/edit_distance.PNG)

### Fig. 2. Alignment distribution

Count of reads that aligns at each of the chromosomes present in the reference genome.
[alignment.PNG](doc/alignment.PNG)

### $ Fig. 3. Read length

Count of reads versus the length of the read.
[length.PNG](doc/length.PNG)

### Fig. 4. Replication rate

Count of reads versus replication number. A replication number of n means that the read sequence has been sequenced n + 1 times (so is replicated n times).
[replication.PNG](doc/replication.PNG)

## some examples

- [example_simple.txt](doc/example_simple.txt)
- [example_multiplex.txt](doc/example_multiplex.txt)
- [example_multiplex_multiple_lanes.txt](doc/example_multiplex_multiple_lanes.txt)
- [example_fastq_mode.txt](doc/example_fastq_mode.txt)

