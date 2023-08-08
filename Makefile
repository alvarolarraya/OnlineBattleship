all:
	gcc -Wall -c resumen.c
	gcc -Wall -c fragmenta.c
	gcc -Wall -c generarRandom.c
	gcc -Wall servidor1.c -o servidor1 resumen.c generarRandom.c fragmenta.c
	gcc -Wall cliente1.c -o cliente1 resumen.c fragmenta.c -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
	gcc -Wall servidor2.c -o servidor2 resumen.c generarRandom.c fragmenta.c
	gcc -Wall cliente2.c -o cliente2 resumen.c fragmenta.c -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
	
cliente:
	gcc -Wall -c resumen.c
	gcc -Wall -c fragmenta.c
	gcc -Wall -c generarRandom.c
	gcc -Wall cliente1.c -o cliente1 resumen.c fragmenta.c -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
	gcc -Wall cliente2.c -o cliente2 resumen.c fragmenta.c -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
	
servidor:
	gcc -Wall -c resumen.c
	gcc -Wall -c fragmenta.c
	gcc -Wall -c generarRandom.c
	gcc -Wall servidor1.c -o servidor1 resumen.c generarRandom.c fragmenta.c
	gcc -Wall servidor2.c -o servidor2 resumen.c generarRandom.c fragmenta.c
	
clean:
	rm fragmenta.o
	rm resumen.o
	rm generarRandom.o
	rm servidor1
	rm servidor2
	rm cliente1
	rm cliente2
	rm servidor2.txt
	rm cliente2.txt
