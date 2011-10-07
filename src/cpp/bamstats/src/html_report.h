#ifndef BAMSTATS_html_report 
#define BAMSTATS_html_report 

#include <string>


// personal libraries
#include "ReadCnt.h"
#include "BaseCnt.h"

namespace std {

  
  string ReadCnt_global_table( ReadCnt* rdscnt ) ;

  string ReadCnt_edit_table( ReadCnt* rdscnt ) ;

  string ReadCnt_readlength_table( ReadCnt* rdscnt ) ;

  string BaseCnt_position_table( BaseCnt* basecnt ) ;

  string htmlReport( ReadCnt* rdscnt,  BaseCnt* aligned,  BaseCnt* nonaligned ) ;

} ;



#endif
