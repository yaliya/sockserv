#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <libconfig.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "shared.h"
#include "clist.h"
#include "tloop.h"
#include "ping.h"

void init_config(const char* conf);

int main(int argc, char *argv[])
{
  //Read config file, if any
  init_config("config.conf");
  //Server address
  struct sockaddr_in server_addr;
  //Client address
  struct sockaddr_in client_addr;

  //Open socket
  int sockfd                  = socket(AF_INET, SOCK_STREAM, 0);
  //Set family
  server_addr.sin_family      = AF_INET;
  //Set port
  server_addr.sin_port        = htons(SERVER_PORT);
  //Set in address
  server_addr.sin_addr.s_addr = INADDR_ANY;

  //Bind socket
  if(bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
    //Throw error on bind fail
    perror("Cannot bind socket");
    //Exit program
    return -1;
  }

  //Ping thread
  pthread_t ping_thread;
  //Start the ping thread
  pthread_create(&ping_thread, NULL, tping, NULL);

  //Console printout
  printf("Listening for connections \n");

  //List for incomming connections
  listen(sockfd, 7);

  //Size of client address
  int cl_addrlen = sizeof(client_addr);

  //Initialize client list
  clist_init(&list);

  //Start accepting connections
  while(1)
  {
    //On accept get socket
    int c_sockfd = accept(sockfd, (struct sockaddr*) &client_addr, (socklen_t*) &cl_addrlen);

    //If invalid socket
    if(c_sockfd < 0) {
      //Throw error
      perror("Cannot accept connections");
      //Exit program
      return -1;
    }

    //Client loop thread
    pthread_t loop_thread;
    //Create sepparate thread for each connected client
    pthread_create(&loop_thread, NULL, tloop, &c_sockfd);
  }

  //Clear client list
  clist_clear(&list);
  //Close server socket
  close(sockfd);

  return 0;
}

void init_config(const char *conf)
{
  printf("Reading config %s\n", conf);
  config_t cfg;
  config_init(&cfg);

  if(config_read_file(&cfg, conf)) {
    config_lookup_string(&cfg, "server.config.IP", &SERVER_IP);
    config_lookup_int(&cfg, "server.config.PORT", &SERVER_PORT);
    config_lookup_string(&cfg, "server.config.HOSTNAME", &SERVER_NAME);
    config_lookup_string(&cfg, "server.config.TOPIC", &SERVER_TOPIC);
    config_lookup_int(&cfg, "server.config.PINGS", &PING_SEND);
    config_lookup_int(&cfg, "server.config.PINGT", &PING_TIMEOUT);
  }
  else {
    perror("Failed to read config file: ");
  }

  printf("%s@%s:%d\n", SERVER_IP, SERVER_NAME, SERVER_PORT);
}
