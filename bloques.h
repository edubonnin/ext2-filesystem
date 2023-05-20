// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include <stdio.h>    //printf(), fprintf(), stderr, stdout, stdin
#include <fcntl.h>    //O_WRONLY, O_CREAT, O_TRUNC
#include <sys/stat.h> //S_IRUSR, S_IWUSR
#include <stdlib.h>   //exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h>   // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>    //errno
#include <string.h>   // strerror()
#include <limits.h>   //
#include <time.h>     //

#define BLOCKSIZE 1024 // bytes

// CÓDIGOS GESTIÓN DE ERRORES
#define EXITO 0
#define FALLO -1

// COLORES
#define AZUL "\033[94m"
#define GRIS "\033[90m"
#define ROJO "\x1b[91m"
#define VERDE "\033[32m"
#define MAGENTA "\033[35m"
#define RESET "\033[0m"

// FUNCIONES
int bmount(const char *camino);
int bumount();
int bwrite(unsigned int nbloque, const void *buf);
int bread(unsigned int nbloque, void *buf);