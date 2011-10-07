#ifndef DIVIDE
#define DIVIDE


#include <stdio.h> 
#include "linkedlist.h"

/**
 *
 *
 *
 **/
struct s_OFile {
        char * sampleid ;
        FILE * f ;
} ;

int divide( char * finp, char * find, ll_item * l) ;


ll_item * get_comparison_file_handles( ll_item * snames, ll_item * fnames ) ;

#endif 

