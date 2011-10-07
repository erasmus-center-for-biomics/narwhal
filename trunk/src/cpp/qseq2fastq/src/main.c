#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

using namespace std ;

//
uint LINE_BUFFER_SIZE = 3000 ;
uint ID_BUFFER_SIZE   = 1000 ;
uint SEQ_BUFFER_SIZE  = 4000 ;
uint QUAL_BUFFER_SIZE = 4000 ;


/**

This program reads a qseq file and outputs the data in the fastq format.

 **/

bool file_exists( char *filename ) {
  struct stat buffer ;
  bool retval = true ;
  int fstat   = stat( filename, &buffer ) ;

  if( fstat == 0 ) retval = false ;
  return retval ;
}

/*
 * Replace . with N
 *
 * Args:
 *   seq   -- 
 *
 * Returns:
 * void
 */
void process_sequence( char *seq ){ 
  uint cn ; 
  for( cn=0; cn<strlen(seq); cn++ ) {
    if( seq[cn] == '.' ) seq[cn] = 'N' ;
  }
}


/*
 * Determine the modified quality scores from 
 *
 * Args:
 * qual  --  quality values
 * d     --  integer
 *
 * Returns:
 * void
 */
void process_qualities( char *qual, int d ){ 
  uint cn ; 
  for( cn=0; cn<strlen(qual); cn++ ) {
    qual[cn] += d  ;
  }
}

/*
 * Process a buffer from a file
 *
 * A buffer will be read from the input stream. This buffer will 
 * be tokenized on end line characters. The tokenized strings 
 * will be processed to a
 *
 *
 *
 * Args:
 * buffer  - char  -  The buffer of the QseQ file
 * 
 * Returns:
 * void
 */
int process_file( FILE * input, FILE * output ) {
  uint l_size = LINE_BUFFER_SIZE ;
  uint i_size = ID_BUFFER_SIZE ;
  uint s_size = SEQ_BUFFER_SIZE ;
  uint q_size = QUAL_BUFFER_SIZE ;

  uint l_len ;
  uint t_count ;           // the number of tabs in a line

  char * fbuffer = NULL ;  // a buffer to hold the processed FastQ read
  char * i_buff  = NULL ;
  char * s_buff  = NULL ;
  char * q_buff  = NULL ;

  char * rbuffer = NULL ;  // a buffer to hold the lines read from the file
  char * pch     = NULL ;  // the pointer going over the tokenized endlines
  
  // allocate a buffer
  rbuffer = (char *)malloc( l_size * sizeof(char) ) ;
  if( rbuffer == NULL ) { 
    perror( "Could not allocate memory for the line buffer" ) ;
    return -1 ;
  }

  // fastq buffer
  fbuffer = (char *)malloc( l_size * sizeof(char) ) ;
  if( fbuffer == NULL ) { 
    perror( "Could not allocate memory for FastQ buffer" ) ;
    return -1 ;
  }
  //
//  i_buff = (char *)malloc( i_size * sizeof(char) ) ;
 // s_buff = (char *)malloc( s_size * sizeof(char) ) ;
  //q_buff = (char *)malloc( q_size * sizeof(char) ) ;


  // run through the file by taking big chunks
  while( fgets( rbuffer, l_size, input ) != NULL ) {        
    // prepare the variables for the next round
    fbuffer[0] = '\0' ;
    /*
    i_buff[0]  = '\0' ;
    s_buff[0]  = '\0' ;
    q_buff[0]  = '\0' ;
    */
    i_buff = (char *)malloc( i_size * sizeof(char) ) ;
    s_buff = (char *)malloc( s_size * sizeof(char) ) ;
    q_buff = (char *)malloc( q_size * sizeof(char) ) ;


    t_count    = 0 ; 

    // remove thr trailing newline
    l_len = strlen( rbuffer ) ;
    if( rbuffer[l_len-1] == '\n' ) rbuffer[l_len-1] = '\0'  ;

    // tokenize the buffer on tabs   
    pch     = strtok( rbuffer, "\t" ) ;
    while( pch != NULL ){          
      t_count++ ; 
      // process the columns
      
      // generate the ID
      if(t_count < 6) {
	strcat( i_buff, pch ) ;
	strcat( i_buff, ":" ) ;
      }else if(t_count == 6) {
	strcat( i_buff, pch ) ;
	strcat( i_buff, "#" ) ;
      }else if(t_count == 7) {
	strcat( i_buff, pch ) ;
	strcat( i_buff, "/" ) ;	
      }else if(t_count == 8) {
	strcat( i_buff, pch ) ;
      }else if(t_count == 11) {
	strcat( i_buff, ":" ) ;
	strcat( i_buff, pch ) ;	

      }else if(t_count == 9) {
	// process the sequence
	process_sequence( pch ) ; 
	strcpy( s_buff, pch ) ;
      }else if(t_count == 10) {
	//process the qualities
	process_qualities( pch, -31 ) ;
	strcpy( q_buff, pch ) ;
      }     
      // next tab
      pch = strtok( NULL, "\t" ) ;
    }  
    fprintf( output, "@%s\n%s\n+\n%s\n", i_buff, s_buff, q_buff ) ;  
    free( i_buff ) ;
    free( s_buff ) ;
    free( q_buff ) ;
  }

  // free the buffers
  free( rbuffer ) ;
  free( fbuffer ) ;
//  free( i_buff ) ;
//  free( s_buff ) ;
//  free( q_buff ) ;
  return 1 ;
}



/*
 * the main program loop
 */
int main( int argc, char** argv ){
  int retval ;
  int retproc ;

  bool infile ;
  bool outfile ;

  FILE * input  = NULL ;
  FILE * output = NULL ;

  //
  // Parameter parsing
  // 
  // determine the input file
  if( argc == 1 ) {
    infile = false ;
    input  = stdin ;
  } else {    
    infile = true ;
    input  = fopen( argv[1], "r" ) ;
  }
  // check if the input is indeed opened
  if( input == NULL ){
    perror( "Failed to open input stream" ) ;
    return EXIT_FAILURE ;
  }

  // determine the output file
  if( argc != 3  ){
    outfile = false ;
    output  = stdout ;
  } else {
    outfile = true ;
    output  = fopen( argv[2], "a" ) ;
  }
  
  // check whether the output stream is open
  if( output == NULL ){
    perror( "Failed to open output stream" ) ;
    return EXIT_FAILURE ;
  }
 
  //
  // 
  //
  retval  = EXIT_SUCCESS ;
  retproc = process_file( input, output ) ;
  if( retproc == -1 )  retval = EXIT_FAILURE ;

 
  // close the input and output file if we are done
  if( infile ) fclose( input ) ; 
  if( outfile ) fclose( output ) ;

  // cleanup the memory
  return retval ;
}
