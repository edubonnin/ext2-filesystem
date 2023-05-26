// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "simulacion.h"

#define DEBUGN12 0
int acabados = 0;
char nombre_fichero[11] = "prueba.dat";

int main(int argc, char *argv[])
{
    // Asociamos la señal SIGCHLD al enterrador
    signal(SIGCHLD, reaper);

    // Comprobación de sintaxis correcta
    if (argc != 2)
    {
        fprintf(stderr, ROJO "Error de sintaxis: ./simulacion <disco>\n" RESET);
        return FALLO;
    }

    // Montar el dispositivo virtual (padre)
    if (bmount(argv[1]) == FALLO)
    {
        fprintf(stderr, ROJO "Error al montar el dispositivo virtual padre" RESET);
        exit(0);
    }

    // Creación del directorio
    char camino[21] = "/simul_";
    time_t hora_act;
    time(&hora_act);
    struct tm *tm = localtime(&hora_act);
    sprintf(camino + strlen(camino), "%d%02d%02d%02d%02d%02d/",
            tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);

    if (mi_creat(camino, 7) == FALLO)
    {
        fprintf(stderr, "./simulacion: Error al crear el directorio '%s'\n", camino);
        return FALLO;
    }

    pid_t pid;
    for (int proceso = 1; proceso <= NUMPROCESOS; proceso++)
    {
        pid = fork();
        if (pid == 0) // Es el hijo
        {
            bmount(argv[1]);

            // Creamos el directorio del hijo
            char camino_hijo[38];
            sprintf(camino_hijo, "%sproceso_%d/", camino, getpid());
            // strcat(camino, camino_hijo);
            if (mi_creat(camino_hijo, 7) < 0)
            {
                bumount();
                return FALLO;
            }

            char camino_total[48];
            sprintf(camino_total, "%sprueba.dat", camino_hijo);
            // Creamos el fichero prueba.dat
            if (mi_creat(camino_total, 7) < 0)
            {
                bumount();
                return FALLO;
            }

            srand(time(NULL) + getpid());
            for (int nescritura = 1; nescritura <= NUMESCRITURAS; nescritura++)
            {
                struct REGISTRO registro;
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = nescritura;
                registro.nRegistro = rand() % REGMAX;
                mi_write(camino_total, &registro, registro.nRegistro * sizeof(struct REGISTRO), sizeof(struct REGISTRO));
#if DEBUGN12
                fprintf(stderr, "[simulación.c → Escritura %i en %s]\n", nescritura, camino_total);
#endif
                usleep(50000);
            }
            fprintf(stderr, "[Proceso %d: Completadas %d escrituras en %s]\n", proceso, NUMESCRITURAS, camino_total);
            // Desmontar dispositivo hijo
            bumount();
            exit(0);
        }
        usleep(150000);
    }

    // Permitir que el padre espere por todos los hijos:
    while (acabados < NUMPROCESOS)
    {
        pause();
    }

    bumount();
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
