// Server side implementation of UDP client-server model
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
#include <net/if.h>
#include <stdbool.h>
   
#define PORT     8080
#define MAXLINE 1024
   
int main(int argc, char *argv[]) {
    int sockfd;
    char buffer[MAXLINE];
    char buffer2[MAXLINE];
    char *message;
    time_t ticks;
    bool ack = false;

    struct sockaddr_in servaddr, cliaddr;
       
    //Criando arquivo que descreve a socket
    //SOCK_DGRAM é um protocolo baseado em datagrama. Você envia um datagrama e recebe uma resposta e a conexão termina.
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    //Seta a memoria com valores válidos
    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
       
    //
    //INADDR_ANY possibilita conectar a qualquer IP que tenta conectar, não necessariamente um local
    //AF_INET serve para determinar o tipo de IP que podemos conectar, no caso IPV4
    //htons - o host faz um conexão curta com a rede
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
       
    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
            sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
       
    int len, n;
   
    len = sizeof(cliaddr);  //len is value/resuslt
   
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                &len);
    buffer[n] = '\0';
    printf("Client : %s\n", buffer);

    message = (char *) malloc(sizeof(char)*50);
    scanf("%s",message);
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

    snprintf(buffer2, sizeof(buffer2), "{ \"Ip_origem\":  %s \"Ip_destino\": %s \"Porta_origem\": %d  \"Porta_destino\": %d \"Timestamp da mensagem original\": %.24s \"Timestamp da mensagem de resposta\": %.24s \"ACK\": %d }", addr, argv[1], PORT, PORT, ctime(&ticks), ctime(&ticks), ack);

    sendto(sockfd, (const char *)buffer2, strlen(buffer2), 
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
            len);
    printf("Hello message sent.\n"); 
       
    return 0;
}