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
int get_read( FILE * f, s_Read * r  ) {
	int retval = 1 ;	// the return value
	uint l_len = 0 ;	// the length in the buffer
	int cln  ;		// a counter in the line in the fastq file

	// empty the read	
	free( r->id ) ;
	free( r->seq ) ;
	free( r->qual ) ;
	

	// prepare the buffer to hold the lines
	char * buffer = (char *)malloc( MAX_CHAR_LINE * sizeof(char) ) ;
	if( buffer == NULL ){ 
		perror( "Could not allocate memory for the line buffer" ) ;
		return 0 ;
	}

	// for the lines in the fastq record
	for( cln=0; cln<4; cln++) {
		buffer[0] = '\0' ;

		//  get a line and 
		if( fgets(buffer, MAX_CHAR_LINE, f) == NULL ) retval = 0 ;
		if( retval ) {
			// remove the trailing new line 
			l_len = strlen( buffer ) ;
			if( buffer[l_len-1] == '\n' ) buffer[l_len-1] = '\0'  ;
			
			// set the new string length of the buffer
			l_len = strlen( buffer ) ;

			// depending on the line counter we will perform an action
			if( cln == 0) {
				// the sequence id runs from the @ sign to the end of the buffer
				r->id    = (char *) malloc( l_len * sizeof(char) ) ;	
				strncpy( r->id, buffer+1, l_len - 1 ) ;
				r->id[ l_len-1 ] = '\0' ;
			} else if( cln == 1) {
				// set the sequence field
				r->seq    = (char *) malloc( (l_len + 1) * sizeof(char) ) ;
                		strncpy( r->seq, buffer, l_len ) ;
				r->seq[ l_len ] = '\0' ;
			} else if( cln == 2) {
				// the second id field: do not process
			} else if( cln == 3 ){
				r->qual = (char *) malloc( (l_len + 1) * sizeof(char) ) ;
				strncpy( r->qual, buffer, l_len ) ;
				r->qual[ l_len ] = '\0' ;
			} else {
				//this should never happen
			}
		} 		
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

