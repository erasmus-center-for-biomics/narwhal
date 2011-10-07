#ifndef bamstats_lib
#define bamstats_lib 

#include <string>
#include <vector>
#include <map>

#include <api/BamAlignment.h>


/*
 * a struct to hold base counts
 */
class BaseCnt {
  unsigned int nbases ;    // the number of bases 
  unsigned int nN ;        // the number of N
  unsigned int nA ;        // the number of A
  unsigned int nC ;        // the number of C
  unsigned int nT ;        // the number of T
  unsigned int nG ;        // the number of G

 public:
  BaseCnt( void ) ;
  ~BaseCnt( void ) ;

 public:
  void update( BamAlignment& aln ) ;

  string toString( void ) ;
} ;


/* 
 * a struct to hold read counts
 */
class ReadCnt {
  // read based counters
  unsigned int n_reads ;    // the number of reads
  unsigned int totalignments ;
  unsigned int n_aligned ;  // the number of aligned reads
  unsigned int n_duplicate ;
  unsigned int n_samestart ;
  unsigned int n_forward ;
  unsigned int n_reverse ;

 public:
  ReadCnt( void ) ;
  ~ReadCnt( void ) ; 

 public:
  void update( BamAlignment& aln ) ;
  string to_string( void ) ; 

}  ;



  /*
   * a class determine read statistics
   */
class BamStats {  
  RdCnt reads ;
  BsCnt aligned ;
  BsCnt nonaligned ;

  // constructor etc
 public:
  BamStats( void ) ;
  ~BamStats( void ) ;

  // normal functions
 public:  
  // update the base counters with read
  void updateBaseCounters( BamAlignment& aln ) ;

  void updateReadCounters( BamAlignment& aln ) ;

  // get the statistics in a string format
  string print(void) ;
} ;




/*
 * determine some statistics over the 
 * BAM files
 */
void bam_statistics( std::string filename, struct s_bamstats& ) ;

/*
 * Initialize the bamstats object by 
 * setting its counters to zero 
 */
void init_bamstats( struct s_bamstats& ) ;




#endif
