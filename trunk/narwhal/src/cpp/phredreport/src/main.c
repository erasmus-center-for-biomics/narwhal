#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


// the buffer for an input line
uint LINE_BUFFER_SIZE = 3000 ;


struct phredstat {
	int sum ;
	int min ;
	int max ;
} ;


/**
 * sum the PHRED scores per read
 *
 * Args:
 *    qual   -  the quality values
 *    offset -  the ASCII offset of the qualtiy scores
 *
 * Returns:
 *    the sum of the PHRED scores
 **/
void statPhred( phredstat * p, char * qual, int offset ) {
	int q ; 
	unsigned cn ;

	// set the initial values
	p->min  = 0 ;
	p->max  = 0 ;
	p->sum  = 0 ;
	

	// get the sum of the phred score
	for( cn=0; cn<strlen(qual); cn++ ) {

		// determine the quality
		q = qual[cn] + offset ;

		// set the initial values in the first cycle
		if( cn == 0 ) {
			p->min = q ;
			p->max = q ;
		} else {
			// 
			if ( q < p->min ) p->min = q ;
			if ( q > p->max ) p->max = q ;
		}

		// add the phred score to the sum
		p->sum += q ;
		
	}

	//
}


/**
 * process the data
 * 
 * Args:
 *   input    --  the input stream
 *   output   --  the output stream 
 *   offset   --  the offset of the base quality scores
 *
 * Returns:
 *   the exit status
 **/
int processData( FILE * input, FILE * output, int offset ) {

	//
	int retval       = EXIT_SUCCESS ; 
	char * buffer    = NULL ;
	phredstat * stat = NULL ;
	uint len ;

	// allocate the buffer
	buffer = (char *)malloc( LINE_BUFFER_SIZE * sizeof(char) ) ;
	if ( buffer == NULL ) {
		perror( "Could not allocate memory for the line buffer" ) ;
		return EXIT_FAILURE ;
  	}

	// foreach line in the input file
	while( fgets( buffer, LINE_BUFFER_SIZE, input ) != NULL ) {
		
		// remove the trailing newline character if present
		len = strlen( buffer ) ; 
		if( buffer[len-1] == '\n' ) buffer[len-1] = '\0'  ;

		// allocate the memmory for our statistics struct
		stat = (phredstat *) malloc( sizeof(phredstat) ) ;
		
		// calculate some basic statistics over the quality 
		// values of the reads
		statPhred( stat, buffer, offset ) ;
		
		// print the file stream
		fprintf( output, "%d\t%d\t%d\n", stat->sum, stat->min, stat->max ) ;

		// free our statistics struct
		free( stat ) ;
	}
	
	// free the buffer
	free( buffer ) ;
	return( retval ) ;
}

/**
 *
 * the main program loop
 *
 **/
int main( int argc, char** argv ) {

	// 
	int retval = EXIT_SUCCESS ;

	//
	FILE * input  = NULL ;
	FILE * output = NULL ;

	//
	input  = stdin ;
	output = stdout ;

	processData(input, output, -33 ) ;

	// return the return value
	return( retval ) ;
}
