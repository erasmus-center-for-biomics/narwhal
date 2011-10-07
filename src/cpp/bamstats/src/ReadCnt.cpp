#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <api/BamAlignment.h>
#include "ReadCnt.h"

// set the namespace that we will use
using namespace std ;
using namespace BamTools ;

/**
 * the ReadCnt constructor
 */
ReadCnt::ReadCnt( vector<string> ref ) {
  n_alignments = 0 ;
  n_primary    = 0 ;
  n_mapped     = 0 ;
  n_pmapped    = 0 ;
  n_duplicate  = 0 ;
  n_samestart  = 0 ; 
  n_forward    = 0 ;
  n_reverse    = 0 ;
  tot_bases    = 0 ;
  tot_edit     = 0 ;
  refids       = ref ;
  repl         = 0 ;
}

ReadCnt::~ReadCnt() {
}

/**
 * Update the counters in the ReadCnt object. 
 *
 * This function updates various counters related to complete reads. In 
 * addition it will update the error-rate based on the mapped primary 
 * alignments
 *
 * Args:
 *   BamAlignment aln
 *
 * Returns:
 *   Void
 */ 
void ReadCnt::update( BamAlignment aln ) {
  // define function variables
  int  dist ;
  bool suc ;  

  // get the basic read statistics
  n_alignments++ ;

  //
  add_inc( map_length, (uint64_t) aln.Length ) ;

  if( aln.IsPrimaryAlignment() ) n_primary++ ;     
  if( aln.IsDuplicate() ) n_duplicate++ ;     

  // is the read mapped?
  if( aln.IsMapped() ) {
    n_mapped++ ;

    // update the reference mapping
    add_inc( map_all_ref, (uint64_t) aln.RefID ) ; 

    // if so: is it a primary alignment?
    if( aln.IsPrimaryAlignment() ) {
      n_pmapped++ ;

      // update the reference mapping for the primary alignments
      add_inc( map_prim_ref, (uint64_t) aln.RefID ) ;
      
      // update the base counter and the edit distance counters
      suc = aln.GetTag( "NM", dist ) ; 
      if( suc ) {    
	tot_bases += (uint64_t) aln.Length ;    
	tot_edit  += (uint64_t) dist ;

	// update the edit distance map
	add_inc( map_edit, (uint64_t) dist ) ;
      }
    }
      
    // forward or reverse strand?
    if( aln.IsReverseStrand() ) {
      n_reverse++ ; 
    } else { 
      n_forward++ ;
    }
    
  }
}

/**
 * run comparative counters, such as n_samestart 
 * which determines whether two reads have the same 
 * start coordinates. This function is often used with 
 * the previous read
 *
 *  Args:
 *   BamAlignment a
 *   BamAlignment b
 *
 *  Returns:
 *   
 */
void ReadCnt::compare( BamAlignment a, BamAlignment b ){
  if( a.IsMapped() && b.IsMapped() ) {
    if ( a.RefID == b.RefID && a.Position == b.Position ) {
      repl++ ;
      n_samestart++ ;
    } else {
      // add the replication counter
      add_inc( replication, repl ) ;
      repl = 0 ;
    }
  }
}

/**
 * Finalize the ReadCnt object and tie up loose ends
 */
void ReadCnt::finalize() {
  if( repl > 0 ) {
    add_inc( replication, (uint64_t)repl ) ;
  }
}

/**
 * generate a text output of the object
 *
 */
string ReadCnt::to_string() {
  stringstream out ;

  // insert formatting code here
  out << "Read Counters" << endl ;
  out << "Total number of alignments\t" << n_alignments << endl ;
  out << "number of primary alignments\t" << n_primary << endl ;
  out << "number of mapped alignments\t" << n_mapped << endl ;
  out << "number of primary mapped alignments\t" << n_pmapped << endl ;
  out << "number of PCR duplicates\t" << n_duplicate << endl ;
  out << "number of alignments mapped on the forward strand\t" << n_forward << endl ;
  out << "number of alignments mapped on the reverse strand\t" << n_reverse << endl ;
  out << "number of alignments with the same start positions\t" << n_samestart << endl ;
  out << "Edit-rate (" << tot_edit << "/" << tot_bases << ")\t" << (double) tot_edit / tot_bases << endl ;
  out << endl ;

  // return the 
  return( out.str() ) ;
}

