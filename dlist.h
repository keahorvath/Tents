#include <stdbool.h>

typedef char* data_type;
typedef struct DList *DList;

// returns an empty list
DList dlist_create_empty(void);

// tests whether a DList is empty
bool dlist_isEmpty(DList L);

// gets next element in a DList ->  fonction suivant(val L:liste d'objet): liste d'objet;
DList dlist_next(DList L);

// gets previous element in a DList ->  fonction precedent(val L:liste d'objet): liste d'objet;
DList dlist_prev(DList L);

// gets data in a SList ->  fonction valeur(val L:liste d'objet): objet;
data_type dlist_data(DList L);

//adds a new element on the start of the list -> fonction insereEnTete(val L: liste d'objet, val x objet): liste d'objet;
DList dlist_prepend(DList L, data_type data);

// -> fonction supprimerEnTete(val L: liste d'objet): liste d'objet;
DList dlist_delete_first(DList L);

// -> fonction insererApres(val L:liste d'objets, val p : liste d'objets, val x: objet): liste d'objet;
DList dlist_insert_after(DList L, DList p, data_type data);

// -> fonction supprimerApres(val L:liste d'objets, val p : liste d'objets): liste d'objet;
DList dlist_delete_after(DList L, DList p);

// -> fonction insererAvant(val L:liste d'objets, val p : liste d'objets, val x: objet): liste d'objet;
DList dlist_insert_before(DList L, DList p, data_type data);

// -> fonction supprimerAvant(val L:liste d'objets, val p : liste d'objets): liste d'objet;
DList dlist_delete_before(DList L, DList p);

// -> fonction supprimerVide(val L:liste d'objets): vide;
void dlist_free(DList L);

