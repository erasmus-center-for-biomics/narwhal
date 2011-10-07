#ifndef BAMSTATS_CigarCnt
#define BAMSTATS_CigarCnt

#include <string>
#include "api/BamAlignment.h"

namespace std {

/* 
 * a struct to hold read counts
 */
class CigarCnt {
  // CIGAR supported information
  unsigned int cigar_alignment_match ;
  unsigned int cigar_deletion  ;
  unsigned int cigar_insertion ; 
  unsigned int cigar_skipped ;
  unsigned int cigar_softclip  ;
  unsigned int cigar_hardclip ; 
  unsigned int cigar_padding ;
  unsigned int cigar_match  ;
  unsigned int cigar_mismatch ; 
  

 public:
  CigarCnt( void ) ;
  ~CigarCnt( void ) ; 

 public:
  void update( BamTools::BamAlignment aln ) ;
  std::string to_string( void ) ; 

} ;

} // namespace

#endif
