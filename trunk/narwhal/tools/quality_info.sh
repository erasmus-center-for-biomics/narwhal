#!/bin/sh

# generate quality info .tex file from statistics plots created by
# bamstats.

# usage:
# generate_quality_info_tex.sh file_prefix sample_name


usage(){ 
    echo "$1"
    echo "Usage $0 -p <file prefix> -n <sample name> -s <stats file> -o <output pdf>"
    exit 2
}


#
#
# Option parsing
#
#

file_prefix=""
sample_name=""
fnstats=""
pdf=""

while getopts "p:s:n:o:h" opt ; do 
	case $opt in
		'h')
			usage "help invoked" 
			;;
		'p')
			file_prefix=$OPTARG
			;;
		'n')
			sample_name=$OPTARG
			;;
		's')
			fnstats=$OPTARG
			;;
		'o')
			pdf=$OPTARG
			;;	 
	esac
done


# if the expected EPS files do not exist
[[ -f "${file_prefix}.editdistance.txt.eps" ]] || {
    usage "The expected EPS files do not exist"
}

[[ -f "$fnstats" ]] || {
	usage "Stats ($fnstats) file was not found"
}

[[ "$pdf" == "" ]] && {
	pdf="${file_prefix}.pdf" 
}


pwd=$( pwd )
odir=$( dirname "$pdf" )
cd "$odir"

#
# the main document
#
authors='R.~W.~W.~Brouwer \\and M.~C.~G.~N.~van~den~Hout \\and W.~F.~J.~van~IJcken'

DOCUMENT='
\documentclass[a4paper]{article}
\usepackage{graphicx}
\usepackage{epstopdf}
\\begin{document}

\\title{[SAMPLE_NAME]: Illumina Hiseq sequencing and bowtie/BWA alignment quality information}
\\author{[AUTHORS]}

\maketitle

\\subsection*{General alignment statistics.}

\\begin{tabular}{ l | r }
	\\hline
	Total number of reads & [total] \\\
	Number of aligned reads & [aligned] \\\
	Alignment percentage & [paligned] \\\
	Forward reads & [forward] \\\
	Reverse reads & [reverse] \\\
%	Reads with same start positions & [samestart] \\\
	Edit-rate & [edit] \\\
\\end{tabular}


\\subsection*{Quality control graphic.}
\\begin{figure}[htb]
\\begin{center}
\includegraphics[angle=270,width=\\textwidth]{[EDITDISTANCE]}
\\end{center}
\\caption{Count of reads that align at the reference genome versus the number of mismatches (edit distance) in this alignment.}
\\end{figure}

\\begin{figure}[htb]
\\begin{center}
\includegraphics[angle=270,width=\\textwidth]{[REFCOUNT]}
\\end{center}
\\caption{Count of reads that aligns at each of the chromosomes present in the reference genome.}
\\end{figure}

\\begin{figure}[htb]
\\begin{center}
\includegraphics[angle=270,width=\\textwidth]{[LENGTH]}
\\end{center}
\\caption{Count of reads versus the length of the read. This number is
calculated over all of the reads in the input}
\\end{figure}

\\begin{figure}[htb]
\\begin{center}
\includegraphics[angle=270,width=\\textwidth]{[REPLICATION]}
\\end{center}
\\caption{Count of reads versus replication number. A replication number of
$n$ means that the read sequence has been sequenced $n+1$ times (so is
replicated $n$ times). This distribution is determined from the aligned reads.}
\\end{figure}
\\end{document}' 


#
#
#
# MAIN
#
#
#


# preparing variables for the document replacement
file_location=$(dirname "${file_prefix}" )
editdistance="${file_prefix}.editdistance.txt.eps" 
refcount="${file_prefix}.refcount.txt.eps" 
length="${file_prefix}.length.txt.eps"
replication="${file_prefix}.replication.txt.eps" 

# parse the stats file
n_total=$( grep "Total number of alignments" "$fnstats" | gawk -F '\t' '{print $2}' ) 
n_align=$( grep "number of mapped alignments" "$fnstats" | gawk -F '\t' '{print $2}')
n_forw=$( grep "number of alignments mapped on the forward strand" "$fnstats" | gawk -F '\t' '{print $2}' )
n_rev=$( grep "number of alignments mapped on the reverse strand" "$fnstats" | gawk -F '\t' '{print $2}')
n_start=$( grep "number of alignments mapped with the same start positions" "$fnstats" | gawk -F '\t' '{print $2}' )
f_edit=$( grep "Edit-rate" "$fnstats" | gawk -F '\t' '{print $2}' )

# determine the alignment percentage
palign=$( echo "$n_align / $n_total * 100 " | bc -l )

# fill the table
DOCUMENT=${DOCUMENT//\[total\]/$n_total}
DOCUMENT=${DOCUMENT//\[aligned\]/$n_align}
DOCUMENT=${DOCUMENT//\[paligned\]/${palign:0:4}}
DOCUMENT=${DOCUMENT//\[forward\]/$n_forw}
DOCUMENT=${DOCUMENT//\[reverse\]/$n_rev}
DOCUMENT=${DOCUMENT//\[samestart\]/$n_start}
DOCUMENT=${DOCUMENT//\[edit\]/$f_edit}

# replacing tags
tsample=$( echo $sample_name | sed 's/\_/\\_/g' )
DOCUMENT=${DOCUMENT//\[AUTHORS\]/$authors}
DOCUMENT=${DOCUMENT//\[SAMPLE_NAME\]/$tsample}
DOCUMENT=${DOCUMENT//\[EDITDISTANCE\]/$editdistance}
DOCUMENT=${DOCUMENT//\[REPLICATION\]/$replication}
DOCUMENT=${DOCUMENT//\[LENGTH\]/$length}
DOCUMENT=${DOCUMENT//\[REFCOUNT\]/$refcount}

#DOCUMENT=${DOCUMENT//\[STATS\]/$stats}

# prepare the variables
tex="${file_prefix}.quality_info.tex"
dvi=${tex/.tex/.dvi}
ps=${tex/.tex/.ps}
#pdf=${tex/.tex/.pdf}

# make the document
echo "Creating TEX file $tex"
echo -e "$DOCUMENT" > "$tex"


#generate the PDF
latex -interaction=batchmode "$tex" >> pdf_messages.log
[[ -f "$dvi" ]] && dvips -o "$ps" "$dvi" >> pdf_messages.log
[[ -f "$ps" ]] && ps2pdf "$ps" "$pdf" >> pdf_messages.log

# Clean up
[[ -f "${file_prefix}.quality_info.log" ]] && rm "${file_prefix}.quality_info.log"
[[ -f "${file_prefix}.quality_info.aux" ]] && rm "${file_prefix}.quality_info.aux"


[[ -f "$pdf" ]] && echo "$pdf was succesfully created"
cd "$pwd"


