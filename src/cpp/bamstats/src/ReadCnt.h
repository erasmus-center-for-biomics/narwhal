#ifndef BAMSTATS_ReadCnt
#define BAMSTATS_ReadCnt

#include <vector>
#include <string>
#include <map>
#include "api/BamAlignment.h"

namespace std {

  
  struct intcomp {
    bool operator()( const uint64_t& lhs, const uint64_t& rhs ) const { 
      return lhs<rhs ; 
    } 
  } ;


  /* 
   * a struct to hold read counts
   */
  class ReadCnt {
  public:
    
    vector<string> refids ;
    
    // read based counters
    uint64_t n_alignments ;    // the number of reads
    uint64_t n_primary ;
    uint64_t n_mapped ;        // the number of aligned reads
    uint64_t n_pmapped ;       // the number of aligned reads
    uint64_t n_duplicate ;
    uint64_t n_forward ;
    uint64_t n_reverse ;
    uint64_t n_samestart ;
    
    // computed variables
    uint64_t tot_bases ;
    uint64_t tot_edit ;

    // an iterator to be used with the int mappings
    map<uint64_t,uint64_t>::iterator iter ;

    // mapping variables to hold dynamically expanding data
    map<uint64_t,uint64_t, intcomp> map_length ;  // the read length counts
    map<uint64_t,uint64_t, intcomp> map_edit ;    // the edit distance counts

    // reference mapping
    map<uint64_t,uint64_t, intcomp> map_prim_ref ;
    map<uint64_t,uint64_t, intcomp> map_all_ref ;

    // a mapping to determine the number of replication
    uint64_t repl ;    
    map<uint64_t,uint64_t, intcomp> replication ;


  public:
    ReadCnt( vector<string> ref ) ;
    ~ReadCnt( void ) ; 
    
  public:
    void update( BamTools::BamAlignment aln ) ;
    void compare( BamTools::BamAlignment a, BamTools::BamAlignment b ) ;
    void finalize() ;

    // output functions
    void EditDistanceTable( string filename ) ;
    void RefCountTable( string filename ) ;
    void PrimRefCountTable( string filename ) ;
    void LengthTable( string filename ) ;
    void ReplicationTable( string filename ) ;

    std::string to_string( void ) ; 

    
  protected:
    void update_errorrate( BamTools::BamAlignment aln ) ;
    void add_inc( map<uint64_t,uint64_t, intcomp>& toupdate, uint64_t value ) ;
  } ;
  
} // namespace

#endif
