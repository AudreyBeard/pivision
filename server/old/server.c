/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    // File descriptors and port number 
    int sockfd, newsockfd, portno;

    // Size of address of client. Different from int for some reason
    socklen_t clilen;
    
    // Message buffer
    char buffer[256];

    // Structs for internet addresses
    struct sockaddr_in serv_addr, cli_addr;

    // Number of characters written
    int n;

    // Error handling on first call
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    // Define a socket 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");

    // Set server address to zeros
    bzero((char *) &serv_addr, sizeof(serv_addr));

    // Get port number from call
    portno = atoi(argv[1]);

    // Set parameters for server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind socket to adress
    int bound = bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) )
    if( bound < 0 ) 
        error("ERROR on binding");

    // Listen for connections on socket
    listen(sockfd,5);

    // Get new connection
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, 
                (struct sockaddr *) &cli_addr, 
                &clilen);
    if (newsockfd < 0) 
         error("ERROR on accept");

    // Zero the buffer
    bzero(buffer,256);

    // Read from the socket
    n = read(newsockfd,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    
    printf("Here is the message: %s\n",buffer);
    
    n = write(newsockfd,"I got your message",18);
    if (n < 0) error("ERROR writing to socket");
    
    // We're done here
    close(newsockfd);
    close(sockfd);
    return 0; 
}
