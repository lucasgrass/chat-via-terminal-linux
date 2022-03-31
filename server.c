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
   
#define PORT     8080
#define MAXLINE 1024
   
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello;
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

    hello = (char *) malloc(sizeof(char)*50);
    scanf("%s",hello);
    getchar();

    sendto(sockfd, (const char *)hello, strlen(hello), 
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
            len);
    printf("Hello message sent.\n"); 
       
    return 0;
}