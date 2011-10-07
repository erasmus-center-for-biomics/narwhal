#include <iostream>
#include <sstream>
#include <string>

#include <api/BamAlignment.h>
#include "CigarCnt.h"

// set the namespace that we will use
using namespace std ;
using namespace BamTools ;

// (supported) CIGAR CONSTANTS (see http://samtools.sourceforge.net/SAM-1.3.pdf )
const int CIGAR_ALIGNMENT_MATCH = 77 ;   // M; match or mismatch
const int CIGAR_INSERTION       = 73 ;   // I; Insertion 
const int CIGAR_DELETION        = 68 ;   // D; Deletion
const int CIGAR_SKIPPED         = 78 ;   // N; skipped region
const int CIGAR_SOFTCLIP        = 83 ;   // S; soft clipping
const int CIGAR_HARDCLIP        = 72 ;   // H; hard clipping
const int CIGAR_PADDING         = 80 ;   // P; padding
const int CIGAR_MATCH           = 61 ;   // =; sequence match
const int CIGAR_MISMATCH        = 88 ;   // X; Sequence mismatch

/**
 *
 * The read count class
 *
 *
 */

/**
 * the ReadCnt constructor
 */
CigarCnt::CigarCnt() {

  //
  cigar_alignment_match = 0 ;
  cigar_deletion  = 0 ;
  cigar_insertion = 0 ;
  cigar_skipped   = 0 ;
  cigar_softclip  = 0 ;
  cigar_hardclip  = 0 ; 
  cigar_padding   = 0 ;
  cigar_match     = 0 ;
  cigar_mismatch  = 0 ; 
}

CigarCnt::~CigarCnt() {
}


/**
 * generate a text output of the object
 *
 */
string CigarCnt::to_string() {
  stringstream out ;

  out << "CIGAR based base counters" << endl ;
  out << "\tAligned bases\t" << cigar_alignment_match << endl ;
  out << "\tBases in deletions\t" << cigar_deletion << endl ;
  out << "\tBases in insertions\t" << cigar_insertion << endl ;
  out << "\tSkipped bases\t" << cigar_skipped << endl ;
  out << "\tSoft clipped bases\t" << cigar_softclip << endl ;
  out << "\tHard clipped bases\t" << cigar_hardclip << endl ;
  out << "\tPadded bases\t" << cigar_padding << endl ;
  out << "\tMatched bases\t" << cigar_match << endl ;
  out << "\tMismatched bases\t" << cigar_mismatch << endl ;

  return( out.str() ) ;
}



/**
 * decode the cigar string and update counters with the result
 *
 *
 **/
void CigarCnt::update( BamAlignment aln ) {  
  unsigned int i ;

  for(i=0; i<aln.CigarData.size(); i++) {
    int type = (int) aln.CigarData[i].Type ;  

    switch( type ) {      
    case CIGAR_ALIGNMENT_MATCH:
      cigar_alignment_match += aln.CigarData[i].Length ;
      break ;	  
    case CIGAR_INSERTION: 
      cigar_insertion += aln.CigarData[i].Length ;
      break ;
    case CIGAR_DELETION: 
      cigar_deletion += aln.CigarData[i].Length ;
      break ;
    case CIGAR_SKIPPED:
      cigar_match += aln.CigarData[i].Length ;
      break ;	  
    case CIGAR_SOFTCLIP:
      cigar_softclip += aln.CigarData[i].Length ;
      break ;	  
    case CIGAR_HARDCLIP:
      cigar_hardclip += aln.CigarData[i].Length ;
      break ;	  
    case CIGAR_PADDING:
      cigar_padding += aln.CigarData[i].Length ;
      break ;	  
    case CIGAR_MATCH:
      cigar_match += aln.CigarData[i].Length ;
      break ;	  
    case CIGAR_MISMATCH:
      cigar_mismatch += aln.CigarData[i].Length ;
      break ;	  
    default:
      // do nothing
      break ;
    }
  }
}

 


