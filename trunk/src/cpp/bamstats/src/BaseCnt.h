#ifndef BAMSTATS_BaseCnt
#define BAMSTATS_BaseCnt

#include <string>
#include <map>
#include <vector>
#include <api/BamAlignment.h>


namespace BaseConst {
  // some constants for the map
  const int MA = 0 ;
  const int MT = 1 ;
  const int MC = 2 ;
  const int MG = 3 ;
  const int MN = 4 ;
} ;

namespace std {

  struct intcomp_BaseCnt {
    bool operator()( const int& lhs, const int& rhs ) const { 
      return lhs<rhs  ; 
    } 
  } ;




  /*
   * a class to hold base counts
   */
  class BaseCnt {
  public:
    unsigned int n_bases ;    // the number of bases 
    unsigned int bases[5] ;
    
    // a map to hold the number of bases per position in the read
    vector< vector<int> > positions ;
    
  public:
    BaseCnt( void ) ;
    ~BaseCnt( void ) ;
    
  public:
    void update( BamTools::BamAlignment aln ) ;
    std::string to_string( void ) ;
    
  private:
    void update_basepos( int pos, int base ) ;
    
  private:
    int get_base_number( char base ) ;
  } ;
} // end std namespace

#endif
