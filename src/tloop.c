#include "tloop.h"

void *tloop(void *arg)
{
  client user;
  user.sock = *(int*)arg;

  privmsg_motd(user.sock, "motd.txt");
  require_nick(user.sock, user.nick);

  if(strlen(user.nick) == 0) {
    close(user.sock);
    return NULL;
  }

  user.quit  = 0;
  user.trecv = time(0);

  clist_insert(&list, &user);

  char msg[128];

  sprintf(msg, "Welcome to server %s", user.nick);
  privmsg(user.sock, msg);

  sprintf(msg, "%s joined", user.nick);
  privmsg_all(msg);

  while(user.quit != 1)
  {
    char buff[1024];
    memset(buff, 0, sizeof(buff));
    int recvResult = recv(user.sock, buff, sizeof(buff), 0);

    if(recvResult == 0) {
      user.quit = 1;
      sprintf(msg, "%s closed connection", user.nick);
      privmsg_all(msg);
      continue;
    }

    if(strlen(buff) == 0) {
      user.quit = 1;
      sprintf(msg, "%s closed connection", user.nick);
      privmsg_all(msg);
      continue;
    }

    user.trecv = time(0);

    char cmd[24];
    char body[1000];
    memset(cmd, 0, sizeof(cmd));
    memset(body, 0, sizeof(body));
    sscanf(buff, "%24s %1000[^\r\n]", cmd, body);

    if(strcmp(cmd, "QUIT") == 0) {
      user.quit = 1;
      sprintf(msg, "%s closed connection", user.nick);
      privmsg_all(msg);
      continue;
    }

    else if(strcmp(cmd, "NICK") == 0) {
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
      privmsg_all(msg);
      strcpy(user.nick, tmpnick);
    }

    else if(strcmp(cmd, "LIST") == 0) {
      strcpy(buff, "");

      for(int i = 0; i < list.length; i++) {
        strcat(buff, list.data[i]->nick);
        strcat(buff, ";");
      }

      privmsg(user.sock, buff);
    }

    else if(strcmp(cmd, "MSG") == 0) {
      char cmsg[1000];
      memset(cmsg, 0, sizeof(cmsg));
      sscanf(body, "%1000[^\r\n]", cmsg);
      privmsg_all(cmsg);
    }
  }

  clist_remove(&list, user.id);
  close(user.sock);
  return NULL;
}

void privmsg(int sockfd, const char *msg)
{
  char buff[1024];
  memset(buff, 0, sizeof(buff));

  strcpy(buff, "200 ");
  strcat(buff, msg);
  strcat(buff, "\r\n");
  send(sockfd, buff, strlen(buff), 0);
}

void privmsg_all(const char *msg)
{
  for(int i = 0; i < list.length; i++) {
    if(list.data[i]->quit == 0) {
      privmsg(list.data[i]->sock, msg);
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
      strcpy(buff, "200 ");
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

void require_nick(int sock, char *tnick)
{
  int nickset = 0;
  char buff[32];
  char cmd[32];
  char nick[32];
  char msg[128];

  while(!nickset)
  {
    memset(buff, 0, sizeof(buff));
    memset(nick, 0, sizeof(nick));
    memset(msg, 0, sizeof(msg));
    memset(cmd, 0, sizeof(cmd));

    strcpy(msg, "200 Enter your nick: ");
    int sendRes = send(sock, msg, strlen(msg), 0);

    if(sendRes == -1) {
      return;
    }

    int recvRes = recv(sock, buff, sizeof(buff), 0);

    if(recvRes == 0) {
      return;
    }

    sscanf(buff, "%32s %32[^\r\n]", cmd, nick);

    if(strlen(nick) <= 3) {
      complain(sock, "Nick too short, min 3 characters");
      continue;
    }

    if(strlen(nick) > 30) {
      complain(sock, "Nick too long, max 30 characters");
      continue;
    }

    int nickinuse = 0;

    for(int i = 0; i < list.length; i++) {
      if(strcmp(nick, list.data[i]->nick) == 0) {
        nickinuse = 1;
      }
    }

    if(nickinuse) {
      complain(sock, "Nick already in use");
      continue;
    }

    strcpy(tnick, nick);
    nickset = 1;
  }
}
