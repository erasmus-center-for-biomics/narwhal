BEGIN {
  print "filename\ttot_align\tprim_align\tmapped_align\tprim_mapped_align\tforward\treverse\tdupl\teditrate";
}
/^Total number of alignments/ {
  tot_align = $5;
}
/^number of primary alignments/ {
  prim_align = $5;
}
/^number of mapped alignments/ {
  mapped_align = $5;
}
/^number of primary mapped alignments/ {
  prim_mapped = $6;
}
/^number of alignments mapped on the forward strand/ {
  forward = $9;
}
/^number of alignments mapped on the reverse strand/ {
  reverse = $9;
}
/^number of alignments with the same start positions/ {
  dupl = $9;
}
/^Edit-rate/ {
  editrate = $3;
  print FILENAME "\t" tot_align "\t" prim_align "\t" mapped_align "\t" prim_mapped "\t" forward "\t" reverse "\t" dupl "\t" editrate;
}
END {
}
