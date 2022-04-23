/* 
1. socket()
2. connect()
After returning from connect(), the client uses 
send() and recv() calls to send out requests and
receive responses from the server. The client calls
3. close()
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#define MAX_BUFFER 1024
#define MY_PORT_NUM 62324 //port of client

int main (int argc, char* argv[])
{
  int connSock, in, i, ret, flags;
  struct sockaddr_in servaddr;
  struct sctp_status status;
  char buffer[MAX_BUFFER];
  int datalen = 0;

  /*Get the input from user*/
  printf("Enter data to send: ");
  fgets(buffer, MAX_BUFFER, stdin);
  /* Sample input */
  //strncpy (buffer, "Hello Server", 12);
  //buffer[12] = '\0';
  datalen = strlen(buffer);

  //create socket  
  connSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);

   //check error during create socket 
  if (connSock == -1)
  {
      printf("Socket creation failed\n");
      perror("socket()");
      exit(1);
  }

  //create address for server  
  bzero ((void *) &servaddr, sizeof (servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons (MY_PORT_NUM);
  servaddr.sin_addr.s_addr = inet_addr ("127.0.0.1");

  //connect to server  
  ret = connect (connSock, (struct sockaddr *) &servaddr, sizeof (servaddr));

  //check error during connect to server  
  if (ret == -1)
  {
      printf("Connection failed\n");
      perror("connect()");
      close(connSock);
      exit(1);
  }

  // sendmsg to server   
  ret = sctp_sendmsg (connSock, (void *) buffer, (size_t) datalen,
        NULL, 0, 0, 0, 0, 0, 0);

  //check status during send mess to server  
  if(ret == -1 )
  {
    printf("Error in sctp_sendmsg\n");
    perror("sctp_sendmsg()");
  }
  else
      printf("Successfully sent %d bytes data to server\n", ret);

  close (connSock);

  return 0;
}