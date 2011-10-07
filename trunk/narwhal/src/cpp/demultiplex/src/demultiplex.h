#ifndef DEMULTIPLEX
#define DEMULTIPLEX

#include "fastq.h"

/**
 *  a struct that represents a search sequence
 **/
struct s_Sseq {
	char * seq ;
	char * name ;
	int start ;
} ;

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
int edit_distance( char * a, char * b ) ;

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
int match_read( s_Read& r, s_Sseq& b, uint8_t max ) ;


/**
 * index the reads for a FastQ file and write the output to the
 * output file .
 *
 * Args:
 *   fnout	char *	the output filename
 *   fnin	char *	the input filename	
 *   codes	array of s_Sseq structs	the barcode information
 *   mismatches	uint	the number of mismatches
 *
 * Return:
 *   0 on failure, 1 on success
 **/
int index( char * fnout, char * fnin, ll_item * codes, uint mismatches ) ;


/**
 * Initialize the match sequences
 *
 * Args:
 *   ids	the barcode ids
 *   codes	the barcode sequences
 *   starts	the starting bases (0-offset)
 *
 * Returns"
 *   An array with the s_Sseq 
 *
 **/
ll_item * init_mseqs( ll_item * ids, ll_item * codes, ll_item * starts ) ;



void clean_s_Sseq( s_Sseq * s ) ;


#endif
