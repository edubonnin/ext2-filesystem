RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

**** MEJORAS REALIZADAS ****

* mi_dir() --> se usa el mi_read_f() para leer un buffer de entradas y explorar 
  éstas en memoria en lugar de leer 1 sola entrada cada vez, y así no tener que
  acceder al dispositivo para cada una de ellas

* mi_ls.c --> diferencia de colores en función de si se trata de fichero o directorios,
  información extendida de las entradas y aplicable directamente a ficheros

* uso del mi_touch.c para crear ficheros