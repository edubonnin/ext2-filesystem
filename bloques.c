// RUBÉN BALLESTEROS JIMÉNEZ, EDUARDO BONNÍN NARVÁEZ, VICENÇ SERVERA FERRER

#include "bloques.h"
#include "semaforo_mutex_posix.h"

int descriptor;
static unsigned int inside_sc = 0;

static sem_t *mutex;

int bmount(const char *camino)
{
    if (!mutex)
    { // el semáforo es único en el sistema y sólo se ha de inicializar 1 vez (padre)
        mutex = initSem();
        if (mutex == SEM_FAILED)
        {
            return FALLO;
        }
    }

    // ABRIMOS EL FICHERO QUE USAREMOS COMO DISPOSITIVO VIRTUAL
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);
    if (descriptor == FALLO)
    {
        perror("ERROR: ");
        return FALLO;
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
        return FALLO;
    }
    deleteSem();
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
        return FALLO;
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
        return FALLO;
    }
    return rdd;
}

void mi_waitSem()
{
    if (!inside_sc)
    { // inside_sc==0
        waitSem(mutex);
    }
    inside_sc++;
}

void mi_signalSem()
{
    inside_sc--;
    if (!inside_sc)
    {
        signalSem(mutex);
    }
}