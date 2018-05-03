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

  
int abrir_socket();

void enlazar_a_puerto(int , int);

int escuchando(int, int);

void leerarchivoconf(FILE*, char*, int, char*, int);

void escribir( char*,int,char*,char*, FILE*,int,char*,struct stat);




int main (int argc, char *argv[])
{  
  
  //VARIABLES
 
	int puerto=8880;
	char mensaje[1024], date[80], tama[100];
	struct sockaddr_in dir_servidor, dir_cliente;
	unsigned int long_dir_cliente;
	int socket2;
	int n, enviados, recibidos,listener,size;
	int proceso,i;
	time_t tiempo;
	struct tm *tm1; 
 	char respuesta[1024]="\n\r";
	FILE *archivoconf, *asset;
	char *metodo, *version,*document,*document_root, *extrn;
    int topeclientes=10;
	document_root=malloc(1024);
	strcpy(document_root,"/home/sergio/Escritorio/GIT/SD");
	extrn=malloc(1024);
	strcpy(extrn,"./index.html");
        
	
	
	//LECTURA
	
	if(argc>1){
	  	
	  if(strcmp(argv[1],"-c")==0){ 
	   		archivoconf=fopen(argv[2], "r"); 
	    	if(archivoconf==NULL){
	      		printf("No está abriendo el archivo\n\r");
	    	}
	    	if(archivoconf!=NULL){
	      		leerarchivoconf(archivoconf,document_root, topeclientes,extrn, puerto);
	   		}
	  	} 
	  else{
	       puerto=atoi(argv[1]);
	       
	     if(argc>2){
		 	if(strcmp(argv[2], "-c")==0){
		 
				archivoconf=fopen(argv[3], "r"); 
		 		if(archivoconf==NULL){
		  			printf("No está abriendo el archivo\n\r");
		 		}
		  	if(archivoconf!=NULL){
		    	leerarchivoconf(archivoconf,document_root, topeclientes,extrn, puerto);
				}
	      	}
	      }
	    }
		 
	}
	
	    
	
	      
	
	
	listener = abrir_socket();
	enlazar_a_puerto(listener,puerto);
	escuchando(listener,topeclientes);




	while (1)
	{
		fprintf(stderr, "Esperando conexión en el puerto %d...\n\r", puerto);
		long_dir_cliente = sizeof (dir_cliente);
		socket2 = accept (listener, (struct sockaddr *)&dir_cliente, &long_dir_cliente);
		// s2 es el socket para comunicarse con el cliente 
		
		if (socket2 == -1)
		{
			break; 
		}
		//Peticiones en paralelo
		proceso = fork();
		if (proceso == -1){
			exit(1);	
		} 
		if (proceso == 0) 
		{
			close(listener); 		

			n = sizeof(mensaje);
			recibidos = read(socket2, mensaje, n);
			          
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
					    enviados = write(socket2, respuesta, n); //respuesta
				    
			
			    if (enviados == -1 || enviados < n){
					    fprintf(stderr, "Error enviando la respuesta (%d)\n\r",enviados);
					    close(listener);
					   
				    }
			  close(socket2);
			}
			
			
			mensaje[recibidos] = '\0'; 
			
			printf("%s", mensaje);
		
			
			
			 if(mensaje!=NULL){
			    metodo=strtok(mensaje, " ");
			      if(metodo!=NULL){
				extrn=strtok(NULL, " ");
				   if(extrn!=NULL){
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


//#################################################################################################
//---------------------------------------------GET-------------------------------------------------
//#################################################################################################
			
			if(strcmp(metodo,"GET")==0){
				struct stat file_stats;

				strcat(document_root, extrn); 
						
				if(strcmp(version,"HTTP/1.1")==0){
				  
					asset=fopen(document_root, "r");
					
					if(asset==NULL){ //no lo encontramos
						
						escribir(respuesta,404,date,document,asset,size,tama,file_stats);
					
					    
						
					}
					else{	//lo encontramos			 
						
						escribir(respuesta,200,date,document,asset,size,tama,file_stats);
						
					}
				}
				else{
				  
				  escribir(respuesta,505,date,document,asset,size,tama,file_stats);
				  
				}
				
//#################################################################################################
//---------------------------------------------HEAD------------------------------------------------
//#################################################################################################		
			}else if(strcmp(metodo,"HEAD")==0){
			  struct stat file_stats;
			  
			  strcat(document_root, extrn);
				asset=fopen(document_root, "r");
				if(strcmp(version,"HTTP/1.1")==0){
					if(asset==NULL){
						escribir(respuesta,404,date,document,asset,size,tama,file_stats);
					}
					else{
					      
						escribir(respuesta,200,date,document,asset,size,tama,file_stats);
					}

					
				}else{
					escribir(respuesta,505,date,document,asset,size,tama,file_stats);			}
				if (asset!=NULL){ 
					fclose(asset);
}
			  
//#################################################################################################
//---------------------------------------------DELETE----------------------------------------------
//#################################################################################################			  
			}else if(strcmp(metodo,"DELETE")==0){
			  
			  struct stat file_stats;
			  int extra = -1;			 
			  char name[strlen(extrn)];
			  strcat(document_root, extrn);
				
			    if(strcmp(version,"HTTP/1.1")==0){
					extra=remove(document_root);
					if(extra!=0){
						escribir(respuesta,404,date,document,asset,size,tama,file_stats);
					}else{
						escribir(respuesta,200,date,document,asset,size,tama,file_stats);
					}
				}else{
					escribir(respuesta,505,date,document,asset,size,tama,file_stats);
} 
			  
//#################################################################################################
//---------------------------------------------PUT-------------------------------------------------
//#################################################################################################			  
			  
			}else if(strcmp(metodo,"PUT")==0){
			  
			 
			  strcat(document_root, extrn);
			  /*
			  	int i;
			  	long tamBody;
			  	char body[MAX_REQUEST];
				for(i=0;i<bleidos-3;i++){
					if(buffer[i]=='\r' && buffer[i+1]=='\n'&& buffer[i+2]=='\r' && buffer[i+3]=='\n'){
						tamBody = bleidos-(i+4);
						memcpy(body,&buffer[i+4],tamBody);
						break;
					}
					
				}


				int tratarPUT(int sd, char recurso, char body, long tamBody){
					int fd;
					char buffer[MAXBUFF];
					fd=open(recurso, O_CREAT(O_WONLY));
					write(fd,body,tamBody);
					strcpy(buffer, HTTP_PUT_OK);
					write(sd,buffer,strlen(buffer));
					close(fd);
					return o;
				}
			  */
				
				
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
					//escribir(respuesta,505,date,document,asset,size,tama,file_stats);				
}
				if (asset!=NULL){
					fclose(asset);
}
			  
			  
			  
			 
			}else{ 
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
			exit(0);
		}
		else 
		{
			close(socket2); 
		}

	}

	//CERRAR SOCKET 

	close(listener);
	printf("Socket cerrado\n\r");
	return 0;
}
void leerarchivoconf(FILE* archivoconf, char* document_root, int topeclientes, char* extrn, int puerto){
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
		else if(strcmp(almacen, "DirectoryIndex")==0){
		  almacen=strtok(NULL,"\n");
		  printf("extrn:%s\n\r",almacen);
		  strcpy(extrn,almacen);
		}
		else if(strcmp(almacen, "MaxClients")==0){
		  almacen=strtok(NULL,"\n");
		  topeclientes=atoi(almacen);
		}
		
	      }
		
	      fclose(archivoconf);
	      if(linea){
			free(linea);
	      }
}
  




int abrir_socket(){

	int sock=socket(PF_INET,SOCK_STREAM,0);  
      
	if(sock==-1){
		fprintf(stderr,"Error de apertura de socket\n\r");
	 }              
    	return sock;
  }


void enlazar_a_puerto(int listener, int puerto){

	  
	  struct sockaddr_in server_addr;
	  server_addr.sin_family=AF_INET;
	  server_addr.sin_port=htons(puerto);
	  server_addr.sin_addr.s_addr=htonl(INADDR_ANY); //Cualquier IP

  

	  int c= bind(listener, (struct sockaddr *) &server_addr, sizeof(server_addr)); 
  
	    if(c==-1){
	      fprintf(stderr,"No se puede enlazar al puerto: dirección ya está en uso\n\r");
	      close(listener);
	    }

	  printf("Establecido el puerto de escucha\n\r");
  }


//#################################################################################################
//---------------------------------------------LISTEN-------------------------------------------------
//#################################################################################################

int escuchando( int listener, int numero){
  
	  int c= listen(listener,numero);
    
	  if(c==-1){
	      fprintf(stderr,"No es posible escuchar en ese puerto\n\r"); 
	      close(listener);
	      return 1;
	  }
	    
	  printf("Enlazado a puerto\n");
}

void escribir( char* respuesta,int numero,char* date,char* document,FILE* asset,int size,char* tama,struct stat file_stats){

	if(numero==200){
			strcat(respuesta, "HTTP/1.1 200 OK\r\n");
			
			fseek(asset,0L,SEEK_END);
			size=ftell(asset);
			sprintf(tama,"%d",size);
			fseek(asset,0L,SEEK_SET);
			document=malloc(size);
			if(document){
			  fread(document,1,size,asset);
			}
			fclose(asset);
			strcat(respuesta, "Connection: close\r\n");
			strcat(respuesta, "Content-Length: ");
			strcat(respuesta,tama);
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
