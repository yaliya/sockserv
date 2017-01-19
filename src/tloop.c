#include "tloop.h"

void *tloop(void *arg)
{
  client user;
  user.sock = *(int*)arg;

  privmsg_motd(user.sock, "motd.txt");
  sprintf(user.nick, "client_%d", list.length);

  user.quit  = 0;
  user.trecv = time(0);

  clist_insert(&list, &user);

  char msg[128];

  sprintf(msg, "Welcome to server %s", user.nick);
  privmsg(user.sock, SERVER_NAME, msg);

  sprintf(msg, "%s joined", user.nick);
  privmsg_all(SERVER_NAME, msg);

  while(user.quit != 1)
  {
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    int recvResult = recv(user.sock, buff, sizeof(buff), 0);

    if(recvResult == 0 || strlen(buff) == 0) {
      user.quit = 1;
      sprintf(msg, "%s closed connection", user.nick);
      privmsg_all(SERVER_NAME, msg);
      continue;
    }

    user.trecv = time(0);

    char cmd[24];
    char body[1000];
    memset(cmd, 0, sizeof(cmd));
    memset(body, 0, sizeof(body));
    sscanf(buff, "%24s %1000[^\r\n]", cmd, body);

    if(strcmp(cmd, "/quit") == 0) {
      user.quit = 1;
      sprintf(msg, "%s closed connection", user.nick);
      privmsg_all(SERVER_NAME, msg);
      continue;
    }

    else if(strcmp(cmd, "/nick") == 0) {
      char tmpnick[32];
      memset(tmpnick, 0, sizeof(tmpnick));
      sscanf(body, "%32s", tmpnick);

      if(strlen(tmpnick) == 0) {
        complain(user.sock, "Nick too short");
        continue;
      }

      int nickused = 0;

      for(int i = 0; i < list.length; i++) {
        if(strcmp(tmpnick, list.data[i]->nick) == 0) {
          complain(user.sock, "Nick already in use");
          nickused = 1;
          break;
        }
      }

      if(nickused) {
        continue;
      }

      sprintf(msg, "%s changed nick to %s", user.nick, tmpnick);
      privmsg_all(SERVER_NAME, msg);
      strcpy(user.nick, tmpnick);
    }

    else if(strcmp(cmd, "/list") == 0) {
      strcpy(buff, "");

      for(int i = 0; i < list.length; i++) {
        strcat(buff, list.data[i]->nick);
        strcat(buff, ";");
      }

      privmsg(user.sock, SERVER_NAME, buff);
    }

    else if(strcmp(cmd, "/msg") == 0) {
      char cmsg[1000];
      memset(cmsg, 0, sizeof(cmsg));
      sscanf(body, "%1000[^\r\n]", cmsg);
      privmsg_all(user.nick, cmsg);
    }
  }

  clist_remove(&list, user.id);
  close(user.sock);
  return NULL;
}

void privmsg(int sockfd, const char *nick, const char *msg)
{
  char buff[strlen(nick) + strlen(msg) + 32];
  memset(buff, 0, sizeof(buff));

  strcpy(buff, "200 ");
  strcat(buff, nick);
  strcat(buff, ":");
  strcat(buff, msg);
  strcat(buff, "\r\n");
  send(sockfd, buff, strlen(buff), 0);
}

void privmsg_all(const char *nick, const char *msg)
{
  for(int i = 0; i < list.length; i++) {
    if(list.data[i]->quit == 0) {
      privmsg(list.data[i]->sock, nick, msg);
    }
  }
}

void privmsg_motd(int sock, const char *filename)
{
  FILE* file = fopen(filename, "r");

  if(file) {
    char line[256];
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    memset(line, 0, sizeof(line));

    while(fgets(line, sizeof(line), file)) {
      memset(buff, 0, sizeof(buff));
      //strcpy(buff, "200 ");
      //strcat(buff, SERVER_NAME);
      //strcat(buff, ":");
      strcat(buff, line);
      send(sock, buff, strlen(buff), 0);
    }

    fclose(file);
  }
}

void complain(int sockfd, const char *msg)
{
  char buff[1024];
  memset(buff, 0, sizeof(buff));

  strcpy(buff, "500 ");
  strcat(buff, msg);
  strcat(buff, "\r\n");
  send(sockfd, buff, strlen(buff), 0);
}
