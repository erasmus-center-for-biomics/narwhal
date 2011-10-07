#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"
#include "fastq.h"
#include "demultiplex.h"


// the UNKNOWN string
char * UNKNOWN  = "UNKNOWN"  ;


/**
 * determine the edit distance between 2 strings
 *
 * Args:
 * a    string a
 * b    string b
 *
 * Returns:
 *  the edit distance between the two strings
 **/
int edit_distance( char * a, char * b ) {
	//
	uint32_t d = 0 ;

	// for each character in string A
	for( uint32_t i=0; i<strlen(a); i++ ){
		// check whether there is a character in B
		if( i < strlen(b) ) {
			// if so check whethet these are the same characters
			if( a[i] != b[i] ) {
				// if not increase the edit-distance
				d += 1 ;
			}
		} else {
			// if not break the loop and return, but this should not happen but can
			break ;
		}
	}
	// return the distance between the 2 strings
	return d ;
}

/**
 * This method will determine whether the read matches the specified barcode sequence
 *
 * Args:
 * r    read struct     the read to match
 * b    barcode struct  the barcode
 * max  uint8_t         the maximum distance between the two
 *
 * Returns:
 * 1 on a match; 0 on a mismatch
 *
 * Notes:
 * if the maximum difference is greater then 0,
 * the edit distance is calculated. In other cases
 * strncmp is performed.
 *
 **/
int match_read( s_Read * r, s_Sseq * b, uint8_t max ) {
	int retval = 0 ;
	int diff ;
	if ( max == 0 ) {
		
		// no mismatches allowed perform a string comparison
		diff = strncmp( r->seq + b->start, b->seq, strlen(b->seq) ) ;
		if (diff == 0 ) {
			retval = 1 ;
		} else {
			retval = 0 ;
		}
	} else {
		// mismatches are allowed: determine the edit distance
		diff = edit_distance( b->seq, r->seq + b->start ) ;
		if( diff <= max ) {
			retval = 1 ;
		} else {
			retval = 0 ;
		}
	}

	// return the choice
	return retval ;
}

/**
 * index the reads for a FastQ file and write the output to the
 * output file .
 *
 * Args:
 *   fnout      char *  the output filename
 *   fnin       char *  the input filename
 *   c          ll_item * 	linked list with the barcode information
 *   mismatches uint    the number of mismatches
 *
 * Return:
 *   0 on failure, 1 on success
 **/
int index( char * fnout, char * fnin, ll_item * c, uint mismatches ) {
	int retval  = EXIT_SUCCESS ;  	// assign the return value
	int mtch ; 			// int to indicate wheter we have a match
        uint64_t rno = 0 ;              // the number of read read so far
	char * rid ;			// the read id that will be written to the output file
	
	s_Sseq * tmp = NULL ;		// a temporary barcode object
	s_Read * cread = init_read() ;	// a read object to hold the reads
		
	//
	// Open the file handles
	//
	
	// open the input file
        FILE * fin ;
        fin = fopen( fnin, "r" ) ;
	if( fin == NULL ) {
		perror( "Could not open input file" ) ;
		return EXIT_FAILURE ;
	}
        // open the output file
        FILE * fout ;
        fout = fopen( fnout, "a" ) ;
	if( fout == NULL ) {
                perror( "Could not open output file" ) ;
                return EXIT_FAILURE ;
        }


	// 
	// process the reads
	//

	// loop through all the reads in the input file
	while( get_read(fin, cread) ) {

		// rewind the linked list
		c   = begin_ll( c ) ;
		rid = NULL ;
		while( 1 ) {
			// get the barcode and match it to a read
			tmp  = (s_Sseq*) c->value ;
			mtch = match_read( cread, tmp, mismatches ) ;
			// if there is a match set the barcode name
			if( mtch ) {
				rid = tmp->name ;
				break ;
			}
			// iterate over the linked list
			if( c->next == NULL ) break ;
			c = c->next ;
		}

		// if unknown tell us also
                if( rid == NULL ) {
			rid = UNKNOWN ;
		}
		// print the index for the read 
		fprintf( fout, "%d\t%s\t%s\n", rno, cread->id, rid ) ;
		rno++ ;
	}

	// free and emtpy the read object
//	free_read( cread ) ;
	fclose( fin ) ;
	fclose( fout ) ;

	//
	return retval ;
}


/**
 * Initialize the match sequences
 *
 * Args:
 *   ids        the barcode ids
 *   codes      the barcode sequences
 *   starts     the starting bases (0-offset)
 *
 * Returns:
 *   An array with the s_Sseq
 *
 **/
ll_item * init_mseqs( ll_item * id, ll_item * sq, ll_item * s ) {
	s_Sseq * tmp = NULL ;
	ll_item * c  = NULL ;
	
	//
	id = begin_ll( id ) ;
	sq = begin_ll( sq ) ;
	s  = begin_ll( s ) ;
		
	while( 1 ) {
		// 
		tmp = (s_Sseq *) malloc( sizeof(struct s_Sseq) ) ;
		tmp->name  = (char *) id->value ;
		tmp->seq   = (char *) sq->value ;
		tmp->start = *((int *) s->value) ;

		// 
		c = add_ll( c, (void*) tmp ) ;

		//
		if( id->next == NULL) break ;
                id = id->next ;	
		sq = sq->next ;	
		s  = s->next ;
	}

	// return the object pointers
	return c ;
}

void clean_s_Sseq( s_Sseq * s ) {
	free( s->seq ) ;
	free( s->name ) ;
}


