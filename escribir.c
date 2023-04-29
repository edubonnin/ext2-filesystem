// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "ficheros.h"

#define DEBUG 0 // Debugger que lee al escribir

int main(int argc, char *argv[])
{
    // CONSULTA QUE LA SINTAXIS SEA CORRECTA
    if (argc != 4)
    {
        fprintf(stderr, ROJO "Sintaxis: escribir <nombre_dispositivo> <'$(cat fichero)'> <diferentes_inodos>\nOffsets: 9000, 209000, 30725000, 409605000, 480000000\nSi diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n" RESET);
        return FALLO;
    }

    printf("longitud texto: %ld\n\n", strlen(argv[2]));
    int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};

    // LLAMA bmount PARA MONTAR EL DISPOSITIVO VIRTUAL
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, "escribir.c: Error al montar el dispositivo virtual.\n");
        return FALLO;
    }

    // RESERVA UN INODO
    int ninodo = reservar_inodo('f', 6);
    if (ninodo == FALLO)
    {
        fprintf(stderr, "escribir.c: Error al reservar el inodo.\n");
        return FALLO;
    }

    //ESCRIBE EL ARGV[2] PARA TODOS LOS OFFSETS
    for (int i = 0; i < (sizeof(offsets) / sizeof(int)); i++)
    {
        printf("Nº inodo reservado: %d\n", ninodo);
        printf("offset: %d\n", offsets[i]);
        int bytesescritos = mi_write_f(ninodo, argv[2], offsets[i], strlen(argv[2]));
        if (bytesescritos == FALLO)
        {
            fprintf(stderr, "escribir.c: Error mi_write_f().\n");
            return FALLO;
        }
        printf("Bytes escritos: %d\n", bytesescritos);

#if DEBUG
        int longitud = strlen(argv[2]);
        char *buffer_texto = malloc(longitud);
        memset(buffer_texto, 0, longitud);
        int leidos = mi_read_f(ninodo, buffer_texto, offsets[i], longitud);
        printf("Bytes leídos: %d\n", leidos);
#endif

        // SE OBTIENE LA INFORMACION DEL INODO DONE SE HA ESCRITO ANTERIORMENTE
        struct STAT p_stat;
        if (mi_stat_f(ninodo, &p_stat))
        {
            fprintf(stderr, "escribir.c: Error mi_stat_f()\n");
            return FALLO;
        }

        printf("stat.tamEnBytesLog=%d\n", p_stat.tamEnBytesLog);
        printf("stat.numBloquesOcupados=%d\n\n", p_stat.numBloquesOcupados);

        // EN EL CASO DE QUE EL TERCER PARAMETRO SEA 0 RESERVARÀ UN NUEVO INODO
        if (strcmp(argv[3], "0"))
        {
            ninodo = reservar_inodo('f', 6);
            if (ninodo == FALLO)
            {
                fprintf(stderr, "escribir.c: Error al reservar el inodo.\n");
                return FALLO;
            }
        }
    }

    // DESMONTA EL DISPOSITIVO VIRTUAL
    if (bumount() < 0)
    {
        fprintf(stderr, "escribir.c: Error al desmonta el dispositivo virtual.\n");
        return FALLO;
    }
    return EXITO;
}