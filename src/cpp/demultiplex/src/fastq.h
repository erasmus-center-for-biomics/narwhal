#ifndef FASTQ
#define FASTQ

/**
  A struct to hold the read information
 **/
struct s_Read {
        char * id ;
        char * seq ;
        char * qual ;
} ;


/**
 * get a read from a file
 *
 * Args:
 *  f   FILE    the opened fastq file
 *  r   s_Read  the read object to fill
 *
 * Returns:
 *  0 on failure, 1 on success
 *
 **/
int get_read( FILE * f, s_Read * r ) ;

void free_read( s_Read * r ) ;

s_Read * init_read( ) ;

void write_fastq( FILE * f, s_Read * r ) ;


#endif
