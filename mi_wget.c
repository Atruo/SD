// main.c
//  
// Descripción: crear un cliente web (HTTP) llamado mi_wget...
//
// Fecha:
// Autor: Pablo Amoros Becerra
// Copyright (c)
//
// Compilación:
//	gcc main.c -0 mi_wget -D DEBUG
//
// Ejemplo de uso:
//	./mi_wget 193.145.322.5 /index.html
//
//FORMATO URL protocolo://host:puerto/recurso?argumentos

//Sección de cabeceras
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>


// Herramientas de depuración
#ifdef DEBUG 
#define log(fmt, args...)	fprintf(stderr, fmt "\n", ## args)
#else
#define log(fmt, args...)
#endif

void pexit(char *msg)
{

	perror(msg);
	exit(EXIT_FAILURE);

} // de pexit

// Sección de definiciones

#define HTTP_REQUEST_PATTERN "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: %s\r\nConnection: keep-alive\r\n\r\n"

// El verdadero cliente web (HTTP)
void clienteweb(char *dirIP, int puerto, char*recurso)
{
	int sockfd;
	static struct sockaddr_in serv_addr;
	char HTTP_request[200];
	char HTTP_response[500];
	int bleidos;

	// SOCKET: obtener un canal por internet
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
		pexit("socket() fallo");

	// CONNECT: conectar con un host (dirIP) y una aplicación (port)
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(puerto);
	serv_addr.sin_addr.s_addr = inet_addr(dirIP);

	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	pexit("connect() fallo");
	
	// WRITE: nos comunicamos con el servidor web (HTTP)
	sprintf(HTTP_request, HTTP_REQUEST_PATTERN, recurso, dirIP, "mi_wget");
	log(" * Enviando %d bytes:\n\n%s\n", (int)strlen(HTTP_request), HTTP_request);
	write(sockfd, HTTP_request, strlen(HTTP_request));

	// READ: recogemos lo que nos envíe el servidor (HTTP)
	while((bleidos=read(sockfd, HTTP_response, sizeof(HTTP_response))) > 0)
		write(STDOUT_FILENO, HTTP_response, bleidos);	

	// CLOSE: cierro el canal
	close(sockfd);
} // de clienteweb
int mostrarUso( char *aplicacion){
	fprintf(stderr, "Debe proporcionar una URL válida \n\n");
	fprintf(stderr, "Uso: %s  protocol host port resource\n\n", aplicacion);
}

// Punto de entrada a mi aplicación
int main(int argc, char *argv[])
{
	char *protocol;
	char *host;
	int port;
	char *resource;
	char * host_ip; 

	struct servent *sp;
	struct hostent *host_struct;
	//verificamos los argumentos de entrada
	if(argc != 5){
		mostrarUso(argv[0]);
	}
	//Parametros de entrada
	
	protocol = argv[1];
	host = argv[2];
	port = atoi( argv[2]);
	resource = argv[4];
	//Si el puerto es 0. Obtenemos el puerto a partir del protocol

	if((protocol[0]!='\0') && (port==0)){
		sp = getservbyname(protocol, "tcp");
		if(!sp){
			fprintf(stderr, "No se ha podido encontrar un puerto válido para el protocolo %s\n\n", protocol);
			return EXIT_FAILURE;
		}
		port = ntohs(sp->s_port);
	}
	//Convertimos el DNS en su equivalente IP
	
	host_struct = gethostbyname(host);
	if(host_struct != NULL)
		host_ip = inet_ntoa(* (struct in_addr *)host_struct->h_addr);

	// Mostrar todas las variables relevantes
	log(" * protocolo = <%s>", protocol);
	log(" * host = <%s>", host);
	log(" * port = <%d>", port);
	log(" * ruta = <%s>", resource);
	log(" * IP = <%s>", host_ip);

	if(!port || !host_ip || resource[0]=='\0'){
		mostrarUso( argv[0]);
	}

	// Invocar al verdadero cliente web (HTTP)
	clienteweb(host_ip, port, resource);

	return EXIT_SUCCESS;

}  // de main
