#include <sstream>
#include <string>
#include <vector>
#include <api/BamAlignment.h>
#include "BaseCnt.h"

// set the namespace that we will use
using namespace std ;
using namespace BamTools ;


// define the bases as integers
const int A = 65 ;
const int T = 84 ;
const int C = 67 ;
const int G = 71 ;


/**
 *
 * The implementation of the BaseCnt class 
 *
 *
 **/
BaseCnt::BaseCnt() {
  n_bases   = 0 ;    // the number of bases 
  bases[BaseConst::MA] = 0 ;
  bases[BaseConst::MT] = 0 ;
  bases[BaseConst::MC] = 0 ;
  bases[BaseConst::MG] = 0 ;
  bases[BaseConst::MN] = 0 ;

}

BaseCnt::~BaseCnt() {
}

/**
 * update the current BaseCnt object
 *
 */
void BaseCnt::update( BamAlignment aln ){
  n_bases += aln.QueryBases.length() ;
  unsigned int i ;
  int base ;

  for( i=0; i<aln.QueryBases.length(); i++) {
    base = get_base_number( aln.QueryBases[i] ) ;
    update_basepos( i, base ) ;
    bases[base]++ ;
  }   
}

string BaseCnt::to_string( void ) {
  stringstream out ;
  
  float p_a ;
  float p_t ;
  float p_c ;
  float p_g ;
  float p_n ;

  if( n_bases > 0 ) {
    p_a = (float) bases[BaseConst::MA] / n_bases ; 
    p_t = (float) bases[BaseConst::MT] / n_bases ; 
    p_c = (float) bases[BaseConst::MC] / n_bases ; 
    p_g = (float) bases[BaseConst::MG] / n_bases ; 
    p_n = (float) bases[BaseConst::MN] / n_bases ;
  } else {
    p_a = 0 ;
    p_t = 0 ;
    p_c = 0 ;
    p_g = 0 ;
    p_n = 0 ;
  }

  // insert formatting code here
  out << "Total\t"<< n_bases << "\t1" << endl ;
  out << "A\t" << bases[BaseConst::MA] << "\t" << p_a << endl ;
  out << "T\t" << bases[BaseConst::MT] << "\t" << p_t << endl ;
  out << "C\t" << bases[BaseConst::MC] << "\t" << p_c << endl ;
  out << "G\t" << bases[BaseConst::MG] << "\t" << p_g << endl ;
  out << "N\t" << bases[BaseConst::MN] << "\t" << p_n << endl ;

  out << endl ;
  out << "Positional base counts" << endl ;
  out << "cycle\tA\tT\tC\tG\tN" << endl ;
  for( int i=0; i<(int)positions.size(); i++ ){    
    out << (i+1) << "\t" << positions[i][BaseConst::MA] << "\t" << positions[i][BaseConst::MT] << "\t"<< positions[i][BaseConst::MC] << "\t"<< positions[i][BaseConst::MG] << "\t"<< positions[i][BaseConst::MN] << endl ;
  }

  //
  return( out.str() ) ;
}

/**
 * Update the base position mapping
 *
 **/
void BaseCnt::update_basepos( int pos, int base ) {

  // 
  if( pos >= (int)positions.size() ) {
    for( int i = positions.size(); i <= pos; i++ ) {          
      vector<int> nuc( 5,0 ) ;    
      positions.push_back( nuc ) ;
    }
  }
 
  //
  positions[pos][base]++ ;
}



/**
 * get the number of the base
 **/
int BaseCnt::get_base_number( char basechar ) {
  int ret ;
  char nuc = toupper( basechar ) ;
  int base = (int) nuc ;    
  switch( base  ){
  case A :
    ret = BaseConst::MA ;
    break ;
  case C :
    ret = BaseConst::MC ;
    break ;
  case T :
    ret = BaseConst::MT ;
    break ;
  case G :
    ret = BaseConst::MG ;
    break ;
  default:
    ret = BaseConst::MN ;
    break ;
  }
  return ret ;
}


