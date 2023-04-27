// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "directorios.h"

#define DEBUGN8 1 // DEBUGGER DE bucar_entrada

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
    unsigned int inodo_dir = 0;
    unsigned int inodo = 0;
    unsigned int entrada = 0;
    int numerror;
    if (numerror = buscar_entrada(camino, inodo_dir, inodo, entrada, '1', permisos) < 0)
    {
        return numerror;
    }
    else
    {

        return EXITO;
    }
}
int mi_dir(const char *camino, char *buffer, char tipo)
{
    struct entrada entrada;
    struct inodo inodo;

    int *ninodo;

    if (buscar_entrada(camino, 0, ninodo, 0, 0, 'r') == FALLO)
    {
        return FALLO;
    }

    leer_inodo(ninodo, &inodo);
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

    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    for (size_t i = 0; i < inodo.tamEnBytesLog / sizeof(entrada.nombre) || strcmp(camino, entrada.nombre); i++)
    {
        /* code */
    }
}
int mi_chmod(const char *camino, unsigned char permisos)
{
    unsigned int inodo_dir = 0;
    unsigned int inodo = 0;
    unsigned int entrada = 0;
    int fallo;
    if (fallo = buscar_entrada(camino, inodo_dir, inodo, entrada, '1', permisos) < 0)
    {
        return fallo;
    }
    else
    {
        mi_chmod_f(inodo, permisos);
        return EXITO;
    }
}