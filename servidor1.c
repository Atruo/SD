  #include <stdio.h>
  #include <string.h>
  #include <arpa/inet.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <sys/types.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <time.h> 
  #include <sys/stat.h>

  
  
  //FUNCIONES PREVIAS//

int abrir_socket();

void enlazar_a_puerto(int , int);

int escuchando(int, int);

void leerarchivoconf(FILE*, char*, int, char*, int);

void errorinterno( char*,int,char*,char*, FILE*,int,char*,struct stat);




int main (int argc, char *argv[])
{  
  
  //DECLARACIÓN E INICIALIZACIÓN DE VARIABLES
 
	int puerto=8880;
	char mensaje[1024], date[80], tamanyo[100];
	struct sockaddr_in dir_servidor, dir_cliente;
	unsigned int long_dir_cliente;
	int socket2;
	int n, enviados, recibidos,listener,size;
	int proceso,i;
	time_t tiempo;
	struct tm *tm1; 
 	char respuesta[1024]="\n\r";
	FILE *archivoconf, *asset;
	char *metodo, *version,*document,*document_root, *uri;
        int topeclientes=10;
	document_root=malloc(1024);
	strcpy(document_root,"/home/p1/sd/sd1");
	uri=malloc(1024);
	strcpy(uri,"/index.html");
        
	/* Comprobar los argumentos */
	
	//GESTION DE LECTURA DEL FICHERO DE CONFIGURACION//
	
	if(argc>1){
	  /*printf("holaaaaaaaaaa\n\r");*/
	
	  if(strcmp(argv[1],"-c")==0){ //aqui controlamos esta ./servidor  [argv[0]] -c argv[1] archivoconf argv[2]
	    archivoconf=fopen(argv[2], "r"); // lo abre únicamente para lectura, de ahí viene la r 
	    if(archivoconf==NULL){
	      printf("No está abriendo el archivito\n\r");
	    }
	    if(archivoconf!=NULL){
	      leerarchivoconf(archivoconf,document_root, topeclientes,uri, puerto);
	    }
	    } 
	  else{
	       puerto=atoi(argv[1]);
	       
	       if(argc>2){
		 if(strcmp(argv[2], "-c")==0){
		 
		 archivoconf=fopen(argv[3], "r"); // lo abre únicamente para lectura, de ahí viene la r 
		 if(archivoconf==NULL){
		  printf("No está abriendo el archivito\n\r");
		 }
		  if(archivoconf!=NULL){
		    leerarchivoconf(archivoconf,document_root, topeclientes,uri, puerto);
		
		  }
	      }
	      }
	      }
		 
	    }
	
	    
	
	      
	//APERTURA DEL SOCKET
	
	listener = abrir_socket();

	//ESTABLECIENDO LA DIRECCIÓN DE ESCUCHA 

	enlazar_a_puerto(listener,puerto); 

	//PREPARANDO AL SERVIDOR PARA ESCUCHAR

	escuchando(listener,topeclientes);

	//ESPERA DE CONEXIONES


	while (1)
	{
		fprintf(stderr, "Esperando conexión en el puerto %d...\n\r", puerto);
		long_dir_cliente = sizeof (dir_cliente);
		socket2 = accept (listener, (struct sockaddr *)&dir_cliente, &long_dir_cliente);
		/* s2 es el socket para comunicarse con el cliente */
		
		if (socket2 == -1)
		{
			break; /* salir del bucle */
		}
		/* Creamos un nuevo proceso para que se pueda atender varias peticiones en paralelo */
		proceso = fork();
		if (proceso == -1) exit(1);
		if (proceso == 0) //SOY EL HIJO
		{
			close(listener); //EL HIJO YA NO NECESITA DEL SOCKET GENERAL
			

			//LECTURA Y EXTRACCIÓN DE LOS PARÁMETROS DEL MENSAJE
			
			
			n = sizeof(mensaje);
			recibidos = read(socket2, mensaje, n);
			          //POSIBLE ERROR DE LECTURA 
			if (recibidos == -1)
			{
				            fprintf(stderr, "Error de lectura del mensaje\n\r"); //strcat lo que hace es concatenar y va guardando el resultado en respuesta
				            strcat(respuesta,"HTTP/1.1 500 Internal Server Error\n");
					    strcat(respuesta, "Connection: close\n\r");
					    strcat(respuesta, "Content-Length: 96");
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "Content-Type: txt/html\n\r");
					    strcat(respuesta, "Server: Servidor SD\n\r");
					    strcat(respuesta, "Date: ");
					    strcat(respuesta, date);
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
					    strcat(respuesta, "\n\r");
					    strcat(respuesta, "<html> <title>Error 500</title>\n<h1> Error 500: Error Interno. </h1> </html>");
					    n = strlen(respuesta); //calcula el numero de caracteres 
					    enviados = write(socket2, respuesta, n); //escribimos la respuesta
				    
			
			    if (enviados == -1 || enviados < n){
					    fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
					    close(listener);
					   
				    }
			  close(socket2);
			}
			
			
			mensaje[recibidos] = '\0'; /* pongo el final de cadena */
			
			printf("%s", mensaje);
		
			
			
			//CONSEGUIR AQUI QUE HAYA PRIORIDAD ANTES DE ANALIZAR LOS METODOS
			  if(mensaje!=NULL){
			    metodo=strtok(mensaje, " ");
			      if(metodo!=NULL){
				uri=strtok(NULL, " ");
				   if(uri!=NULL){
				     version=strtok(NULL, "\n\r");
				      if(version!=NULL){
				      }
				      else{
					printf("Version NULA");
				   }
			      }
			      else{
				printf("Versión HTTP NULA");
			      }
			      }
			      else {
				printf("Método NULO");
			  }
			  }
			  else{
			    printf("La petición recibida es NULA");
			  }
		
			
	
			tiempo = time(NULL);
			tm1 = localtime(&tiempo);
			strftime(date, 80, "%H:%M:%S, %A de %B de %Y", tm1);


			//ANALISIS DE CADA METODO
			//IGUAL PONER AQUI, IF METODO ES != NULL
			
			
			//validandoversion(version); ahorrariamos codigo, mas eficiencia ostiaaaaaaaaaaaas 
	
			if(strcmp(metodo,"GET")==0){
				struct stat file_stats;
				strcat(document_root, uri); 
				//anyade un bloque de memoria a otro, aqui anyado la ruta interna del servidor con la externa del cliente
				
				
		
				if(strcmp(version,"HTTP/1.1")==0){
				  
					asset=fopen(document_root, "r"); //buscamos en la ruta 	
					/*printf("Document_root: %s\n", document_root);*/
					if(asset==NULL){ //no lo encontramos
						
						errorinterno(respuesta,404,date,document,asset,size,tamanyo,file_stats);
					
					    
						
					}
					else{	//lo encontramos			 
						
						errorinterno(respuesta,200,date,document,asset,size,tamanyo,file_stats);
						
					}
				}
				else{
				  
				  errorinterno(respuesta,505,date,document,asset,size,tamanyo,file_stats);
				  
				}
				
		/*		
			n=strlen(respuesta);
		enviados = write(socket2, respuesta, n);
		mensaje[recibidos] = '\0';
			if (enviados == -1 || enviados < n)
			{
				fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
				close(listener);
				
			}
			close(socket2);*/
			}else if(strcmp(metodo,"HEAD")==0){
			  struct stat file_stats;
			  
			  strcat(document_root, uri);
				asset=fopen(document_root, "r");
				if(strcmp(version,"HTTP/1.1")==0){
					if(asset==NULL){
						errorinterno(respuesta,404,date,document,asset,size,tamanyo,file_stats);
					}
					else{
					      
						errorinterno(respuesta,200,date,document,asset,size,tamanyo,file_stats);
					}

					
				}else{
					errorinterno(respuesta,505,date,document,asset,size,tamanyo,file_stats);			}
				if (asset!=NULL){ 
					fclose(asset);
}
			  
			  
			}else if(strcmp(metodo,"DELETE")==0){
			  
			  struct stat file_stats;
			  int aux;
			  aux=-1;
			  char name[strlen(uri)];
			  strcat(document_root, uri);
				/*Operamos para el metodo DELETE*/
			    if(strcmp(version,"HTTP/1.1")==0){
					aux=remove(document_root);
					if(aux!=0){
						errorinterno(respuesta,404,date,document,asset,size,tamanyo,file_stats);
					}else{
						errorinterno(respuesta,200,date,document,asset,size,tamanyo,file_stats);
					}
				}else{
					errorinterno(respuesta,505,date,document,asset,size,tamanyo,file_stats);
} 
			  
			  
			  
			}else if(strcmp(metodo,"PUT")==0){
			  
			  struct stat file_stats;
			  strcat(document_root, uri);
				
				/*Operamos para el metodo PUT*/
				if(strcmp(version,"HTTP/1.1")==0){
					asset=fopen(document_root, "w"); //lo abrimos con el permiso de escritura
					
					if(asset==NULL){
						strcat(respuesta,"HTTP/1.1 403 Forbidden\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 77");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "<html> <title>Error 403</title>\n<h1> Error 403: Acceso Denegado. </h1> </html>");
						
					}else{
						strcat(respuesta, "HTTP/1.1 201 CREATED\n");
						strcat(respuesta, "Connection: close\n\r");
						strcat(respuesta, "Content-Length: 0");
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Content-Type: txt/html\n\r");
						strcat(respuesta, "Server: Servidor SD\n\r");
						strcat(respuesta, "Date: ");
						strcat(respuesta, date);
						strcat(respuesta, "\n\r");
						strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
						strcat(respuesta, "\n");

					}
					

				}else{
					errorinterno(respuesta,505,date,document,asset,size,tamanyo,file_stats);				
}
				if (asset!=NULL){
					fclose(asset);
}
			  
			  
			  
			 
			}else{ //METODO NO SOPORTADO!!!!!!!!
			  strcat(respuesta, "HTTP/1.1 405 method not allowed\n");
				strcat(respuesta, "Connection: close\n\r");
				strcat(respuesta, "Content-Length: 82");
				strcat(respuesta, "\n\r");
				strcat(respuesta, "Content-Type: txt/html\n\r");
				strcat(respuesta, "Server: Servidor SD\n\r");
				strcat(respuesta, "Date: ");
				strcat(respuesta, date);
				strcat(respuesta, "\n\r");
			
				strcat(respuesta, "Cache-control: max-age=0, no-cache\n\r");
				strcat(respuesta, "\n\r");
				strcat(respuesta,"<html> <title>  Error 405 </title>\n<h1> Error 405: Método no permitido. </h1> </html>");
			}
			
			
			//ESCRIBIENDO LA RESPUESTA EN EL SOCKET
			
			
			n = strlen(respuesta);
			enviados = write(socket2, respuesta, n);
			if (enviados == -1 || enviados < n)
			{
				fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
				close(listener);
				return 1;
			}

						

			close(socket2);
			exit(0); /* el hijo ya no tiene que hacer nada */
		}
		else /* soy el padre */
		{
			close(socket2); /* el padre no usa esta conexión */
		}

	}

	/**** Paso 7: Cerrar el socket ****/
	close(listener);
	printf("Socket cerrado\n\r");
	return 0;
}





  











