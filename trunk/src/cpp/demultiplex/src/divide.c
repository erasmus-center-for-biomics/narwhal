#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"
#include "fastq.h"
#include "divide.h" 

int MAX_LINE = 100000 ;


/**
 * get the sample id from a line
 *
 * Args:
 *   line char*	a line
 *
 **/
char * get_sampleid( char * buffer ) {
	int cn = 0 ;
	char * pch = NULL ;
	char * ret = NULL ;
	pch = strtok( buffer, "\t" ) ;
	while( pch != NULL ) {
		if(cn == 2) {
			ret = pch ;
		}
		cn++ ;
		pch = strtok( NULL, "\t" ) ;	
	}
	// remove the trailing endline and return the sampleid
        ret[strlen(ret)-1] = '\0' ;
	return ret ;
}


//
//
// Iinterfaced functions
//
//
//
//
//


/**
 *
 *
 * 
 *
 **/
int divide( char * fninp, char * fnind, ll_item * l) {
	int retval = EXIT_SUCCESS ;
	char * sid ;
	char * buffer ;
	s_Read * r ;
	s_OFile * t ;

	FILE * finp ;
	finp = fopen( fninp, "r" ) ;
        if( finp == NULL ) {
                perror( "Could not open input file" ) ;
                return EXIT_FAILURE ;
        }
        // open the output file
        FILE * find ;
        find = fopen( fnind, "r" ) ;
        if( find == NULL ) {
                perror( "Could not open index file" ) ;
                return EXIT_FAILURE ;
        }

	// allocate the buffer and the read	
	buffer = (char *) malloc( MAX_LINE * sizeof(char) ) ;
	r      = init_read( ) ;

	while( get_read(finp, r) )  {
		// get the sample id, but if there was a read in the input file
		// but no remaining index in the output file announce failure and exit
		if( fgets( buffer, MAX_LINE, find ) == NULL ) {
			perror( "Error: Insuffient number of lines in the index file" ) ;
			retval = EXIT_FAILURE ;
			break ;
		}

		// get the sample id
		sid = get_sampleid( buffer ) ;
		// rewind the linked list with the output files
		l = begin_ll( l ) ;			

		// loop through the output file objects
		while( 1 ) {
			// get the output object
			t = (s_OFile *) l->value ;	

			// if the sample id is the same as the one specified for the output file 
			// write the read to the file
			if ( strlen(t->sampleid) == strlen(sid) && strncmp(t->sampleid, sid, strlen(t->sampleid)) == 0 ) {
				write_fastq( t->f, r ) ;
			}

			// iterate to the next possible output file			
			if( l->next == NULL ) break ;
			l = l->next ;	
		}

	}
	// clean up
	free( buffer ) ;
	fclose( find ) ;
	fclose( finp ) ;

	//
	return retval ;
}

/**
 *
 *
 *
 *
 *
 **/
ll_item * get_comparison_file_handles( ll_item * snames, ll_item * fnames ) {
	ll_item * retval = NULL ;
	s_OFile * tmp    = NULL ;

	// single line error check
	if( count_ll(snames) != count_ll(fnames) || count_ll(fnames) == 0 ) return
NULL ;

	snames = begin_ll( snames ) ;
	fnames = begin_ll( fnames ) ;
		
	while( 1 ) {
		tmp = (s_OFile *) malloc( sizeof(struct s_OFile) ) ;
		tmp->f = fopen( (char *) fnames->value, "a" ) ;
		tmp->sampleid = (char *) snames->value ;

		retval = add_ll( retval, tmp ) ;
		if( fnames->next == NULL ) break ;
		fnames = fnames->next ;
		snames = snames->next ;
	}
	//
	return retval ;
}


