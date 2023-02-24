// bloques.c

#include "bloques.h"

int descriptor;

int bmount(const char *camino)
{
    // ABRIMOS EL FICHERO QUE USAREMOS COMO DISPOSITIVO VIRTUAL
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);
    if (descriptor == FALLO)
    {
        perror("ERROR: ");
    }
    return descriptor;
}

int bumount()
{
    // LIBERA EL DESCRIPTOR DEL FICHERO, EN CASO DE ERROR SE NOTIFICA
    int err = close(descriptor);
    if (err == FALLO)
    {
        perror("ERROR: ");
    }
    return err;
}

int bwrite(unsigned int nbloque, const void *buf)
{
    // POSICIONAMOS EL PUNTERO EN EL LUGAR DESEADO
    lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);
    // ESCRIBIMOS UN BLOQUE, EN CASO DE ERROR LO NOTIFICARÀ
    int wtd = write(descriptor, buf, BLOCKSIZE);
    if (wtd == FALLO)
    {
        perror("ERROR: ");
    }
    return wtd;
}

int bread(unsigned int nbloque, void *buf)
{
    // POSICIONAMOS EL PUNTERO EN EL LUGAR DESEADO
    lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);
    // LEEMOS UN BLOQUE, EN CASO DE ERROR LO NOTIFICARÀ
    int rdd = read(descriptor, buf, BLOCKSIZE);
    if (rdd == FALLO)
    {
        perror("ERROR: ");
    }
    return rdd;
}
