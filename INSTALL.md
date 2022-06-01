# Installation instructions

## Prerequisites

- Interpreters
  -- Python (> 2,6)
  -- BASH
  -- R
- GNU/Linux software
  -- gzip
  -- pdflatex
  -- nice
  -- gawk
  -- GCC (optional)
  -- libz (optional)
- Bioinformatics tools: see below

## Bioinformatics tools needed to run NARWHAL

NARWHAL relies on a number of previously published Bioinformatics tools. These are all open-source and freely available. We have not tested Narwhal with all version of these tools, but we expect no problems with newer versions of these tools. However, to ensure proper operation of Narwhal explicitely compatible versions of these tools are listed below. Through the version control mechanisms present in the hosting websites, these versions should be still available available.

| Tool | Version | URL |
| ---- | ------- | --- |
| Bowtie | 0.12.5 | [http://bowtie-bio.sourceforge.net/](http://bowtie-bio.sourceforge.net/) |
| BWA | 0.5.9 | [http://bio-bwa.sourceforge.net/](http://bio-bwa.sourceforge.net/) |
| TopHat? | 1.3.1 | [http://tophat.cbcb.umd.edu/](http://tophat.cbcb.umd.edu/) |
| SAMtools | 0.1.18 | [http://samtools.sourceforge.net/](http://samtools.sourceforge.net/) |
| BAMtools (optional) | 0.9 | [https://github.com/pezmaster31/bamtools]([https://github.com/pezmaster31/bamtools) |

## Downloading NARWHAL

NARWHAL can be easily installed on most UNIX(r) based operating systems. The stable version can be downloaded from this website (see Downloads) or checked out from SVN. The downloaded tar.gz file needs to be unpacked in a directory of choice:

``` 
> wget link_to_stable_version.tar.gz
> gunzip narwhal.tar.gz
> tar -xf narhwal.tar
``` 

For the SVN download do the following:

```
svn co https://trac.nbic.nl/svn/narwhal
```

Be warned the SVN version is bleeding edge and not guaranteed to work!

This procedure should create several files and directories in the directory similar to the following:

```
> ls narwhal
bin  conf  lib  src  tools
```

## Linking external tools

The tools used by NARWHAL should be present in the users PATH. As it may be undesirable to make the NARWHAL tools available to all users all the time, NARWHAL will add the tools directory to the PATH before running the tools. By placing symbolic links to tools in the tool directory that will be called by the NARWHAL.

```
> cd tools
> ln -s $path_to_bowtie bowtie                       # for Bowtie and Tophat alignments
> ln -s $path_to_bowtie-build bowtie-build           # for Tophat alignments
> ln -s $path_to_bowtie-inspect bowtie-inspect       # for Tophat alignments
> ln -s $path_to_tophat tophat                       # for Tophat alignments
> ln -s $path_to_bwa bwa                             # for BWA alignments
> ln -s $path_to_samtools samtools
> ln -s $path_to_nice nice
> ln -s $path_to_gawk gawk
```

## Re-compiling C tools

In the tools directory precompiled versions of the data-processing tools are present. These need to be recompiled if your system is not 64-bit i386 compatible The sources of these tools can be found in `${NARWHAL_dir}/src/cpp directory`. To recompile a tool type "make clean; make" in the tool root directory. After compilation, the new executable needs to be copied from the tool bin directory to `${NARWHAL_dir}/tools`.

This procedure is outlined for the bamstats tool. For some tools the Bamtools libraries should be installed.

```
> cd ${NARWHAL_dir}/src/cpp/bamstats
> make clean
> make
# lots of make output
> cp bin/bamstats ${NARWHAL_dir}/tools/
```

## Environmental variables

During a NARWHAL run, the `PYTHONPATH` and `NARWHAL_TOOLS` environmental variables need to be set correctly. 
In normal use, these variables are set by the `run_tools.sh` script based on the NARWHAL installation directory.

```
> export NARWHAL_TOOLS="${narwhal_dir}/tools"
> export PYTHONPATH=$PYTHONPATH:"${narwhal_dir}/tools/lib/python/"
```

NARWHAL should now be installed. To analyze data with NARWHAL, see the Quickstart guide
