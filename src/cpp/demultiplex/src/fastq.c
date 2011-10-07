#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "fastq.h"

// 
uint32_t MAX_CHAR_LINE = 100000 ;


s_Read * init_read( ) {
	s_Read * r ;
	r = (s_Read *) malloc( sizeof(struct s_Read) ) ;
	r->id   = (char *) malloc( sizeof(char) ) ;
	r->seq  = (char *) malloc( sizeof(char) ) ;
	r->qual = (char *) malloc( sizeof(char) ) ;
	return r ;
}

/**
 * get a read from a file
 *
 * Args:
 *  f	FILE	the opened fastq file
 *  r	s_Read	the read object to fill
 *
 * Returns:
 *  0 on failure, 1 on success
 *
 **/
int get_read( FILE * f, s_Read * r ) {
	int retval = 1 ;
	
	// empty the read
	free( r->id ) ;
        free( r->seq ) ;
        free( r->qual ) ;

	// prepare the buffer to hold the lines
	char * buffer = (char *)malloc( MAX_CHAR_LINE * sizeof(char) ) ;
	
	// determine the sequence id
	if( fgets(buffer, MAX_CHAR_LINE, f) == NULL ) retval = 0 ;
	if( retval ) {
		r->id = (char *) malloc( (strlen(buffer)-2) * sizeof(char) ) ;	
		strncpy( r->id, buffer+1, strlen(buffer)-2 ) ;
	}
	// the sequence
	if( fgets(buffer, MAX_CHAR_LINE, f) == NULL ) retval = 0 ;
	if( retval ) { 
		r->seq = (char *) malloc( (strlen(buffer)-1) * sizeof(char) ) ;
                strncpy( r->seq, buffer, strlen(buffer)-1 ) ;
	}
	// do not process the optional field
	if( fgets(buffer, MAX_CHAR_LINE, f) == NULL ) retval = 0 ;

	// get the quality information
	if( fgets(buffer, MAX_CHAR_LINE, f) == NULL ) retval = 0 ;
	if( retval ) { 
		r->qual = (char *) malloc( (strlen(buffer)-1) * sizeof(char) ) ;
                strncpy( r->qual, buffer, strlen(buffer)-1 ) ;
	}
	
	// cleanup after ourselves
	
	free( buffer )	;

	// return the value
	return retval ;
}

/**
 * Write a read to a file in the FastQ format
 *
 * Args:
 * f	FILE * 		the file to read to
 * r	s_Read *	the read to write
 *
 * Returns:
 * void
 * 
 **/
void write_fastq( FILE * f, s_Read * r ) {
		
	fprintf( f, "@%s\n%s\n+\n%s\n", r->id, r->seq, r->qual ) ;
}

/**
 *
 * 
 **/
void free_read(  s_Read * r ){
        // empty the read
        free( r->id ) ;
        free( r->seq ) ;
        free( r->qual ) ;
	free( r ) ;
}

