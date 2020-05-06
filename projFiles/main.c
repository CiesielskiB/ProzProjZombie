#include "main.h"


int BOATS_COUNT;
/**
   Metoda tworzy customowy typ wiadomosci wysyłanej przez MPI
**/
void createMessageType();

/**
   tworzymy łodzie i losujemy ich pojemności
**/
void initBoats(int tId, int size, int boatsCount/*, struct Boat *boats*/);

void initMPI(int *argc, char ***argv)
{
    BOATS_COUNT = 1 //TODO
    int size, tId, provided;   //size - number of processes ; tid - current process id ; provided - to check mpi threading
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided < MPI_THREAD_MULTIPLE){
        printf("ERROR: The MPI library does not have full thread support\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    createMessageType(); // TODO
    MPI_Comm_rank( MPI_COMM_WORLD, &tid );
    //przygotowanie łodzi
    initBoats(tId, size, BOATS_COUNT) //TODO
}


//TODO
void createMessageType(){
   int blocklengths[5] = {1,1,1,1,1};
   /*Przykład do robienia wiadomości
   MPI_Datatype types[5] = {MPI_INT,MPI_INT,MPI_INT,MPI_INT,MPI_INT};
   MPI_Aint offsets[5];
   offsets[0] = offsetof(struct Message, sender_id);
   MPI_Type_create_struct(5, blocklengths, offsets, types, &mpi_message_type);
   MPI_Type_commit(&mpi_message_type);
   */
}

/* usunięcie zamkków, czeka, aż zakończy się drugi wątek, zwalnia przydzielony typ MPI_PAKIET_T
   wywoływane w funkcji main przed końcem
*/
void cleanUp()
{
    pthread_mutex_destroy( &stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    println("czekam na wątek \"komunikacyjny\"\n" );
    //pthread_join(threadKom,NULL);
    //if (rank==0) pthread_join(threadMon,NULL);
    MPI_Type_free(&MPI_PAKIET_T);
    MPI_Finalize();
}

int main(int argc, char **argv)
{
    //przygotowanie MPI i struktur
    initMPI(&argc,&argv);   
    
    
    cleanUp();
    return 0;
}