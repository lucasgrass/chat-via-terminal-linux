// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <json-c/json.h>
#include <time.h> 
#include <ifaddrs.h>
#include <sys/ioctl.h>


   
#define PORT     8080
#define MAXLINE 1024
   
// Driver code
int main(int argc, char *argv[]) {
    int sockfd;
    char buffer[MAXLINE];
    char buffer2[MAXLINE];
    char *message;
    time_t ticks; 
    
    struct sockaddr_in     servaddr;

    // Ip_origem, Ip_destino, Porta_origem, Porta_destino, Timestamp da mensagem, Mensagem 

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

    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
        }
    }

    freeifaddrs(ifap);

    snprintf(buffer2, sizeof(buffer2), "{ \"Ip_origem\":  %s \"Ip_destino\": %s \"Porta_origem\": %d  \"Porta_destino\": %d \"Timestamp da mensagem\": %.24s \"Mensagem\": %s }", addr, argv[1], PORT, PORT, ctime(&ticks), message);
    
    int n, len;
    printf("Message sent.\n");

    sendto(sockfd, (const char *)buffer2, strlen(buffer2),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);

    buffer[n] = '\0';
    
    printf("Server : %s\n", buffer);
   
    close(sockfd);
    return 0;
}