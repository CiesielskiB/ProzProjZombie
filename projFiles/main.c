#include "main.h"

MPI_Datatype MPI_Msg;

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

    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);

    if (provided == MPI_THREAD_SINGLE)
    {
        printf("ERROR: The MPI library does not have full thread support\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &tId);
    createMessageType(); // TODO

    //przygotowanie łodzi
    if (tId == ROOT)
        initBoats(tId, size, BOATS_COUNT); //TODO
}

// TODO
void createMessageType()
{
    const int nItems = 5;
    int blocklengths[5] = {1, 1, 1, 1, 1};
    MPI_Datatype types[5] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[5];
    offsets[0] = offsetof(Message, senderId);
    offsets[1] = offsetof(Message, timestamp);
    offsets[2] = offsetof(Message, priority);
    offsets[3] = offsetof(Message, type);
    offsets[4] = offsetof(Message, data);

    MPI_Type_create_struct(nItems, blocklengths, offsets, types, &MPI_Msg);
    MPI_Type_commit(&MPI_Msg);
}

void initBoats(int tId, int size, int count)
{
    
}

void cleanUp()
{
    //pthread_mutex_destroy(&stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    printf("czekam na wątek \"komunikacyjny\"\n");
    //pthread_join(threadKom,NULL);
    //if (rank==0) pthread_join(threadMon,NULL);
    // MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}