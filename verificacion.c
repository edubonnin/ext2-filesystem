// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "verificacion.h"

#define DEBUG 1

int main(int argc, char const *argv[])
{

    // Comprobar la sintaxis
    if (argc != 3)
    {
        fprintf(stderr, ROJO "Sintaxis: ./verificacion <nombre_dispositivo> <directorio_simulación>\n" RESET);
        return FALLO;
    }

    // Montamos el disco
    if (bmount(argv[1]) == FALLO)
    {
        return FALLO;
    }

    struct STAT st;
    mi_stat(argv[2], &st); // Stat del inodo del directorio de simulación

#if DEBUG
    fprintf(stderr, "Directorio de simulación: %s\n", argv[2]);
#endif

    int numentradas = (st.tamEnBytesLog / sizeof(struct entrada));
    if (numentradas != NUMPROCESOS)
    {
        printf(ROJO "verificacion.c: Error en el número de entradas.\n" RESET);
        bumount();
        return -1;
    }

#if DEBUG
    fprintf(stderr, "numentradas: %i, NUMPROCESOS: %i\n", numentradas, NUMPROCESOS);
#endif

    // Creación fichero informe.txt en el directorio de simulación
    char nfichero[100];
    sprintf(nfichero, "%s%s", argv[2], "informe.txt");
    if (mi_creat(nfichero, 7) < 0)
    {
        bumount(argv[1]);
        exit(0);
    }

    // MEJORA ---> Lectura inicial de todas las entradas a un buffer
    struct entrada entradas[numentradas];
    int error;
    if ((error = mi_read(argv[2], entradas, 0, sizeof(entradas))) < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    int desplazamiento = 0;
    for (int nentr = 0; nentr < numentradas; nentr++)
    {

        // Extracción del PID de la entrada
        pid_t pid = atoi(strchr(entradas[nentr].nombre, '_') + 1);
        struct INFORMACION info;
        info.pid = pid; // Guardamos el pid en el registro info
        info.nEscrituras = 0;

        char prueba_dat[128];
        sprintf(prueba_dat, "%s%s/%s", argv[2], entradas[nentr].nombre, "prueba.dat");

        // Buffer de 256 registros
        int cant_registros_buffer_escrituras = 256;
        struct REGISTRO buffer_escrituras[cant_registros_buffer_escrituras];
        memset(buffer_escrituras, 0, sizeof(buffer_escrituras));

        int offset = 0;

        // Mientras haya escrituras en prueba.dat
        while (mi_read(prueba_dat, buffer_escrituras, offset, sizeof(buffer_escrituras)) > 0)
        {

            int nregistro = 0;
            while (nregistro < cant_registros_buffer_escrituras)
            {
                // Si la escritura es valida (coinciden los pid's)
                if (buffer_escrituras[nregistro].pid == info.pid)
                {
                    // En caso de que sea la primera escritura que leemos almacenamos todos los valores
                    if (!info.nEscrituras)
                    {
                        info.MenorPosicion = buffer_escrituras[nregistro];
                        info.MayorPosicion = buffer_escrituras[nregistro];
                        info.PrimeraEscritura = buffer_escrituras[nregistro];
                        info.UltimaEscritura = buffer_escrituras[nregistro];
                        info.nEscrituras++;
                    }
                    else
                    {
                        // En caso contrario, para cada escritura, cambiaremos los datos del buffer segun cada caso
                        // Actualizacion de los datos (las fechas, la primera escritura y la última)
                        if ((difftime(buffer_escrituras[nregistro].fecha, info.PrimeraEscritura.fecha)) <= 0 &&
                            buffer_escrituras[nregistro].nEscritura < info.PrimeraEscritura.nEscritura)
                        {
                            info.PrimeraEscritura = buffer_escrituras[nregistro];
                        }
                        if ((difftime(buffer_escrituras[nregistro].fecha, info.UltimaEscritura.fecha)) >= 0 &&
                            buffer_escrituras[nregistro].nEscritura > info.UltimaEscritura.nEscritura)
                        {
                            info.UltimaEscritura = buffer_escrituras[nregistro];
                        }
                        if (buffer_escrituras[nregistro].nRegistro < info.MenorPosicion.nRegistro)
                        {
                            info.MenorPosicion = buffer_escrituras[nregistro];
                        }
                        if (buffer_escrituras[nregistro].nRegistro > info.MayorPosicion.nRegistro)
                        {
                            info.MayorPosicion = buffer_escrituras[nregistro];
                        }
                        info.nEscrituras++;
                    }
                }
                nregistro++;
            }
            memset(&buffer_escrituras, 0, sizeof(buffer_escrituras));
            offset += sizeof(buffer_escrituras);
        }

#if DEBUG
        fprintf(stderr, "[%i) %i escrituras validadas en %s]\n", nentr + 1, info.nEscrituras, prueba_dat);
#endif
        // Se añade la informacion del struct en el fichero
        char tiempoPrimero[100];
        char tiempoUltimo[100];
        char tiempoMenor[100];
        char tiempoMayor[100];
        struct tm *tm;

        tm = localtime(&info.PrimeraEscritura.fecha);
        strftime(tiempoPrimero, sizeof(tiempoPrimero), "%a %Y-%m-%d %H:%M:%S", tm);
        tm = localtime(&info.UltimaEscritura.fecha);
        strftime(tiempoUltimo, sizeof(tiempoUltimo), "%a %Y-%m-%d %H:%M:%S", tm);
        tm = localtime(&info.MenorPosicion.fecha);
        strftime(tiempoMenor, sizeof(tiempoMenor), "%a %Y-%m-%d %H:%M:%S", tm);
        tm = localtime(&info.MayorPosicion.fecha);
        strftime(tiempoMayor, sizeof(tiempoMayor), "%a %Y-%m-%d %H:%M:%S", tm);

        char buffer[BLOCKSIZE];
        memset(buffer, 0, BLOCKSIZE);

        sprintf(buffer, "PID: %i\nNumero de escrituras: %i\n", pid, info.nEscrituras);
        sprintf(buffer + strlen(buffer), "%s %i %i %s",
                "Primera escritura",
                info.PrimeraEscritura.nEscritura,
                info.PrimeraEscritura.nRegistro,
                asctime(localtime(&info.PrimeraEscritura.fecha)));

        sprintf(buffer + strlen(buffer), "%s %i %i %s",
                "Ultima escritura",
                info.UltimaEscritura.nEscritura,
                info.UltimaEscritura.nRegistro,
                asctime(localtime(&info.UltimaEscritura.fecha)));

        sprintf(buffer + strlen(buffer), "%s %i %i %s",
                "Menor posicion",
                info.MenorPosicion.nEscritura,
                info.MenorPosicion.nRegistro,
                asctime(localtime(&info.MenorPosicion.fecha)));

        sprintf(buffer + strlen(buffer), "%s %i %i %s",
                "Mayor posicion",
                info.MayorPosicion.nEscritura,
                info.MayorPosicion.nRegistro,
                asctime(localtime(&info.MayorPosicion.fecha)));

        sprintf(buffer,
                "PID: %d\nNumero de escrituras:\t%d\n"
                "Primera escritura:\t%d\t%d\t%s\n"
                "Ultima escritura:\t%d\t%d\t%s\n"
                "Menor posición:\t\t%d\t%d\t%s\n"
                "Mayor posición:\t\t%d\t%d\t%s\n\n",
                info.pid, info.nEscrituras,
                info.PrimeraEscritura.nEscritura,
                info.PrimeraEscritura.nRegistro,
                tiempoPrimero,
                info.UltimaEscritura.nEscritura,
                info.UltimaEscritura.nRegistro,
                tiempoUltimo,
                info.MenorPosicion.nEscritura,
                info.MenorPosicion.nRegistro,
                tiempoMenor,
                info.MayorPosicion.nEscritura,
                info.MayorPosicion.nRegistro,
                tiempoMayor);
        // ESCRITURA EN EL FICHERO Y ACTUALIZACION DEL DESPALZAMIENTO
        if ((desplazamiento += mi_write(nfichero, &buffer, desplazamiento, strlen(buffer))) < 0)
        {
            printf("verifiacion.c: Error al escribir el fichero: '%s'\n", nfichero);
            bumount();
            return FALLO;
        }
    }
    bumount();
}