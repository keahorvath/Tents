#include <stdbool.h>

typedef char* data_type;
typedef struct DList *DList;

// returns an empty list
DList dlist_create_empty(void);

// tests whether a DList is empty
bool dlist_isEmpty(DList L);

// gets next element in a DList
DList dlist_next(DList L);

// gets previous element in a DList
DList dlist_prev(DList L);

// gets data in a SList
data_type dlist_data(DList L);

//adds a new element on the start of the list
DList dlist_prepend(DList L, data_type data);

//deletes the first element of a DList
DList dlist_delete_first(DList L);

//inserts the data after
DList dlist_insert_after(DList L, DList p, data_type data);

//deletes the data after
DList dlist_delete_after(DList L, DList p);

//inserts the data before
DList dlist_insert_before(DList L, DList p, data_type data);

//deletes the data before
DList dlist_delete_before(DList L, DList p);

//frees the empty list
void dlist_free(DList L);

