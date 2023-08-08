#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include "resumen.h"
#include "fragmenta.h"
#define LETRA_ROJA "\x1b[31m"
#define LETRA_VERDE "\x1b[32m"
#define RESET_COLOR "\x1b[0m"

GtkWidget *window;
GtkWidget *fixed1;
GtkWidget *text;
GtkWidget *button1;
GtkWidget *button2;
GtkWidget *label;
GtkWidget *label2;
GtkWidget *label3;
GtkBuilder *builder;
GtkWidget *button[10][10];
char nombreVentana[16];
Bool nombreCorrecto = True;
Bool direccionBarcoEsHorizontal;
int filaSeleccionada, columnaSeleccionada;

int main(int argc, char** argv){
	filaSeleccionada = -1;
	columnaSeleccionada = -1;
	direccionBarcoEsHorizontal = True;
	memset(nombreVentana,0,16);
	char* resuelve;
	char id[300];
    char c[300];
    int socket1;
    int err;
	char respuesta[300];
    FILE * f,*fichero, *ficheroDatos;
    if((resuelve = malloc(100*sizeof(char))) == NULL)
		perror("no se ha reservado bien la memoria");
    if(argc != 3)
	{
		printf(LETRA_ROJA"el comando para ejecutar el programa es:\n");
		printf("\t./cliente1 <ip> <puerto>\n"RESET_COLOR);
		exit(0);
	}
    socket1 = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in cliente;
	cliente.sin_family = AF_INET;
    cliente.sin_addr.s_addr = inet_addr(argv[1]);
    cliente.sin_port = htons(atoi(argv[2]));
    memset(&(cliente.sin_zero),'\0',8);
    err = connect (socket1, (struct sockaddr *)&cliente, sizeof(cliente));
    if (err == -1){
        perror("Error al conectarse al servidor\n");
        exit(-1);
    }
    else{
        printf("Estoy conectado\n");
    }
    char hola[100];
    memset(hola,0,100);
    f = fdopen (socket1, "w+");
    fgets(hola,100,f);
    printf("%s", hola);
    if(strcmp(hola, "S HOLA\n") == 0)
    {
		if((fichero = fopen("cliente2.txt","r")) == NULL)
		{
			fprintf(f,"P REGISTRAR\n");
			fflush(f);
			printf("Es la primera vez que usas esta aplicacion, crea una cuenta\n");
			char sumando1[100],sumando2[100];
			fgets(resuelve, 20000, f);
			printf("%s", resuelve);
			char **trozosRespuesta;
			trozosRespuesta = fragmenta(resuelve);
			strcpy(sumando1,trozosRespuesta[2]);
			strcpy(sumando2,trozosRespuesta[3]);
			int i = 0;
			fprintf(f,"P RESPUESTA %d\n",atoi(sumando1) + atoi(sumando2));
			printf("P RESPUESTA %d\n",atoi(sumando1) + atoi(sumando2));
			char input[2000];
			fgets(input, 2000, f);
			char** exito;
			exito = fragmenta(input);
			if(strcmp(exito[2],"ERROR") == 0)
			{
				printf("%s",input);
				exit(0);
			}
			printf("%s",input);
			//pongo un pause porque sino se cierra el servidor, habria que mejorar esto
			char datosCliente[3000];
			if((fichero = fopen("cliente2.txt", "w")) == NULL)
				perror("No se ha abierto bien el fichero");
			char sumaString[4];
			char espacios[12];
			memset(espacios,0,6);
			memset(sumaString,0,4);
			sprintf(sumaString,"%d",atoi(sumando1) + atoi(sumando2));
			for(int i = 0;i < 12-strlen(exito[3]); i++)
				espacios[i] = ' ';
			char espaciosSuma[3];
			memset(espaciosSuma,0,3);
			for(i = 0;i<4-strlen(sumaString); i++)
				espaciosSuma[i] = ' ';
			i = 0;
			sprintf(datosCliente,"%s%s\t%s%s\n",exito[3],espacios,sumaString,espaciosSuma);
			fputs(datosCliente, fichero);
			fclose(fichero);
		}
		else
		{
			ficheroDatos = fopen("cliente2.txt", "r+");
			memset(id,0,300);
			memset(c,0,300);
			if(feof(ficheroDatos) == 0)
			{
				char linea[100];
				memset(linea,0,100);
				fgets(linea,100,ficheroDatos);
				resumenCliente(linea,id,c,NULL);
				printf("P LOGIN %s %s\n", id, c);
				fprintf(f,"P LOGIN %s %s\n", id, c);
				fflush(f);
			}
			else
			{
				printf("No se ha registrado ningun cliente en este ordenador\n");
				remove("cliente2.txt");
				exit(-1);
			}
			fgets(respuesta, 300, f);
			fclose(ficheroDatos);
			if(strcmp(respuesta,"S LOGIN ERROR\n") == 0)
			{
				memset(respuesta,0,300);
				fgets(respuesta, 300, f);
				if(strcmp("ya esta conectado\n", respuesta) == 0)
					printf(LETRA_ROJA"Ya estas conectado desde otra terminal, cierra la otra sesion para conectarte de nuevo\n"RESET_COLOR);
				else if(strcmp("no esta en la base\n", respuesta) == 0)
				{
					printf(LETRA_ROJA"No estás en la base de datos, registrate antes de acceder a la aplicacion\n"RESET_COLOR);
					remove("cliente2.txt");
				}
				else
					printf(LETRA_ROJA"Error desconocido\n"RESET_COLOR);
				exit(-1);
			}
			else if(strcmp(respuesta,"S LOGIN OK\n") == 0)
			{
				printf(LETRA_VERDE"Se ha hecho el login con exito"RESET_COLOR);
				printf("\n");
			}
		}
	}
	else if(strcmp(hola, "S LLENO\n") == 0)
	{
		printf(LETRA_ROJA"El servidor ha alcanzado el número máximo de clientes que puede atender, lo sentimos. Una vez termine la conexion un cliente de los que están conectados actualmente podrás entrar.\n"RESET_COLOR);
		exit(-1);
	}
	//muestro la interfaz gráfica de llegada de un cliente
	gtk_init(&argc,&argv);
	builder = gtk_builder_new_from_file("ventanaPresentacion.glade");
	window = GTK_WIDGET(gtk_builder_get_object(builder,"VentanaPrincipal"));
	g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_builder_connect_signals(builder,NULL);
	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder,"ContenedorPrincipal"));
	button1 = GTK_WIDGET(gtk_builder_get_object(builder,"BotonEnviar"));
	label = GTK_WIDGET(gtk_builder_get_object(builder,"EtiquetaError"));
	gtk_widget_show(window);
	gtk_main();
	gtk_widget_destroy(button1);
	gtk_widget_destroy(label);
	gtk_widget_destroy(window);
	g_object_unref (G_OBJECT (builder));
	if(strlen(nombreVentana) == 0)
		strcpy(nombreVentana,"Invitado");
	printf("P NOMBRE %s\n",nombreVentana);
	fprintf(f,"P NOMBRE %s\n",nombreVentana);
	fflush(f);
	char pnombre[100];
	fgets(pnombre,100,f);
	printf("%s", pnombre);
	if(strcmp(pnombre,"S NOMBRE OK\n") == 0)
	{
		printf("Nombre cambiado con exito\n");
		char barco[100];
		fgets(barco,100,f);
		char textoEtiqueta[100];
		gtk_init(&argc,&argv);
		builder = gtk_builder_new_from_file("posicionarFlota.glade");
		window = GTK_WIDGET(gtk_builder_get_object(builder,"Tablero"));
		g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
		gtk_builder_connect_signals(builder,NULL);
		fixed1 = GTK_WIDGET(gtk_builder_get_object(builder,"ContenedorPrincipal"));
		button1 = GTK_WIDGET(gtk_builder_get_object(builder,"HorizontalButton"));
		button2 = GTK_WIDGET(gtk_builder_get_object(builder,"VerticalButton"));
		label = GTK_WIDGET(gtk_builder_get_object(builder,"BarcoActual"));
		label2 = GTK_WIDGET(gtk_builder_get_object(builder,"EtiquetaError"));
		label3 = GTK_WIDGET(gtk_builder_get_object(builder,"EtiquetaExplicatoria"));
		char posicion[4];
		for (int i = 0; i < 10; i++)
		{
			for(int j = 0; j < 10; j++)
			{
				memset(posicion,0,4);
				sprintf(posicion,"%d,%d", j, i);
				button[i][j] = GTK_WIDGET(gtk_builder_get_object(builder,posicion));
				gtk_button_set_label((GtkButton *) button[i][j],"Agua");
			}
		}
		gtk_widget_show(window);
		int numCasillas = (int) barco[8] - 48;
		barco[8] = ' ';
		//muestro la interfaz gráfica para posicionar la flota
		while(strcmp(barco,"S BARCO  \n") == 0)
		{
			sprintf(textoEtiqueta,"BARCO A PONER: %d casillas", numCasillas);
			gtk_label_set_text(GTK_LABEL(label),(const gchar*) textoEtiqueta);
			do
			{
				if(strcmp(barco,"S BARCO ERROR\n") == 0)
				{
					gtk_label_set_text(GTK_LABEL(label2),(const gchar*) "HAS PUESTO EL BARCO MAL, PRUEBA OTRA VEZ");
				}
				gtk_main();
				if(direccionBarcoEsHorizontal)
				{
					fprintf(f,"P BARCO %d %d %c\n",columnaSeleccionada,filaSeleccionada,'h');
					fflush(f);
				}
				else
				{
					fprintf(f,"P BARCO %d %d %c\n",columnaSeleccionada,filaSeleccionada,'v');
					fflush(f);
				}
				fgets(barco,100,f);
			} while(strcmp(barco,"S BARCO ERROR\n") == 0);
			gtk_label_set_text(GTK_LABEL(label2),(const gchar*) "");
			if(direccionBarcoEsHorizontal)
			{
				for(int i = columnaSeleccionada; i < columnaSeleccionada+numCasillas; i++)
					gtk_button_set_label((GtkButton *) button[filaSeleccionada][i],"BARCO");
			}
			else
			{
				for(int i = filaSeleccionada; i < filaSeleccionada+numCasillas; i++)
					gtk_button_set_label((GtkButton *) button[i][columnaSeleccionada],"BARCO");
			}
			if(strcmp(barco,"S ESPERA\n") != 0)
			{
				numCasillas = (int) barco[8] - 48;
				barco[8] = ' ';
			}
			else if (strcmp(barco,"S ESPERA\n") == 0)
			{
				for (int i = 0; i < 10; i++)
				{
					for(int j = 0; j < 10; j++)
					{
						if(i != 2 || j != 3)
							gtk_widget_destroy(button[i][j]);
					}
				}
				gtk_widget_destroy(button1);
				gtk_widget_destroy(button2);
				gtk_widget_destroy(label);
				gtk_widget_destroy(label2);
				gtk_widget_destroy(label3);
				gtk_button_set_label((GtkButton *) button[2][3],"Entendido");
				label = GTK_WIDGET(gtk_builder_get_object(builder,"FinalPosicionar"));
				gtk_label_set_text(GTK_LABEL(label),(const gchar*) "YA HAS POSICIONADO LA FLOTA, ESPERANDO A QUE EMPIECE LA PARTIDA...\n\t\t(cuando hayas leido este mensaje dale a entendido)");
				gtk_main();
			}
		}
		printf("%s", barco);
		fflush(stdout);
		gtk_window_close((GtkWindow *) window);
		gtk_widget_destroy(label);
		g_object_unref (G_OBJECT (builder));
		char juego[300];
		fgets(juego,300,f);
		printf("%s", juego);
		fflush(stdout);
		fgets(juego,300,f);
		printf("%s", juego);
		fflush(stdout);
		if(strcmp(juego,"S TUTURNO\n") != 0)
		{
			fgets(juego,300,f);
			printf("%s", juego);
			fflush(stdout);
		}
		gtk_init(&argc,&argv);
		builder = gtk_builder_new_from_file("jugar.glade");
		window = GTK_WIDGET(gtk_builder_get_object(builder,"Tablero"));
		g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
		gtk_builder_connect_signals(builder,NULL);
		fixed1 = GTK_WIDGET(gtk_builder_get_object(builder,"ContenedorPrincipal"));
		label = GTK_WIDGET(gtk_builder_get_object(builder,"Turno"));
		label2 = GTK_WIDGET(gtk_builder_get_object(builder,"EtiquetaExplicatoria"));
		label3 = GTK_WIDGET(gtk_builder_get_object(builder,"EtiquetaError"));
		for (int i = 0; i < 10; i++)
		{
			for(int j = 0; j < 10; j++)
			{
				memset(posicion,0,4);
				sprintf(posicion,"%d,%d", j, i);
				button[i][j] = GTK_WIDGET(gtk_builder_get_object(builder,posicion));
				gtk_button_set_label((GtkButton *) button[i][j],"?");
			}
		}
		gtk_widget_show(window);
		int hundidos = 0;
		char textoHundidos[100];
		while(strcmp(juego,"S TUTURNO\n") == 0)
		{
			gtk_label_set_text(GTK_LABEL(label),(const gchar*) "\t\t\t\t\t\t\tES TU TURNO, SELECCIONA UNA POSICION");
			gtk_main();
			while(strcmp(gtk_button_get_label((GtkButton *) button[filaSeleccionada][columnaSeleccionada]), "?") != 0)
			{
				gtk_label_set_text(GTK_LABEL(label3),(const gchar*) "\t\t\t\t\t\tESA POSICION YA LA HAS DESBLOQUEADO, ELIGE OTRA");
				gtk_main();
			}
			fprintf(f,"P DISPARA %d %d\n",columnaSeleccionada,filaSeleccionada);
			fflush(f);
			fgets(juego,100,f);
			printf("%s", juego);
			fflush(stdout);
			if(strcmp(juego,"S AGUA\n") == 0 || strcmp(juego,"S TOCADO\n") == 0 || strcmp(juego,"S HUNDIDO\n") == 0)
			{
				if(juego[2] == 'A')
					gtk_button_set_label((GtkButton *) button[filaSeleccionada][columnaSeleccionada],"Agua");
				else
				{
					gtk_button_set_label((GtkButton *) button[filaSeleccionada][columnaSeleccionada],"BARCO");
					if(juego[2] == 'H')
					{
						hundidos++;
						sprintf(textoHundidos, "BARCOS HUNDIDOS: %d/7", hundidos);
						gtk_label_set_text(GTK_LABEL(label2),(const gchar*) textoHundidos);
					}
				}
			}
			gtk_label_set_text(GTK_LABEL(label),(const gchar*) "\t\t\t\t\t\t\tTURNO DE TU OPONENTE, ESPERA\n\t\t\t(si aparece un mensaje de forzar salida, dale a esperar está dentro de lo previsto)");
			gtk_label_set_text(GTK_LABEL(label3),(const gchar*) "");
			for(int i = 0; i < 100; i++)
				gtk_main_iteration ();
			fgets(juego,100,f);
			printf("%s", juego);
			fflush(stdout);
			if(strcmp(juego,"S PREMIO\n") != 0 && strcmp(juego,"S SIGUE JUGANDO\n") != 0)
				fgets(juego,100,f);
			if(strcmp(juego,"S TUTURNO\n") != 0)
			{
				for (int i = 0; i < 10; i++)
				{
					for(int j = 0; j < 10; j++)
					{
						gtk_widget_destroy(button[i][j]);
					}
				}
				gtk_widget_destroy(label);
				gtk_widget_destroy(label2);
				gtk_widget_destroy(label3);
				label = GTK_WIDGET(gtk_builder_get_object(builder,"FinalPosicionar"));
				if(strcmp(juego,"S PREMIO\n") == 0)
					gtk_label_set_text(GTK_LABEL(label),(const gchar*) "ENHORABUENA, HAS GANADO");
				else if(strcmp(juego,"S SIGUE JUGANDO\n") == 0)
					gtk_label_set_text(GTK_LABEL(label),(const gchar*) "HAS PERDIDO");
				gtk_main();
			}
		}
		printf("%s", juego);
		fflush(stdout);
	}
	else if(strcmp(pnombre,"S OBSERVADOR\n") == 0)
	{
		printf("En este servidor ya se está jugando una partida, así que seras un observador\n");
		char info[100];
		fgets(info,100,f);
		while(strcmp(info,"S GANA") != 0)
		{
			fgets(info,100,f);
			printf("%s",info);
			fflush(stdout);
			info[6] = '\0';
		}
	}
}

