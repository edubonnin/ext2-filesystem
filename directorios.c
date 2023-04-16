#include "directorios.h"

#define DEBUGN8 1 // DEBUGGER DE bucar_entrada

// int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
// {
//     char s = "/";

//     char *aux = strtok(camino, s);
//     aux = strtok(NULL, s);
//     strcpy(inicial, aux);

//     if (strtok(NULL, s) == NULL)
//     {
//         final = "";
//         tipo = 'f';
//     }
//     else
//     {
//         strncpy(final, camino, strlen(inicial) + 1);
//         tipo = 'd';
//     }

//     return EXITO;
// }

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    // Si el camino no comienza con '/' entonces error.
    if (camino[0] != '/')
    {
        return FALLO;
    }

    // Localizar la primera barra despues de la inicial.
    //+1 para evitar la primera '/'
    char *rest = strchr((camino + 1), '/');
    strcpy(tipo, "f");

    // Si se ha encotrado el caracter '/'
    if (rest)
    {
        // Inicial = camino - resto (Copiamos todo en inicial menos el resto)
        strncpy(inicial, (camino + 1), (strlen(camino) - strlen(rest) - 1));
        // Final = resto
        strcpy(final, rest);

        // Mirar si se trata de un directorio
        if (final[0] == '/')
        {
            strcpy(tipo, "d");
        }
    }
    else // Si no se ha encotrado
    {
        // Inicial = camino
        strcpy(inicial, (camino + 1));
        // Final: vacio
        strcpy(final, "");
    }

    return EXITO;
}

// int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char permisos)
// {
//     struct entrada entrada;
//     struct inodo inodo_dir;
//     char inicial[sizeof(entrada.nombre)];
//     char final[strlen(camino_parcial)];
//     char tipo;
//     int cant_entradas_inodo, num_entrada_inodo;

//     memset(inicial, 0, sizeof(entrada.nombre));
//     memset(final, 0, strlen(camino_parcial));
//     memset(entrada.nombre, 0, sizeof(entrada.nombre));

//     if (!strcmp(camino_parcial, "/"))
//     {
//         struct superbloque SB;
//         bread(posSB, &SB);
//         *(p_inodo) = SB.posInodoRaiz;
//         *(p_entrada) = 0;
//         return EXITO;
//     }

//     if (extraer_camino(camino_parcial, inicial, final, &tipo) == FALLO)
//     {
//         return ERROR_CAMINO_INCORRECTO;
//     }

// #if DEBUGN8
//     fprintf(stderr, "[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
// #endif

//     if (leer_inodo(*p_inodo_dir, &inodo_dir) == FALLO)
//     {
//         return ERROR_PERMISO_LECTURA;
//     }

//     struct entrada buff_lec[BLOCKSIZE / sizeof(struct entrada)];
//     memset(buff_lec, 0, (BLOCKSIZE / sizeof(struct entrada)) * sizeof(struct entrada));
//     cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada);
//     num_entrada_inodo = 0;

//     if (cant_entradas_inodo > 0)
//     {
//         if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
//         {
//             return ERROR_PERMISO_LECTURA;
//         }
//         while ((num_entrada_inodo < cant_entradas_inodo) && (inicial != entrada.nombre))
//         {
//             num_entrada_inodo++;
//             memset(entrada.nombre, 0, sizeof(entrada.nombre)); // RE-INICIALIZACIÓN DEL BUFFER DE LECTURA CON 0's
//             if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
//             {
//                 return ERROR_PERMISO_LECTURA;
//             }
//         }
//     }

//     if ((inicial != entrada.nombre) && (num_entrada_inodo = cant_entradas_inodo))
//     {
//         switch (reservar)
//         {
//         case 0:
//             return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
//             break;

//         case 1:
//             if (inodo_dir.tipo == 'f')
//             {
//                 return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
//             }

//             if ((inodo_dir.permisos & 2) != 2)
//             {
//                 return ERROR_PERMISO_ESCRITURA;
//             }

//             else
//             {
//                 strcpy(entrada.nombre, inicial);
//                 if (tipo == 'd')
//                 {
//                     if (strcmp(final, "/") == 0)
//                     {
//                         entrada.ninodo = reservar_inodo(tipo, permisos);
// #if DEBUGN8
//                         fprintf(stderr, "[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
// #endif
//                     }
//                     else
//                     {
//                         return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
//                     }
//                 }

//                 else
//                 {
//                     entrada.ninodo = reservar_inodo(tipo, permisos);
// #if DEBUGN8
//                     fprintf(stderr, "[buscar()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
// #endif
//                 }

// #if DEBUGN8
//                 fprintf(stderr, "[buscar_entrada()->creada entrada: %s, %d] \n", inicial, entrada.ninodo);
// #endif

//                 if (mi_write_f(*p_inodo_dir, &entrada, inodo_dir.tamEnBytesLog, sizeof(struct entrada)) == FALLO)
//                 {
//                     if (entrada.ninodo != -1)
//                     {
//                         liberar_inodo(entrada.ninodo);
// #if DEBUGN8
//                         fprintf(stderr, "[buscar_entrada()-> liberado inodo %i, reservado a %s\n", num_entrada_inodo, inicial);
// #endif
//                     }
//                     return FALLO;
//                 }
//             }
//             // break;
//         }
//     }

