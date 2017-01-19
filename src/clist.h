#ifndef CLIST_H
#define CLIST_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Client data struct
typedef struct
{
  int     id;        //Used as index on the list
  int     sock;      //Socket number
  int     quit;      //Should Quit flag
  time_t  trecv;     //Last received message time
  char    nick[32];
  char    ipaddr[32];
  char    email[128];
  char    password[32];
} client;

//Client list struct
typedef struct
{
  int size;
  int length;
  int capacity;
  client** data;
} client_list;

//R

//Init list
void clist_init(client_list* list);
//Inserts new client on list
void clist_insert(client_list* list, client* data);
//Resizes the list
void clist_resize(client_list* list, int capacity);
//Removes a client by index from list
void clist_remove(client_list* list, int index);
//Clears the list
void clist_clear(client_list* list);

//Global list variable
//To keep track of clients
//Accessible across threads
client_list list;

#endif
