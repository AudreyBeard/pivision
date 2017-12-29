/*
code for creating a simple socket connection

*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      // gethostbyname()
#include <ctype.h>
#define SIZE_BUFF_IN 256
#define SIZE_BUFF_OUT 256


void error(const char *msg)
{
    perror(msg);
    exit(0);
}


char* lower(char* word){
    unsigned int i;
    for(i = 0; i < strlen(word); i++ )
        word[i] = tolower(word[i]);
    return word;
}


int main(int argc, char *argv[])
{
    int sockfd, portno, n_in, n_out;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer_in[SIZE_BUFF_IN], buffer_out[SIZE_BUFF_OUT];
    
    // Check number of arguments
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
    // Get port number
    portno = atoi(argv[2]);

    // Open socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    // Get server
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    // Zero out server address and set it
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(  (char *)server->h_addr, 
            (char *)&serv_addr.sin_addr.s_addr,
            server->h_length);
    serv_addr.sin_port = htons(portno);

    // Connect
    printf("Attempting connection...\n");
    int connected = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
    if (connected < 0) 
        error("ERROR connecting");
   
    // Zero the buffer and read the socket 
    bzero(buffer_in, SIZE_BUFF_IN);
    n_in = read(sockfd, buffer_in, SIZE_BUFF_IN-1);
    if (n_in < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer_in);

    // Write a command
    printf("Enter command: ");
    bzero(buffer_out, SIZE_BUFF_OUT);
    fgets(buffer_out, SIZE_BUFF_OUT-1, stdin);
    n_out = write(sockfd, buffer_out, strlen(buffer_out));
    if( n_out < 0 )
        error("ERROR writing to socket");

    
    while( strncmp(lower(buffer_out), "stop", 4) != 0 ) {
        printf("Enter command: ");
        bzero(buffer_out, SIZE_BUFF_OUT);
        fgets(buffer_out, SIZE_BUFF_OUT-1, stdin);
        n_out = write(sockfd, buffer_out, strlen(buffer_out));
        if( n_out < 0 )
            error("ERROR writing to socket");
    }

    // Closing remarks
    bzero(buffer_in, SIZE_BUFF_IN);
    n_in = read(sockfd, buffer_in, SIZE_BUFF_IN-1);
    if( n_in < 0 ) error("ERROR reading from socket");
    printf("%s\n", buffer_in);

    // We're done here
    close(sockfd);
    return 0;
}