/**
 * A convenience function to update uint64_t,uint64_t mappings
 *
 */
void ReadCnt::add_inc( map<uint64_t,uint64_t,intcomp>& toupdate, uint64_t value ) {
  iter = toupdate.find( value ) ; 
  if( iter == toupdate.end() ) {
    toupdate.insert( pair<uint64_t,uint64_t>(value, 1) ) ;
  } else {
    toupdate[iter->first] = iter->second + 1 ;
  }
}

//
// FILE OUTPUT FUNCTIONS 
// 
// these functions will write differet aspects of 
// the gathered information in to seperate files. 
// 

/**
 * Output the Reference count table to a file 
 */
void ReadCnt::RefCountTable( string filename ) {
  ofstream out ;
  out.open( filename.c_str(), ofstream::out ) ;
  out << "ID\tcount" << endl ;
  for( iter=map_all_ref.begin(); iter!=map_all_ref.end(); iter++ ){
    out << refids[ iter->first ] << "\t" << iter->second << endl ;
  }
  out.close() ;
}

/**
 * Output the Primary Reference count table to a file 
 */
void ReadCnt::PrimRefCountTable( string filename ) {
  ofstream out ;
  out.open( filename.c_str(), ofstream::out ) ;
  out << "ID\tcount" << endl ;
  for( iter=map_prim_ref.begin(); iter!=map_prim_ref.end(); iter++ ){
    out << refids[ iter->first ] << "\t" << iter->second << endl ;
  }
  out.close() ;
}

/** 
 * Output the edit distance to a file 
 *
 */
void ReadCnt::EditDistanceTable( string filename ) {
  ofstream out ;
  out.open( filename.c_str(), ofstream::out ) ;
  out << "Distance\tcount" << endl ;
  for( iter=map_edit.begin(); iter!=map_edit.end(); iter++ ){
    out << iter->first << "\t" << iter->second << endl ;
  }
  out.close() ;
}

/** 
 * Output the map-length summary to a file 
 *
 */
void ReadCnt::LengthTable( string filename ) {
  ofstream out ;
  out.open( filename.c_str(), ofstream::out ) ;
  out << "Length\tcount" << endl ;
  for( iter=map_length.begin(); iter!=map_length.end(); iter++ ){
    out << iter->first << "\t" << iter->second << endl ;
  }
  out.close() ;
}

void ReadCnt::ReplicationTable( string filename ) {
  ofstream out ;
  out.open( filename.c_str(), ofstream::out ) ; 
  out << "Replication\tcount" << endl ; 
  for( iter=replication.begin(); iter!=replication.end(); iter++ ){
    out << iter->first << "\t" << iter->second << endl ;
  }
  out.close() ;
}

/**
 * update the error-rate with the provided alignment. 
 *
 * The error-rate is determined as follows:
 *   e = sum(edit_distance)/sum(bases)
 * 
 * The incremental update function is then as follows:
 *   e(current) = edit_distance / length_read
 *
 *   in the last step the error-rate should be divided 
 *   by the total number of reads 
 *
 */
void ReadCnt::update_errorrate( BamAlignment aln ) {
  int dist ;
  bool suc ;  
  suc = aln.GetTag( "NM", dist ) ;
  
  // if successful update the total error-rate with this value
  if ( suc ) {    
    tot_bases += (uint64_t) aln.Length ;    
    tot_edit  += (uint64_t) dist ;
    add_inc( map_edit, (uint64_t) dist ) ;
  }   
}



 


