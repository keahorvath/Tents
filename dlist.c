#include <stdio.h>
#include <stdlib.h>
#include "dlist.h"

struct DList{
    data_type data;
    struct DList* next;
    struct DList* previous;
};

static void memoryError(void){
    fprintf(stderr, "Not enough memory!\n");
    exit(EXIT_FAILURE);
}

static DList dlist_alloc(void){
    DList p = malloc(sizeof(struct DList));
    if (p == NULL)
        memoryError();
    p->next = NULL;
    p->previous = NULL;
    return p;
}

void dlist_free (DList L){
    free(L);
    L = NULL;
}

DList dlist_create_empty(void){
    return NULL;
}

bool dlist_isEmpty (DList L){
    return (L == NULL);
}

DList dlist_next (DList L){
    if( dlist_isEmpty(L) ){
        fprintf(stderr, "next on empty list!\n");
        exit(EXIT_FAILURE);
    }
    return L->next;
}

DList dlist_prev (DList L){
    if( dlist_isEmpty(L) ){
        fprintf(stderr, "previous on empty list!\n");
        exit(EXIT_FAILURE);
    }
    return L->previous;
}

data_type dlist_data (DList L){
    if( dlist_isEmpty(L) ){
        fprintf(stderr, "data on empty list!\n");
        exit(EXIT_FAILURE);
    }
    return L->data;
}

DList dlist_prepend (DList L, data_type data){
    DList p = dlist_alloc();
    p->data = data;
    p->next = L;
    if (L != NULL)
        L->previous= p;
    return p;
}

DList dlist_delete_first (DList L){
    if (L != NULL){
        DList nxt= L->next;
        dlist_free(L);
        L = nxt;
        if (L != NULL)
            L->previous= NULL;
    }
    return L;
}

DList dlist_insert_after(DList L, DList p, data_type data){
    if (p != NULL){
        DList next= p->next;
        DList new= dlist_alloc();
        new->data= data;
        new->previous= p;
        p->next= new;
        new->next= next;
        if(next != NULL)
            next->previous= new;
    }
    return L;
}

DList dlist_delete_after (DList L, DList p){
    if (p != NULL){
        if (p->next != NULL){
            DList to_be_deleted= p->next;
            p->next= to_be_deleted->next;
            if (to_be_deleted->next != NULL)
                to_be_deleted->next->previous= p;
            dlist_free(to_be_deleted);
        }
    }
    return L;
}

DList dlist_insert_before (DList L, DList p, data_type data){
    if(p != NULL){
        DList previous= p->previous;
        if(previous == NULL)
            return dlist_prepend(L, data);
        DList new= dlist_alloc();
        new->data= data;
        previous->next= new;
        new->previous= previous;
        new->next= p;
        p->previous= new;
    }
    return L;
}

DList dlist_delete_before (DList L, DList p){
    if(p != NULL){
        DList to_be_deleted= p->previous;
        if(to_be_deleted != NULL){
            DList previous=  to_be_deleted->previous;
            p->previous= previous;
            if(previous != NULL)
                previous->next= p;
            else
                L= p;
            dlist_free(to_be_deleted);
        }
    }
    return L;
}