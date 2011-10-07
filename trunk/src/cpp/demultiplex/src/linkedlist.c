#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "linkedlist.h"



/**
 * Initialize the linked list
 * 
 */
ll_item * make_item( void * value ) {
	ll_item * retval = NULL ;

	retval = (ll_item *) malloc( sizeof(ll_item) ) ;
	retval->value = value ;
	retval->next  = NULL ;
	retval->prev  = NULL ;	
	return retval ;  
}

/**
 *
 **/
ll_item * next_ll( ll_item * c) {
	if( c != NULL ) {
		return c->next ;
	} else {
		return NULL ;
	}
}

/**
 *
 **/
ll_item * prev_ll( ll_item * c) {
	if( c != NULL ) {
		return c->prev ;
	} else {
		return NULL ;
        }
}

/**
 *
 *
 *
 **/
 ll_item * add_ll( ll_item * c, void * value) {
	if( c != NULL ) {
		ll_item * t = NULL ;
		ll_item * x = c ;

		// traverse the stack to the end
		x = end_ll( x ) ;

		// make a new item and add it to the end of the list
		t = make_item( value )  ;
		t->prev = x ;
		x->next = t ;
		c = t ;
	} else {
		c = make_item( value ) ;
	}
	return c ;
}

/**
 *
 *
 *
 **/
ll_item * push_ll( ll_item * c, void * value) {
	if( c != NULL ) {
	        ll_item * t = NULL ;
        	ll_item * x = c ;

	        // traverse the stack to the beginning
        	x = begin_ll( x ) ;

	        // make a new item and add it to the beginning of the list
        	t = make_item( value )  ;
	        t->next = x ;
	        x->prev = t ;
		c = t ;
	} else {
		c = make_item( value ) ;
	}
	return c ;
}

ll_item * begin_ll( ll_item * c ) {
	ll_item * x = NULL ;
	if ( c != NULL ) {
		x = c ; 
		while( 1 ) {
			if( x->prev == NULL ) break ;
			x = x->prev ;
		}
	}
	return x ;
}


ll_item * end_ll( ll_item * c ) {
        ll_item * x = NULL ; 
	if( c != NULL ) {
		x = c ;
        	while( 1 ) {
			if( x->next == NULL ) break ;
	                x = x->next ;
        	}
	}
        return x ;
}

int64_t count_ll( ll_item * c ) { 
	int64_t cn = 0 ;
	if( c != NULL ) {
		c = end_ll( c ) ;
		while( 1 ) {
			cn++ ;
			if( c->prev == NULL ) break ;
			c = c->prev ;
		}
	}
	return cn ; 
}



void free_ll( ll_item * c ) {
	if( c != NULL ) {
		c = end_ll( c ) ;
		while( 1 ) {
			if( c->prev == NULL ) break ;
			c = c->prev ;
			free( c->next ) ;
			c->next = NULL ;
		}
		free(c) ;
	}
}
