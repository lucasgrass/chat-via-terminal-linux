// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT     8080
#define MAXLINE 1024
   
// Driver code
int main(int argc, char *argv[]) {
    int sockfd;
    char buffer[MAXLINE];
    char *message;
    struct sockaddr_in     servaddr;
   
    // Confere se tem argumento de entreda quando executamos o objeto do arquivo
    // No nosso caso é o IP do servidor
    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
   
    memset(&servaddr, 0, sizeof(servaddr));
       
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Confere se o endereço de IP é valido
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    message = (char*) malloc (sizeof(char)*50);
    scanf ("%s", message);
    getchar();

    int n, len;
       
    sendto(sockfd, (const char *)message, strlen(message),
        MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));
    printf("Hello message sent.\n");
           
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
   
    close(sockfd);
    return 0;
}