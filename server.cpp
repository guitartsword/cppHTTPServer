#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
//Libreria de hilos
#include <pthread.h>

#include <vector>
#include <sstream>

using namespace std;



void error(const char *msg)
{
    perror(msg);
    exit(1);
}

//Variable globales
int sockfd, newsockfd, portno;
socklen_t clilen;
char buffer[1024];
int n;
const char HEADERS[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: ";
const char OUTPUT[] = "<html><head><title>Example</title></head><body<h1>SOY CRACK</h1><p>Funciona!!!</p></body></html>";
int OUTPUT_LENGTH = strlen(OUTPUT);
const char HEADERS_END[]= "\r\n\r\n";

//Funciones hilo
void* entradaMensaje(void*);
void* salidaMensaje(void*);

//Tokenize vector
vector<string> getAllTokens(string);

int main(int argc, char *argv[])
{ 
    //Creacion de la estructura de hilo   
    pthread_t entrada; 
    pthread_t salida; 

    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
     
    if (sockfd < 0) 
	   error("ERROR NO se puede abrir el socket");
     
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 8080;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    bool err = false;
    int maxRetry = 100;
    while (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        printf("retrying(%d) on port %d\n",(101-(maxRetry--)),portno);
        if(!maxRetry)
            break;
        sleep(1);
    }
    if(!maxRetry){
        error("ERROR en asociar datos a conexion");
    }
    cout << "Servidor iniciado. Esperando conexiones..." << endl;
    listen(sockfd,5);	

    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR en aceptar");
    cout << "Cliente conectado" << endl;
    
    //Creación del hilo
    pthread_create( &entrada, NULL, entradaMensaje, (void*) NULL );
    pthread_create( &salida, NULL, salidaMensaje, (void*) NULL );	
    while (1);/*{
         char x;
         x = cin.get();
         if(x == 's' || x == 'S')
            break;
    }	
    close(newsockfd);
    close(sockfd);*/
    return 0; 
}

void* entradaMensaje(void*) {
	while (1) {
	    bzero(buffer,1024);
	    n = read(newsockfd,buffer,1023);
	    if (n < 0) error("ERROR reading from socket");
        //Parse request

        //vector<string> tokens = getTokens();
        cout << "Mensaje entrada: " << buffer << endl;
        stringstream response;
        response << HEADERS << OUTPUT_LENGTH << HEADERS_END << OUTPUT;
        cout << "MENSAJE A RESPONDER: " << endl;
        cout << response.str().c_str() << endl;
        cout << response.str().length() << endl;
        n= write(newsockfd, response.str().c_str(),response.str().length());
        if (n < 0)
            error("ERROR writing to socket");
	}
}

void* salidaMensaje(void*) {
	while (1) {
        while(1){
            sleep(30);
        }
		cout << "Mensaje salida: ";
    		bzero(buffer,1024);
	        fgets(buffer,1023,stdin);
		n = write(newsockfd,buffer,strlen(buffer));
		if (n < 0) 
		         error("ERROR writing to socket");
	}
}

vector<string> getAllTokens(string toTokenize){
    vector<string> v;
    string::iterator stringIT = toTokenize.begin();
    for(string::iterator i = toTokenize.begin(); i != toTokenize.end(); i++){
        if(*i == ' '){
            v.push_back(string(stringIT,i));
            stringIT=i+1;
        }
    }
    return v;
}
