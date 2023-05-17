// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

#define DEBUGN8 0 // DEBUGGER DE bucar_entrada

#include <stdbool.h>

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    if (*camino != '/')
    {
        return FALLO;
    }

    // Encontrar la posición del segundo '/'
    const char *segundo_slash = strchr(camino + 1, '/');

    // Copiar la porción de 'camino' entre los dos primeros '/' en 'inicial'
    if (segundo_slash != NULL)
    {
        strncpy(inicial, camino + 1, segundo_slash - (camino + 1));
        inicial[segundo_slash - (camino + 1)] = '\0';
        strcpy(final, segundo_slash);
        *tipo = 'd';
    }
    else
    {
        strcpy(inicial, camino + 1);
        *final = '\0';
        *tipo = 'f';
    }

    return EXITO;
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo,
                   unsigned int *p_entrada, char reservar, unsigned char permisos)
{
    struct entrada entrada;
    struct inodo inodo_dir;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    int cant_entradas_inodo, num_entrada_inodo;

    memset(inicial, 0, sizeof(entrada.nombre));
    memset(final, 0, strlen(camino_parcial));
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    // camino_parcial es “/”
    if (!strcmp(camino_parcial, "/")) // SI EL DIRECTORIO ES RAÍZ
    {
        struct superbloque SB;
        bread(posSB, &SB);

        // nuestra raiz siempre estará asociada al inodo 0
        *p_inodo = SB.posInodoRaiz;

        *p_entrada = 0;

        return EXITO;
    }

    if (extraer_camino(camino_parcial, inicial, final, &tipo) == FALLO)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

#if DEBUGN8
    fprintf(stderr, AZUL "[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n" RESET, inicial, final, reservar);
#endif

    // buscamos la entrada cuyo nombre se encuentra en inicial
    leer_inodo(*p_inodo_dir, &inodo_dir);
    if ((inodo_dir.permisos & 4) != 4)
    {
        return ERROR_PERMISO_LECTURA;
    }

    // inicializar el buffer de lectura con 0s
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada); // cantidad de entradas que contiene el inodo
    num_entrada_inodo = 0;                                                  // nº de entrada inicial

    if (cant_entradas_inodo > 0)
    {
        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            return ERROR_PERMISO_LECTURA;
        }

        while (num_entrada_inodo < cant_entradas_inodo && strcmp(inicial, entrada.nombre) != 0)
        {
            num_entrada_inodo++;
            // re-inicialización del buffer con 0s
            memset(entrada.nombre, 0, sizeof(entrada.nombre));
            if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
            {
                return ERROR_PERMISO_LECTURA;
            }
        }
    }

    if ((num_entrada_inodo == cant_entradas_inodo) && (strcmp(entrada.nombre, inicial) != 0))
    { // la entrada no existe
        switch (reservar)
        {
        case 0: // modo consulta. Como no existe retornamos error
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;
        case 1: // modo escritura
            // Creamos la entrada en el directorio referenciado por *p_inodo_dir
            // si es fichero no permitir escritura
            if (inodo_dir.tipo == 'f')
            {
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            // si es directorio comprobar que tiene permiso de escritura
            if ((inodo_dir.permisos & 2) != 2)
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd')
                {
                    if (strcmp(final, "/") == 0)
                    {
                        entrada.ninodo = reservar_inodo('d', permisos);
#if DEBUGN8
                        fprintf(stderr, AZUL "[buscar_entrada()->reservado inodo: %d tipo %c con permisos %d para '%s']\n" RESET, entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                    }
                    else
                    { // cuelgan más diretorios o ficheros
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                { // es un fichero
                    entrada.ninodo = reservar_inodo('f', permisos);
#if DEBUGN8
                    fprintf(stderr, AZUL "[buscar()->reservado inodo: %d tipo %c con permisos %d para '%s']\n" RESET, entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                }

#if DEBUGN8
                fprintf(stderr, AZUL "[buscar_entrada()->creada entrada: %s, %d] \n" RESET, inicial, entrada.ninodo);
#endif
                if (mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == FALLO)
                {
                    if (entrada.ninodo != FALLO)
                    {
                        liberar_inodo(entrada.ninodo);
#if DEBUGN8
                        fprintf(stderr, AZUL "[buscar_entrada()-> liberado inodo %i, reservado a %s\n" RESET, num_entrada_inodo, inicial);
#endif
                    }
                    return FALLO;
                }
            }
        }
    }

    if (!strcmp(final, "/") || !strcmp(final, "")) // SI HEMOS LLEGADO AL FINAL DEL CAMINO
    {
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1))
        {
            // modo escritura y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        // cortamos la recursividad
        *p_inodo = entrada.ninodo;
        *p_entrada = num_entrada_inodo;
        return EXITO;
    }
    else
    {
        *p_inodo_dir = entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
    }
    return EXITO;
}

void mostrar_error_buscar_entrada(int error)
{
    // fprintf(stderr, "Error: %d\n", error);
    switch (error)
    {
    case -2:
        fprintf(stderr, ROJO "Error: Camino incorrecto.\n" RESET);
        break;
    case -3:
        fprintf(stderr, ROJO "Error: Permiso denegado de lectura.\n" RESET);
        break;
    case -4:
        fprintf(stderr, ROJO "Error: No existe el archivo o el directorio.\n" RESET);
        break;
    case -5:
        fprintf(stderr, ROJO "Error: No existe algún directorio intermedio.\n" RESET);
        break;
    case -6:
        fprintf(stderr, ROJO "Error: Permiso denegado de escritura.\n" RESET);
        break;
    case -7:
        fprintf(stderr, ROJO "Error: El archivo ya existe.\n" RESET);
        break;
    case -8:
        fprintf(stderr, ROJO "Error: No es un directorio.\n" RESET);
        break;
    }
}

int mi_creat(const char *camino, unsigned char permisos)
{
    mi_waitSem();
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0)
    {
        mi_signalSem();
        return error;
    }

    mi_signalSem();
    return EXITO;
}

int mi_dir(const char *camino, char *buffer, char tipo)
{
    struct entrada entrada;
    struct inodo inodo;
    struct tm *tm;

    int totalentradas, error;

    char tmp[TAMFILA];
    char tamEnBytes[10];

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    // COMPROBAMOS QUE LA ENTRADA CORRESPONDIENTE A CAMINO EXISTE
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    // LEEMOS EL INODO DE LA ENTRADA
    if (leer_inodo(p_inodo, &inodo) == FALLO)
    {
        return FALLO;
    }

    // COMPROBACIÓN PERMISOS DE LECTURA
    if ((inodo.permisos & 4) != 4)
    {
        return FALLO;
    }

    // COMPARACIÓN DE LA SINTAXIS CON EL TIPO REAL DEL INODO
    if (inodo.tipo != tipo)
    {
        fprintf(stderr, ROJO "Error: la sintaxis no concuerda con el tipo\n" RESET);
        return FALLO;
    }

    if (inodo.tipo == 'd')
    {
        struct entrada entradas[BLOCKSIZE / sizeof(struct entrada)];
        memset(&entradas, 0, sizeof(struct entrada));
        totalentradas = inodo.tamEnBytesLog / sizeof(struct entrada);

        int offset;
        if ((offset = mi_read_f(p_inodo, entradas, 0, BLOCKSIZE)) < 0)
        {
            return FALLO;
        }

        for (int i = 0; i < totalentradas; i++)
        {
            if (leer_inodo(entradas[i % (BLOCKSIZE / sizeof(struct entrada))].ninodo, &inodo) == FALLO)
            {
                return FALLO;
            }

            // INFORMACIÓN ACERCA DE TIPO
            if (inodo.tipo == 'd')
            {
                strcat(buffer, VERDE "d\t");
            }
            else
            {
                strcat(buffer, MAGENTA "f\t");
            }

            // INFORMACIÓN ACERCA DE PERMISOS
            if (inodo.permisos & 4)
                strcat(buffer, "r");
            else
                strcat(buffer, "-");
            if (inodo.permisos & 2)
                strcat(buffer, "w");
            else
                strcat(buffer, "-");
            if (inodo.permisos & 1)
                strcat(buffer, "x");
            else
                strcat(buffer, "-");

            strcat(buffer, "\t");

            // INFORMACIÓN ACERCA DEL TIEMPO
            tm = localtime(&inodo.mtime);
            sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
            strcat(buffer, tmp);
            strcat(buffer, "\t");

            // INFORMACIÓN ACERCA DEL TAMAÑO
            sprintf(tamEnBytes, "%d", inodo.tamEnBytesLog);
            strcat(buffer, tamEnBytes);
            strcat(buffer, "\t");

            // INFORMACIÓN ACERCA DEL NOMBRE
            strcat(buffer, entradas[i % (BLOCKSIZE / sizeof(struct entrada))].nombre);
            strcat(buffer, RESET "\n");

            if (offset % (BLOCKSIZE / sizeof(struct entrada)) == 0)
            {
                offset += mi_read_f(p_inodo, entradas, offset, BLOCKSIZE);
            }
        }
    }
    else
    {
        // SI SE EJECUTA EL COMANDO SOBRE UN FICHERO, SÓLO HABRÁ UNA ENTRADA
        totalentradas = 1;
        mi_read_f(p_inodo_dir, &entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada));
        leer_inodo(entrada.ninodo, &inodo);
        strcat(buffer, MAGENTA "f\t");

        // INFORMACIÓN ACERCA DE PERMISOS
        if (inodo.permisos & 4)
            strcat(buffer, "r");
        else
            strcat(buffer, "-");
        if (inodo.permisos & 2)
            strcat(buffer, "w");
        else
            strcat(buffer, "-");
        if (inodo.permisos & 1)
            strcat(buffer, "x");
        else
            strcat(buffer, "-");

        strcat(buffer, "\t");

        // INFORMACIÓN ACERCA DEL TIEMPO
        tm = localtime(&inodo.mtime);
        sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        strcat(buffer, tmp);
        strcat(buffer, "\t");

        // INFORMACIÓN ACERCA DEL TAMAÑO
        sprintf(tamEnBytes, "%d", inodo.tamEnBytesLog);
        strcat(buffer, tamEnBytes);
        strcat(buffer, "\t");

        // INFORMACIÓN ACERCA DEL NOMBRE
        strcat(buffer, entrada.nombre);
        strcat(buffer, RESET "\n");
    }

    return totalentradas;
}

