#include "ping.h"

void *tping(void *arg)
{
  int ping_seconds      = 0;        //in seconds
  time_t ping_start     = time(0);  //current time

  while(1)
  {
    //If time to ping
    if(ping_seconds >= PING_SEND) {
      //Console printout
      printf("Sending ping to %d clients \n", list.length);
      //Iterate list
      for(int i = 0; i < list.length; i++) {
        //If client last response time is older than ping timeout time
        if(difftime(time(0), list.data[i]->trecv) > PING_TIMEOUT) {
          //Set flag to quit
          list.data[i]->quit = 1;
          //Close the socket
          close(list.data[i]->sock);
          //Continue to next client
          continue;
        }

        //Prepare buffer
        char buff[32];
        //Clear buffer
        memset(buff, 0, 32);
        //Set PING message to buffer
        strcpy(buff, "200 ");
        strcat(buff, SERVER_NAME);
        strcat(buff, ":PING\r\n");
        //Send PING message to client
        send(list.data[i]->sock, buff, strlen(buff), 0);
      }

      //Reset ping timer
      ping_start = time(0);
    }

    //Get elapsed time between current time and ping start time
    ping_seconds = difftime(time(0), ping_start);
    //Wait 1 second
    sleep(1);
  }

  //Return from thread
  return NULL;
}
