#ifndef LINKEDLIST
#define LINKEDLIST



struct ll_item {
        void * value ;
        ll_item * next ;
        ll_item * prev ;
}  ;



ll_item * make_item( void * value ) ;

ll_item * next_ll( ll_item * c) ;

ll_item * prev_ll( ll_item * c) ;

ll_item * add_ll( ll_item * c, void * value) ;

ll_item * push_ll( ll_item * c, void * value) ;

ll_item * begin_ll( ll_item * c ) ;

ll_item * end_ll( ll_item * c ) ;

int64_t count_ll( ll_item * c ) ;

void free_ll( ll_item * c ) ;


#endif



