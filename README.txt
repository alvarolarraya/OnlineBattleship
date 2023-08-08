Tanto el objetivo 1 como el objetivo 2 poseen clientes que se comunican con el usuario unica y exclusivamente mediante interfaz gráfica (menos señales del tipo control C). Para poder asegurar que el programa funcione como debe, pedimos que se ejecute el programa desde un sistema operativo linux en el que esté instalado gtk version (mínimo) 3.20

sudo docker build -t battleship .

sudo docker run --rm -ti --net=host -e DISPLAY=$DISPLAY battleship

sudo xhost +

