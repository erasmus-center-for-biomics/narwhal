#!/bin/env Rscript

library( getopt )

opt = getopt( matrix(byrow=TRUE, nrow=2, ncol=4, data=c(
   'prefix','p','1','character', 
   'help','h', '0', 'logical'
)) )

usage <- function( mess="", quit=FALSE ) {
   cat( paste( mess, "\n", sep='' ) ) 
   cat("Usage: Rscript plotter.R -p <prefix>\n\nOptions:\n-p/--prefix <string>   - the prefix of the data files\n\nDescription:\nThis script will generate dotcharts and/or boxplots from\ntabular data in files starting with prefix.\n")
   if(quit) {
      q("no")
   } 
}

if( !is.null(opt$help) ) {
   usage( quit=TRUE )
}

if ( is.null(opt$prefix) ) {
   usage( mess="Error: Prefix not defined", quit=TRUE )
}

prefix   <- opt$prefix
suffixes <- c('.editdistance.txt', '.refcount.txt', '.replication.txt', '.length.txt' )

# foreach suffix
for( s in suffixes ) {
   #
   f <- paste( prefix, s, sep='' )
   o <- paste( f, '.eps', sep='' )
   
   # read the tables
   d <- read.table( f, sep="\t", header=T )
   lg <- FALSE
   if( diff(range(d[,2], na.rm=TRUE) ) / mean(d[,2], na.rm=TRUE) > 100 ) { 
      lg <- TRUE
   }

   #
   type <- 'dotchart'
   if ( nrow(d) > 50  ){ 
      type <- 'barchart'
   } else if( lg ){
      type <- 'barchart'
   }

   # plot the data in a barplot
   postscript( file=o, paper="a4", onefile=FALSE) # paper="a4" )
   if (type == 'dotchart') {
      dotchart( d[,2], labels=d[,1], ylab=colnames(d)[1], xlab=colnames(d)[2] , pch=15)
   } else if( type == 'barchart'  ) {
      l <- ''
      if( lg == TRUE ) l <- 'y'
      barplot( d[,2], names=d[,1], xlab=colnames(d)[1], ylab=colnames(d)[2] , col="black", log=l)
   }
   dev.off()
}



