#include "main.h"
#include "communicationThread.h"
#include "mainThread.h"

MPI_Datatype MPI_Msg;

Boat *boats;
CostumesPool *costumesPool;

int BOATS_COUNT;
int COSTUMES_COUNT;

int weight;
int tId;
int size;
int state;

pthread_t commThread;
pthread_mutex_t mutexState = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexBoats = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCostumes = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv)
{
    initMPI(&argc, &argv);

    pthread_create(&commThread, NULL, communicationThreadLoop, 0);
    mainThreadLoop();

    cleanUp();
    return 0;
}

void initMPI(int *argc, char ***argv)
{
    BOATS_COUNT = 5;    //TODO
    COSTUMES_COUNT = 3; //TODO

    int provided;
    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);

    if (provided == MPI_THREAD_SINGLE)
    {
        printf("ERROR: The MPI library does not have full thread support\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &tId);

    createMessageType();
    initBoats(tId, size);
    initCostumes(tId, size);
    generatePassengerWeight(tId);
}

void createMessageType()
{
    const int nItems = 6;
    int blocklengths[6] = {1, 1, 1, 1, 1, 1};
    MPI_Datatype types[6] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};

    MPI_Aint offsets[6];
    offsets[0] = offsetof(Message, senderId);
    offsets[1] = offsetof(Message, timestamp);
    offsets[2] = offsetof(Message, priority);
    offsets[3] = offsetof(Message, type);
    offsets[4] = offsetof(Message, boatId);
    offsets[5] = offsetof(Message, senderWeight);

    MPI_Type_create_struct(nItems, blocklengths, offsets, types, &MPI_Msg);
    MPI_Type_commit(&MPI_Msg);
}

void initBoats(int tId, int size)
{
    srand(time(NULL));
    if (tId == ROOT)
        printf("*****Generating boats*****\n");

    boats = (Boat *)malloc(sizeof(Boat) * BOATS_COUNT);

    for (int i = 0; i < BOATS_COUNT; i++)
    {
        boats[i].queue = (int **)malloc(sizeof(int *) * size);
        boats[i].isOnACruise = FALSE;
        boats[i].capacity = -1;
        for (int j = 0; j < size; j++)
        {
            boats[i].queue[j] = (int *)malloc(sizeof(int) * 3);
            boats[i].queue[j][0] = -1; //process id
            boats[i].queue[j][1] = -1; //timestamp
            boats[i].queue[j][2] = -1; //process weight (occupied capacity)
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

void initCostumes(int tId, int size)
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
        int maxCostumes = MAX_NUMBER_OF_COSTUMES > size ? size / 2 : MAX_NUMBER_OF_COSTUMES;
        randNumberOfCostumes = (rand() % (maxCostumes - MIN_NUMBER_OF_COSTUMES) + MIN_NUMBER_OF_COSTUMES);
        printf("Number of available costumes: %d \n", randNumberOfCostumes);
    }
    MPI_Bcast(&randNumberOfCostumes, 1, MPI_INT, ROOT, MPI_COMM_WORLD);
    costumesPool->availableCostumes = randNumberOfCostumes;

    MPI_Barrier(MPI_COMM_WORLD);
}

void generatePassengerWeight(int tId)
{
    srand(tId);
    weight = (rand() % (PASSENGER_MAX_WEIGHT - PASSENGER_MIN_WEIGHT) + PASSENGER_MIN_WEIGHT);
    printf("Generated weight for passenger %d: %d\n", tId, weight);

    MPI_Barrier(MPI_COMM_WORLD);
}

void changeState(int newState)
{
    pthread_mutex_lock(&mutexState);
    if (state == STATE_FINISH)
    {
        pthread_mutex_unlock(&mutexState);
        return;
    }
    state = newState;
    pthread_mutex_unlock(&mutexState);
}

void sendMessage(Message *message, int destination, int tag)
{
    int shouldMemoryBeFreed = FALSE;
    if (message == 0)
    {
        message = malloc(sizeof(Message));
        shouldMemoryBeFreed = TRUE;
    }

    MPI_Send(message, 1, MPI_Msg, destination, tag, MPI_COMM_WORLD);

    if (shouldMemoryBeFreed)
        free(message);
}

void cleanUp()
{
    pthread_mutex_destroy(&mutexState);
    pthread_join(commThread, NULL);

    MPI_Type_free(&MPI_Msg);
    MPI_Finalize();

    free(boats);
    free(costumesPool);
}