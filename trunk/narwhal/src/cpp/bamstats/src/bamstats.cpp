#include <iostream>
#include <sstream>
#include <string>
#include <api/BamAlignment.h>

// set the namespace that we will use
using namespace std ;
using namespace BamTools ;

// define some constants
const char MISMATCH  = 'X' ;
const char INSERTION = 'I' ;
const char DELETION  = 'D' ;
const char MATCH     = 77 ;

// define the bases as integers
const int A = 65 ;
const int T = 84 ;
const int C = 67 ;
const int G = 71 ;

/**
 *
 * The read count class
 *
 *
 */

/**
 * the ReadCnt constructor
 */
ReadCnt::ReadCnt() {
  n_reads       = 0 ;
  totalignments = 0 ;
  n_aligned     = 0 ;
  n_dupR        = 0 ;
  n_samestart   = 0 ; 
  n_forward     = 0 ;
  n_reverse     = 0 ;
}

/**
 * update the counters in the ReadCnt object
 */ 
void ReadCnt::update( BamAlignment& aln ) {
  //
  n_reads++ ;
  if( al.IsMapped() ) n_aligned++ ;
  if( al.IsDuplicate() ) n_duplicate++ ;     
  if (al.IsReverseStrand() ) {
    n_reverse++ ; 
  } else { 
    n_forward++ ;
  }
}

/**
 * generate a text output of the object
 *
 */
string ReadCnt::to_string() {

}


/**
 *
 *
 *
 *
 **/



/*
 * initialize the variables in the base counter struct
 */
void init_basecounter( struct s_basecounter& cnt ){  
  cnt.nbases = 0 ;
  cnt.nN = 0 ;
  cnt.nA = 0 ;
  cnt.nC = 0 ;
  cnt.nT = 0 ;
  cnt.nG = 0 ;
}

/**
 * initialize the variables in the bamstats struct
 */
void init_bamstats( struct s_bamstats& stat ) {

  // initialize the bamstats per read
  stat.nreads     = 0 ; 
  stat.totalignments = 0 ;
  stat.naligned   = 0 ; 
  stat.ndupR      = 0 ; 
  stat.nsamestart = 0 ;
  stat.nforward   = 0 ;
  stat.nreverse   = 0 ;

  // set the stats per base
  stat.nmm        = 0 ;
  stat.ndel       = 0 ;
  stat.nin        = 0 ; 
  
  //
  init_basecounter( stat.aligned ) ;
  init_basecounter( stat.nonaligned ) ;
} 


//
//
//
//
//

void read_counters( BamAlignment al, struct s_bamstats& stat ) {
  //
  // statistics per read
  //
  stat.nreads++ ;
  if( al.IsMapped() ) stat.naligned++ ;
  if( al.IsDuplicate() ) stat.ndupR++ ; 
    
  if (al.IsReverseStrand() ) {
    stat.nreverse++ ; 
  } else { 
    stat.nforward++ ;
  }
}


void decode_cigar( BamAlignment al, struct s_bamstats& stat ) {
  // determine the number of mismatches
  unsigned int i ;
  //cout << "----" << endl ;
  for(i=0; i<al.CigarData.size(); i++) {
    // determine the 
    int type = (int) al.CigarData[i].Type ;  
    cout << al.CigarData[i].Type << "\t" << type << endl ;
    switch( type ) {
      
    case MISMATCH:
      stat.nmm += al.CigarData[i].Length ;
      break ;	  
    case INSERTION: 
      stat.nin += al.CigarData[i].Length ;
      break ;
    case DELETION: 
      stat.ndel += al.CigarData[i].Length ;
      break ;
    default:
      // do nothing
      break ;
    }
  }
}

void base_counters(BamAlignment al, struct s_basecounter& basecnt ){
  //
  basecnt.nbases += al.QueryBases.length() ;
  unsigned int i ;
  
  for( i=0; i<al.QueryBases.length(); i++) {
    char nuc = toupper( al.QueryBases[i] ) ;
    int base = (int) nuc ;    
    switch( base  ){
    case A :
      basecnt.nA++ ;
      break ;
    case C :
      basecnt.nC++ ;
      break ;
    case T :
      basecnt.nT++ ;
      break ;
    case G :
      basecnt.nG++ ;
      break ;
    default:
      basecnt.nN++ ;
      break ;
    }
  }   
}


/*
 * initialize the reference counts
 *
 * hmm, we are creating a buffer overflow because 
 * the map is dynamic in memmory, while the struct 
 * is not 
 * 
 */
/*
void init_reference_info( struct s_bamstats& stat, BamReader reader ) {
  
  // get the references   
  string refname ; 
  RefVector refs = reader.GetReferenceData() ;

  for( int i=0; i<refs.size(); i++ ) {    
    refname = refs[i].RefName ;
    cout << refname << "\t" << reader.GetReferenceID(refname) << endl ;
    stat.nrRef.insert( pair<int,int>(reader.GetReferenceID(refname), 0) ) ;
    stat.RefID.insert( pair<int,string>(reader.GetReferenceID(refname), refname) ) ;
  }
}
*/

/*
 * determine the BAM statistics for the file filename
 *
 */
void bam_statistics( string filename, struct s_bamstats& stat ) {
  
  // create a Bam file reader
  BamReader reader ;
  reader.Open( filename, ""  ) ;
 
  // init_reference_info( stat, reader ) ;

  // for each alignment
  int ppos    = 0 ;  
  string pchr = "" ;
  BamAlignment al ;

  while ( reader.GetNextAlignment( al ) ) {
    // increase the total number of alignments
    stat.totalignments++ ;

    // if this is the primary (first/best) alignment
    if ( al.IsPrimaryAlignment() ) {
      
      read_counters( al, stat ) ;
      if ( al.IsMapped() ) {
	base_counters( al, stat.aligned ) ; 
      } else {
	base_counters( al, stat.nonaligned ) ; 
      }
      // decode_cigar( al, stat ) ;


      // things we need to determine with 
      // respect to the previous read
      if( al.Position == ppos ) stat.nsamestart++ ;

      // set prev to the current position
      ppos = al.Position ;
    }
  }
}
 