int mi_chmod(const char *camino, unsigned char permisos)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        return error;
    }

    mi_chmod_f(p_inodo, permisos);
    return EXITO;
}

int mi_stat(const char *camino, struct STAT *p_stat)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, p_stat->permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        return error;
    }

    mi_stat_f(p_inodo, p_stat);
    return p_inodo;
}

// FUNCION NECESARA PARA EL mi_escribir.c
int mi_write(const char *camino, const void *buf, unsigned int offset, unsigned int nbytes)
{
    mi_waitSem();
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int error;
    // BUSCA EL NUMERO DEL INODO SEGUN LA ENTRADA
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '0', 0)) < 0)
    { // reservar = 1 ya que se tiene que escribir !!!!!!(revisar)!!!!!!
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }
    mi_signalSem();
    // DEVUELVE EL NUMERO DE BYTES ESCRITOS
    return mi_write_f(p_inodo, buf, offset, nbytes);
}

int mi_read(const char *camino, void *buf, unsigned int offset, unsigned int nbytes)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int error;

    // BUSCA EL NUMERO DEL INODO SEGUN LA ENTRADA
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, '0', 0)) < 0)
    { // reservar = 0 ya que se tiene que leer!!!!!!(revisar)!!!!!!
        mostrar_error_buscar_entrada(error);
        return FALLO;
    }

    // DEVUELVE EL NUMERO DE BYTES LEIDOS
    return mi_read_f(p_inodo, buf, offset, nbytes);
}

