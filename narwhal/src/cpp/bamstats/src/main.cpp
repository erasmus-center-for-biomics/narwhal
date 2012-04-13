#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <sys/stat.h>

// personal libraries
#include "ReadCnt.h"
#include "CigarCnt.h"
#include "BaseCnt.h"

#include <api/BamReader.h>

using namespace std ;

//
//
// get the statistics
//
//

/**
 *
 * print the bam statistics for the file indicated by filename
 *
 */
void read_statistics( string filename, string outputbase ) {  
  int cn ;
  string out ;
  ReadCnt *rds ;
  // BaseCnt *aligned ;
  // BaseCnt *nonaligned ;

  BamTools::BamReader reader ;
  BamTools::BamAlignment aln ;
  BamTools::BamAlignment prev ;
  BamTools::RefVector rv ;

  vector<string> refids ;

  // open the input alignment file 
  reader.Open( filename ) ;

  // build the reference ID vector
  rv = reader.GetReferenceData() ;
  refids.resize( rv.size() ) ;        // make sure we can place all the elements
  for( cn=0; cn<(int)rv.size(); cn++ ) {   // foreach reference sequence:
    refids[cn] = rv[cn].RefName ;     // add the sequence names
  }
  
  // make new counter object
  rds = new ReadCnt( refids ) ;

  // get all the alignments from the input file and process these  
  while( reader.GetNextAlignment( aln ) ) {   
    rds->update( aln ) ;              // update the read counter
    if( aln.IsMapped() ) {            // if the read is mapped      
      //      aligned->update( aln ) ;        // add the statistics to the aligned statistics
      rds->compare( aln, prev ) ;     // add the comparison based statistics
    } else {      
      //      nonaligned->update( aln ) ;     // if not mapped at to the nonaligned basecounter
    }    
    prev = aln ;                      // set the previous to the current
  }
 
  rds->finalize() ;

  // output the data
  cout << rds->to_string() << endl ;
  rds->RefCountTable( outputbase + ".refcount.txt" ) ;
  rds->PrimRefCountTable( outputbase + ".primary.refcount.txt" ) ;
  rds->EditDistanceTable( outputbase + ".editdistance.txt" ) ;
  rds->LengthTable( outputbase + ".length.txt" ) ;
  rds->ReplicationTable( outputbase + ".replication.txt" ) ;

  // delete the objects we created to count the data
  delete rds ;
}

//
//
// HOUSE KEEPING FUNCTIONS
//
//

/*
 * print the help message
 */
int usage() {
  cerr << endl ;
  cerr << "Usage: bamstats [--help] <input.sorted.bam> <output.file.prefix>" << endl ;
  cerr << endl ;
  cerr << "Description: " << endl ;
  cerr << "This tool will determine several statistics for a sorted BAM file. General statistics including the alignment percentage will be printed to the standard out. Other counts will be written to tabular files prefixed with the <output.file.prefix>."  << endl ;
  cerr << endl ;
  return 0 ;
}


/*
 * determine whether a file exists 
 *
 */
bool file_exists( char * filename ) {
  int fstat ;
  struct stat buffer ;
  // 
  fstat = stat( filename, &buffer ) ;
  // if we can get filestatistics, 
  // the file seems to exist
  if( fstat == 0) {
    return true ;
  } else {
    return false ;
  }
}

//
//
// MAIN
//
//

/*
 * the main loop
 */
int main( int argc, char* argv[]) {
  if ( (argc != 3 ) ) return usage() ;
  
  // if the input file does not exists  
  // argv[0] is the name of this tool
  if ( !file_exists( argv[1]) ) {
    cerr << endl ;
    cerr << "Input file does not exist: "<< argv[0] << endl ;
    usage() ;
    return 1 ;
  }
  
  //
  read_statistics( string(argv[1]), string(argv[2]) ) ;

  return 0 ;
}
