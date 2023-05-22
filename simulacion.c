// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "simulacion.h"

#define DEBUGN12 1

int acabados = 0;
char camino_fichero[10] = "prueba.dat";

int main(int argc, char const *argv[])
{
    // Asociamos la señal SIGCHLD al enterrador
    signal(SIGCHLD, reaper);

    // Comprobación de sintaxis correcta
    if (argc != 2)
    {
        fprintf(stderr, ROJO "Error de sintaxis\n" RESET);
        return FALLO;
    }
    // Montar el dispositivo virtual
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, ROJO "Error al montar el dispositivo virtual padre" RESET);
        return FALLO;
    }

    // Creación del directorio
    char camino[19] = "/simul_";
    time_t hora_act;
    time(&hora_act);
    struct tm *tm = localtime(&hora_act);
    sprintf(camino + strlen(camino), "%d%02d%02d%02d%02d%02d/",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    if (mi_creat(camino, 7) == FALLO)
    {
        fprintf(stderr, ROJO "./simulacion: Error al crear el directorio '%s'\n" RESET, camino);
        exit(0);
    }
    printf("xddddd");

    for (size_t proceso = 1; proceso <= NUMPROCESOS; proceso++)
    {
        int pid = fork();
        if (pid == 0)
        {
            bmount(argv[1]);

            // Creamos el directorio del hijo
            char camino_hijo[15];
            sprintf(camino_hijo, "/proceso_%d/", getpid());
            strcat(camino, camino_hijo);
            if (mi_creat(camino_hijo, 7) == FALLO)
            {
                bumount();
                return FALLO;
            }

            // Creamos el fichero prueba.dat
            strcat(camino, camino_fichero);
            if (mi_creat(camino, 7) == FALLO)
            {
                bumount();
                return FALLO;
            }

            srand(time(NULL) + getpid());
            for (size_t nescritura = 1; nescritura <= NUMESCRITURAS; nescritura++)
            {
                struct REGISTRO registro;
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = nescritura;
                registro.nRegistro = rand() % REGMAX;
                mi_write(camino, &registro, registro.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO));
#if DEBUGN12
                fprintf(stderr, "[simulación.c → Escritura %ld en %s]\n", nescritura, camino);
#endif
                usleep(50000);
            }

            // Desmontar dispositivos
            if (bumount() == FALLO)
            {
                return FALLO;
            }
            exit(0);
        }
        usleep(150000);
    }

    // Permitir que el padre espere por todos los hijos
    while (acabados < NUMPROCESOS)
    {
        pause();
    }

    // Desmontar el dispositivo
    if (bumount(argv[1]) == FALLO)
    {
        fprintf(stderr, ROJO "Error al desmontar el dispositivo virtual padre" RESET);
        return FALLO;
    }
    exit(0);
}

void reaper()
{
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        acabados++;
    }
}