int mi_link(const char *camino1, const char *camino2)
{
    mi_waitSem();
    struct inodo inodo;
    struct entrada entrada;

    unsigned int p_inodo_dir1 = 0;
    unsigned int p_inodo1 = 0;
    unsigned int p_entrada1 = 0;

    unsigned int p_inodo_dir2 = 0;
    unsigned int p_inodo2 = 0;
    unsigned int p_entrada2 = 0;

    int error;

    // COMPROBAMOS QUE camino1 EXISTE Y OBTENEMOS p_inodo1
    if ((error = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1, &p_entrada1, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    if (leer_inodo(p_inodo1, &inodo) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }

    // COMPROBAMOS QUE EL INODO ASOCIADO TIENE PERMISOS DE LECTURA
    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, ROJO "Inodo asociado a camino1 no tiene permisos de lectura\n" RESET);
        mi_signalSem();
        return FALLO;
    }

    // DEVOLVEMOS ERROR SI NO SE REFIERE A UN FICHERO
    if (inodo.tipo != 'f')
    {
        fprintf(stderr, ROJO "camino1 no es un fichero\n" RESET);
        mi_signalSem();
        return FALLO;
    }

    // CREAMOS LA ENTRADA DE camino2
    if ((error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2, &p_entrada2, 1, 6)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    // LECTURA DE LA ENTRADA p_entrada2 DE p_inodo_dir2
    if (mi_read_f(p_inodo_dir2, &entrada, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }

    // CREACIÓN ENLACE
    entrada.ninodo = p_inodo1;

    // ESCRITURA ENTRADA MODIFICADA
    if (mi_write_f(p_inodo_dir2, &entrada, p_entrada2 * sizeof(struct entrada), sizeof(struct entrada)) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }

    // LIBERAMOS EL INODO QUE SE HA ASOCIADO A LA ENTRADA CREADA
    if (liberar_inodo(p_inodo2) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }

    // ACTUALIZAMOS Y SALVAMOS p_inodo1
    inodo.nlinks++;
    inodo.ctime = time(NULL);
    if (escribir_inodo(p_inodo1, &inodo) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }

    mi_signalSem();
    return EXITO;
}

int mi_unlink(const char *camino)
{
    mi_waitSem();
    struct inodo inodo;
    struct inodo inodo_dir;

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int error, nEntradasInodoDir;

    // NO SE HA DE PODER BORRAR EL DIRECTORIO RAÍZ
    if (!strcmp(camino, "/"))
    {
        fprintf(stderr, ROJO "No se puede borrar el direcotrio raíz\n" RESET);
        mi_signalSem();
        return FALLO;
    }
    // COMPROBAMOS QUE LA ENTRADA A CAMINO EXISTE Y OBTENEMOS SU NÚMERO DE ENTRADA p_entrada
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 4)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        mi_signalSem();
        return FALLO;
    }

    if (leer_inodo(p_inodo, &inodo) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }

    if (inodo.tipo == 'd' && inodo.tamEnBytesLog > 0) // SI SE TRATA DE UN DIRECTORIO Y NO ESTÁ VACÍO
    {
        fprintf(stderr, ROJO "Error: El directorio %s no está vacío\n" RESET, camino);
        mi_signalSem();
        return FALLO;
    }

    // LEEMOS EL INODO ASOCIADO AL DIRECTORIO QUE CONTIENE LA ENTRADA A ELIMINAR, Y OBTENMOS SU nEntradas
    if (leer_inodo(p_inodo_dir, &inodo_dir) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }
    nEntradasInodoDir = inodo_dir.tamEnBytesLog / sizeof(struct entrada);

    if (p_entrada != nEntradasInodoDir - 1)
    {

        struct entrada entrada;
        if (mi_read_f(p_inodo_dir, &entrada, (nEntradasInodoDir - 1) * sizeof(struct entrada), sizeof(struct entrada)) == FALLO)
        {
            mi_signalSem();
            return FALLO;
        }
        if (mi_write_f(p_inodo_dir, &entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) == FALLO)
        {
            mi_signalSem();
            return FALLO;
        }
    }
    if (mi_truncar_f(p_inodo_dir, sizeof(struct entrada) * (nEntradasInodoDir - 1)) == FALLO)
    {
        mi_signalSem();
        return FALLO;
    }

    inodo.nlinks--;

    if ((inodo.nlinks) == 0)
    {
        if (liberar_inodo(p_inodo) == FALLO)
        {
            mi_signalSem();
            return FALLO;
        }
    }
    else
    {
        inodo.ctime = time(NULL);
        if (escribir_inodo(p_inodo, &inodo) == FALLO)
        {
            mi_signalSem();
            return FALLO;
        }
    }

    mi_signalSem();
    return EXITO;
}