void leerarchivoconf(FILE* archivoconf, char* document_root, int topeclientes, char* uri, int puerto){
	      char* linea;
	      size_t length=0;
	      ssize_t read;
	      while((read=getline(&linea, &length, archivoconf))!=-1){
		char *almacen;
		almacen=strtok(linea," ");
		if(strcmp(almacen, "DocumentRoot")==0){
		  almacen=strtok(NULL,"\n");
		  printf("Document_root:%s\n\r",almacen);
		  strcpy(document_root,almacen);
		}else if(strcmp(almacen, "Listen")==0){
		  almacen=strtok(NULL,"\n");
		  puerto=atoi(almacen);
		}
		else if(strcmp(almacen, "MaxClients")==0){
		  almacen=strtok(NULL,"\n");
		  topeclientes=atoi(almacen);
		}
		else if(strcmp(almacen, "DirectoryIndex")==0){
		  almacen=strtok(NULL,"\n");
		  printf("Uri:%s\n\r",almacen);
		  strcpy(uri,almacen);
		}
	      }
		
	      fclose(archivoconf);
	      if(linea){
		free(linea);
	      }
}
  




int abrir_socket(){

	int sock=socket(PF_INET,SOCK_STREAM,0); //creo el socket 
      
	if(sock==-1){
		fprintf(stderr,"Error de apertura de socket\n\r");
	 }              
    	return sock;
  }


