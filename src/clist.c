#include "clist.h"

void clist_init(client_list* list)
{
  list->size = 0;
  list->length = 0;
  list->capacity = 32;
  list->data = malloc(sizeof(client) * list->capacity);
}

void clist_resize(client_list* list, int capacity)
{
  client** data = realloc(list->data, sizeof(client *) * capacity);

  if (data) {
    list->data = data;
    list->capacity = capacity;
  }
}

void clist_insert(client_list* list, client* data)
{
  if(list->length == list->capacity) {
    clist_resize(list, list->capacity * 2);
  }

  data->id = list->length;
  list->data[list->length++] = data;
}

void clist_remove(client_list* list, int index)
{
  //version1
  list->length -= 1;
  memmove(&list->data[index], &list->data[index + 1], (list->capacity - index) * sizeof(client));
  list->data = realloc(list->data, list->capacity * sizeof(client));

  for(int i = 0; i < list->length; i++) {
    list->data[i]->id = i;
  }

  if(list->length > 0 && list->length == list->capacity / 4)
    clist_resize(list, list->capacity / 2);
}

void clist_clear(client_list* list)
{
  list->length = 0;
  list->capacity = 2;
  free(list->data);
}