//     if (!strcmp(final, "/") || !strcmp(final, "")) // SI SE HA LLEGADO AL FINAL DEL CAMINO
//     {
//         if ((num_entrada_inodo < cant_entradas_inodo) && (reservar = 1))
//         {
//             // MODO ESCRITURA Y LA ENTRADA YA EXISTE
//             return ERROR_ENTRADA_YA_EXISTENTE;
//         }
//         // CORTAMOS LA RECURSIVIDAD
//         // asignar a *p_inodo el numero de inodo del directorio o fichero creado o leido
//         *p_inodo = entrada.ninodo;
//         // asignar a *p_entrada el número de su entrada dentro del último directorio que lo contiene
//         *p_entrada = num_entrada_inodo;
//         return EXITO;
//     }
//     else
//     {
//         // asignamos a *p_inodo_dir el puntero al inodo que se indica en la entrada encontrada
//         *p_inodo_dir = entrada.ninodo;
//         return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar, permisos);
//     }
//     return EXITO;
// }

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

    //camino_parcial es “/”
    if (!strcmp(camino_parcial, "/"))
    {
        struct superbloque SB;
        bread(posSB, &SB);

        *p_inodo = SB.posInodoRaiz; //nuestra raiz siempre estará asociada al inodo 0
        *p_entrada = 0;

        return EXITO;
    }

    if (extraer_camino(camino_parcial, inicial, final, &tipo) == FALLO)
    {
        return ERROR_CAMINO_INCORRECTO;
    }

#if DEBUGN8
    fprintf(stderr, "[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial, final, reservar);
#endif

    //buscamos la entrada cuyo nombre se encuentra en inicial
    leer_inodo(*p_inodo_dir, &inodo_dir);
    //Comprobamos que el inodo tenga permiso de lectura.
    if ((inodo_dir.permisos & 4) != 4)
    {
        return ERROR_PERMISO_LECTURA;
    }

    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    cant_entradas_inodo = inodo_dir.tamEnBytesLog / sizeof(struct entrada); //cantidad de entradas que contiene el inodo
    num_entrada_inodo = 0;                                                  //nº de entrada inicial

    if (cant_entradas_inodo > 0)
    {
        if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            return ERROR_PERMISO_LECTURA;
        }

        while (num_entrada_inodo < cant_entradas_inodo && strcmp(inicial, entrada.nombre) != 0)
        {
            num_entrada_inodo++;
            memset(entrada.nombre, 0, sizeof(entrada.nombre));
            if (mi_read_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) < 0)
            {
                return ERROR_PERMISO_LECTURA;
            }
        }
    }

    // Si inicial ≠ entrada.nombre:
    // Si inicial no se ha encontrado y se han procesado todas las entradas.
    if ((num_entrada_inodo == cant_entradas_inodo) && (strcmp(entrada.nombre, inicial) != 0))
    { //la entrada no existe
        switch (reservar)
        {
        case 0: //modo consulta. Como no existe retornamos error
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;
        case 1: //modo escritura
            //Creamos la entrada en el directorio referenciado por *p_inodo_dir
            //si es fichero no permitir escritura
            if (inodo_dir.tipo == 'f')
            {
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            //si es directorio comprobar que tiene permiso de escritura
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
                        //reservar un nuevo inodo como directorio y asignarlo a la entrada
                        entrada.ninodo = reservar_inodo('d', permisos);
#if DEBUGN8
                        fprintf(stderr, "[buscar_entrada()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                    }
                    else
                    { //cuelgan más diretorios o ficheros
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                { //es un fichero
                    //reservar un inodo como fichero y asignarlo a la entrada
                    entrada.ninodo = reservar_inodo('f', permisos);
#if DEBUGN8
                    fprintf(stderr, "[buscar()->reservado inodo: %d tipo %c con permisos %d para '%s']\n", entrada.ninodo, tipo, permisos, entrada.nombre);
#endif
                }

#if DEBUGN8
                fprintf(stderr, "[buscar_entrada()->creada entrada: %s, %d] \n", inicial, entrada.ninodo);
#endif

                //escribir la entrada en el directorio padre
                if (mi_write_f(*p_inodo_dir, &entrada, num_entrada_inodo * sizeof(struct entrada), sizeof(struct entrada)) == FALLO)
                {
                    if (entrada.ninodo != FALLO)
                    {
                        liberar_inodo(entrada.ninodo);
#if DEBUGN8
                        fprintf(stderr, "[buscar_entrada()-> liberado inodo %i, reservado a %s\n", num_entrada_inodo, inicial);
#endif
                    }
                    return FALLO;
                }
            }
        }
    }

    //Si hemos llegado al final del camino
    if (!strcmp(final, "/") || !strcmp(final, ""))
    {
        if ((num_entrada_inodo < cant_entradas_inodo) && (reservar == 1))
        {
            //modo escritura y la entrada ya existe
            return ERROR_ENTRADA_YA_EXISTENTE;
        }

        //cortamos la recursividad
        *p_inodo = entrada.ninodo;      //asignar a *p_inodo el numero de inodo del directorio o fichero creado o leido
        *p_entrada = num_entrada_inodo; //asignar a *p_entrada el número de su entrada dentro del último directorio que lo contiene

        return EXITO;
    }
    else
    {

        *p_inodo_dir = entrada.ninodo; //asignamos a *p_inodo_dir el puntero al inodo que se indica en la entrada encontrada;
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