void enlazar_a_puerto(int listener, int puerto){

	  
	  struct sockaddr_in server_addr;
	  server_addr.sin_family=AF_INET;
	  server_addr.sin_port=htons(puerto);
	  server_addr.sin_addr.s_addr=htonl(INADDR_ANY); /*significa cualquier ip del servidor*/

  

	  int c= bind(listener, (struct sockaddr *) &server_addr, sizeof(server_addr)); //Le pasamos el socket, direccion de memoria de la 	estructura, tamanyo
  
	    if(c==-1){
	      fprintf(stderr,"No se puede enlazar al puerto: dirección ya está en uso\n\r");
	      close(listener);
	    }

	  printf("Establecido el puerto de escucha\n\r");
  }


//FUNCIÓN LISTEN

int escuchando( int listener, int numero){
  
	  int c= listen(listener,numero);
    
	  if(c==-1){
	      fprintf(stderr,"No es posible escuchar en ese puerto\n\r"); //cliente 11
	      close(listener);
	      return 1;
	  }
	    
	  printf("Enlazado a puerto\n");
}
//RESPUESTA A ERROR
void errorinterno( char* respuesta,int numero,char* date,char* document,FILE* asset,int size,char* tamanyo,struct stat file_stats){

	if(numero==200){
			strcat(respuesta, "HTTP/1.1 200 OK\r\n");
			//strcat(respuesta, "\n\r");
			fseek(asset,0L,SEEK_END);
			size=ftell(asset);
			sprintf(tamanyo,"%d",size);
			fseek(asset,0L,SEEK_SET);
			document=malloc(size);
			if(document){
			  fread(document,1,size,asset);
			}
			fclose(asset);
			strcat(respuesta, "Connection: close\r\n");
			strcat(respuesta, "Content-Length: ");
			strcat(respuesta,tamanyo);
			strcat(respuesta, "\r\n");
			strcat(respuesta, "Content-Type: txt/html\r\n");
			strcat(respuesta, "Server: Servidor SD\r\n");
			strcat(respuesta, "Date: ");
			strcat(respuesta, date);
			strcat(respuesta, "\r\n");
			strcat(respuesta, "Last-Modified: ");
			strcat(respuesta, ctime(&file_stats.st_mtime));
			strcat(respuesta, "Cache-control: max-age=0, no-cache");
			strcat(respuesta,"\r\n");
			strcat(respuesta,"\r\n");

						
			if(document){
			  strcat(respuesta,document);
			}
			strcat(respuesta, "\n\r");
	}
	else if(numero==404){
		strcat(respuesta, "HTTP/1.1 404 not found\r\n");
		   strcat(respuesta, "Connection: close\r\n");
		    strcat(respuesta, "Content-Length: 96");
		    strcat(respuesta, "\r\n");
		    strcat(respuesta, "Content-Type: txt/html\r\n");
		    strcat(respuesta, "Server: Servidor SD\r\n");
		    strcat(respuesta, "Date: ");
		    strcat(respuesta, date);
		    strcat(respuesta, "\r\n");
		    strcat(respuesta, "Cache-control: max-age=0, no-cache");
		    strcat(respuesta, "\r\n");
		    strcat(respuesta, "\r\n");
		    strcat(respuesta, "<html> <title>Error 404</title>\n<h1> 404 ERROR: File not found </h1> </html>");
		    strcat(respuesta, "\n\r");
	}
	else{
		strcat(respuesta, "HTTP/1.1 505, HTTP version not supported\r\n");
				  	  
		   strcat(respuesta, "Connection: close\r\n");
		    strcat(respuesta, "Content-Length: 90");
		    strcat(respuesta, "\r\n");
		    strcat(respuesta, "Content-Type: txt/html\r\n");
		    strcat(respuesta, "Server: Servidor SD\r\n");
		    strcat(respuesta, "Date: ");
		    strcat(respuesta, date);
		    strcat(respuesta, "\r\n");
		    strcat(respuesta, "Cache-control: max-age=0, no-cache");
		    strcat(respuesta, "\r\n");
		    strcat(respuesta, "\r\n");
					    
		    strcat(respuesta, "<html> <title>Error 500</title>\n<h1> Error 505: Versión de HTTP no soportada </h1> </html>");
	}


}
