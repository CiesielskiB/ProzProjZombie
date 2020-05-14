#include "main.h"

MPI_Datatype MPI_Msg;

int BOATS_COUNT;
int COSTUMES_COUNT;

Boat *boats;
CostumesPool *costumesPool;

int main(int argc, char **argv)
{
    initMPI(&argc, &argv);
    cleanUp();
    return 0;
}

void initMPI(int *argc, char ***argv)
{
    BOATS_COUNT = 5;    //TODO
    COSTUMES_COUNT = 3; //TODO

    int size, tId, provided;
    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);

    if (provided == MPI_THREAD_SINGLE)
    {
        printf("ERROR: The MPI library does not have full thread support\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &tId);

    createMessageType();
    initBoats(tId, size, BOATS_COUNT);
    initCostumes(tId, size, COSTUMES_COUNT);
}

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
    srand(time(NULL));
    if (tId == ROOT)
        printf("*****Generating boats*****\n");

    boats = (Boat *)malloc(sizeof(Boat) * BOATS_COUNT);

    for (int i = 0; i < count; i++)
    {
        boats[i].queue = (int **)malloc(sizeof(int *) * size);
        boats[i].isOnACruise = FALSE;
        boats[i].capacity = -1;
        for (int j = 0; j < size; j++)
        {
            boats[i].queue[j] = (int *)malloc(sizeof(int) * 2);
            boats[i].queue[j][0] = -1; //process id
            boats[i].queue[j][1] = -1; //timestamp
        }
        int randBoatCapacity = -1;
        if (tId == ROOT)
        {
            randBoatCapacity = (rand() % (BOAT_CAPACITY_MAX - BOAT_CAPACITY_MIN) + BOAT_CAPACITY_MIN);
            printf("Boat id: %d, capacity: %d \n", i, randBoatCapacity);
        }
        MPI_Bcast(&randBoatCapacity, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
        boats[i].capacity = randBoatCapacity;
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

void initCostumes(int tId, int size, int count)
{
    srand(time(NULL));
    if (tId == ROOT)
        printf("*****Generating costumes*****\n");

    costumesPool = (CostumesPool *)malloc(sizeof(CostumesPool));

    costumesPool->queue = (int **)malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++)
    {
        costumesPool->queue[i] = (int *)malloc(sizeof(int) * 2);
        costumesPool->queue[i][0] = -1; //process id
        costumesPool->queue[i][1] = -1; //timestamp
    }

    int randNumberOfCostumes = -1;
    if (tId == ROOT)
    {
        int maxCostumes = MAX_NUMBER_OF_COSTUMES > size ? size : MAX_NUMBER_OF_COSTUMES;
        randNumberOfCostumes = (rand() % (maxCostumes - MIN_NUMBER_OF_COSTUMES) + MIN_NUMBER_OF_COSTUMES);
        printf("Number of available costumes: %d \n", randNumberOfCostumes);
    }
    MPI_Bcast(&randNumberOfCostumes, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    costumesPool->availableCostumes = randNumberOfCostumes;

    MPI_Barrier(MPI_COMM_WORLD);
}

void cleanUp()
{
    //pthread_mutex_destroy(&stateMut);
    /* Czekamy, aż wątek potomny się zakończy */
    printf("Ending... \n");
    //pthread_join(threadKom,NULL);
    //if (rank==0) pthread_join(threadMon,NULL);
    MPI_Type_free(&MPI_Msg);
    MPI_Finalize();
    free(boats);
    free(costumesPool);
}