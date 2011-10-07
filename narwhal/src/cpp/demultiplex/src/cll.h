#ifndef DEMULTIPLEX
#define LINKEDLIST



struct citem {
        char * value ;
        citem * next ;
        citem * prev ;
}  ;



citem * make_citem( char * value ) ;

citem * next_ll( citem * c) ;

citem * prev_ll( citem * c) ;

citem * add_ll( citem * c, char * value) ;

citem * push_ll( citem * c, char * value) ;


citem * begin_ll( citem * c ) ;

citem * end_ll( citem * c ) ;

#endif



