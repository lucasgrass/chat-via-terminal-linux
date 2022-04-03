# chat-via-terminal-linux

# Instalar o gcc nas máquinas para executar o código de servidor e cliente

# Passos para rodar o chat 

## Máquina Servidor
-> gcc server.c -o server 
-> ./server

## Máquina Cliente
-> gcc client.c -o client -lpthread
-> ./client 'IP do servidor'

## Troca de mensagens
-> Enviar uma mensagem pela máquina do cliente;

  -> Após o servidor receber a mensagem ele mandará para o cliente uma confirmação de que recebeu a mensagem;
  
-> Enviar uma resposta do servidor ao cliente.

## Linhas com o print do json
-> Linha 71: json com as informações da mensagem que o cliente enviou para o servidor;

-> Linha 70: json com as informações do recebimento da mensagem no servidor;

-> Linha 193: json com as informações da mensagem que o servidor mandou como resposta para o cliente.
