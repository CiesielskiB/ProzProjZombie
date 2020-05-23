#include "main.h"
#include "communicationThread.h"
#include "mainThread.h"
#include "functions.h"

MPI_Datatype MPI_Msg;

int clockValue;

Boat *boats;
CostumesPool *costumesPool;

int BOATS_COUNT;
int COSTUMES_COUNT;

int weight;
int tId;
int size;
int state;

int numberOfReceivedCostumePermissions;
int numberOfReceivedBoatPermissions;

pthread_t commThread;

pthread_mutex_t mutexState = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexBoats = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexCostumes = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexClock = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condition_ACK_C = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_ACK_B = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_REL_C = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_REL_B = PTHREAD_COND_INITIALIZER;
pthread_cond_t condition_CRUISE = PTHREAD_COND_INITIALIZER;

int main(int argc, char **argv)
{
    clockValue = 0;
    initArguments(argc, argv);
    initMPI(&argc, &argv);
    createMessageType();
    changeState(STATE_INIT);
    initBoats(tId, size);
    initCostumes(tId, size);
    generatePassengerWeight(tId);

    pthread_create(&commThread, NULL, communicationThreadLoop, 0);
    mainThreadLoop();

    cleanUp();
    return 0;
}

void initArguments(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("ERROR: You must pass two arguments- number of boats and costumes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    BOATS_COUNT = atoi(argv[1]);
    COSTUMES_COUNT = atoi(argv[2]);
}

void initMPI(int *argc, char ***argv)
{
    int provided;
    MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);

    if (provided == MPI_THREAD_SINGLE)
    {
        printf("ERROR: The MPI library does not have full thread support\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &tId);
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
    numberOfReceivedBoatPermissions = 0;

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
    numberOfReceivedCostumePermissions = 0;

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
        printf("Number of available costumes: %d \n", COSTUMES_COUNT);
    }
    costumesPool->availableCostumes = COSTUMES_COUNT;

    MPI_Barrier(MPI_COMM_WORLD);
}

void generatePassengerWeight(int tId)
{
    srand(tId);
    weight = (rand() % (PASSENGER_MAX_WEIGHT - PASSENGER_MIN_WEIGHT) + PASSENGER_MIN_WEIGHT);
    printf("Generated weight for passenger %d: %d\n", tId, weight);

    MPI_Barrier(MPI_COMM_WORLD);
}

void cleanUp()
{
    pthread_mutex_destroy(&mutexState);
    pthread_mutex_destroy(&mutexCostumes);
    pthread_mutex_destroy(&mutexBoats);
    pthread_mutex_destroy(&mutexBoats);

    pthread_cond_destroy(&condition_ACK_B);
    pthread_cond_destroy(&condition_ACK_C);
    pthread_cond_destroy(&condition_CRUISE);
    pthread_cond_destroy(&condition_REL_B);
    pthread_cond_destroy(&condition_REL_C);

    pthread_join(commThread, NULL);

    MPI_Type_free(&MPI_Msg);
    MPI_Finalize();

    free(boats);
    free(costumesPool);
}