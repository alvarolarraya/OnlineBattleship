#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include "generarRandom.h"
#include "resumen.h"
#include "fragmenta.h"
#include <sys/time.h>
#include <time.h>
#define FONDO_BLANCO "\x1b[47m"
#define RESET_COLOR "\x1b[0m"
#define LETRA_NEGRA "\x1b[30m"
#define LETRA_ROJA "\x1b[31m"
#define LETRA_VERDE "\x1b[32m"
#define LETRA_AZUL "\x1b[34m"

int main(int argc, char** argv){
	int contadorHundidos1 = 0, contadorHundidos2 = 0;
	int toquesFlota[2][7];
	bool jugador1Listo = false, jugador2Listo = false,haTerminado = false, haEmpezado = false;
	char queHacer[100];
	char nombre[100];
	int numClientes,i;
	int offset = 0;
	char opcionString[100];
	numClientes = 0;
	int nLinea;
	char resuelve [100];
	FILE* fichero, *fsocket1;
	if(argc != 3)
	{
		printf(LETRA_ROJA"el comando para ejecutar el programa es:\n");
		printf("\t./servidor1 <puerto> <maximo_conexiones>\n"RESET_COLOR);
		exit(0);
	}
	int numMaxConexiones;
	numMaxConexiones = atoi(argv[2]);
	int numCasillas[7] = {5,4,4,3,3,2,2};
	//cada jugador tiene dos tableros: uno con su flota y otro con el historial de disparos que ha hecho
	int tableroJugador1_1[10][10],tableroJugador2_1[10][10];
	bool barcosPosicionados[2][7];
	char nombresClientes[numMaxConexiones] [100];
	char idClientes[numMaxConexiones] [100];
	int descriptores[numMaxConexiones];
	for(i = 0; i < numMaxConexiones;i++)
	{
		descriptores[i] = -1;
		strcpy(nombresClientes[i], "");
		strcpy(idClientes[i], "");
	}
	for(i = 0; i < 2;i++)
	{
		for(int j = 0; j < 7; j++)
		{
			barcosPosicionados[i][j] = false;
			toquesFlota[i][j] = 0;
		}
	}
	for(i = 0;i < 10; i++)
	{
		for(int j = 0;j < 10; j++)
		{
			tableroJugador1_1[i][j] = -1;
			tableroJugador2_1[i][j] = -1;
		}
	}
	//si no existe el fichero de registro de clientes lo creo y lo cierro porque por ahora no lo necesito
	if((fichero = fopen("servidor2.txt","a")) == NULL)
		perror("no se ha abierto bien el fichero");
	fclose(fichero);
	int socket1;
    struct sockaddr_in servidor;
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(atoi(argv[1]));
    servidor.sin_addr.s_addr = INADDR_ANY;
    int socketCliente;
    if((socket1 = socket(PF_INET,SOCK_STREAM,0)) == -1)
        perror("no se ha obtenido bien el socket");
    if (setsockopt(socket1, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        perror("no se pudo reutilizar el puerto cerrado hace unos pocos segundos");
    if (bind(socket1, (struct sockaddr *)&servidor, sizeof(servidor)) == -1)
        perror("error al asignar la direccion al socket");
    listen (socket1, 20);
    int tamanioServidor = sizeof(servidor);
	printf("Esperando a que se conecte un cliente\n");
	//paraleer es el conjunto de clientes que tenemos conectados
	fd_set paraleer;
	int fd_maximo;
	int pidEsperaClientes = fork();
	if(pidEsperaClientes == -1)
		perror("no se ha hecho bien el fork");
	else if(pidEsperaClientes == 0)
	{
		while(1)
		{
			//actualiza el paraleer y el maximo, para saber que clientes escuchar
			FD_ZERO(&paraleer);
			FD_SET(socket1,&paraleer);
			fd_maximo = socket1;
			for(i = 0;i < numMaxConexiones; i++)
			{
				if(descriptores[i] != -1)
				{
					FD_SET(descriptores[i],&paraleer);
					if(descriptores[i] > fd_maximo)
					{
						fd_maximo = descriptores[i];
					}
				}
			}
			fd_maximo ++;
			if(select(fd_maximo,&paraleer,NULL,NULL,NULL) > 0)
			{
				//se quiere conectar un nuevo cliente
				if(FD_ISSET(socket1,&paraleer))
				{
					if((socketCliente = accept(socket1, (struct sockaddr *) &servidor,(socklen_t *) &tamanioServidor)) < 0)
						perror("error al aceptar la peticion de un cliente");
					printf(FONDO_BLANCO LETRA_NEGRA"SE HA CONECTADO UN CLIENTE"RESET_COLOR);
					printf("\n");
					numClientes++;
					if(numClientes <= numMaxConexiones)
					{
						printf("Ahora hay %d clientes\n", numClientes);
						fsocket1 = fdopen (socketCliente, "w+");
						printf("S HOLA\n");
						fprintf(fsocket1,"S HOLA\n");
						fflush(fsocket1);
						memset(queHacer,0,100);
						fgets(queHacer,100,fsocket1);
						if(strcmp(queHacer,"P REGISTRAR\n") == 0)
						{
							if((fichero = fopen("servidor2.txt","a")) == NULL)
								perror("no se ha abierto bien el fichero");
							fclose(fichero);
							char mensajeSinSalto[100];
							char* mensaje;
							int sumando1,sumando2,i;
							char id[12];
							char suma[100];
							if((mensaje = malloc(100*sizeof(char))) == NULL)
								perror("no se ha reservado bien la memoria");
							printf("El cliente no está registrado, procedemos a asignarle un id\n");
							sumando1 = generaRandom(100);
							//hago un sleep porque la funcion que genera los random utiliza el tiempo actual para ello
							//si el programa genera dos random muy rapido (en el mismo microsegundo) devuelve el mismo numero
							usleep(1);
							sumando2 = generaRandom(100);
							memset(resuelve,0,100);
							sprintf(resuelve,"S RESUELVE %d %d\n",sumando1,sumando2);
							printf("%s",resuelve);
							fprintf(fsocket1,"%s",resuelve);
							fflush(fsocket1);
							memset(mensaje,0,100);
							fgets(mensaje,100,fsocket1);
							printf("%s",mensaje);
							char **trozosRespuesta;
							trozosRespuesta = fragmenta(mensaje);
							memset(mensajeSinSalto,0,100);
							strcpy(mensajeSinSalto,trozosRespuesta[2]);
							char respuesta[200];
							if((sumando1+sumando2) == atoi(mensajeSinSalto))
							{
								bool repe;
								char datosCliente[100];
								char leerRegistro[100];
								int longitudContrasenia = generaRandom(6)+6;
								char contrasenia[12];
								memset(contrasenia,0,12);
								do
								{
									repe = false;
									int i;
									for(i = 0;i<longitudContrasenia;i++)
									{
										int letraONumero = generaRandom(2);
										if(letraONumero == 0)	//genero una letra
											contrasenia[i] = 97+generaRandom(26);
										else	//genero un numero
											contrasenia[i] = 48+generaRandom(10);
										usleep(1);
									}
									for(int j = i;i < 12-longitudContrasenia;i++)
										contrasenia[j] = ' ';
									if((fichero = fopen("servidor2.txt","r")) == NULL)
										perror("no se ha abierto bien el fichero");
									fgets(leerRegistro,100,fichero);
									//compruebo si la contraseña es o no unica
									while(feof(fichero) == 0 && !repe)
									{
										memset(suma,0,100);
										memset(id,0,12);
										memset(nombre,0,16);
										resumenCliente(leerRegistro,id,suma,nombre);
										if(strcmp(contrasenia, id) == 0)
											repe = true;
										fgets(leerRegistro,100,fichero);
									}
								} while(repe);	//si la contraseña está ya asignada genero otra
								fclose(fichero);
								memset(respuesta,0,200);
								sprintf(respuesta,"S REGISTRADO OK %s\n", contrasenia);
								printf("%s",respuesta);
								fprintf(fsocket1,"%s",respuesta);
								fflush(fsocket1);
								char sumaString[4];
								memset(sumaString,0,4);
								char espacios[6],espaciosSuma[3];
								memset(espaciosSuma,0,3);
								memset(espacios,0,6);
								for(int i = 0;i < 12-strlen(contrasenia); i++)
									espacios[i] = ' ';
								sprintf(sumaString,"%d",sumando1+sumando2);
								for(i = 0;i<4-strlen(sumaString); i++)
									espaciosSuma[i] = ' ';
								sprintf(datosCliente,"%s%s\t%s%s\tInvitado        \n",contrasenia,espacios,sumaString,espaciosSuma);
								if((fichero = fopen("servidor2.txt","a")) == NULL)
									perror("no se ha abierto bien el fichero");
								nLinea = ftell(fichero)/35;
								fputs(datosCliente,fichero);
								fclose(fichero);
								strcpy(nombresClientes[numClientes-1], "Invitado");
								strcpy(idClientes[numClientes-1], contrasenia);
							}
							else
							{
								sprintf(respuesta,"S REGISTRADO ERROR\n");
								printf("%s",respuesta);
								fprintf(fsocket1,"%s\n",respuesta);
								fflush(fsocket1);
								close(socketCliente);
							}
						}
						else
						{
							bool yaConectado;
							yaConectado = false;
							bool esta;
							char id[12];
							int i;
							char suma[100],leerRegistro[100];
							char ** clienteTrozeado;
							char cidSinSalto[100];
							FILE *serv;
							printf("%s", queHacer);
							printf("Ya existe un cliente registrado\n");
							printf("Comprobando si está en la base de datos...\n");
							clienteTrozeado = fragmenta(queHacer);
							char sumaSinEspacios[100],idSinEspacios[12];
							memset(sumaSinEspacios,0,100);
							strcpy(sumaSinEspacios, clienteTrozeado[3]);
							strcpy(idSinEspacios,clienteTrozeado[2]);
							serv = fopen ("servidor2.txt", "r+");
							esta = false;
							nLinea = 0;
							fgets(leerRegistro,100,serv);
							while(feof(serv) == 0)
							{
								memset(suma,0,100);
								memset(id,0,12);
								memset(nombre,0,16);
								memset(cidSinSalto,0,100);
								resumenCliente(leerRegistro,id,suma, nombre);
								i = 0;
								while(id[i] != ' ')
								{
									cidSinSalto[i] = id[i];
									i++;
								}
								if(strcmp(cidSinSalto, idSinEspacios) == 0 && strcmp(suma, sumaSinEspacios) == 0)
								{
									for(int k = 0; k < numMaxConexiones;k++)
									{
										if(strcmp(cidSinSalto,idClientes[k]) == 0)
										{
											printf("El cliente ya está conectado desde otra terminal\n");
											yaConectado = true;
										}
									}
									if(!yaConectado)
									{
										esta = true;
										strcpy(nombresClientes[numClientes-1],nombre);
										strcpy(idClientes[numClientes-1],cidSinSalto);
										break;
									}
								}
								fgets(leerRegistro,100,serv);
								nLinea++;
							}
							if(esta)
							{
								printf(LETRA_VERDE"S LOGIN OK"RESET_COLOR);
								printf("\n");
								fprintf(fsocket1,"S LOGIN OK\n");
								fflush(fsocket1);
							}
							else
							{
								printf(LETRA_ROJA"S LOGIN ERROR: cierro la conexion"RESET_COLOR);
								printf("\n");
								fprintf(fsocket1,"S LOGIN ERROR\n");
								if(yaConectado)
									fprintf(fsocket1,"ya esta conectado\n");
								else
									fprintf(fsocket1,"no esta en la base\n");
								fflush(fsocket1);
							}
						}
						for(i = 0; i < numMaxConexiones; i++)
						{
							if(descriptores[i] == -1)
							{
								descriptores[i] = socketCliente;
								break;
							}
						}
					}
					else if(numClientes > numMaxConexiones)
					{
						numClientes--;
						fsocket1 = fdopen (socketCliente, "w+");
						fprintf(fsocket1,"S LLENO\n");
						fflush(fsocket1);
						printf(LETRA_ROJA"El servidor ha alcanzado el número máximo de clientes que puede atender, lo sentimos. Una vez termine la conexion un cliente de los que están conectados actualmente podrás entrar."RESET_COLOR);
						printf("\n");
						close(socketCliente);
					}
				}
				//un cliente ya conectado quiere hacer algo
				for(i = 0; i < numMaxConexiones; i++)
				{
					//si el socket esta en uso
					if(descriptores[i] != -1)
					{
						//si el descriptor es uno de los que ha mandado algo
						if(FD_ISSET(descriptores[i],&paraleer))
						{
							fsocket1 = fdopen (descriptores[i], "w+");
							memset(opcionString,0,100);
							fgets(opcionString,100,fsocket1);
							int leidos;
							leidos = strlen(opcionString);
							if(leidos > 0)
							{
								char ** opcionTrozos;
								opcionTrozos = fragmenta(opcionString);
								if(strcmp(opcionString,"0\n") != 0)
								{
									printf("El cliente %s solicita: %s",nombresClientes[i],opcionString);
								}
								else
								{
									printf("El cliente %s se desconecta\n", nombresClientes[i]);
								}
								if((strcmp(opcionTrozos[0],"P") == 0) && (strcmp(opcionTrozos[1],"NOMBRE") == 0))
								{
									char idSinSalto[100];
									if((fichero = fopen("servidor2.txt","r+")) == NULL)
										fprintf(fsocket1,"ERROR\n");
									char nombre[16],id[100],leerRegistro[100],suma[100];
									memset(leerRegistro,0,100);
									fgets(leerRegistro,100,fichero);
									nLinea = 1;
									while(feof(fichero) == 0)
									{
										memset(id,0,100);
										memset(nombre,0,16);
										memset(idSinSalto,0,100);
										resumenCliente(leerRegistro,id,suma, nombre);
										int k = 0;
										while(id[k] != ' ')
										{
											idSinSalto[k] = id[k];
											k++;
										}
										if(strcmp(idSinSalto, idClientes[i]) == 0)
											break;
										fgets(leerRegistro,100,fichero);
										nLinea++;
									}
									offset = 35*nLinea;
									char espacios[16];
									memset(espacios,0,16);
									memset(nombre,0,16);
									memset(nombresClientes[i],0,100);
									if((fichero = fopen("servidor2.txt","r+")) == NULL)
										fprintf(fsocket1,"ERROR\n");
									fseek(fichero,offset-17,SEEK_SET);
									strcpy(nombre,opcionTrozos[2]);
									strcpy(nombresClientes[i], nombre);
									for(int i = 0;i < 16-strlen(nombre);i++)
										espacios[i] = ' ';
									if(fprintf(fichero,"%s%s",nombre,espacios) == -1)
										fprintf(fsocket1,"ERROR\n");
									if(numClientes > 2)
									{
										fsocket1 = fdopen (socketCliente, "w+");
										fprintf(fsocket1,"S OBSERVADOR\n");
										fflush(fsocket1);
										printf("Ahora hay %d clientes\n", numClientes);
										printf("Este va a ser observador\n");
									}
									else
									{
										fprintf(fsocket1,"S NOMBRE OK\n");
										fprintf(fsocket1,"S BARCO 5\n");
										printf("S NOMBRE OK\n");
										fflush(fsocket1);
									}
									fclose(fichero);
								}
								else if((strcmp(opcionTrozos[0],"P") == 0) && (strcmp(opcionTrozos[1],"BARCO") == 0))
								{
									bool correcto = true;
									int columna,fila,queBarco = 0,queJugador = i+1;
									char direccion;
									while(barcosPosicionados[i][queBarco])
										queBarco++;
									fila = atoi(opcionTrozos[3]);
									columna = atoi(opcionTrozos[2]);
									direccion = opcionTrozos[4][0];
									int k;
									//compruebo que la eleccion este en orden
									if(direccion == 'h')
									{
										if(fila > 9 || fila < 0 || (columna) < 0 || (columna) > 9 || (columna+numCasillas[queBarco]-1) < 0 || (columna+numCasillas[queBarco]-1) > 9)
											correcto = false;
										k = columna-1;
										while(k < (columna+numCasillas[queBarco]+1) && correcto)
										{
											if(queJugador == 1 && k >= 0 && k <= 9)
											{
												if(tableroJugador1_1[fila][k] != -1)
													correcto = false;
												if((fila+1) <= 9 && tableroJugador1_1[fila+1][k] != -1)
													correcto = false;
												if((fila-1) >= 0 && tableroJugador1_1[fila-1][k] != -1)
													correcto = false;
											}
											else if(queJugador == 2 && k >= 0 && k <= 9)
											{
												if(tableroJugador2_1[fila][k] != -1)
													correcto = false;
												if((fila+1) <= 9 && tableroJugador2_1[fila+1][k] != -1)
													correcto = false;
												if((fila-1) >= 0 && tableroJugador2_1[fila-1][k] != -1)
													correcto = false;
											}
											k++;
										}
									}
									else
									{
										if(columna > 9 || columna < 0 || (fila) < 0 || (fila) > 9 || (fila+numCasillas[queBarco]-1) < 0 || (fila+numCasillas[queBarco]-1) > 9)
											correcto = false;
										k = fila-1;
										while(k < (fila+numCasillas[queBarco]+1) && correcto)
										{
											if(queJugador == 1 && k >= 0 && k <= 9)
											{
												if(tableroJugador1_1[k][columna] != -1)
													correcto = false;
												if((columna+1) <= 9 && tableroJugador1_1[k][columna+1] != -1)
													correcto = false;
												if((columna-1) >= 0 && tableroJugador1_1[k][columna-1] != -1)
													correcto = false;
											}
											else if(queJugador == 2 && k >= 0 && k <= 9)
											{
												if(tableroJugador2_1[k][columna] != -1)
													correcto = false;
												if((columna+1) <= 9 && tableroJugador2_1[k][columna+1] != -1)
													correcto = false;
												if((columna-1) >= 0 && tableroJugador2_1[k][columna-1] != -1)
													correcto = false;
											}
											k++;
										}
									}
									if(correcto)
									{
										//actualizo la matriz tablero
										if(queJugador == 1)
										{
											if(direccion == 'h')
											{
												for(int i = columna; i < columna+numCasillas[queBarco]; i++)
													tableroJugador1_1[fila][i] = queBarco;
											}
											else
											{
												for(int i = fila; i < fila+numCasillas[queBarco]; i++)
													tableroJugador1_1[i][columna] = queBarco;
											}
										}
										else
										{
											if(direccion == 'h')
											{
												for(int i = columna; i < columna+numCasillas[queBarco]; i++)
													tableroJugador2_1[fila][i] = queBarco;
											}
											else
											{
												for(int i = fila; i < fila+numCasillas[queBarco]; i++)
													tableroJugador2_1[i][columna] = queBarco;
											}
										}
										barcosPosicionados[i][queBarco] = true;
										if(queBarco == 6)
										{
											printf("Ya ha posicionado su flota, espere a que empiece la partida\n");
											fprintf(fsocket1,"S ESPERA\n");
											fflush(fsocket1);
											if(queJugador == 1)
												jugador1Listo = true;
											else
												jugador2Listo = true;
										}
										else
										{
											fprintf(fsocket1,"S BARCO %d\n", numCasillas[queBarco+1]);
											fflush(fsocket1);
											printf("S BARCO %d\n", numCasillas[queBarco+1]);
										}
									}
									else
									{
										printf("S BARCO ERROR\n");
										fprintf(fsocket1,"S BARCO ERROR\n");
										fflush(fsocket1);
									}
								}
								else if((strcmp(opcionTrozos[0],"P") == 0) && (strcmp(opcionTrozos[1],"DISPARA") == 0))
								{
									char queHaPasado[100];
									int fila = atoi(opcionTrozos[3]), columna = atoi(opcionTrozos[2]),queJugador = i+1;
									if(queJugador == 1)
									{
										if(tableroJugador2_1[fila][columna] == -1)
										{
											sprintf(queHaPasado,"AGUA");
											fprintf(fsocket1,"S AGUA\n");
											printf("S AGUA\n");
										}
										else
										{
											int queBarco = tableroJugador2_1[fila][columna];
											toquesFlota[queJugador-1][queBarco]++;
											if(toquesFlota[queJugador-1][queBarco] == numCasillas[queBarco])
											{
												sprintf(queHaPasado,"HUNDIDO");
												fprintf(fsocket1,"S HUNDIDO\n");
												printf("S HUNDIDO\n");
												contadorHundidos1++;
											}
											else
											{
												sprintf(queHaPasado,"TOCADO");
												fprintf(fsocket1,"S TOCADO\n");
												printf("S TOCADO\n");
											}
										}
										fflush(fsocket1);
										if(contadorHundidos1 == 7)
										{
											haTerminado = true;
											fprintf(fsocket1,"S PREMIO\n");
											fflush(fsocket1);
											for(i = 0; i < 2;i++)
											{
												for(int j = 0; j < 7; j++)
												{
													barcosPosicionados[i][j] = false;
													toquesFlota[i][j] = 0;
												}
											}
											for(i = 0;i < 10; i++)
											{
												for(int j = 0;j < 10; j++)
												{
													tableroJugador1_1[i][j] = -1;
													tableroJugador2_1[i][j] = -1;
												}
											}
											fsocket1 = fdopen(descriptores[1], "w+");
											fprintf(fsocket1,"S SIGUE JUGANDO\n");
											fflush(fsocket1);
											for(int k = 2; k < numClientes; k++)
											{
												fsocket1 = fdopen(descriptores[k], "w+");
												fprintf(fsocket1,"S GANA %s\n", nombresClientes[queJugador-1]);
												fflush(fsocket1);
											}
										}
										else
										{
											for(int k = 0; k < numClientes; k++)
											{
												if(k != 0)
												{
													fsocket1 = fdopen(descriptores[k], "w+");
													fprintf(fsocket1,"S INFO %s %s %d %d\n", queHaPasado, nombresClientes[queJugador-1],columna, fila);
													fflush(fsocket1);
												}
											}
											fsocket1 = fdopen(descriptores[1], "w+");
											fprintf(fsocket1,"S TUTURNO\n");
											fflush(fsocket1);
										}
									}
									else
									{
										if(tableroJugador1_1[fila][columna] == -1)
										{
											sprintf(queHaPasado,"AGUA");
											fprintf(fsocket1,"S AGUA\n");
											printf("S AGUA\n");
										}
										else
										{
											int queBarco = tableroJugador1_1[fila][columna];
											toquesFlota[queJugador-1][queBarco]++;
											if(toquesFlota[queJugador-1][queBarco] == numCasillas[queBarco])
											{
												sprintf(queHaPasado,"HUNDIDO");
												fprintf(fsocket1,"S HUNDIDO\n");
												printf("S HUNDIDO\n");
												contadorHundidos2++;
											}
											else
											{
												sprintf(queHaPasado,"TOCADO");
												fprintf(fsocket1,"S TOCADO\n");
												printf("S TOCADO\n");
											}
										}
										fflush(fsocket1);
										if(contadorHundidos2 == 7)
										{
											haTerminado = true;
											fprintf(fsocket1,"S PREMIO\n");
											fflush(fsocket1);
											for(i = 0; i < 2;i++)
											{
												for(int j = 0; j < 7; j++)
												{
													barcosPosicionados[i][j] = false;
													toquesFlota[i][j] = 0;
												}
											}
											for(i = 0;i < 10; i++)
											{
												for(int j = 0;j < 10; j++)
												{
													tableroJugador1_1[i][j] = -1;
													tableroJugador2_1[i][j] = -1;
												}
											}
											fsocket1 = fdopen(descriptores[0], "w+");
											fprintf(fsocket1,"S SIGUE JUGANDO\n");
											fflush(fsocket1);
											for(int k = 2; k < numClientes; k++)
											{
												fsocket1 = fdopen(descriptores[k], "w+");
												fprintf(fsocket1,"S GANA %s\n", nombresClientes[queJugador-1]);
												fflush(fsocket1);
											}
										}
										else
										{
											for(int k = 0; k < numClientes; k++)
											{
												if(k != 1)
												{
													fsocket1 = fdopen(descriptores[k], "w+");
													fprintf(fsocket1,"S INFO %s %s %d %d\n", queHaPasado, nombresClientes[queJugador-1],columna, fila);
													fflush(fsocket1);
												}
											}
											fsocket1 = fdopen(descriptores[0], "w+");
											fprintf(fsocket1,"S TUTURNO\n");
											fflush(fsocket1);
										}
									}
								}
								else
								{
									fprintf(fsocket1,"S PROTOCOL ERROR\n");
									printf("S PROTOCOL ERROR\n");
									fflush(fsocket1);
									fclose(fsocket1);
									close(descriptores[i]);
									for(i = 0; i < 2;i++)
									{
										for(int j = 0; j < 7; j++)
										{
											barcosPosicionados[i][j] = false;
											toquesFlota[i][j] = 0;
										}
									}
									for(i = 0;i < 10; i++)
									{
										for(int j = 0;j < 10; j++)
										{
											tableroJugador1_1[i][j] = -1;
											tableroJugador2_1[i][j] = -1;
										}
									}
									if(haEmpezado)
									{
										if(i == 0 && !haTerminado && numClientes > 1)
										{
											fsocket1 = fdopen(descriptores[1], "w+");
											fprintf(fsocket1,"S PREMIO\n");
											fflush(fsocket1);
											for(int k = 2; k < numClientes; k++)
											{
												fsocket1 = fdopen(descriptores[k], "w+");
												fprintf(fsocket1,"S GANA %s\n", nombresClientes[i-1]);
												fflush(fsocket1);
											}
										}
										if(i == 1 && !haTerminado)
										{
											fsocket1 = fdopen(descriptores[0], "w+");
											fprintf(fsocket1,"S PREMIO\n");
											fflush(fsocket1);
											for(int k = 2; k < numClientes; k++)
											{
												fsocket1 = fdopen(descriptores[k], "w+");
												fprintf(fsocket1,"S GANA %s\n", nombresClientes[i-1]);
												fflush(fsocket1);
											}
										}
									}
									for(int j = i; j < numClientes; j++)
									{
										strcpy(idClientes[j],idClientes[j+1]);
										strcpy(nombresClientes[j],nombresClientes[j+1]);
										descriptores[j] = descriptores[j+1];
									}
									numClientes--;
									strcpy(nombresClientes[numClientes],"");
									strcpy(idClientes[numClientes],"");
									descriptores[numClientes] = -1;
									printf("Ahora hay %d clientes\n", numClientes);
								}
							}
							else
							{
								fclose(fsocket1);
								close(descriptores[i]);
								numClientes--;
								for(int j = i; j < numClientes; j++)
								{
									strcpy(idClientes[j],idClientes[j+1]);
									strcpy(nombresClientes[j],nombresClientes[j+1]);
									descriptores[j] = descriptores[j+1];
								}
								strcpy(nombresClientes[numClientes],"");
								strcpy(idClientes[numClientes],"");
								descriptores[numClientes] = -1;
								printf("Ahora hay %d clientes\n", numClientes);
								if(i == 0 || i == 1)
								{
									if(haEmpezado && numClientes > 0)
									{
										if(!haTerminado)
										{
											fsocket1 = fdopen(descriptores[0], "w+");
											fprintf(fsocket1,"S PREMIO\n");
											fflush(fsocket1);
											for(i = 0; i < 2;i++)
											{
												for(int j = 0; j < 7; j++)
												{
													barcosPosicionados[i][j] = false;
													toquesFlota[i][j] = 0;
												}
											}
											for(i = 0;i < 10; i++)
											{
												for(int j = 0;j < 10; j++)
												{
													tableroJugador1_1[i][j] = -1;
													tableroJugador2_1[i][j] = -1;
												}
											}
											if(numClientes > 1)
											{
												for(int k = 1; k < numClientes; k++)
												{
													fsocket1 = fdopen(descriptores[k], "w+");
													fprintf(fsocket1,"S GANA %s\n", nombresClientes[0]);
													fflush(fsocket1);
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
			if(jugador1Listo && jugador2Listo)
			{
				haEmpezado = true;
				jugador1Listo = false;
				jugador2Listo = false;
				char inicio[300];
				sprintf(inicio,"S INICIO %s %s\n", nombresClientes[0], nombresClientes[1]);
				for(i = 0; i < numClientes; i++)
				{
					fsocket1 = fdopen(descriptores[i], "w+");
					fprintf(fsocket1,"%s",inicio);
					if(i == 0)
						fprintf(fsocket1,"S TUTURNO\n");
					fflush(fsocket1);
				}
			}
		}
	}
	int sig;
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGINT);
	sigwait(&sigset,&sig);
	kill(pidEsperaClientes,SIGKILL);
	close(socket1);
	return 0;
}
