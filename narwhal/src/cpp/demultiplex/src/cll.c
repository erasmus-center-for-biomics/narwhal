#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"



/**
 * Initialize the linked list
 * 
 */
citem * make_citem( char * value ) {
	citem * retval = NULL ;

	retval = (citem *) malloc( sizeof(citem) ) ;
	retval->value = value ;
	retval->next  = NULL ;
	retval->prev  = NULL ;	
	return retval ;  
}



/**
 *
 **/
citem * next_ll( citem * c) {
	return c->next ;
}



/**
 *
 **/
citem * prev_ll( citem * c) {
        return c->prev ;
}


/**
 *
 *
 *
 **/
citem * add_ll( citem * c, char * value) {
	citem * retval = NULL ;
	citem * x      = c ;

	// traverse the stack to the end
	while( x->next != NULL ) {	
		x = x->next ;
	}

	// make a new item and add it to the end of the list
	retval = make_citem( value )  ;
	retval->prev = x ;
	x->next      = retval ;

	// return 
        return retval ;
}


/**
 *
 *
 *
 **/
citem * push_ll( citem * c, char * value) {
        citem * retval = NULL ;
        citem * x      = c ;

        // traverse the stack to the beginning
        while( x->prev != NULL ) {
                x = x->prev ;
        }

        // make a new item and add it to the beginning of the list
        retval = make_citem( value )  ;
        retval->next = x ;
        x->prev      = retval ;

        // return
        return retval ;
}

citem * begin_ll( citem * c ) {
	citem * x = c ;
	while( x->prev != NULL ) {
		x = x->prev ;
	}
	return x ;
}


citem * end_ll( citem * c ) {
        citem * x = c ;
        while( x->next != NULL ) {
                x = x->next ;
        }
        return x ;
}


