// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h> 
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <pthread.h>
   
#define PORT     8080
#define MAXLINE 1024
   
bool msgRecieved = false;

struct thread_args {
    int sockfd; 
    int *len; 
    struct sockaddr_in *servaddr;
};

void waitAck(void *arguments){
    int n;
    struct thread_args *args = arguments;
    char buffer[MAXLINE];
    n = recvfrom( args->sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) args->servaddr,
                args->len);
    buffer[n] = '\0';
    
    msgRecieved = true;
    printf("Server : %s\n", buffer);
}

// Driver code
int main(int argc, char *argv[]) {
    int sockfd, n, len;
    char buffer[MAXLINE], buffer2[MAXLINE], *message, *addr;
    struct sockaddr_in servaddr, *sa;
    struct ifaddrs *ifap, *ifa;
    time_t ticks; 
    pthread_t acknowledgement;

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

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
        }
    }

    freeifaddrs(ifap);

    snprintf(buffer2, sizeof(buffer2), "\n{\n\t\"Ip_origem\": \"%s\", \n\t\"Ip_destino\": \"%s\", \n\t\"Porta_origem\": %d,  \n\t\"Porta_destino\": %d, \n\t\"Timestamp da mensagem\": \"%.24s\", \t\"Mensagem\": \"%s\" \n}", addr, argv[1], PORT, PORT, ctime(&ticks), message);

    char **palavras = (char**) malloc(sizeof(char*)*6);
    for(int i = 0; i<6; i++){
        palavras[i] = (char*) malloc(sizeof(char)*50);
    }

    char *palavra = (char*) malloc(sizeof(char)*50);

    int j = 0, qtdPalavras = 0;
    for(int i = 0; i<strlen(buffer2); i++){
        if(buffer2[i] == ':'){
            i+=2;
            if(buffer2[i] == '"'){
                i++;
                while(buffer2[i] != '"'){
                    palavra [j] = buffer2[i];
                    j++;
                    i++;
                }
            }else{
                while(!(buffer2[i] != ',' ^ buffer2[i] != '\n')){
                    palavra[j] = buffer2[i];
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

    for(int i = 0; i<6; i++){
        puts(palavras[i]);
    }

    printf("Message sent.\n");

    struct thread_args args;
    args.sockfd = sockfd;
    args.len = &len;
    args.servaddr = &servaddr;

    pthread_create(&acknowledgement, NULL, (void*) waitAck, &args);

    while (!msgRecieved)
    {
        sendto(sockfd, (const char *)buffer2, strlen(buffer2),
            MSG_CONFIRM, (const struct sockaddr *) &servaddr, 
            sizeof(servaddr));
    }

    pthread_join(acknowledgement, NULL);
    //waitAck(sockfd, &len, &servaddr)

    n = recvfrom(sockfd, (char *)buffer, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) &servaddr,
                &len);
                
    buffer[n] = '\0';
    
    msgRecieved = true;
    printf("Server : %s\n", buffer);

    close(sockfd);
    return 0;
}

