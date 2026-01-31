# FTAP (File-based TCP Authentication Protocol)

## O que é?

Este repositório contém dois programas escritos em C: server.c e client.c (servidor e client), eles fazem junto um protocolo de comunicação sob TCP (Transport Control Protocol) que funciona por meio da API de socket.

## Como Funciona?

O protocolo FTAP dita, com códigos próprios, como a comunicação entre os dois programas (cliente e servidor) irá funcionar. Veja os significados dos códigos em "doc/protocol.txt".

O servidor captura diversos logs das interações do cliente com o servidor. Veja os significados dos logs em "doc/logs.txt".

Veja pré requisitos em "docs/requirements.txt."

### Setup

O projeto conta com um arquivo bash que cria a estrutura dos diretórios que são necessários para o funcionamento do servidor:


```text
output/
├── connections_log/
│   ├── client_connections.log
│   └── client_messages.log
├── db/
│   └── db.txt
└── server_log/
    └── server.log
```


### Makefile

Para a compilação o projeto conta com a ferramenta Makefile que compila os programas servidor e cliente em seus respectivos diretórios.

Servidor: server/src/server.c e server/src/server_utils são compilados para o diretório: output/main

### Cliente:

O programa cliente, assim que abre com sucesso (código: 100), o socket (Fluxo de comunicação), pelo comando "open", com o programa servidor é pedido que faça o login com nome de usuário e senha que são cadastradas previamente pelo servidor em um arquivo de texto que simula um banco de dados. Assim que o login for bem sucedido (código: 320) o cliente pode, através do comando "send", enviar uma mensagem de texto ao servidor e o servidor confirma a chegada da mensagem com código: 420. Para finalizar a conexão com o servidor, o cliente usa o comando "close" e para parar a execução do programa do cliente se utiliza o comando "exit". 

Obs: o cliente, do jeito que o projeto está, sempre procurará um servidor com o IP 192.168.100.249 na porta 2250. É necessário modificar manualmente no código o IP e a porta onde o servidor ficará online.

### Servidor:

O programa servidor assim que executado fornece um menu interativo para o admin escolher entre colocar o servidor online ou adicionar clientes ao banco de dados. A primeira opção (inserir clientes ao BD) conta com um algoritmo que pergunta o nome e sobrenome do usuário e a partir do nome fornecido gera um 
nome de usuário que funciona da seguinte maneira: primeira letra do nome + sobrenome. Após o nome de usuário do cliente ser gerado o algoritmo gera a senha do cliente que é um número pseudo aleatório entre 1000 e 9999. A segunda opção (colocar o servidor online) põe o servidor em estado de LISTENING e escuta por conexões de clientes, para colocar o servidor offline é necessário interromper a execução do programa com Ctrl + C.


## Tecnologias utilizadas

<p>
  <img src="assets/c.png" width="100" height="100"/>  <img src="assets/bash.png" width="200" height="200"/>  <img src="assets/makefile.png" width="200" height="200"/> 
</p>