void on_AreaTexto_changed (GtkEntry *e)
{
	sprintf(nombreVentana, "%s",gtk_entry_get_text(e));
	nombreCorrecto = True;
}

void on_BotonEnviar_clicked (GtkButton *b)
{
	int i = 0;
	while(i < strlen(nombreVentana) && nombreCorrecto)
	{
		if(!((nombreVentana[i] >= 65 && nombreVentana[i] <= 90) || (nombreVentana[i] >= 97 && nombreVentana[i] <= 122) || (nombreVentana[i] >= 48 && nombreVentana[i] <= 57)))
			nombreCorrecto = False;
		i++;
	}
	if(!nombreCorrecto)
	{
		gtk_label_set_text(GTK_LABEL(label),(const gchar*) "SOLO PUEDE CONTENER MAYUSCULAS, MINUSCULAS O NUMEROS");
		return;
	}
	gtk_window_close((GtkWindow *) window);
}

void on_button_clicked(GtkButton *b)
{
	if(strcmp(gtk_button_get_label(b), "Entendido") == 0)
		gtk_window_close((GtkWindow *) window);
	else
	{
		int fila = -1, columna = -1;
		for(int i = 0; i < 10; i++)
		{
			for(int j = 0; j < 10; j++)
			{
				if(b == (GtkButton*) button[i][j])
				{
					fila = i;
					columna = j;
				}
			}
		}
		filaSeleccionada = fila;
		columnaSeleccionada = columna;
		gtk_main_quit();
	}
}

void on_HorizontalButton_toggled (GtkRadioButton *b)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b)))
	{
		direccionBarcoEsHorizontal = True;
		printf("Has pulsado el boton horizontal\n");
	}
}

void on_VerticalButton_toggled (GtkRadioButton *b)
{
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(b)))
	{
		direccionBarcoEsHorizontal = False;
		printf("Has pulsado el boton vertical\n");
	}
}
