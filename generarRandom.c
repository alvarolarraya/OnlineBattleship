#include "generarRandom.h"

int generaRandom(int max)
{
	struct timeval tiempoActualMicrosegundos;
	double ahora;
	int random;
	gettimeofday(&tiempoActualMicrosegundos, NULL);
	ahora = tiempoActualMicrosegundos.tv_usec;
	srand(ahora);
	random = rand()%max;
	return random;
}
