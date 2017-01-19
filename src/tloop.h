#ifndef TLOOP_H
#define TLOOP_H

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "shared.h"

void *tloop(void *arg);
void privmsg(int sock, const char *nick, const char *msg);
void privmsg_all(const char *nick, const char *msg);
void privmsg_motd(int sock, const char *filename);
void complain(int sock, const char *msg);
void require_nick(int sock, char *nick);

#endif
