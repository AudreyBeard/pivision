/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define SIZE_BUFF_IN 256
#define SIZE_BUFF_OUT 256


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

char* lower(char* word){
    unsigned int i;
    for(i = 0; i < strlen(word); i++ )
        word[i] = tolower(word[i]);
    return word;
}


int receive_command(int socket_fd, char* buffer, int buffer_size)
{
    bzero( buffer, buffer_size );
    int n = read( socket_fd, buffer, buffer_size-1 );
    if( n < 0 )
        error("ERROR reading from socket");
    return n;
}


int send(int socket_fd, char* buffer, int buffer_size)
{
    int n = write( socket_fd, buffer, buffer_size );
    if( n < 0 )
        error("ERROR writing to socket");
    //bzero( buffer, buffer_size );
    return n;
}


int main(int argc, char *argv[])
{
    // File descriptors and port number 
    int sockfd, newsockfd, portno;

    // Size of address of client. Different from int for some reason
    socklen_t clilen;
    
    // Message buffer
    char buffer_in[SIZE_BUFF_IN], buffer_out[SIZE_BUFF_OUT];

    // Structs for internet addresses
    struct sockaddr_in serv_addr, cli_addr;

    // Number of characters written
    int n_in, n_out;

    char client_query_message[] = "\n---------------\nLegal commands:\nphoto\nvideo\nstop\n>>> ";
    int cqm_length = strlen(client_query_message);
    char connection_message[] = "Connection established";
    int conmsg_length = sizeof(connection_message) / sizeof(char);

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
    int bound = bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr) );
    if( bound < 0 ) 
        error("ERROR on binding");

    // Listen for connections on socket
    printf("Waiting for connection...\n");
    listen(sockfd,5);

    // Get new connection
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, 
                (struct sockaddr *) &cli_addr, 
                &clilen);
    if (newsockfd < 0) 
         error("ERROR on accept");

    // Connection message
    printf("%s\n", connection_message);

    n_out = write(newsockfd, connection_message, conmsg_length);
    if (n_out < 0) error("ERROR writing to socket");

    // Receive input command
    /*
    printf("Waiting for command...\n");
    bzero(buffer_in, SIZE_BUFF_IN);
    n_in = read(newsockfd, buffer_in, SIZE_BUFF_IN-1);
    if(n_in < 0)
        error("ERROR reading from socket");
    printf("Command received: %s", lower(buffer_in));
    */
    while( strncmp(lower(buffer_in), "stop", 4) != 0) {
        // Client query message  
        send(newsockfd, client_query_message, cqm_length);
        /*
        n_out = write(newsockfd, client_query_message, cqm_length);
        if (n_out < 0) error("ERROR writing to socket");
        */
        // Zero buffer and get command
        receive_command(newsockfd, buffer_in, SIZE_BUFF_IN);
        /*
        bzero(buffer_in, SIZE_BUFF_IN);
        n_in = read(newsockfd, buffer_in, SIZE_BUFF_IN-1);
        if(n_in < 0)
            error("ERROR reading from socket");
        */
        
        strcpy( buffer_in, lower(buffer_in));
        printf("Command received: %s", buffer_in);
        
        // Update buffer with lowered command
        // TODO does not enter if statements for some reason
        //printf(buffer_in);
        if( strncmp(buffer_in, "photo", 5) == 0 ) {
            // Take a photo, ship it to the PC
            send(newsockfd, "Command: photo\n", 15);
            printf("Command accepted\n");
        } else if( strncmp(buffer_in, "video", 5) == 0 ) {
            // Query the client for how long of a video, ship it
            send(newsockfd, "Command: video\n", 15); 
            printf("Command accepted\n");
        } else if( strncmp(buffer_in, "stop", 4) == 0) {
            send(newsockfd, "\nStopping\n", 9);
            printf("Stopping\n");
        } else {
            // Ask again
            send(newsockfd, "Command not understood\n", 23);
            printf("Command not accepted: %s", buffer_in);
            /*
            n_out = write(newsockfd, "Command not understood\n", 23);
            if( n_out < 0) error("ERROR writing to socket"); 
            */
        }             
            
    }
   

     
    // We're done here
    close(newsockfd);
    close(sockfd);
    return 0; 
}
