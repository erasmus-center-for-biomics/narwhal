#include <iostream>
#include <sstream>
#include <string>
#include <map>

// personal libraries

#include "ReadCnt.h"
#include "BaseCnt.h"

using namespace std ;

/**
 * generate a text output of the object
 *
 */
string ReadCnt_global_table( ReadCnt* rdscnt ) {
  stringstream out ;
 
  // insert formatting code here

  out << "<table>" ;
  out << "<thead>" ; 
  out << "<tr>" ; 
  out << "<th>situation</th>" ; 
  out << "<th>reads</th>" ;
  out << "</tr>" ; 
  out << "</thead>" ;
  out << "<tbody>" ;
  out << "<tr>" ; 
  out << "<td>Total number of alignments</td>" ; 
  out << "<td>" << rdscnt->n_alignments << "</td>" ; 
  out << "</tr>" ;
  out << "<tr><td>number of primary alignments</td><td>" << rdscnt->n_primary << "</td></tr>" ;
  out << "<tr><td>number of mapped alignments</td><td>" << rdscnt->n_mapped << "</td></tr>" ;
  out << "<tr><td>number of primary mapped alignments</td><td>" << rdscnt->n_pmapped << "</td></tr>" ;
  out << "<tr><td>number of PCR duplicates</td><td>" << rdscnt->n_duplicate << "</td></tr>" ;
  out << "<tr><td>number of alignments mapped on the forward strand</td><td>" << rdscnt->n_forward << "</td></tr>" ;
  out << "<tr><td>number of alignments mapped on the reverse strand</td><td>" << rdscnt->n_reverse << "</td></tr>" ;
  out << "<tr><td>number of alignments with the same start positions</td><td>" << rdscnt->n_samestart << "</td></tr>" ;
  out << "<tr><td>Error-rate</td><td>" << (double) rdscnt->tot_edit / rdscnt->tot_bases << "</td></tr>" ;
  out << "</tbody>" ;
  out << "</table>" ;
  out << endl ;

  return( out.str() ) ;
}

string ReadCnt_edit_table( ReadCnt* rdscnt ) {  
  stringstream out ;
  map<int,int>::iterator itA ;

  out << "<table>" ;
  out << "<thead>" ; 
  out << "<tr>" ; 
  out << "<th>Distance</th>" ; 
  out << "<th>reads</th>" ; 
  out << "</tr>" ; 
  out << "</thead>" ;
  out << "<tbody>" ;


  for( itA=rdscnt->map_edit.begin(); itA!=rdscnt->map_edit.end(); itA++ ) {
    out << "<tr>" ; 
    out << "<td>" << itA->first << "</td>" ; 
    out << "<td>" << itA->second << "</td>" ;
    out << "</tr>" ;
  }

  out << "</tbody>" ;
  out << "</table>" ;

  return( out.str() ) ;
}

string ReadCnt_readlength_table( ReadCnt* rdscnt ) {
  stringstream out ;
  map<int,int>::iterator itB ;

  out << "<table>" ;
  out << "<thead>" ; 
  out << "<tr>" ; 
  out << "<th>Length</th>" ; 
  out << "<th>reads</th>" ; 
  out << "</tr>" ; 
  out << "</thead>" ;
  out << "<tbody>" ;

  for( itB=rdscnt->map_length.begin(); itB!=rdscnt->map_length.end(); itB++ ) {
    out << "<tr>" ; 
    out << "<td>" << itB->first << "</td>" ; 
    out << "<td>" << itB->second << "</td>" ;
    out << "</tr>" ;
  }

  out << "</tbody>" ;
  out << "</table>" ;

  return( out.str() ) ;
}


string BaseCnt_position_table( BaseCnt* basecnt ) {
  stringstream out ;

  out << "<table><thead><tr>" ;
  out << "<th>Cycle</th>" ; 
  out << "<th>A</th>"; 
  out << "<th>T</th>"; 
  out << "<th>C</th>"; 
  out << "<th>G</th>"; 
  out << "<th>N</th>"; 
  out << "</tr>" ;
  out << "</thead><tbody>" ;

  for( int i=0; i<basecnt->positions.size(); i++ ){    
    out << "<tr>"  ;
    out << "<td>" << (i+1) << "</td>" ; 
    out << "<td>" << basecnt->positions[i][BaseConst::MA] << "</td>" ; 
    out << "<td>" << basecnt->positions[i][BaseConst::MT] << "</td>" ; 
    out << "<td>" << basecnt->positions[i][BaseConst::MC] << "</td>" ;
    out << "<td>" << basecnt->positions[i][BaseConst::MG] << "</td>" ;
    out << "<td>" << basecnt->positions[i][BaseConst::MN] << "</td>" ;
    out << "</tr>" ;
  }

  out << "</tbody>" ;
  out << "</table>" ;

  return( out.str() ) ;
}


/**
 *
 * make a html report 
 *
 */
string htmlReport( ReadCnt* rdscnt, BaseCnt* aligned, BaseCnt* nonaligned ) {
  stringstream out ;

  out << "<html>" ;
  out << "<head>" ;
  out << "<title>Datasheet</title>" ;
  out << "</head>" ;
  out << "<body>" ;

  out << "<div id='global' >" ;
  out << ReadCnt_global_table( rdscnt ) ;
  out << "</div>" ;
  
  out << "<div id='edit_distance' >" ;
  out << ReadCnt_edit_table( rdscnt ) ;
  out << "</div>" ;
  
  out << "<div id='read_length' >" ;
  out << ReadCnt_readlength_table( rdscnt ) ;
  out << "</div>" ;
  
  out << "<div id='aligned_bases' >" ;
  out << BaseCnt_position_table( aligned ) ;
  out << "</div>" ;

  out << "<div id='nonaligned_bases' >" ;
  out << BaseCnt_position_table( nonaligned ) ;
  out << "</div>" ;

  out << "</body>" ;
  out << "</html>" ;

  return( out.str() ) ;
}
