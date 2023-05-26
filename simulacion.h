// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include <sys/wait.h>
#include <signal.h>
#include "directorios.h"

#define NUMPROCESOS 100
#define NUMESCRITURAS 50
#define REGMAX 500000

struct REGISTRO
{                   // sizeof(struct REGISTRO): 24 bytes
    time_t fecha;   // Precisión segundos
    pid_t pid;      // PID del proceso que lo ha creado
    int nEscritura; // Entero con el número de escritura, de 1 a 50 (orden por tiempo)
    int nRegistro;  // Entero con el número del registro dentro del fichero (orden por posición)
};

void reaper();