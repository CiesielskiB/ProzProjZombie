#include "main.h"

int BOATS_COUNT;

int main(int argc, char **argv)
{
    initMPI(&argc, &argv);
    cleanUp();
    return 0;
}

void initMPI(int *argc, char ***argv)
{
    BOATS_COUNT = 1; //TODO
    int size, tId, provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    if (provided < MPI_THREAD_MULTIPLE)
    {
        printf("ERROR: The MPI library does not have full thread support\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &tid);
    createMessageType(); // TODO

    //przygotowanie łodzi
    initBoats(tId, size, BOATS_COUNT); //TODO
}

// TODO
void createMessageType()
{
    int blocklengths[5] = {1, 1, 1, 1, 1};
    /*Przykład do robienia wiadomości
   MPI_Datatype types[5] = {MPI_INT,MPI_INT,MPI_INT,MPI_INT,MPI_INT};
   MPI_Aint offsets[5];
   offsets[0] = offsetof(struct Message, sender_id);
   MPI_Type_create_struct(5, blocklengths, offsets, types, &mpi_message_type);
   MPI_Type_commit(&mpi_message_type);
   */
}

void cleanUp()
{
    pthread_mutex_destroy(&stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n");
    //pthread_join(threadKom,NULL);
    //if (rank==0) pthread_join(threadMon,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}