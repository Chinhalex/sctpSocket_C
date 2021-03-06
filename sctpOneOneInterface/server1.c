/*
1. socket()
2. bind()
3. listen()
4. accept()

The accept() call blocks until a new association is set up.
It returns with a new socket descriptor. The server then uses 
the new socket descriptor to communicate with the client, 
using recv() and send() calls to get requests and send back responses. 
Then it calls

5. close()
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#define MAX_BUFFER 1024
#define MY_PORT_NUM 62324 /* This can be changed to suit the need and should be same in server and client */

int main ()
{
    int listenSock, connSock, ret, in, flags, i, lenClient;
    struct sockaddr_in servaddr, clientaddr;
    struct sctp_initmsg initmsg;
    struct sctp_event_subscribe events;
    struct sctp_sndrcvinfo sndrcvinfo;
    char buffer[MAX_BUFFER + 1];

    // create socket
    listenSock = socket (AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    //check status of creating a socket
    if(listenSock == -1)
    {
        printf("Failed to create socket\n");
        perror("socket()");
        exit(1);
    }

    //create address for server
    bzero ((void *) &servaddr, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    servaddr.sin_port = htons (MY_PORT_NUM);

    //bind address for unamed socket
    ret = bind (listenSock, (struct sockaddr *) &servaddr, sizeof (servaddr));
    //check status of binding
    if(ret == -1 )
    {
        printf("Bind failed \n");
        perror("bind()");
        close(listenSock);
        exit(1);
    }

    //Specify that a maximum of 5 streams will be available per socket 
    memset (&initmsg, 0, sizeof (initmsg));
    initmsg.sinit_num_ostreams = 5;
    initmsg.sinit_max_instreams = 5;
    initmsg.sinit_max_attempts = 4;
    ret = setsockopt (listenSock, IPPROTO_SCTP, SCTP_INITMSG,
        &initmsg, sizeof (initmsg));

    if(ret == -1 )
    {
        printf("setsockopt() failed \n");
        perror("setsockopt()");
        close(listenSock);
        exit(1);
    }
    
    //listen connecting from client
    ret = listen (listenSock, 5);
    if(ret == -1 )
    {
        printf("listen() failed \n");
        perror("listen()");
        close(listenSock);
        exit(1);
    }

    while (1)
        {

        char buffer[MAX_BUFFER];
       
        //Clear the buffer
        bzero (buffer, MAX_BUFFER);

        printf ("Awaiting a new connection\n");

        lenClient = sizeof(clientaddr);
        connSock = accept (listenSock, (struct sockaddr *) &clientaddr, &lenClient);
        if (connSock == -1)
        {
            printf("accept() failed\n");
            perror("accept()");
            close(connSock);
            continue;
        }
        else
            printf ("New client connected....\n");

        //receiving mess from client
        in = sctp_recvmsg (connSock, buffer, sizeof (buffer),
        (struct sockaddr *) NULL, 0, &sndrcvinfo, &flags);
        
        if( in == -1)
        {
            printf("Error in sctp_recvmsg\n");
            perror("sctp_recvmsg()");
            close(connSock);
            continue;
        }
        else
        {
            printf (" Length of Data received: %d\n", in);
            printf (" Data : %s\n", (char *)buffer);
        }
        close (connSock);
        }

    return 0;
}

// Build: gcc -o server1 server1.c -lsctp