#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "getopt.h"

#include "linkedlist.h"
#include "demultiplex.h"
#include "divide.h"

using namespace std ;


/** 
 *
 *
 **/
int divide( char * fnin, char * fnindex ) {
	int retval = 0 ;

	return retval ;
}

/**
 *
 *
 *
 **/
void usage( char error[] ) {
	char * message = "Usage demultiplex [index|divide|both] -i [INDEX FILE] -f [INPUT FASTQ] -o [OUTPUT FASTQ] -s [BARCODE START] -c [SAMPLE IDS] -m [NUMBER OF MISMATCHES] -b [BARCODES]" ;

        fprintf( stderr, "%s\n\n%s\n", error, message ) ;      
}



/**
 *
 *
 *
 **/
int main( int argc, char** argv ) {
	
	//
	// Parse the options
	//	
	int retval  = EXIT_SUCCESS ;
	char * task = NULL ;	
	if( argc < 2 ) {
		usage("No function selected") ;	
		return EXIT_FAILURE ;
	}
	task = argv[1] ;


	// define and initialize the parameters
        int mismatches   = 0 ;
        char * fnin      = NULL ;
        char * fnind     = NULL ;
        ll_item * seqs   = NULL ;
        ll_item * names  = NULL ;
        ll_item * starts = NULL ;
        ll_item * fnout  = NULL ;

        // temporary variables
        int c ;
        char * pch ;
	int * ps ;

        // parse the options
        while( (c=getopt(argc, argv, "f:o:i:s:c:b:m:" )) != -1 ) {
                switch( c ){
                        case 'f':
                                // input file
                                fnin = optarg ;
                                break ;
                        case 'i':
                                // output file
                                fnind = optarg ;
                                break ;

                        case 's':
                                // barcode start bases (0-offset)
                                pch = strtok( optarg, "," ) ;

                                while( pch != NULL ) {
                                        // expand the int array to incorporate the new values
                                        int s  = atoi( pch ) ;
					ps     = (int *) malloc( sizeof(int) ) ;
					memcpy( ps, &s, sizeof(int) ) ;

					// register the value and continue the loop
                                        starts = add_ll( starts, (void *) ps) ;
                                        pch  = strtok( NULL, "," ) ;
                                }
                                break ;

                        case 'b':
                                // barcode sequences
                                pch = strtok( optarg, "," ) ;
                                while( pch != NULL ) {
                                        seqs = add_ll( seqs, (void *) pch ) ;
                                        pch  = strtok( NULL, "," ) ;
                                }
                                break ;
                        case 'c':
                                // barcode sample ids
                                pch = strtok( optarg, "," ) ;
                                while( pch != NULL ) {
                                        names = add_ll( names, (void *) pch ) ;
                                	pch   = strtok( NULL, "," ) ;
                                }
                                break ;
                        case 'm':
				// number of mismatches
                                mismatches = atoi( optarg ) ;
                                break ;
			case 'o':
				// output files
				pch = strtok( optarg, "," ) ;
				while( pch != NULL ){
					fnout = add_ll( fnout, (void *)pch ) ;
					pch   = strtok( NULL, "," ) ;
				}
				break ;
			default:
				fprintf(stderr, "Error: Unknown option %s", c ) ;
                }
        }


        //
        // Option testing
        //

        if( fnind == NULL ) {
                perror( "Index file not defined" ) ;
                return EXIT_FAILURE ;
        }
        if( fnin == NULL ) {
                perror( "Input file not defined" ) ;
                return EXIT_FAILURE ;
        }

	//	
	// Determine whether we need to index or divide
	//
	if( strncmp( task,"index", 1) == 0 || strncmp( task,"both", 1) == 0 ) {
		// tell the operator we are indexing the reads
		printf( "%s\n", "Indexing" ) ;

		// check the options
		int64_t n_seqs  = count_ll( seqs ) ;
	        int64_t n_nms   = count_ll( names ) ;
        	int64_t n_strts = count_ll( starts ) ;

		if( n_seqs != n_nms ) {
                	perror( "Error: number of sequences differs from number of sample ids" ) ;
                	return EXIT_FAILURE ;
        	}
        	if( n_seqs != n_strts  ) {
                	perror( "Error: number of sequences differs from number of starts" ) ;
                	return EXIT_FAILURE ;
        	}	
		if( fnin == NULL ) {
                	perror( "Input file not defined" ) ;
                	return EXIT_FAILURE ;
		}

        	// Prepare the barcodes
	        ll_item * bcodes = NULL ;
        	bcodes = init_mseqs( names, seqs, starts ) ;

		// report the barcodes
	        bcodes = begin_ll( bcodes ) ;
		s_Sseq * tmp ;
		while(1){
			tmp = (s_Sseq *) bcodes->value ;
			printf( "%s\t%s\t%d\n", tmp->name, tmp->seq, tmp->start ) ;

			if( bcodes->next == NULL ) break ;
			bcodes = bcodes->next ;
		}


	        // Index the reads
        	retval = index( fnind, fnin, bcodes, mismatches )  ;
		free_ll( bcodes ) ;

	}
	if (strncmp( task,"divide", 1) == 0 || strncmp( task,"both", 1) == 0) {
		// tell the operator we are dividing the reads
		printf( "%s\n", "Dividing" ) ;

		//
		int64_t n_fnout = count_ll( fnout ) ;
		int64_t n_nms   = count_ll( names ) ;
		//
		if( n_nms != n_fnout ) {
			perror( "Error: number of sample names differs from number of output files" ) ;
                        return EXIT_FAILURE ;
		}

		// Prepare the output files
		ll_item * sfhandles = get_comparison_file_handles( names, fnout ) ;

		sfhandles   = begin_ll( sfhandles ) ;

		//
		retval = divide( fnin, fnind, sfhandles ) ;
		
	}

	// cleanup
	free_ll( seqs ) ;
	free_ll( names ) ;
	free_ll( starts	) ;
	free_ll( fnout ) ;

	// report SUCCESS (tm)
	return retval ;
}
