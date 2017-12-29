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

    char client_query_message[] = "Please issue one of the following commands:\nphoto\nvideo\nstop\n";
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
    printf("Waiting for command...\n");
    bzero(buffer_in, SIZE_BUFF_IN);
    n_in = read(newsockfd, buffer_in, SIZE_BUFF_IN-1);
    if(n_in < 0)
        error("ERROR reading from socket");
    printf("Command received: %s", lower(buffer_in));
   
    while( strncmp(lower(buffer_in), "stop", 4) != 0) {
        
        n_out = write(newsockfd, client_query_message, cqm_length);
        if (n_out < 0) error("ERROR writing to socket");


        bzero(buffer_in, SIZE_BUFF_IN);
        n_in = read(newsockfd, buffer_in, SIZE_BUFF_IN-1);
        if(n_in < 0)
            error("ERROR reading from socket");
        printf("Command received: %s", lower(buffer_in));
        
        // Update buffer with lowered command
        strcpy( buffer_in, lower(buffer_in));
        if( strcmp(buffer_in, "photo") == 0 ){
            // Take a photo, ship it to the PC
        }
        else if( strcmp(buffer_in, "video") == 0 ){
            // Query the client for how long of a video, ship it
        }
        else if( strcmp(buffer_in, "stop") == 0){
            n_out = write(newsockfd, "Stopping", 8);
            if( n_out < 0) error("ERROR writing to socket"); 
        }
        else{
            // Ask again
            n_out = write(newsockfd, "Command not understood", 22);
            if( n_out < 0) error("ERROR writing to socket"); 
        }             
            
    }
   

     
    // We're done here
    close(newsockfd);
    close(sockfd);
    return 0; 
}
