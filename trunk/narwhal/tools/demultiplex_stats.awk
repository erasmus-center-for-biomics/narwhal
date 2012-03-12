BEGIN {
   SUBSEP="\t"
   FS="\t"
}
{
   readcount[$3, FILENAME] = readcount[$3, FILENAME] + 1
}
END {
   print "SampleID\tFilename\tnum_reads"
   for (s in readcount) {
     print s "\t" readcount[s]
   }
}
