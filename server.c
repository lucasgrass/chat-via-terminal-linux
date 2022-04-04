// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h> 
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdbool.h>
   
#define PORT     8080
#define MAXLINE 1024
   
int main(int argc, char *argv[]) {
    int sockfd, len, n;
    char buffer[MAXLINE], buffer2[MAXLINE],  *message, *addr;
    time_t ticks;
    char ack[10] = "false";
    struct sockaddr_in servaddr, cliaddr, *sa;
    struct ifaddrs *ifap, *ifa;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
        } 
    }

    freeifaddrs(ifap);
   
    len = sizeof(cliaddr);  //len is value/resuslt
       
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
    // Recebe o json com a mensagem
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
            MSG_WAITALL, ( struct sockaddr *) &cliaddr,
            &len);
    buffer[n] = '\0';
    if (strlen(buffer) > 1) strcpy(ack,"true");
    //printf("Client : %s\n", buffer);

    // Desmonta o json 
    char **palavras = (char**) malloc(sizeof(char*)*6);
    for(int i = 0; i<6; i++){
        palavras[i] = (char*) malloc(sizeof(char)*50);
    }

    char *palavra = (char*) malloc(sizeof(char)*50);

    int j = 0, qtdPalavras = 0;
    for(int i = 0; i<strlen(buffer); i++){
        if(buffer[i] == ':'){
            i+=2;
            if(buffer[i] == '"'){
                i++;
                while(buffer[i] != '"'){
                    palavra [j] = buffer[i];
                    j++;
                    i++;
                }
            }else{
                while(!(buffer[i] != ',' ^ buffer[i] != '\n')){
                    palavra[j] = buffer[i];
                    j++;
                    i++;
                }
            }
            palavra[j] = '\0';
            strcpy(palavras[qtdPalavras] ,palavra);
            j = 0;
            qtdPalavras++;
        }
    }

    printf("[%s] Client: %s\n", palavras[4], palavras[5]);

    
    
    

    // Envia o json de recebimento
    snprintf(buffer2, sizeof(buffer2), "\n{\n\t\"Ip_origem\": \"%s\", \n\t\"Ip_destino\": \"%s\", \n\t\"Porta_origem\": %d,  \n\t\"Porta_destino\": %s, \n\t\"Timestamp da mensagem original\": \"%s\", \n\t\"Timestamp da mensagem de resposta\": \"%.24s\", \n\t\"ACK\": %s \n}", addr, palavras[1], PORT, palavras[2], palavras[4], ctime(&ticks), ack);

    sendto(sockfd, (const char *)buffer2, strlen(buffer2), 
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
            len);

    // Envia o json com a resposta
    message = (char *) malloc(sizeof(char)*50);
    scanf("%s",message);
    getchar();

    snprintf(buffer2, sizeof(buffer2), "\n{\n\t\"Ip_origem\":  \"%s\", \n\t\"Ip_destino\": \"%s\", \n\t\"Porta_origem\": %d,  \n\t\"Porta_destino\": %s, \n\t\"Timestamp da mensagem original\": \"%.24s\", \n\t\"Timestamp da mensagem de resposta\": \"%.24s\", \n\t\"Mensagem Original\": \"%s\", \n\t\"Mensagem de resposta\": \"%s\" \n}", addr, palavras[1], PORT, palavras[2], palavras[4], ctime(&ticks), palavras[5], message);

    sendto(sockfd, (const char *)buffer2, strlen(buffer2), 
        MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
            len);
    printf("Mensagem enviada.\n");   
    free(palavra);
    for(int i = 0; i<6; i++){
        free(palavras[i]);
    }
    free(palavras);

    return 0;
}
